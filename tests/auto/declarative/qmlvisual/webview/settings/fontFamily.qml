import QtQuick 1.0
import QtWebKit 1.0

WebView {
    property string skip: "WebView tests not counting until resources allocated to WebView maintenance"
    id: web
    width: 200
    height: 200
    Column {
        anchors.fill: parent
        Text { text: "standard: " + web.settings.standardFontFamily }
        Text { text: "fixed: " + web.settings.fixedFontFamily }
        Text { text: "serif: " + web.settings.serifFontFamily }
        Text { text: "sansserif: " + web.settings.sansSerifFontFamily }
        Text { text: "cursive: " + web.settings.cursiveFontFamily }
        Text { text: "fantasy: " + web.settings.fantasyFontFamily }
    }
}
