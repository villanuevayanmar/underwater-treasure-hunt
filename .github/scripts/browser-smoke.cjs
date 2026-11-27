const { chromium } = require("playwright");
const { PNG } = require("pngjs");
const fs = require("node:fs");

function changedRatio(firstImage, secondImage) {
    const first = PNG.sync.read(firstImage);
    const second = PNG.sync.read(secondImage);

    if (first.width !== second.width || first.height !== second.height) {
        throw new Error("Cannot compare screenshots with different dimensions.");
    }

    let changed = 0;
    const total = first.width * first.height;

    for (let i = 0; i < first.data.length; i += 4) {
        if (Math.abs(first.data[i] - second.data[i]) > 24 ||
            Math.abs(first.data[i + 1] - second.data[i + 1]) > 24 ||
            Math.abs(first.data[i + 2] - second.data[i + 2]) > 24) {
            changed++;
        }
    }

    return changed / total;
}

function countOrange(image, region) {
    const png = PNG.sync.read(image);
    const left = Math.max(0, Math.floor(region.x));
    const top = Math.max(0, Math.floor(region.y));
    const right = Math.min(png.width, Math.ceil(region.x + region.width));
    const bottom = Math.min(png.height, Math.ceil(region.y + region.height));
    let count = 0;

    for (let y = top; y < bottom; y++) {
        for (let x = left; x < right; x++) {
            const i = (y * png.width + x) * 4;
            const red = png.data[i];
            const green = png.data[i + 1];
            const blue = png.data[i + 2];

            if (red > 165 && green > 75 && green < 185 &&
                blue < 115 && red > green * 1.15) {
                count++;
            }
        }
    }

    return count;
}

async function clickAt(page, x, y) {
    await page.mouse.move(x, y);
    await page.mouse.down();
    await page.waitForTimeout(120);
    await page.mouse.up();
}

async function waitForKey(page, key, milliseconds) {
    await page.keyboard.down(key);
    await page.waitForTimeout(milliseconds);
    await page.keyboard.up(key);
}

async function main() {
    fs.mkdirSync("screenshots", { recursive: true });

    const browser = await chromium.launch({
        headless: true,
        args: [
            "--use-gl=angle",
            "--use-angle=swiftshader",
            "--enable-webgl",
            "--ignore-gpu-blocklist"
        ]
    });

    const pageErrors = [];

    try {
        const page = await browser.newPage({
            viewport: { width: 960, height: 540 },
            deviceScaleFactor: 1
        });
        page.on("pageerror", error => pageErrors.push(error.message));

        await page.goto("http://127.0.0.1:8000/underwater_treasure_hunt.html",
            { waitUntil: "networkidle" });
        await page.waitForSelector("canvas");
        await page.waitForTimeout(1800);

        const canvas = page.locator("canvas").first();
        const canvasBox = await canvas.boundingBox();

        if (!canvasBox || Math.abs(canvasBox.width - 960) > 1 ||
            Math.abs(canvasBox.height - 540) > 1) {
            throw new Error("Desktop canvas does not fill the game viewport: " +
                JSON.stringify(canvasBox));
        }

        const menuShot = await page.screenshot({ path: "screenshots/main-menu.png" });

        await clickAt(
            page,
            canvasBox.x + canvasBox.width * 0.5,
            canvasBox.y + canvasBox.height * (394 / 540)
        );
        await page.waitForTimeout(450);

        const gameplayShot = await page.screenshot({ path: "screenshots/gameplay.png" });
        const startChange = changedRatio(menuShot, gameplayShot);

        if (startChange < 0.08) {
            throw new Error("Start button did not change the menu into gameplay. " +
                "Screenshot difference: " + startChange.toFixed(3));
        }

        await waitForKey(page, "w", 700);
        await waitForKey(page, "d", 3300);
        await waitForKey(page, "s", 1000);
        await page.waitForTimeout(250);

        const caveShot = await page.screenshot({ path: "screenshots/treasure-cave.png" });
        const submarineAtCave = countOrange(caveShot, {
            x: 745, y: 280, width: 115, height: 95
        });

        if (submarineAtCave < 500) {
            throw new Error("The submarine did not reach the expected cave area. " +
                "Orange pixels in cave region: " + submarineAtCave);
        }

        await page.keyboard.press("Space");
        await page.waitForTimeout(450);

        const completeShot = await page.screenshot({
            path: "screenshots/mission-complete.png"
        });
        const completionChange = changedRatio(caveShot, completeShot);

        if (completionChange < 0.08) {
            throw new Error("Collecting the nearby treasure did not open the completion screen. " +
                "Screenshot difference: " + completionChange.toFixed(3));
        }

        const mobilePage = await browser.newPage({
            viewport: { width: 844, height: 475 },
            deviceScaleFactor: 1,
            isMobile: true,
            hasTouch: true
        });
        mobilePage.on("pageerror", error => pageErrors.push(error.message));

        await mobilePage.goto("http://127.0.0.1:8000/underwater_treasure_hunt.html",
            { waitUntil: "networkidle" });
        await mobilePage.waitForTimeout(1200);

        const mobileCanvas = mobilePage.locator("canvas").first();
        const mobileBox = await mobileCanvas.boundingBox();

        if (!mobileBox || mobileBox.width < 800 || mobileBox.height < 450 ||
            mobileBox.width > 845 || mobileBox.height > 476) {
            throw new Error("The game canvas does not fit a landscape phone viewport: " +
                JSON.stringify(mobileBox));
        }

        const mobileMenuShot = await mobilePage.screenshot();
        await mobilePage.touchscreen.tap(
            mobileBox.x + mobileBox.width * 0.5,
            mobileBox.y + mobileBox.height * (394 / 540)
        );
        await mobilePage.waitForTimeout(350);

        const mobileGameShot = await mobilePage.screenshot();
        if (changedRatio(mobileMenuShot, mobileGameShot) < 0.08) {
            throw new Error("Touching the Start button did not open mobile gameplay.");
        }

        const touchSession = await mobilePage.context().newCDPSession(mobilePage);
        await touchSession.send("Input.dispatchTouchEvent", {
            type: "touchStart",
            touchPoints: [{
                id: 1,
                x: mobileBox.x + mobileBox.width * (160 / 960),
                y: mobileBox.y + mobileBox.height * (430 / 540),
                radiusX: 8,
                radiusY: 8,
                force: 1
            }]
        });
        await mobilePage.waitForTimeout(450);
        await touchSession.send("Input.dispatchTouchEvent", {
            type: "touchEnd",
            touchPoints: []
        });

        const mobileMovedShot = await mobilePage.screenshot({
            path: "screenshots/mobile-landscape.png"
        });
        const submarineMovedByTouch = countOrange(mobileMovedShot, {
            x: 175, y: 220, width: 95, height: 75
        });

        if (submarineMovedByTouch < 500) {
            throw new Error("The mobile touch direction did not move the submarine. " +
                "Orange pixels in expected region: " + submarineMovedByTouch);
        }

        if (pageErrors.length > 0) {
            throw new Error("Browser runtime errors: " + pageErrors.join(" | "));
        }

        console.log("Desktop and touch-enabled browser tests passed.");
        console.log("Verified menu start, keyboard navigation, cave arrival, and treasure completion.");
        console.log("Verified landscape canvas scaling and a held touch movement.");
    } finally {
        await browser.close();
    }
}

main().catch(error => {
    console.error(error);
    process.exit(1);
});
