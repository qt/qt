import Qt 4.7

Item {
    property var intInput: intInput
    property var dblInput: dblInput
    property var strInput: strInput

    width: 800; height: 600;

    Column{
        TextInput { id: intInput;
            validator: IntValidator{top: 11; bottom: 2}
        }
        TextInput { id: dblInput;
            validator: DoubleValidator{top: 12.12; bottom: 2.93; decimals: 2; notation: DoubleValidator.StandardNotation}
        }
        TextInput { id: strInput;
            validator: RegExpValidator { regExp: RegExp(/[a-zA-z]{2,4}/) }
        }
    }
        
}
