/* This script file handles the game logic */
//Note that X/Y referred to here are in game coordinates
var maxX = 10;//Nums are for tileSize 40
var maxY = 15;
var tileSize = 40;
var maxIndex = maxX*maxY;
var board = new Array(maxIndex);
var tileSrc = "content/BoomBlock.qml";
var component;

//Index function used instead of a 2D array
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

    //Initialize Board
    board = new Array(maxIndex);
    gameCanvas.score = 0;
    for(xIdx=0; xIdx<maxX; xIdx++){
        for(yIdx=0; yIdx<maxY; yIdx++){
            board[index(xIdx,yIdx)] = null;
            startCreatingBlock(xIdx,yIdx);
        }
    }
}

var fillFound;//Set after a floodFill call to the number of tiles found
var floodBoard;//Set to 1 if the floodFill reaches off that node
//NOTE: Be careful with vars named x,y, as the calling object's x,y are still in scope
function handleClick(x,y)
{
    xIdx = Math.floor(x/tileSize);
    yIdx = Math.floor(y/tileSize);
    if(xIdx >= maxX || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return;
    if(board[index(xIdx, yIdx)] == null)
        return;
    //If it's a valid tile, remove it and all connected (does nothing if it's not connected)
    floodFill(xIdx,yIdx, -1);
    if(fillFound <= 0)
        return;
    gameCanvas.score += (fillFound - 1) * (fillFound - 1);
    shuffleDown();
    victoryCheck();
}

function floodFill(xIdx,yIdx,type)
{
    if(board[index(xIdx, yIdx)] == null)
        return;
    var first = false;
    if(type == -1){
        first = true;
        type = board[index(xIdx,yIdx)].type;
        
        //Flood fill initialization
        fillFound = 0;
        floodBoard = new Array(maxIndex);
    }
    if(xIdx >= maxX || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return;
    if(floodBoard[index(xIdx, yIdx)] == 1 || (!first && type != board[index(xIdx,yIdx)].type))
        return;
    floodBoard[index(xIdx, yIdx)] = 1;
    floodFill(xIdx+1,yIdx,type);
    floodFill(xIdx-1,yIdx,type);
    floodFill(xIdx,yIdx+1,type);
    floodFill(xIdx,yIdx-1,type);
    if(first==true && fillFound == 0)
        return;//Can't remove single tiles
    board[index(xIdx,yIdx)].dying = true;
    board[index(xIdx,yIdx)] = null;
    fillFound += 1;
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
    //awards bonuses for no tiles left
    deservesBonus = true;
    for(xIdx=maxX-1; xIdx>=0; xIdx--)
        if(board[index(xIdx, maxY - 1)] != null)
            deservesBonus = false;
    if(deservesBonus)
        gameCanvas.score += 500;
    //Checks for game over
    if(deservesBonus || !(floodMoveCheck(0,maxY-1, -1))){
        dialog.text = "Game Over. Your score is " + gameCanvas.score;
        dialog.opacity = 1;
    }
}

//only floods up and right, to see if it can find adjacent same-typed tiles 
function floodMoveCheck(xIdx, yIdx, type)
{
    if(xIdx >= maxX || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return false;
    if(board[index(xIdx, yIdx)] == null)
        return false;
    myType = board[index(xIdx, yIdx)].type;
    if(type == myType)
        return true;
    return floodMoveCheck(xIdx + 1, yIdx, myType) ||
           floodMoveCheck(xIdx, yIdx - 1, board[index(xIdx,yIdx)].type);
}

//If the component isn't ready, then the signal doesn't include the game x,y
//So we store any x,y sent that we couldn't create at the time, and use those
//if we are triggered by the signal.
var waitStack = new Array(maxIndex);
var waitTop = -1;

function finishCreatingBlock(xIdx,yIdx){
    if(component.isReady){
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
    }else if(component.isError){
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
    if(component!=null){
        finishCreatingBlock(xIdx,yIdx);
        return;
    }

    component = createComponent(tileSrc);
    if(finishCreatingBlock(xIdx,yIdx))
        return;
    component.statusChanged.connect(finishCreatingBlock());
    return;
}
