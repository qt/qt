var dynamicObject = null;
var fourthBox = null;
var component;
var started = false;
function createWithEvalQml(p) {
    return evalQml('Rect { color: "lightsteelblue"; width: 100;'
            + 'height: 100; id: newRect}','DynPart.qml');
}

function destroyDynamicObject() {
    if(!(dynamicObject==null)){
        dynamicObject.destroy();
        dynamicObject = null;
    }
}

function instantCreateWithComponent() {//Like create, but assumes instant readyness
    if(dynamicObject!=null)//Already made
        return null;
    component = createComponent("dynamic.qml");
    dynamicObject = component.createObject();
    if(dynamicObject == null){
        print("error creating component");
    }else{
        dynamicObject.parent = targetItem;
        return dynamicObject;
    }
    return null;
}

function finishCreation(){
    if(component.isReady()){
        dynamicObject = component.createObject();
        dynamicObject.parent = targetItem;
    }else if(component.isError()){
        dynamicObject = null;
        print("error creating component");
        print(component.errorsString());
    }
}

function createWithComponent(){
    if(started!=false){
        finishCreation();//Remakes if destroyed
        return dynamicObject;
    }
    started = true;
    component = createComponent("dynamic.qml");
    finishCreation();
    if(dynamicObject != null){
        return dynamicObject;
    }
    component.statusChanged.connect(finishCreation);
    return null;
}
