function intersects(item, x, y, e){
    return x+e >= item.x && x-e <= item.x + item.width && y+e >= item.y && y-e <= item.y + item.height;
}

function direction(x1, y1, x2, y2){
    return Math.atan2(y2-y1, x2-x1) * (180/Math.PI);
}

