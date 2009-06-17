var sprite = null;
var component;
var started = false;
function make(p) {
    return evalQml('Rect { color: "lightsteelblue"; width: 100;'
            + 'height: 100; id: newRect}','DynPart.qml');
}

function death() {
    if(!(sprite==null)){
        sprite.destroy();
        sprite = null;
    }
}

function spawn() {//Like create, but assumes instant readyness
    if(sprite!=null)//Already made
        return null;
    component = createComponent("dynamic.qml");
    sprite = component.createObject();
    if(sprite == null){
        print("err");
    }else{
        sprite.parent = targetItem;
        return sprite;
    }
    return null;
}

function finishCreation(){
    if(component.isReady()){
        sprite = component.createObject();
        sprite.parent = targetItem;
    }else if(component.isError()){
        sprite = null;
    }
}

function create(){
    if(started!=false){
        finishCreation();//Remakes if destroyed
        return sprite;
    }
    started = true;
    component = createComponent("dynamic.qml");
    finishCreation();
    if(sprite != null){
        return sprite;
    }
    component.statusChanged.connect(finishCreation);
    return null;
}
