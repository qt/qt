//![0]
//Note that X/Y referred to here are in game coordinates
var maxX = 10;//Nums are for tileSize 40
var maxY = 15;
var tileSize = 40;
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
    //Calculate board size
    maxX = Math.floor(background.width/tileSize);
    maxY = Math.floor(background.height/tileSize);
    maxIndex = maxY*maxX;

    //Initialize Board
    board = new Array(maxIndex);
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
        dynamicObject.parent = background;
        dynamicObject.x = xIdx*tileSize;
        dynamicObject.y = yIdx*tileSize;
        dynamicObject.width = tileSize;
        dynamicObject.height = tileSize;
        board[index(xIdx,yIdx)] = dynamicObject;
    }else{//isError or isLoading
        print("error loading block component");
        print(component.errorsString());
        return false;
    }
    return true;
}
//![0]
