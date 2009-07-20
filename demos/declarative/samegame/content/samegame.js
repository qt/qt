/* This script file handles the game logic */

var maxX = 10;//Nums are for tileSize 40
var maxY = 15;
var tileSize = 40;
var maxIndex = maxX*maxY;
var board = new Array(maxIndex);
var tileSrc = "content/BoomBlock.qml";
var swapped = false;

var compSrc;
var component;

function swapTileSrc(){
    if(swapped)
        return;
    if(tileSrc == "content/SpinBlock.qml"){
        tileSrc = "content/BoomBlock.qml";
    }else{
        tileSrc = "content/SpinBlock.qml";
    }
    swapped = true;
}

function index(xIdx,yIdx){
    return xIdx + (yIdx * maxX);
}

function initBoard()
{
    for(i = 0; i<maxIndex; i++){
        //Delete old blocks
        if(board[i] != null)
            board[i].destroy();
    }

    maxX = Math.floor(gameCanvas.width/tileSize);
    maxY = Math.floor(gameCanvas.height/tileSize);
    maxIndex = maxX*maxY;
    board = new Array(maxIndex);
    gameCanvas.score = 0;

    for(xIdx=0; xIdx<maxX; xIdx++){
        for(yIdx=0; yIdx<maxY; yIdx++){
            board[index(xIdx,yIdx)] = null;
            startCreatingBlock(xIdx,yIdx);
        }
    }
    //TODO: a flag that handleMouse uses to ignore clicks when we're loading
}

var removed;
var floodBoard;
function handleClick(x,y)
{
    //NOTE: Be careful with vars named x,y - they can set to the calling object?
    xIdx = Math.floor(x/tileSize);
    yIdx = Math.floor(y/tileSize);
    if(xIdx >= maxX || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return;
    if(board[index(xIdx, yIdx)] == null)
        return;
    removed = 0;
    floodBoard = new Array(maxIndex);
    floodKill(xIdx,yIdx, -1);
    if(removed <= 0)
        return;
    gameCanvas.score += (removed - 1) * (removed - 1);
    shuffleDown();
    victoryCheck();
}

function floodKill(xIdx,yIdx,type)
{
    if(xIdx >= maxX || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return;
    if(floodBoard[index(xIdx, yIdx)] == 1)
        return;
    if(board[index(xIdx, yIdx)] == null)
        return;
    var first = false;
    if(type == -1){
        type = board[index(xIdx,yIdx)].type;
        first = true;
    }else{
        if(type != board[index(xIdx,yIdx)].type)
            return;
    }
    floodBoard[index(xIdx, yIdx)] = 1;
    floodKill(xIdx+1,yIdx,type);
    floodKill(xIdx-1,yIdx,type);
    floodKill(xIdx,yIdx+1,type);
    floodKill(xIdx,yIdx-1,type);
    if(first==true && removed == 0){
        //TODO: Provide a way to inform the delegate
        return;//Can't remove single tiles
    }
    board[index(xIdx,yIdx)].dying = true;
    board[index(xIdx,yIdx)] = null;//They'll have to destroy themselves(can we do that?)
    removed += 1;
}

function shuffleDown()
{
    //Fall down
    for(xIdx=0; xIdx<maxX; xIdx++){
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
    //Fall to the left
    fallDist = 0;
    for(xIdx=0; xIdx<maxX; xIdx++){
        if(board[index(xIdx, maxY - 1)] == null){
            fallDist += 1;
        }else{
            if(fallDist > 0){
                for(yIdx=0; yIdx<maxY; yIdx++){
                    obj = board[index(xIdx,yIdx)];
                    if(obj == null)
                        continue;
                    obj.targetX -= fallDist * tileSize;
                    board[index(xIdx-fallDist,yIdx)] = obj;
                    board[index(xIdx,yIdx)] = null;
                }
            }
        }
    }
}

function victoryCheck()
{
    //awards bonuses
    deservesBonus = true;
    for(xIdx=maxX-1; xIdx>=0; xIdx--)
        if(board[index(xIdx, maxY - 1)] != null)
            deservesBonus = false;
    if(deservesBonus)
        gameCanvas.score += 500;
    //Checks for game over
    if(deservesBonus || noMoreMoves()){
        dialog.text = "Game Over. Your score is " + gameCanvas.score;
        dialog.opacity = 1;
    }
}

function noMoreMoves()
{
    return !floodMoveCheck(0, maxY-1, -1);
}

function floodMoveCheck(xIdx, yIdx, type)
{
    if(xIdx >= maxX || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return false;
    if(board[index(xIdx, yIdx)] == null)
        return false;
    myType = board[index(xIdx, yIdx)].type;
    if(type == myType)
        return true;
    return floodMoveCheck(xIdx + 1, yIdx, myType) || floodMoveCheck(xIdx, yIdx - 1, myType);
}

//Need a simpler method of doing this?
var waitStack = new Array(maxIndex);
var waitTop = -1;

function finishCreatingBlock(xIdx,yIdx){
    //TODO: Doc that the 'x', 'y' that were here are hidden properties from the calling QFxItem
    if(component.isReady()){
        if(xIdx == undefined){
            //Called without arguments, create a previously stored (xIdx,yIdx)
            if(waitTop == -1)
                return;//Don't have a previously stored (xIdx,yIdx)
            xIdx = waitStack[waitTop] % maxX;
            yIdx = Math.floor(waitStack[waitTop] / maxX);
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
        dynamicObject.x = xIdx*tileSize;
        dynamicObject.targetX = xIdx*tileSize;
        dynamicObject.targetY = yIdx*tileSize;
        dynamicObject.width = tileSize;
        dynamicObject.height = tileSize;
        dynamicObject.spawned = true;
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
