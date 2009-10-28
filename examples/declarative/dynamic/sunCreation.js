var sunComponent = null;
var draggedItem = null;
var startingMouse;
//Until QT-2385 is resolved we need to convert to scene coordinates manually
var xOffset;
var yOffset;

function startDrag(mouse)
{
    xOffset = toolbox.x + toolboxPositioner.x;
    yOffset = toolbox.y + toolboxPositioner.y;
    startingMouse = mouse;
    loadComponent();
}

//Creation is split into two functions due to an asyncronous wait while
//possible external files are loaded.

function loadComponent() {
    if (sunComponent != null) //Already loaded the component
        createSun();

    sunComponent = createComponent("Sun.qml");
    if(sunComponent.isLoading){
        component.statusChanged.connect(finishCreation);
    }else{//Depending on the content, it can be ready or error immediately
        createSun();
    }
}

function createSun() {
    if (sunComponent.isReady && draggedItem == null) {
        draggedItem = sunComponent.createObject();
        draggedItem.parent = window;
        draggedItem.x = startingMouse.x + xOffset;
        draggedItem.y = startingMouse.y + yOffset;
        draggedItem.z = 4;//On top
    } else if (sunComponent.isError) {
        draggedItem = null;
        print("error creating component");
        print(component.errorsString());
    }
}

function moveDrag(mouse)
{
    if(draggedItem == null)
        return;

    draggedItem.x = mouse.x + xOffset;
    draggedItem.y = mouse.y + yOffset;
}

function endDrag(mouse)
{
    if(draggedItem == null)
        return;

    if(draggedItem.x + draggedItem.width > toolbox.x //Don't drop it in the toolbox
        || draggedItem.y > ground.y){//Don't drop it on the ground
        draggedItem.destroy();
        draggedItem = null;
    }else{
        draggedItem.z = 1;
        draggedItem.created = true;
        draggedItem = null;
    }
}

