import Qt 4.7

//
// The QML runtime automatically loads a translation from the i18n subdirectory of the root
// QML file, based on the system language.
//
// The files are created/updated by running:
//
//   lupdate i18n.qml -ts i18n/base.ts
//
// Translations for new languages are created by copying i18n/base.ts to i18n/qml_<lang>.ts
// The .ts files can then be edited with Linguist:
//
//   linguist i18n/qml_fr.ts
//
// The run-time translation files are then generaeted by running:
//
//   lrelease i18n/*.ts
//

Rectangle {
    width: 640; height: 480

    Column {
        anchors.fill: parent; spacing: 20

        Text {
            text: "If a translation is available for the system language (eg. French) then the string below will translated (eg. 'Bonjour'). Otherwise is will show 'Hello'."
            width: parent.width; wrapMode: Text.WordWrap
        }

        Text {
            text: qsTr("Hello")
            font.pointSize: 25; anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
