import Qt 4.7
import org.webkit 1.0

WebView {
    url: "test.html"
    width: 300
    height: 300
    settings.standardFontFamily: font.name
    // WebKit doesn't seem to honour any other FontFamily settings
    FontLoader { id: font; source: "tarzeau_ocr_a.ttf" }
}
