import Qt 4.7
import "componentCreation.js" as MyModule

Rectangle {
    id: appWindow
    width: 300; height: 300

    Component.onCompleted: MyModule.createSpriteObjects();
}
