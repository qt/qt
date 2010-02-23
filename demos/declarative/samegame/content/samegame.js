/* This script file handles the game logic */
//Note that X/Y referred to here are in game coordinates
var maxX = 10;//Nums are for gameCanvas.tileSize 40
var maxY = 15;
var maxIndex = maxX*maxY;
var board = new Array(maxIndex);
var tileSrc = "content/BoomBlock.qml";
var scoresURL = "http://qtfx-nokia.trolltech.com.au/samegame/scores.php";
var scoresURL = "";
var timer;
var component = createComponent(tileSrc);

//Index function used instead of a 2D array
function index(xIdx,yIdx) {
    return xIdx + (yIdx * maxX);
}

function timeStr(msecs) {
    var secs = Math.floor(msecs/1000);
    var m = Math.floor(secs/60);
    var ret = "" + m + "m " + (secs%60) + "s";
    return ret;
}

function getTileSize()
{
    return tileSize;
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
    scoreName.forceClose();
    dialog.forceClose();

    var a = new Date();
    //Initialize Board
    board = new Array(maxIndex);
    gameCanvas.score = 0;
    for(var xIdx=0; xIdx<maxX; xIdx++){
        for(var yIdx=0; yIdx<maxY; yIdx++){
            board[index(xIdx,yIdx)] = null;
            createBlock(xIdx,yIdx);
        }
    }
    timer = new Date();

    //print(timer.valueOf() - a.valueOf());
}

var fillFound;//Set after a floodFill call to the number of tiles found
var floodBoard;//Set to 1 if the floodFill reaches off that node
//NOTE: Be careful with vars named x,y, as the calling object's x,y are still in scope
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
    for(var xIdx=0; xIdx<maxX; xIdx++){
        var fallDist = 0;
        for(var yIdx=maxY-1; yIdx>=0; yIdx--){
            if(board[index(xIdx,yIdx)] == null){
                fallDist += 1;
            }else{
                if(fallDist > 0){
                    var obj = board[index(xIdx,yIdx)];
                    obj.targetY += fallDist * gameCanvas.tileSize;
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
                    obj.targetX -= fallDist * gameCanvas.tileSize;
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
    var deservesBonus = true;
    for(var xIdx=maxX-1; xIdx>=0; xIdx--)
        if(board[index(xIdx, maxY - 1)] != null)
            deservesBonus = false;
    if(deservesBonus)
        gameCanvas.score += 500;
    //Checks for game over
    if(deservesBonus || !(floodMoveCheck(0,maxY-1, -1))){
        timer = new Date() - timer;
        //scoreName.show("You won! Please enter your name:                 ");
        scoreName.show("You won! Please enter your name:                 ");
        scoreName.initialWidth = scoreName.text.width + 20;
        scoreName.width = scoreName.initialWidth;
        scoreName.text.opacity = 0;//Just a spacer
        //dialog.show("Game Over. Your score is " + gameCanvas.score);
    }
}

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

function createBlock(xIdx,yIdx){
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
        dynamicObject.targetX = xIdx*gameCanvas.tileSize;
        dynamicObject.targetY = yIdx*gameCanvas.tileSize;
        dynamicObject.width = gameCanvas.tileSize;
        dynamicObject.height = gameCanvas.tileSize;
        dynamicObject.spawned = true;
        board[index(xIdx,yIdx)] = dynamicObject;
    }else{//isError or isLoading
        print("error loading block component");
        print(component.errorsString());
        return false;
    }
    return true;
}

function saveHighScore(name) {
    if(scoresURL!="")
        sendHighScore(name);
    //OfflineStorage
    var db = openDatabaseSync("SameGameScores", "1.0", "Local SameGame High Scores",100);
    var dataStr = "INSERT INTO Scores VALUES(?, ?, ?, ?)";
    var data = [name, gameCanvas.score, maxX+"x"+maxY ,Math.floor(timer/1000)];
    db.transaction(
        function(tx) {
            tx.executeSql('CREATE TABLE IF NOT EXISTS Scores(name TEXT, score NUMBER, gridSize TEXT, time NUMBER)');
            tx.executeSql(dataStr, data);

            //Only show results for the current grid size
            var rs = tx.executeSql('SELECT * FROM Scores WHERE gridSize = "'+maxX+"x"+maxY+'" ORDER BY score desc LIMIT 10');
            var r = "\nHIGH SCORES for this grid size\n\n"
            for(var i = 0; i < rs.rows.length; i++){
                r += (i+1)+". " + rs.rows.item(i).name +' got '
                    + rs.rows.item(i).score + ' points in '
                    + rs.rows.item(i).time + ' seconds.\n';
            }
            dialog.show(r);
        }
    );
}

function sendHighScore(name) {
    var postman = new XMLHttpRequest()
    var postData = "name="+name+"&score="+gameCanvas.score
        +"&gridSize="+maxX+"x"+maxY +"&time="+Math.floor(timer/1000);
    postman.open("POST", scoresURL, true);
    postman.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    postman.onreadystatechange = function() { 
        if (postman.readyState == postman.DONE) {
            dialog.show("Your score has been uploaded.");
        }
    }
    postman.send(postData);
}
