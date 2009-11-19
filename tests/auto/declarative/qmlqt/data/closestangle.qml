import Qt 4.6

QtObject {
    property var testSame: Qt.closestAngle(0,1)
    property var testLess: Qt.closestAngle(0,-359)
    property var testMore: Qt.closestAngle(0,361)
    property var testFail: Qt.closestAngle(0)
    property var test5: Qt.closestAngle(0,1,2)
    property var test6: Qt.closestAngle(123.45465768,1.11)
    property var test7: Qt.closestAngle(-3.1415,1.11)
}

