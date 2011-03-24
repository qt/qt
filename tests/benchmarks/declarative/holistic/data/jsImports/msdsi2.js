// This JavaScript file is a single, small, imported script.
// It imports other small scripts which are deeply nested.
// It is imported by another script.

.import "msdsi3.js" as Msdsi3

function testFunc(seedValue) {
    var retn = 0.25;
    retn *= Msdsi3.testFunc(seedValue + retn);
    return retn;
}
