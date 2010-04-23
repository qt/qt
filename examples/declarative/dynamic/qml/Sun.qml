import Qt 4.7

Image {
    id: sun
    property bool created: false
    property string image: "../images/sun.png"
    onCreatedChanged: if(created){window.activeSuns++;}else{window.activeSuns--;}

    source: image; 
    z: 1

    //x and y get set when instantiated
    //head offscreen
    NumberAnimation on y {
        to: window.height / 2;
        running: created
        onRunningChanged: if (running) duration = (window.height - sun.y) * 10; else state = "OffScreen";
    }

    states: State {
        name: "OffScreen";
        StateChangeScript { script: { sun.created = false; sun.destroy() } }
    }
}
