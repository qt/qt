import Qt 4.7

//![0]
import "myscript.js" as Script

Rectangle { color: "blue"; width: Script.calculateWidth(parent) }
//![0]
