// This JavaScript file is a single, small, imported script.
// It imports other small scripts which are deeply nested.
// It is imported by another script.

.import "msdsi6.js" as Msdsi6

function testFunc(seedValue) {
    var retn = 0.55;
    retn *= Msdsi6.testFunc(seedValue + retn);
    return retn;
}
