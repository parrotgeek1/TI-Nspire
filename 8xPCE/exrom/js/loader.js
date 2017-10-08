/*
    This file is part of 'ti_jsemu_wrapper' - see https://github.com/TI-Planet/ti_jsemu_wrapper
    LGPL3-licensed
*/

objs = {
    js: [ /\/js\/.*-min\.js/ ],
    css: [ /\/css\/.*\.css/ ],
    buttons: [ /\/keyimages\/KEY.*@2x\.png/ ],
    skin: [ /\/images\/.*_S[^y]*\.svg/ ]
};
buttonImages = {};

var fileInput = $id("file-input");
fileInput.addEventListener('change', function() {
    var file = this.files[0];
    if (file.name.slice(-3) != "jar") {
        return alert("This doesn't look like an emulator JAR!");
    }
    this.disabled = true;
    this.style.display = "none";
    $id("initLoading").style.display = "block";
    zip.createReader(new zip.BlobReader(file), function(zipReader) {
        zipReader.getEntries(function(entries) {
            entries.forEach(function(entry) {
                for (var key in objs) {
                    var obj = objs[key];
                    if (!obj[1]) {
                        if (obj[0].test(entry.filename)) {
                            entry.getData(key == "buttons" ? new zip.Data64URIWriter("image/png")
                                                           : new zip.TextWriter(),
                                (function(passedKey) {
                                return function (data) {
                                    if (passedKey == "buttons") {
                                        var btnName = entry.filename.split(" ")[0];
                                        btnName = btnName.substr(btnName.indexOf("KEY_"));
                                        buttonImages[btnName] = data;
                                    } else {
                                        objs[passedKey][1] = true;
                                        objs[passedKey][2] = data;
                                    }
                                    if (objs.js[1] && objs.skin[1] && objs.css[1]) {
                                        eval(objs.js[2]);
                                        var s = document.createElement("style");
                                       // s.innerHTML = objs.css[2];
                                        document.getElementsByTagName("head")[0].appendChild(s);
                                        initWithSVG(objs.skin[2]);
                                        zipReader.close();
                                        //fileInput.parentNode.style.display = "none";
                                        [].forEach.call($selAll("#histo, #calculatorDiv, #zoom"), function(el) {
                                            //el.style.display = "block";
                                        });
                                    }
                                }
                            })(key));
                        }
                    }
                }
            });
        });
    }, alert );
}, false);
