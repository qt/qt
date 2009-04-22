
var curVal = 0;
var memory = 0;
var lastOp = "";
var timer = 0;

function disabled(op) {
    if (op == "." && CurNum.text.toString().search(/\./) != -1) {
        return true;
    } else if (op == "Sqrt" &&  CurNum.text.toString().search(/-/) != -1) {
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
        if (CurNum.text.toString().length >= 14)
            return; // No arbitrary length numbers
        if (lastOp.toString().length == 1 && ((lastOp >= "0" && lastOp <= "9") || lastOp==".") ) {
            CurNum.text = CurNum.text + op.toString();
        } else {
            CurNum.text = op;
        }
        lastOp = op;
        return;
    }
    lastOp = op;

    // Pending operations
    if (CurrentOperation.text == "+") {
        CurNum.text = Number(CurNum.text.valueOf()) + Number(curVal.valueOf());
    } else if (CurrentOperation.text == "-") {
        CurNum.text = Number(curVal) - Number(CurNum.text.valueOf());
    } else if (CurrentOperation.text == "x") {
        CurNum.text = Number(curVal) * Number(CurNum.text.valueOf());
    } else if (CurrentOperation.text == "/") {
        CurNum.text = Number(Number(curVal) / Number(CurNum.text.valueOf())).toString();
    } else if (CurrentOperation.text == "=") {
    }

    if (op == "+" || op == "-" || op == "x" || op == "/") {
        CurrentOperation.text = op;
        curVal = CurNum.text.valueOf();
        return;
    }
    curVal = 0;
    CurrentOperation.text = "";

    // Immediate operations
    if (op == "1/x") { // reciprocal
        CurNum.text = (1 / CurNum.text.valueOf()).toString();
    } else if (op == "^2") { // squared
        CurNum.text = (CurNum.text.valueOf() * CurNum.text.valueOf()).toString();
    } else if (op == "Abs") {
        CurNum.text = (Math.abs(CurNum.text.valueOf())).toString();
    } else if (op == "Int") {
        CurNum.text = (Math.floor(CurNum.text.valueOf())).toString();
    } else if (op == "+/-") { // plus/minus
        CurNum.text = (CurNum.text.valueOf() * -1).toString();
    } else if (op == "Sqrt") { // square root
        CurNum.text = (Math.sqrt(CurNum.text.valueOf())).toString();
    } else if (op == "MC") { // memory clear
        memory = 0;
    } else if (op == "M+") { // memory increment
        memory += CurNum.text.valueOf();
    } else if (op == "MR") { // memory recall
        CurNum.text = memory.toString();
    } else if (op == "MS") { // memory set
        memory = CurNum.text.valueOf();
    } else if (op == "Bksp") {
        CurNum.text = CurNum.text.toString().slice(0, -1);
    } else if (op == "C") {
        CurNum.text = "0";
    } else if (op == "AC") {
        curVal = 0;
        memory = 0;
        lastOp = "";
        CurNum.text ="0";
    }
}

