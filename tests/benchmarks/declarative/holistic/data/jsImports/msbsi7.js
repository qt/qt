// This JavaScript file is a single, small, imported script.
// It doesn't import any other scripts.
// It is imported from another script.

function testFunc(seedValue) {
    var retn = 5;
    retn += (seedValue * 0.745);
    retn *= (1 + (3.1415962 / seedValue));
    retn /= 2.41497;
    retn -= (seedValue * -1);
    return retn;
}
