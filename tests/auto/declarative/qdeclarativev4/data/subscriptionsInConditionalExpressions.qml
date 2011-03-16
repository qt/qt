import QtQuick 1.0

Item {
    id: thisTest

    property bool cond: true
    property real a: 1
    property real result: cond ? a : a

    PropertyAction { running: true; target: thisTest; property: "a"; value: 2; }
}
