import Qt 4.7
import org.webkit 1.0

WebView {
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
