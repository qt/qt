// This JavaScript file is a single, small, imported script.
// It imports other small scripts which are deeply nested.
// It is imported by another script.

.import "msdsi5.js" as Msdsi5

function testFunc(seedValue) {
    var retn = 0.45;
    retn *= Msdsi5.testFunc(seedValue + retn);
    return retn;
}
