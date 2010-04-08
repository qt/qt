/* This script file handles the game logic */
//Note that X/Y referred to here are in game coordinates
var maxColumn = 10;//Nums are for tileSize 40
var maxRow = 15;
var maxIndex = maxColumn*maxRow;
var board = new Array(maxIndex);
var tileSrc = "Block.qml";
var component;

//Index function used instead of a 2D array
function index(column,row) {
    return column + (row * maxColumn);
}

function initBoard()
{
    //Calculate board size
    maxColumn = Math.floor(gameCanvas.width/gameCanvas.tileSize);
    maxRow = Math.floor(gameCanvas.height/gameCanvas.tileSize);
    maxIndex = maxRow*maxColumn;

    //Close dialogs
    dialog.forceClose();

    //Initialize Board
    board = new Array(maxIndex);
    gameCanvas.score = 0;
    for(var column=0; column<maxColumn; column++){
        for(var row=0; row<maxRow; row++){
            board[index(column,row)] = null;
            createBlock(column,row);
        }
    }
}

function createBlock(column,row){
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
        dynamicObject.x = column*gameCanvas.tileSize;
        dynamicObject.y = row*gameCanvas.tileSize;
        dynamicObject.width = gameCanvas.tileSize;
        dynamicObject.height = gameCanvas.tileSize;
        board[index(column,row)] = dynamicObject;
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
    var column = Math.floor(x/gameCanvas.tileSize);
    var row = Math.floor(y/gameCanvas.tileSize);
    if(column >= maxColumn || column < 0 || row >= maxRow || row < 0)
        return;
    if(board[index(column, row)] == null)
        return;
    //If it's a valid tile, remove it and all connected (does nothing if it's not connected)
    floodFill(column,row, -1);
    if(fillFound <= 0)
        return;
    gameCanvas.score += (fillFound - 1) * (fillFound - 1);
    shuffleDown();
    victoryCheck();
}
//![1]

function floodFill(column,row,type)
{
    if(board[index(column, row)] == null)
        return;
    var first = false;
    if(type == -1){
        first = true;
        type = board[index(column,row)].type;
        
        //Flood fill initialization
        fillFound = 0;
        floodBoard = new Array(maxIndex);
    }
    if(column >= maxColumn || column < 0 || row >= maxRow || row < 0)
        return;
    if(floodBoard[index(column, row)] == 1 || (!first && type != board[index(column,row)].type))
        return;
    floodBoard[index(column, row)] = 1;
    floodFill(column+1,row,type);
    floodFill(column-1,row,type);
    floodFill(column,row+1,type);
    floodFill(column,row-1,type);
    if(first==true && fillFound == 0)
        return;//Can't remove single tiles
    board[index(column,row)].opacity = 0;
    board[index(column,row)] = null;
    fillFound += 1;
}

function shuffleDown()
{
    //Fall down
    for(var column=0; column<maxColumn; column++){
        var fallDist = 0;
        for(var row=maxRow-1; row>=0; row--){
            if(board[index(column,row)] == null){
                fallDist += 1;
            }else{
                if(fallDist > 0){
                    var obj = board[index(column,row)];
                    obj.y += fallDist * gameCanvas.tileSize;
                    board[index(column,row+fallDist)] = obj;
                    board[index(column,row)] = null;
                }
            }
        }
    }
    //Fall to the left
    var fallDist = 0;
    for(var column=0; column<maxColumn; column++){
        if(board[index(column, maxRow - 1)] == null){
            fallDist += 1;
        }else{
            if(fallDist > 0){
                for(var row=0; row<maxRow; row++){
                    var obj = board[index(column,row)];
                    if(obj == null)
                        continue;
                    obj.x -= fallDist * gameCanvas.tileSize;
                    board[index(column-fallDist,row)] = obj;
                    board[index(column,row)] = null;
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
    for(var column=maxColumn-1; column>=0; column--)
        if(board[index(column, maxRow - 1)] != null)
            deservesBonus = false;
    if(deservesBonus)
        gameCanvas.score += 500;
    //Checks for game over
    if(deservesBonus || !(floodMoveCheck(0,maxRow-1, -1)))
        dialog.show("Game Over. Your score is " + gameCanvas.score);
}
//![2]

//only floods up and right, to see if it can find adjacent same-typed tiles 
function floodMoveCheck(column, row, type)
{
    if(column >= maxColumn || column < 0 || row >= maxRow || row < 0)
        return false;
    if(board[index(column, row)] == null)
        return false;
    var myType = board[index(column, row)].type;
    if(type == myType)
        return true;
    return floodMoveCheck(column + 1, row, myType) ||
           floodMoveCheck(column, row - 1, board[index(column,row)].type);
}
