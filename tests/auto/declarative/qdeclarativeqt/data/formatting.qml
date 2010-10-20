import QtQuick 1.0

QtObject {
    property date date1: "2008-12-24"
    property string test1: Qt.formatDate(date1)
    property string test2: Qt.formatDate(date1, Qt.DefaultLocaleLongDate)
    property string test3: Qt.formatDate(date1, "ddd MMMM d yy")

    property variant time1: new Date(0,0,0,14,15,38,200)
    property string test4: Qt.formatTime(time1)
    property string test5: Qt.formatTime(time1, Qt.DefaultLocaleLongDate)
    property string test6: Qt.formatTime(time1, "H:m:s a")
    property string test7: Qt.formatTime(time1, "hh:mm:ss.zzz")

    property variant dateTime1: new Date(1978,2,4,9,13,54)
    property string test8: Qt.formatDateTime(dateTime1)
    property string test9: Qt.formatDateTime(dateTime1, Qt.DefaultLocaleLongDate)
    property string test10: Qt.formatDateTime(dateTime1, "M/d/yy H:m:s a")

    // Error cases
    property string test11: Qt.formatDate()
    property string test12: Qt.formatDate(new Date, new Object)

    property string test13: Qt.formatTime()
    property string test14: Qt.formatTime(new Date, new Object)

    property string test15: Qt.formatDateTime()
    property string test16: Qt.formatDateTime(new Date, new Object)
}
