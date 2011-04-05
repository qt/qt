import QtQuick 1.0

Item {
    function runtest() {
        var a = 0;
        for (var ii = 0; ii < 100000; ++ii)
            a += Text.RichText;
        return a;
    }
}
