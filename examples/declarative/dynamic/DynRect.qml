import Qt 4.6

Item {
    states: State{ name: "dying"; SetProperties{ target: newRect; opacity: 0 } }
    transitions: Transition{ 
        NumberAnimation{ properties: "opacity"; target: newRect; duration:500 } 
    }
    Rect {color: "steelblue"; width: 100; height: 100; id: newRect }
}
