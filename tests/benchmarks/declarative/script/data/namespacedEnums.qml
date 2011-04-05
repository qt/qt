import QtQuick 1.0 as QtQuick

QtQuick.Item {
    function runtest() {
        var a = 0;
        for (var ii = 0; ii < 100000; ++ii)
            a += QtQuick.Text.RichText;
        return a;
    }
}

