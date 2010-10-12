import QtQuick 1.0

Item{
    height: 50; width: 200
    Column{
        //Not an exhaustive echo mode test, that's in QLineEdit (since the functionality is in QLineControl)
        TextInput{ id: main; focus: true; echoMode: TextInput.Password; passwordCharacter: '.' }
        Text{ text: main.text }
    }
}
