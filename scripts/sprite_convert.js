const fs = require('fs');
const argv = require('minimist')(process.argv.slice(2));
const BMP = require('bitmap-manipulation');

const inFileName = argv._[0];
const outFileName = argv._.length == 2 ?
    argv._[1] :
    inFileName.split(".").slice(0, -1).join(".") + ".gtg";


function flipV(inputImage) {
    let flippedImage = new BMP.BMPBitmap(inputImage.width, inputImage.height, 1);
    for(var i = 0; i < spriteSheet.getHeight(); i++) {
        flippedImage.drawBitmap(inputImage, 0, i, null, 0, inputImage.height - i - 1, inputImage.width, inputImage.height);
    }
    return flippedImage;
}

const randchars = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNNM1234567890";
function rndStr(len) {
    var out = "";
    for(var i = 0; i < len; i++) {
        out += randchars[Math.floor(Math.random() * randchars.length)];
    }
    return out;
}

function saveHeadless(img, filename) {
    var tmpname = rndStr(16) + ".tmp";
    var pixelCount = img.width * img.height;
    img.save(tmpname);
    const tmpFileSize = fs.statSync(tmpname).size;
    const outputStream = fs.createWriteStream(filename);
    outputStream.on("finish", () => {
        fs.unlinkSync(tmpname);
    });
    fs.createReadStream(tmpname, { start : (tmpFileSize - pixelCount)}).pipe(outputStream);
}

let spriteSheet = BMP.BMPBitmap.fromFile(inFileName);
let outSheet = flipV(spriteSheet);
saveHeadless(outSheet, outFileName);