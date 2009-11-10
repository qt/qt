/* This script file handles the game logic */
//Note that X/Y referred to here are in game coordinates
var maxX = 10;//Nums are for tileSize 40
var maxY = 15;
var maxIndex = maxX*maxY;
var board = new Array(maxIndex);
var tileSrc = "Block.qml";
var component;

//Index function used instead of a 2D array
function index(xIdx,yIdx) {
    return xIdx + (yIdx * maxX);
}

function initBoard()
{
    for(var i = 0; i<maxIndex; i++){
        //Delete old blocks
        if(board[i] != null)
            board[i].destroy();
    }

    //Calculate board size
    maxX = Math.floor(gameCanvas.width/gameCanvas.tileSize);
    maxY = Math.floor(gameCanvas.height/gameCanvas.tileSize);
    maxIndex = maxY*maxX;

    //Close dialogs
    dialog.forceClose();

    //Initialize Board
    board = new Array(maxIndex);
    gameCanvas.score = 0;
    for(var xIdx=0; xIdx<maxX; xIdx++){
        for(var yIdx=0; yIdx<maxY; yIdx++){
            board[index(xIdx,yIdx)] = null;
            createBlock(xIdx,yIdx);
        }
    }
}

function createBlock(xIdx,yIdx){
    if(component==null)
        component = createComponent(tileSrc);

    // Note that we don't wait for the component to become ready. This will
    // only work if the block QML is a local file. Otherwise the component will
    // not be ready immediately. There is a statusChanged signal on the
    // component you could use if you want to wait to load remote files.
    if(component.isReady){
        var dynamicObject = component.createObject();
        if(dynamicObject == null){
            print("error creating block");
            print(component.errorsString());
            return false;
        }
        dynamicObject.type = Math.floor(Math.random() * 3);
        dynamicObject.parent = gameCanvas;
        dynamicObject.x = xIdx*gameCanvas.tileSize;
        dynamicObject.y = yIdx*gameCanvas.tileSize;
        dynamicObject.width = gameCanvas.tileSize;
        dynamicObject.height = gameCanvas.tileSize;
        board[index(xIdx,yIdx)] = dynamicObject;
    }else{//isError or isLoading
        print("error loading block component");
        print(component.errorsString());
        return false;
    }
    return true;
}

var fillFound;//Set after a floodFill call to the number of tiles found
var floodBoard;//Set to 1 if the floodFill reaches off that node
//NOTE: Be careful with vars named x,y, as the calling object's x,y are still in scope
//![1]
function handleClick(x,y)
{
    var xIdx = Math.floor(x/gameCanvas.tileSize);
    var yIdx = Math.floor(y/gameCanvas.tileSize);
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
//![1]

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
    board[index(xIdx,yIdx)].opacity = 0;
    board[index(xIdx,yIdx)] = null;
    fillFound += 1;
}

function shuffleDown()
{
    //Fall down
    for(var xIdx=0; xIdx<maxX; xIdx++){
        var fallDist = 0;
        for(var yIdx=maxY-1; yIdx>=0; yIdx--){
            if(board[index(xIdx,yIdx)] == null){
                fallDist += 1;
            }else{
                if(fallDist > 0){
                    var obj = board[index(xIdx,yIdx)];
                    obj.y += fallDist * gameCanvas.tileSize;
                    board[index(xIdx,yIdx+fallDist)] = obj;
                    board[index(xIdx,yIdx)] = null;
                }
            }
        }
    }
    //Fall to the left
    var fallDist = 0;
    for(var xIdx=0; xIdx<maxX; xIdx++){
        if(board[index(xIdx, maxY - 1)] == null){
            fallDist += 1;
        }else{
            if(fallDist > 0){
                for(var yIdx=0; yIdx<maxY; yIdx++){
                    var obj = board[index(xIdx,yIdx)];
                    if(obj == null)
                        continue;
                    obj.x -= fallDist * gameCanvas.tileSize;
                    board[index(xIdx-fallDist,yIdx)] = obj;
                    board[index(xIdx,yIdx)] = null;
                }
            }
        }
    }
}

//![2]
function victoryCheck()
{
    //awards bonuses for no tiles left
    var deservesBonus = true;
    for(var xIdx=maxX-1; xIdx>=0; xIdx--)
        if(board[index(xIdx, maxY - 1)] != null)
            deservesBonus = false;
    if(deservesBonus)
        gameCanvas.score += 500;
    //Checks for game over
    if(deservesBonus || !(floodMoveCheck(0,maxY-1, -1)))
        dialog.show("Game Over. Your score is " + gameCanvas.score);
}
//![2]

//only floods up and right, to see if it can find adjacent same-typed tiles 
function floodMoveCheck(xIdx, yIdx, type)
{
    if(xIdx >= maxX || xIdx < 0 || yIdx >= maxY || yIdx < 0)
        return false;
    if(board[index(xIdx, yIdx)] == null)
        return false;
    var myType = board[index(xIdx, yIdx)].type;
    if(type == myType)
        return true;
    return floodMoveCheck(xIdx + 1, yIdx, myType) ||
           floodMoveCheck(xIdx, yIdx - 1, board[index(xIdx,yIdx)].type);
}
