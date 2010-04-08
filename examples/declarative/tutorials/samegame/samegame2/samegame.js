//![0]
//Note that X/Y referred to here are in game coordinates
var maxColumn = 10;//Nums are for tileSize 40
var maxRow = 15;
var tileSize = 40;
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
    //Delete old blocks
    for(var i = 0; i<maxIndex; i++){
        if(board[i] != null)
            board[i].destroy();
    }

    //Calculate board size
    maxColumn = Math.floor(background.width/tileSize);
    maxRow = Math.floor(background.height/tileSize);
    maxIndex = maxRow*maxColumn;

    //Initialize Board
    board = new Array(maxIndex);
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
        dynamicObject.parent = background;
        dynamicObject.x = column*tileSize;
        dynamicObject.y = row*tileSize;
        dynamicObject.width = tileSize;
        dynamicObject.height = tileSize;
        board[index(column,row)] = dynamicObject;
    }else{//isError or isLoading
        print("error loading block component");
        print(component.errorsString());
        return false;
    }
    return true;
}
//![0]
