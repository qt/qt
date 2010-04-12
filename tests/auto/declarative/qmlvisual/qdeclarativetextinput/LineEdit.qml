import Qt 4.7

Item {
    id:lineedit
    property alias text: textInp.text

    width: textInp.width + 11 
    height: 13 + 11 

    Rectangle{
        color: 'lightsteelblue'
        anchors.fill: parent
    }
    clip: true
    Component.onCompleted: textInp.cursorPosition = 0;
    TextInput{
        id:textInp
        cursorDelegate: Item{
            Rectangle{
                visible: parent.parent.focus
                color: "#009BCE"
                height: 13
                width: 2
                y: 1
            }
        }
        property int leftMargin: 6 
        property int rightMargin: 6 
        x: leftMargin
        y: 5
        //Below function implements all scrolling logic
        onCursorPositionChanged: {
            if(cursorRect.x < leftMargin - textInp.x){//Cursor went off the front
                textInp.x = leftMargin - Math.max(0, cursorRect.x);
            }else if(cursorRect.x > parent.width - leftMargin - rightMargin - textInp.x){//Cusor went off the end
                textInp.x = leftMargin - Math.max(0, cursorRect.x - (parent.width - leftMargin - rightMargin));
            }
        }

        text:""
        horizontalAlignment: TextInput.AlignLeft
        font.pixelSize:15
    }
    MouseArea{
        //Implements all line edit mouse handling
        id: mainMouseArea
        anchors.fill: parent;
        function translateX(x){
            return x - textInp.x
        }
        onPressed: {
            textInp.focus = true;
            textInp.cursorPosition = textInp.xToPosition(translateX(mouse.x));
        }
        onPositionChanged: {
            textInp.moveCursorSelection(textInp.xToPosition(translateX(mouse.x)));
        }
        onReleased: {
        }
        onDoubleClicked: {
            textInp.selectionStart=0;
            textInp.selectionEnd=textInp.text.length;
        }
        z: textInp.z + 1
    }

}
