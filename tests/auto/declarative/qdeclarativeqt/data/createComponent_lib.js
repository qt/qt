.pragma library

function loadComponent() {
    var component = Qt.createComponent("createComponentData.qml");
    return component.status;
}

