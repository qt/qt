// This JavaScript file is a single, small, imported script.
// It imports other small scripts which are deeply nested.
// It is imported by another script.

.import "msdsi7.js" as Msdsi7

function testFunc(seedValue) {
    var retn = 0.65;
    retn *= Msdsi7.testFunc(seedValue + retn);
    return retn;
}
