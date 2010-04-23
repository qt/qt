import Qt 4.7

Item {
    width: 400; height: 400;
    property int step: 0
    function tick()
    {
        step++;
        if(step == 1){
            row1.destroy(); //Not dynamically created, so is this valid?
        }else if(step == 2){
            r2a.destroy();
        }else if(step == 3){
            r2b.destroy();
        }else if(step == 4){
            r2c.destroy();
        }else if(step == 5){
            r3a.parent = row2;
        }else if(step == 6){
            r3b.parent = row2;
        }else if(step == 7){
            r3c.parent = row2;
        }else if(step == 8){
            row3.destroy();
        }else{
            repeater.model++;
        }
    }

    //Tests base positioner functionality, so just using row
    Row{
        id: row1
        Rectangle{id: r1a; width:20; height:20; color: "red"}
        Rectangle{id: r1b; width:20; height:20; color: "green"}
        Rectangle{id: r1c; width:20; height:20; color: "blue"}
    }
    Row{
        y:20
        id: row2
        move: Transition{NumberAnimation{properties:"x"}}
        Repeater{ 
            id: repeater 
            model: 0; 
            delegate: Component{ Rectangle { color: "yellow"; width:20; height:20;}}
        }
        Rectangle{id: r2a; width:20; height:20; color: "red"}
        Rectangle{id: r2b; width:20; height:20; color: "green"}
        Rectangle{id: r2c; width:20; height:20; color: "blue"}
    }
    Row{
        move: Transition{NumberAnimation{properties:"x"}}
        y:40
        id: row3
        Rectangle{id: r3a; width:20; height:20; color: "red"}
        Rectangle{id: r3b; width:20; height:20; color: "green"}
        Rectangle{id: r3c; width:20; height:20; color: "blue"}
    }
    Timer{
        interval: 500;
        running: true;
        repeat: true;
        onTriggered: tick();
    }
}

