import Qt 4.7

Item {
    width: 400
    height: 200

    ListModel {
        id: myModel
        ListElement { display: "One" }
        ListElement { display: "Two" }
        ListElement { display: "Three" }
        ListElement { display: "Four" }
        ListElement { display: "Five" }
        ListElement { display: "Six" }
        ListElement { display: "Seven" }
        ListElement { display: "Eight" }
    }
    //![0]
    VisualDataModel {
        id: visualModel
        delegate: Delegate {}
        model: myModel
    }

    ListView {
        width: 200; height:200
        model: visualModel.parts.list
    }
    GridView {
        x: 200; width: 200; height:200
        cellHeight: 50
        model: visualModel.parts.grid
    }
    //![0]
}
