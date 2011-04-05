// This JavaScript file is a single, small, imported script.
// It imports other small scripts which are deeply nested.
// It is imported by another script.

.import "msdsi13.js" as Msdsi13

function testFunc(seedValue) {
    var retn = 0.125;
    retn *= Msdsi13.testFunc(seedValue + retn);
    return retn;
}
