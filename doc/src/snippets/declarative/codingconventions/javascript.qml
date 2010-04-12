import Qt 4.7

Rectangle {

//![0]
Rectangle { color: "blue"; width: parent.width / 3 }
//![0]

//![1]
Rectangle {
    color: "blue"
    width: {
        var w = parent.width / 3
        console.debug(w)
        return w
    }
}
//![1]

//![2]
function calculateWidth(object)
{
    var w = object.width / 3
    // ...
    // more javascript code
    // ...
    console.debug(w)
    return w
}

Rectangle { color: "blue"; width: calculateWidth(parent) }
//![2]
}
