/* This script file handles the game logic */

var maxIdx = 18/2;//Nums are for tileSize 20 (desired tile size but too slow)
var maxY = 30/2;
var tileSize = 40;
var maxIndex = maxIdx*maxY;
var board = new Array(maxIndex);
var tileSrc = "content/FastBlock.qml";
var backSrc = "content/pics/background.png";
var swapped = false;

var compSrc;
var component;

function swapTileSrc(){
    if(swapped)
        return;
    if(tileSrc == "content/FastBlock.qml"){
        tileSrc = "content/BoomBlock.qml";
        backSrc = "content/pics/background.png";
    }else{
        backSrc = "content/pics/qtlogo.png";
        tileSrc = "content/FastBlock.qml";
    }
    swapped = true;
}

function index(xIdx,yIdx){
    return xIdx + (yIdx * maxIdx);
}

function initBoard()
{
    background.source = backSrc;
    swapped = false;
    gameCanvas.score = 0;
    for(xIdx=0; xIdx<maxIdx; xIdx++){
        for(yIdx=0; yIdx<maxY; yIdx++){
            if(board[index(xIdx,yIdx)] != null){
                //Delete old blocks
                board[index(xIdx,yIdx)].destroy();
            }
            board[index(xIdx,yIdx)] = null;
            startCreatingBlock(xIdx,yIdx);
        }
    }
    //TODO: a flag that handleMouse uses to ignore clicks when we're loading
}

var removed;
function handleClick(x,y)
{
    //NOTE: Be careful with vars named x,y - they can set to the calling object?
    xIdx = Math.floor(x/tileSize);
    yIdx = Math.floor(y/tileSize);
    if(xIdx >= maxIdx || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return;
    if(board[index(xIdx, yIdx)] == null)
        return;
    removed = 0;
    floodKill(xIdx,yIdx, -1);
    gameCanvas.score += removed * removed;
    shuffleDown();
}

function floodKill(xIdx,yIdx,type)
{
    if(xIdx >= maxIdx || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return;
    if(board[index(xIdx, yIdx)] == null)
        return;
    if(type == -1){
        type = board[index(xIdx,yIdx)].type;
    }else{
        if(type != board[index(xIdx,yIdx)].type)
            return;
    }
    board[index(xIdx,yIdx)].dying = true;
    board[index(xIdx,yIdx)] = null;//They'll have to destroy themselves(can we do that?)
    removed += 1;
    floodKill(xIdx+1,yIdx,type);
    floodKill(xIdx-1,yIdx,type);
    floodKill(xIdx,yIdx+1,type);
    floodKill(xIdx,yIdx-1,type);
}

function shuffleDown()
{
    for(xIdx=0; xIdx<maxIdx; xIdx++){
        fallDist = 0;
        for(yIdx=maxY-1; yIdx>=0; yIdx--){
            if(board[index(xIdx,yIdx)] == null){
                fallDist += 1;
            }else{
                if(fallDist > 0){
                    obj = board[index(xIdx,yIdx)];
                    obj.targetY += fallDist * tileSize;
                    board[index(xIdx,yIdx+fallDist)] = obj;
                    board[index(xIdx,yIdx)] = null;
                }
            }
        }
    }
}

//Need a simpler method of doing this?
var waitStack = new Array(maxIndex);
var waitTop = -1;

function finishCreatingBlock(xIdx,yIdx){
    //TODO: Doc that the 'xIdx', 'yIdx' here are hidden properties from the calling QFxItem
    if(component.isReady()){
        if(xIdx == undefined){
            //Called without arguments, create a previously stored (xIdx,yIdx)
            if(waitTop == -1)
                return;//Don't have a previously stored (xIdx,yIdx)
            xIdx = waitStack[waitTop] % maxIdx;
            yIdx = Math.floor(waitStack[waitTop] / maxIdx);
            waitTop -= 1;
        }
        dynamicObject = component.createObject();
        if(dynamicObject == null){
            print("error creating block");
            print(component.errorsString());
            return false;
        }
        dynamicObject.type = Math.floor(Math.random() * 3);
        dynamicObject.parent = gameCanvas;
        dynamicObject.targetX = xIdx*tileSize;
        dynamicObject.targetY = yIdx*tileSize;
        dynamicObject.width = tileSize;
        dynamicObject.height = tileSize;
        dynamicObject.spawning = true;
        board[index(xIdx,yIdx)] = dynamicObject;
        return true;
    }else if(component.isError()){
        print("error creating block");
        print(component.errorsString());
    }else{
        //It isn't ready, but we'll be called again when it is.
        //So store the requested (xIdx,yIdx) for later use
        waitTop += 1;
        waitStack[waitTop] = index(xIdx,yIdx);
    }
    return false;
}

function startCreatingBlock(xIdx,yIdx){
    if(component!=null && compSrc == tileSrc){
        finishCreatingBlock(xIdx,yIdx);
        return;
    }

    if(component!=null){//Changed source
        //delete component; //Does the engine handle this?
        compSrc = tileSrc;
    }
    component = createComponent(tileSrc);
    if(finishCreatingBlock(xIdx,yIdx))
        return;
    component.statusChanged.connect(finishCreatingBlock());
    return;
}
