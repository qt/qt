var itemComponent = null;
var draggedItem = null;
var startingMouse;
var startingZ;
//Until QT-2385 is resolved we need to convert to scene coordinates manually
var xOffset;
var yOffset;
function setSceneOffset()
{
    xOffset = 0;
    yOffset = 0;
    var p = itemButton;
    while(p != window){
        xOffset += p.x;
        yOffset += p.y;
        p = p.parent;
    }
}

function startDrag(mouse)
{
    setSceneOffset();
    startingMouse = { x: mouse.x, y: mouse.y }
    loadComponent();
}

//Creation is split into two functions due to an asyncronous wait while
//possible external files are loaded.

function loadComponent() {
    if (itemComponent != null) //Already loaded the component
        createItem();

    itemComponent = Qt.createComponent(itemButton.file);
    //console.log(itemButton.file)
    if(itemComponent.isLoading){
        component.statusChanged.connect(finishCreation);
    }else{//Depending on the content, it can be ready or error immediately
        createItem();
    }
}

function createItem() {
    if (itemComponent.isReady && draggedItem == null) {
        draggedItem = itemComponent.createObject();
        draggedItem.parent = window;
        draggedItem.image = itemButton.image;
        draggedItem.x = xOffset;
        draggedItem.y = yOffset;
        startingZ = draggedItem.z;
        draggedItem.z = 4;//On top
    } else if (itemComponent.isError) {
        draggedItem = null;
        console.log("error creating component");
        console.log(component.errorsString());
    }
}

function moveDrag(mouse)
{
    if(draggedItem == null)
        return;

    draggedItem.x = mouse.x + xOffset - startingMouse.x;
    draggedItem.y = mouse.y + yOffset - startingMouse.y;
}

function endDrag(mouse)
{
    if(draggedItem == null)
        return;

    if(draggedItem.x + draggedItem.width > toolbox.x){ //Don't drop it in the toolbox
        draggedItem.destroy();
        draggedItem = null;
    }else{
        draggedItem.z = startingZ;
        draggedItem.created = true;
        draggedItem = null;
    }
}

