
var curVal = 0;
var memory = 0;
var lastOp = "";
var timer = 0;

function disabled(op) {
    if (op == "." && curNum.text.toString().search(/\./) != -1) {
        return true;
    } else if (op == "Sqrt" &&  curNum.text.toString().search(/-/) != -1) {
        return true;
    } else {
        return false;
    }
}

function doOp(op) {
    if (disabled(op)) {
        return;
    }

    if (op.toString().length==1 && ((op >= "0" && op <= "9") || op==".") ) {
        if (curNum.text.toString().length >= 14)
            return; // No arbitrary length numbers
        if (lastOp.toString().length == 1 && ((lastOp >= "0" && lastOp <= "9") || lastOp==".") ) {
            curNum.text = curNum.text + op.toString();
        } else {
            curNum.text = op;
        }
        lastOp = op;
        return;
    }
    lastOp = op;

    // Pending operations
    if (currentOperation.text == "+") {
        curNum.text = Number(curNum.text.valueOf()) + Number(curVal.valueOf());
    } else if (currentOperation.text == "-") {
        curNum.text = Number(curVal) - Number(curNum.text.valueOf());
    } else if (currentOperation.text == "x") {
        curNum.text = Number(curVal) * Number(curNum.text.valueOf());
    } else if (currentOperation.text == "/") {
        curNum.text = Number(Number(curVal) / Number(curNum.text.valueOf())).toString();
    } else if (currentOperation.text == "=") {
    }

    if (op == "+" || op == "-" || op == "x" || op == "/") {
        currentOperation.text = op;
        curVal = curNum.text.valueOf();
        return;
    }
    curVal = 0;
    currentOperation.text = "";

    // Immediate operations
    if (op == "1/x") { // reciprocal
        curNum.text = (1 / curNum.text.valueOf()).toString();
    } else if (op == "^2") { // squared
        curNum.text = (curNum.text.valueOf() * curNum.text.valueOf()).toString();
    } else if (op == "Abs") {
        curNum.text = (Math.abs(curNum.text.valueOf())).toString();
    } else if (op == "Int") {
        curNum.text = (Math.floor(curNum.text.valueOf())).toString();
    } else if (op == "+/-") { // plus/minus
        curNum.text = (curNum.text.valueOf() * -1).toString();
    } else if (op == "Sqrt") { // square root
        curNum.text = (Math.sqrt(curNum.text.valueOf())).toString();
    } else if (op == "MC") { // memory clear
        memory = 0;
    } else if (op == "M+") { // memory increment
        memory += curNum.text.valueOf();
    } else if (op == "MR") { // memory recall
        curNum.text = memory.toString();
    } else if (op == "MS") { // memory set
        memory = curNum.text.valueOf();
    } else if (op == "Bksp") {
        curNum.text = curNum.text.toString().slice(0, -1);
    } else if (op == "C") {
        curNum.text = "0";
    } else if (op == "AC") {
        curVal = 0;
        memory = 0;
        lastOp = "";
        curNum.text ="0";
    }
}

