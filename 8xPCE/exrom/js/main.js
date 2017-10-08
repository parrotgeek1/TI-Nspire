/*
    This file is part of 'ti_jsemu_wrapper' - see https://github.com/TI-Planet/ti_jsemu_wrapper
    LGPL3-licensed
*/

var destCanvas;
var destCanvasCtx;
var srcCanvas;
var histoList;
var calculatorDiv;
var hand;
var displayDiv;
var zoomDiv;
var calcDivZoom = 1.0;
var zoomedScreenRatio = 1.5;
var histoListMaxEls = 8;
var hasHand = false;
var hasZoomedDisplay = true;
var keyHistoryArray = [];

$id = function(id) { return document.getElementById(id); };
$sel = function(sel) { return document.querySelector(sel); };
$selAll = function(sel) { return document.querySelectorAll(sel); };

function dl() {
var arr= defaultROM.asic_rom_flash;
if(arr == undefined) {
alert("It only supports 84+CE/83PCE emulators!");
// need to block cse!
location.reload();
return;
}
var byteArray;
var dl="rom.rom";
if(confirm("Press cancel to extract ONLY the boot code, or OK to extract the whole ROM")) {
	byteArray = new Uint8Array(arr);
} else {
	if(confirm("Press cancel to create a blank ROM with just boot code, or OK to create the 128kb bootcode.bin")) {
		byteArray = new Uint8Array(arr.splice(0,0x20000));
		dl="bootcode.bin";
	} else {
		arr = arr.fill(0xFF,0x20000);
		byteArray = new Uint8Array(arr);
		dl="rom_no_os.rom";
	}
}
if(confirm("Press cancel if this ROM will be used on an emulator, or OK if it will be used on real hardware")) {
	// http://wikiti.brandonw.net/index.php?title=Category:84PCE:RAM:By_Address&oldid=10982
	byteArray[0x7E] = 0xFF;
}
var a = window.document.createElement('a');

a.href = window.URL.createObjectURL(new Blob([byteArray], { type: 'application/octet-stream' }));
a.download = dl;

// Append anchor to body.
document.body.appendChild(a);
a.click();
// Remove anchor from body
document.body.removeChild(a);

    $id("initLoading").innerHTML = "Done!";


}

function initWithSVG(svgStr) {
    svgVar = svgStr;
    clog_bak = console.log;
    theCalc = new TI84ForSmartView();
    console.log = clog_bak;
    if (!theCalc) {
        return alert("Error initializing the emulator :(")
    }
    theCalc.initSVG();
    theCalc.boot();
setTimeout(dl,2000);
}

