//![0]
var component;
var sprite;

function finishCreation() {
    if (component.isReady) {
        sprite = component.createObject();
        if (sprite == null) {
            // Error Handling
        } else {
            sprite.parent = appWindow;
            sprite.x = 100;
            sprite.y = 100;
            // ...
        }
    } else if (component.isError()) {
        // Error Handling
        console.log("Error loading component:", component.errorsString());
    }
}
//![0]

//![1]
function createSpriteObjects() {
//![1]

    //![2]
    component = Qt.createComponent("Sprite.qml");
    if (component.isReady)
        finishCreation();
    else
        component.statusChanged.connect(finishCreation);
    //![2]

    //![3]
    component = Qt.createComponent("Sprite.qml");
    sprite = component.createObject();

    if (sprite == null) {
        // Error Handling
        console.log("Error loading component:", component.errorsString());
    } else {
        sprite.parent = appWindow;
        sprite.x = 100;
        sprite.y = 100;
        // ...
    }
    //![3]

//![4]
}
//![4]

//![5]
createSpriteObjects();
//![5]

