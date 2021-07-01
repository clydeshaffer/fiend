const fs = require('fs');
const argv = require('minimist')(process.argv.slice(2));
const BMP = require('bitmap-manipulation');

const inFileName = argv._[0];
const tmpFileName = inFileName + ".tmp";
const outFileName = argv._.length == 2 ?
    argv._[1] :
    inFileName.split(".").slice(0, -1).join(".") + ".gtg";


/*Jimp.read(inFileName)
    .then( spriteSheet => {
        spriteSheet.flip(false,true);
        const w = spriteSheet.getWidth();
        const h = spriteSheet.getHeight();
        spriteSheet.getBuffer(Jimp.MIME_BMP).then(
            gtgBuf => {
                fs.writeFileSync(outFileName, gtgBuf.slice(-w * h));
            }
        );
    });*/

let spriteSheet = BMP.BMPBitmap.fromFile(inFileName);
let outSheet = new BMP.BMPBitmap(spriteSheet.getWidth(), spriteSheet.getHeight(), 1);
let pixelCount = spriteSheet.getHeight() * spriteSheet.getWidth();
for(var i = 0; i < spriteSheet.getHeight(); i++) {
    outSheet.drawBitmap(spriteSheet, 0, i, null, 0, spriteSheet.getHeight() - i - 1, spriteSheet.getWidth(), spriteSheet.getHeight());
}
outSheet.save(tmpFileName);

const tmpFileSize = fs.statSync(tmpFileName).size;
const outputStream = fs.createWriteStream(outFileName);

outputStream.on("finish", () => {
    fs.unlinkSync(tmpFileName);
});

fs.createReadStream(tmpFileName, { start : (tmpFileSize - pixelCount)}).pipe(outputStream);