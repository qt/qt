// This JavaScript file is a single, large, imported script.
// It imports a shared library script.
.import "pragmaLib.js" as PragmaLibJs

function testFuncTwo(seedValue) {
    var retn = seedValue + 3;
    retn += PragmaLibJs.testFunc();
    return retn;
}
