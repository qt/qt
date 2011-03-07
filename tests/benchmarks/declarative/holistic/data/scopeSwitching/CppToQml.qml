import QtQuick 1.0

Item {
    id: root
    property int arbitrary: 10
    property int dependent: arbitrary + 5
}
