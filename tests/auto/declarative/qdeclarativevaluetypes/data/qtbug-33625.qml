import QtQuick 1.0
Item {
    property string stringtest: "16777237"
    property int stringint: stringtest

    property int inttest: 16777237
    property int intint: 0

    Component.onCompleted: {
        intint = inttest;
    }
}
