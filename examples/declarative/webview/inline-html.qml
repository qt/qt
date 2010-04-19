import Qt 4.7
import org.webkit 1.0

// Inline HTML with loose formatting can be
// set on the html property.
WebView {
    html:"\
        <body bgcolor=white>\
        <table border=1>\
            <tr><th><th>One<th>Two<th>Three\
            <tr><th>1<td>X<td>1<td>X\
            <tr><th>2<td>0<td>X<td>0\
            <tr><th>3<td>X<td>1<td>X\
        </table>"
}
