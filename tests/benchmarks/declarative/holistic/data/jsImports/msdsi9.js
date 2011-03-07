// This JavaScript file is a single, small, imported script.
// It imports other small scripts which are deeply nested.
// It is imported by another script.

.import "msdsi10.js" as Msdsi10

function testFunc(seedValue) {
    var retn = 0.95;
    retn *= Msdsi10.testFunc(seedValue + retn);
    return retn;
}
