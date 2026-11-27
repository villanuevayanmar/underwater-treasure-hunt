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

function orangeCentroid(image, region) {
    const png = PNG.sync.read(image);
    const left = Math.max(0, Math.floor(region.x));
    const top = Math.max(0, Math.floor(region.y));
    const right = Math.min(png.width, Math.ceil(region.x + region.width));
    const bottom = Math.min(png.height, Math.ceil(region.y + region.height));
    let count = 0;
    let sumX = 0;
    let sumY = 0;

    for (let y = top; y < bottom; y++) {
        for (let x = left; x < right; x++) {
            const i = (y * png.width + x) * 4;
            const red = png.data[i];
            const green = png.data[i + 1];
            const blue = png.data[i + 2];

            if (red > 165 && green > 75 && green < 185 &&
                blue < 115 && red > green * 1.15) {
                count++;
                sumX += x;
                sumY += y;
            }
        }
    }

    if (count === 0) return null;
    return { x: sumX / count, y: sumY / count, count };
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

        await page.keyboard.press("Space");
        await page.waitForTimeout(300);
        const farInteractionShot = await page.screenshot();

        if (changedRatio(gameplayShot, farInteractionShot) > 0.08) {
            throw new Error("The treasure was collected while the submarine was still far away.");
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

        const savedResult = await page.evaluate(() => {
            const fileSystem = window.FS || (window.Module && window.Module.FS);
            if (!fileSystem) return null;
            return fileSystem.readFile("mission_result.txt", { encoding: "utf8" });
        });

        if (!savedResult || !savedResult.includes("Mission: Complete") ||
            !savedResult.includes("Time:")) {
            throw new Error("mission_result.txt was not written with the mission result.");
        }

        await clickAt(
            page,
            canvasBox.x + canvasBox.width * (365 / 960),
            canvasBox.y + canvasBox.height * (409 / 540)
        );
        await page.waitForTimeout(350);

        const restartedShot = await page.screenshot();
        const submarineAfterRestart = countOrange(restartedShot, {
            x: 55, y: 210, width: 145, height: 95
        });

        if (changedRatio(completeShot, restartedShot) < 0.08 ||
            submarineAfterRestart < 500) {
            throw new Error("Play Again did not restart the mission at the starting position.");
        }

        await page.keyboard.press("Escape");
        await page.waitForTimeout(350);

        const returnedMenuShot = await page.screenshot();
        if (changedRatio(restartedShot, returnedMenuShot) < 0.08) {
            throw new Error("ESC did not return to the title screen.");
        }

        await page.reload({ waitUntil: "networkidle" });
        await page.waitForTimeout(1200);
        const menuForSecondRound = await page.screenshot();

        await clickAt(
            page,
            canvasBox.x + canvasBox.width * 0.5,
            canvasBox.y + canvasBox.height * (394 / 540)
        );
        await page.waitForTimeout(350);
        const secondRoundStart = await page.screenshot();

        if (changedRatio(menuForSecondRound, secondRoundStart) < 0.08) {
            throw new Error("The title screen could not start another mission.");
        }

        await waitForKey(page, "w", 700);
        await waitForKey(page, "d", 3300);
        await waitForKey(page, "s", 1000);
        await page.keyboard.press("Space");
        await page.waitForTimeout(350);

        const secondCompleteShot = await page.screenshot();
        await clickAt(
            page,
            canvasBox.x + canvasBox.width * (595 / 960),
            canvasBox.y + canvasBox.height * (409 / 540)
        );
        await page.waitForTimeout(350);

        const mainMenuButtonShot = await page.screenshot();
        if (changedRatio(secondCompleteShot, mainMenuButtonShot) < 0.08) {
            throw new Error("The Main Menu button did not return to the title screen.");
        }

        const collisionPage = await browser.newPage({
            viewport: { width: 960, height: 540 },
            deviceScaleFactor: 1
        });
        collisionPage.on("pageerror", error => pageErrors.push(error.message));

        await collisionPage.goto("http://127.0.0.1:8000/underwater_treasure_hunt.html",
            { waitUntil: "networkidle" });
        await collisionPage.waitForTimeout(1200);
        const collisionCanvas = collisionPage.locator("canvas").first();
        const collisionBox = await collisionCanvas.boundingBox();

        await clickAt(
            collisionPage,
            collisionBox.x + collisionBox.width * 0.5,
            collisionBox.y + collisionBox.height * (394 / 540)
        );
        await collisionPage.waitForTimeout(350);
        await waitForKey(collisionPage, "w", 250);
        await waitForKey(collisionPage, "d", 1300);

        const blockedPosition = await collisionPage.screenshot();
        const firstRockPosition = orangeCentroid(blockedPosition, {
            x: 215, y: 175, width: 160, height: 70
        });

        await waitForKey(collisionPage, "d", 500);
        const heldAgainstRock = await collisionPage.screenshot();
        const secondRockPosition = orangeCentroid(heldAgainstRock, {
            x: 215, y: 175, width: 160, height: 70
        });

        if (!firstRockPosition || !secondRockPosition ||
            firstRockPosition.x < 250 || firstRockPosition.x > 320 ||
            Math.abs(firstRockPosition.x - secondRockPosition.x) > 3) {
            throw new Error("The submarine did not stop against the first rock. " +
                JSON.stringify({ firstRockPosition, secondRockPosition }));
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
        const touchSession = await mobilePage.context().newCDPSession(mobilePage);

        await touchSession.send("Input.dispatchTouchEvent", {
            type: "touchStart",
            touchPoints: [{
                id: 2,
                x: mobileBox.x + mobileBox.width * 0.5,
                y: mobileBox.y + mobileBox.height * (394 / 540),
                radiusX: 9,
                radiusY: 9,
                force: 1
            }]
        });
        await mobilePage.waitForTimeout(350);
        await touchSession.send("Input.dispatchTouchEvent", {
            type: "touchEnd",
            touchPoints: []
        });
        await mobilePage.waitForTimeout(250);

        const mobileGameShot = await mobilePage.screenshot();
        if (changedRatio(mobileMenuShot, mobileGameShot) < 0.08) {
            throw new Error("Touching the Start button did not open mobile gameplay.");
        }

        await touchSession.send("Input.dispatchTouchEvent", {
            type: "touchStart",
            touchPoints: [{
                id: 1,
                x: mobileBox.x + mobileBox.width * (181 / 960),
                y: mobileBox.y + mobileBox.height * (451 / 540),
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
        console.log("Verified rock blocking, treasure collection, FILE save, Play Again, ESC, and Main Menu.");
        console.log("Verified landscape canvas scaling, touch start, and held touch movement.");
    } finally {
        await browser.close();
    }
}

main().catch(error => {
    console.error(error);
    process.exit(1);
});
