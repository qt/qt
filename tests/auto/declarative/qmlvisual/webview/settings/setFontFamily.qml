import QtQuick 1.0
import QtWebKit 1.0

WebView {
    property string skip: "WebView tests not counting until resources allocated to WebView maintenance"
    url: "test.html"
    width: 300
    height: 300
    settings.standardFontFamily: font.name
    // WebKit doesn't seem to honour any other FontFamily settings
    FontLoader { id: font; source: "tarzeau_ocr_a.ttf" }
}
