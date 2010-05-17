import Qt 4.7

Image {
    id: sun

    property bool created: false
    property string image: "../images/sun.png"

    source: image

    // once item is created, start moving offscreen
    NumberAnimation on y {
        to: window.height / 2
        running: created
        onRunningChanged: {
            if (running)
                duration = (window.height - sun.y) * 10;
            else
                state = "OffScreen"
        }
    }

    states: State {
        name: "OffScreen"
        StateChangeScript {
            script: { sun.created = false; sun.destroy() }
        }
    }

    onCreatedChanged: {
        if (created) {
            sun.z = 1;    // above the sky but below the ground layer 
            window.activeSuns++;
        } else {
            window.activeSuns--;
        }
    }
}
