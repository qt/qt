/* This script file handles the game logic */
//Note that X/Y referred to here are in game coordinates
var maxColumn = 10;//Nums are for gameCanvas.tileSize 40
var maxRow = 15;
var maxIndex = maxColumn*maxRow;
var board = new Array(maxIndex);
var tileSrc = "content/BoomBlock.qml";
//var scoresURL = "http://qtfx-nokia.trolltech.com.au/samegame/scores.php";
var scoresURL = "";
var timer;
var component = createComponent(tileSrc);

//Index function used instead of a 2D array
function index(column,row) {
    return column + (row * maxColumn);
}

function timeStr(msecs) {
    var secs = Math.floor(msecs/1000);
    var m = Math.floor(secs/60);
    var ret = "" + m + "m " + (secs%60) + "s";
    return ret;
}

function initBoard()
{
    for(var i = 0; i<maxIndex; i++){
        //Delete old blocks
        if(board[i] != null)
            board[i].destroy();
    }

    //Calculate board size
    maxColumn = Math.floor(gameCanvas.width/gameCanvas.tileSize);
    maxRow = Math.floor(gameCanvas.height/gameCanvas.tileSize);
    maxIndex = maxRow*maxColumn;

    //Close dialogs
    nameInputDialog.forceClose();
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

var fillFound;//Set after a floodFill call to the number of tiles found
var floodBoard;//Set to 1 if the floodFill reaches off that node
//NOTE: Be careful with vars named x,y, as the calling object's x,y are still in scope
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
    board[index(column,row)].dying = true;
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
                    obj.targetY += fallDist * gameCanvas.tileSize;
                    board[index(column,row+fallDist)] = obj;
                    board[index(column,row)] = null;
                }
            }
        }
    }
    //Fall to the left
    fallDist = 0;
    for(column=0; column<maxColumn; column++){
        if(board[index(column, maxRow - 1)] == null){
            fallDist += 1;
        }else{
            if(fallDist > 0){
                for(row=0; row<maxRow; row++){
                    obj = board[index(column,row)];
                    if(obj == null)
                        continue;
                    obj.targetX -= fallDist * gameCanvas.tileSize;
                    board[index(column-fallDist,row)] = obj;
                    board[index(column,row)] = null;
                }
            }
        }
    }
}

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
    if(deservesBonus || !(floodMoveCheck(0,maxRow-1, -1))){
        timer = new Date() - timer;
        nameInputDialog.show("You won! Please enter your name:                 ");
        //dialog.show("Game Over. Your score is " + gameCanvas.score);
    }
}

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

function createBlock(column,row){
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
        dynamicObject.targetX = column*gameCanvas.tileSize;
        dynamicObject.targetY = row*gameCanvas.tileSize;
        dynamicObject.width = gameCanvas.tileSize;
        dynamicObject.height = gameCanvas.tileSize;
        dynamicObject.spawned = true;
        board[index(column,row)] = dynamicObject;
    }else{//isError or isLoading
        print("error loading block component");
        print(component.errorsString());
        return false;
    }
    return true;
}

//![2]
function saveHighScore(name) {
    if(scoresURL!="")
        sendHighScore(name);
    //OfflineStorage
    var db = openDatabaseSync("SameGameScores", "1.0", "Local SameGame High Scores",100);
    var dataStr = "INSERT INTO Scores VALUES(?, ?, ?, ?)";
    var data = [name, gameCanvas.score, maxColumn+"x"+maxRow ,Math.floor(timer/1000)];
    db.transaction(
        function(tx) {
            tx.executeSql('CREATE TABLE IF NOT EXISTS Scores(name TEXT, score NUMBER, gridSize TEXT, time NUMBER)');
            tx.executeSql(dataStr, data);

            var rs = tx.executeSql('SELECT * FROM Scores WHERE gridSize = "12x17" ORDER BY score desc LIMIT 10');
            var r = "\nHIGH SCORES for a standard sized grid\n\n"
            for(var i = 0; i < rs.rows.length; i++){
                r += (i+1)+". " + rs.rows.item(i).name +' got '
                    + rs.rows.item(i).score + ' points in '
                    + rs.rows.item(i).time + ' seconds.\n';
            }
            dialog.show(r);
        }
    );
}
//![2]

//![1]
function sendHighScore(name) {
    var postman = new XMLHttpRequest()
    var postData = "name="+name+"&score="+gameCanvas.score
        +"&gridSize="+maxColumn+"x"+maxRow +"&time="+Math.floor(timer/1000);
    postman.open("POST", scoresURL, true);
    postman.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    postman.onreadystatechange = function() { 
        if (postman.readyState == postman.DONE) {
            dialog.show("Your score has been uploaded.");
        }
    }
    postman.send(postData);
}
//![1]
