//![0]
var component;
var sprite;

function finishCreation() {
    if (component.status == Component.Ready) {
        sprite = component.createObject(appWindow);
        if (sprite == null) {
            // Error Handling
        } else {
            sprite.x = 100;
            sprite.y = 100;
            // ...
        }
    } else if (component.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString());
    }
}
//![0]

function createSpriteObjects() {

//![1]
component = Qt.createComponent("Sprite.qml");
if (component.status == Component.Ready)
    finishCreation();
else
    component.statusChanged.connect(finishCreation);
//![1]

//![2]
component = Qt.createComponent("Sprite.qml");
sprite = component.createObject(appWindow);

if (sprite == null) {
    // Error Handling
    console.log("Error loading component:", component.errorString());
} else {
    sprite.x = 100;
    sprite.y = 100;
    // ...
}
//![2]

}

