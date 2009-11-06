import Qt 4.6

WebView {
    url: "test.html"
    width: 200
    height: 200
    settings.standardFontFamily: font.name
    // WebKit doesn't seem to honour any other FontFamily settings
    FontLoader { id: font; source: "FreeMono.ttf" }
}
