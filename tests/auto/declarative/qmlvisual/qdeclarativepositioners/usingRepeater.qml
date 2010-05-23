import Qt 4.7

Item{
    width: 200; height: 600
    Column{
        Rectangle{color:"Red"; width:40; height:40;}
        Repeater{
            id: rep
            model: 3
            delegate: Component{Rectangle{color:"Green"; width:40; height:40; radius: 20;}}
        }
        Rectangle{color:"Blue"; width:40; height:40;}
    }
    Timer{ interval: 500; running: true; onTriggered: rep.model=6;}
}
