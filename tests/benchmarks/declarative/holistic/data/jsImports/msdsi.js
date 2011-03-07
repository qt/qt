// This JavaScript file is a single, small, imported script.
// It imports other small scripts which are deeply nested.

.import "msdsi1.js" as Msdsi1

function testFunc(seedValue) {
    var retn = 0.5;
    retn *= Msdsi1.testFunc(seedValue + retn);
    return retn;
}
