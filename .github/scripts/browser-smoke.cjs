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

        if (pageErrors.length > 0) {
            throw new Error("Browser runtime errors: " + pageErrors.join(" | "));
        }

        console.log("Browser smoke test passed.");
        console.log("Canvas: " + canvas.width + "x" + canvas.height);
        console.log("Captured main menu, gameplay, treasure cave, and completion screens.");
    } finally {
        await browser.close();
    }
}

main().catch(error => {
    console.error(error);
    process.exit(1);
});
