const { chromium } = require("playwright");
const fs = require("node:fs");

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

    const page = await browser.newPage({
        viewport: { width: 960, height: 540 },
        deviceScaleFactor: 1
    });

    const pageErrors = [];
    page.on("pageerror", error => pageErrors.push(error.message));

    try {
        await page.goto("http://127.0.0.1:8000/underwater_treasure_hunt.html",
            { waitUntil: "networkidle" });
        await page.waitForSelector("canvas");
        await page.waitForTimeout(2500);

        const canvas = await page.locator("canvas").first().evaluate(element => ({
            width: element.width,
            height: element.height
        }));

        if (canvas.width !== 960 || canvas.height !== 540) {
            throw new Error("Unexpected game canvas size: " + JSON.stringify(canvas));
        }

        await page.screenshot({ path: "screenshots/main-menu.png" });

        await page.mouse.click(480, 394);
        await page.waitForTimeout(500);
        await page.screenshot({ path: "screenshots/gameplay.png" });

        await waitForKey(page, "w", 700);
        await waitForKey(page, "d", 3300);
        await waitForKey(page, "s", 1000);
        await page.waitForTimeout(300);
        await page.screenshot({ path: "screenshots/treasure-cave.png" });

        await page.keyboard.press("Space");
        await page.waitForTimeout(500);
        await page.screenshot({ path: "screenshots/mission-complete.png" });

        const mobilePage = await browser.newPage({
            viewport: { width: 844, height: 475 },
            deviceScaleFactor: 1,
            isMobile: true,
            hasTouch: true
        });
        mobilePage.on("pageerror", error => pageErrors.push(error.message));

        await mobilePage.goto("http://127.0.0.1:8000/underwater_treasure_hunt.html",
            { waitUntil: "networkidle" });
        await mobilePage.waitForTimeout(1500);

        const mobileCanvas = mobilePage.locator("canvas").first();
        const mobileBox = await mobileCanvas.boundingBox();

        if (!mobileBox || mobileBox.width < 800 || mobileBox.height < 450 ||
            mobileBox.width > 845 || mobileBox.height > 476) {
            throw new Error("The game canvas does not fit a landscape phone viewport: " +
                JSON.stringify(mobileBox));
        }

        await mobilePage.touchscreen.tap(
            mobileBox.x + mobileBox.width * 0.5,
            mobileBox.y + mobileBox.height * (394 / 540)
        );
        await mobilePage.waitForTimeout(300);

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
        await mobilePage.screenshot({ path: "screenshots/mobile-landscape.png" });

        if (pageErrors.length > 0) {
            throw new Error("Browser runtime errors: " + pageErrors.join(" | "));
        }

        console.log("Browser smoke test passed.");
        console.log("Desktop canvas: " + canvas.width + "x" + canvas.height);
        console.log("Mobile landscape canvas: " +
            Math.round(mobileBox.width) + "x" + Math.round(mobileBox.height));
        console.log("Captured desktop game screens and a touch-enabled mobile layout.");
    } finally {
        await browser.close();
    }
}

main().catch(error => {
    console.error(error);
    process.exit(1);
});
