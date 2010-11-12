import QtQuick 1.0
import "../shared" 1.0

Text{
    FontLoader { id: fixedFont; source: "Vera.ttf" }
    font.family: fixedFont.name
    font.pixelSize: 12
}
