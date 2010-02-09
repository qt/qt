import Qt 4.6

Item {
    property var intInput: intInput
    property var dblInput: dblInput
    property var strInput: strInput

    width: 800; height: 600;

    Column{
        TextInput { id: intInput;
            validator: QIntValidator{top: 11; bottom: 2}
        }
        TextInput { id: dblInput;
            validator: QDoubleValidator{top: 12.12; bottom: 2.93; decimals: 2; notation: QDoubleValidator.StandardNotation}
        }
        TextInput { id: strInput;
            //Requires QTBUG-8025 to be implemented first
            //validator: QRegExpValidator { regExp: /[a-zA-z]{2,4}/;}
        }
    }
        
}
