import Qt 4.6

Image {
    id: sun
    property bool created: false
    property string image: "../images/sun.png"
    onCreatedChanged: if(created){window.activeSuns++;}else{window.activeSuns--;}

    source: image; 
    z: 1

    //x and y get set when instantiated
    //head offscreen
    y: NumberAnimation {
        to: parent.height;
        duration: 10000;
        running: created
    }

    states: State {
        name: "OffScreen"; when: created && y > window.height / 2;//Below the ground
        StateChangeScript { script: { sun.created = false; sun.destroy() } }
    }
}
