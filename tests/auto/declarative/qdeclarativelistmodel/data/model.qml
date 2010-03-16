import Qt 4.6

Item {
    id: item
    property var model
    property bool done: false

    function evalExpressionViaWorker(expr) {
        done = false
        if (expr[expr.length-1] == ';')
            expr = expr.substring(0, expr.length-1)
        var cmds = expr.split(';')

        worker.sendMessage({'commands': cmds, 'model': model})
    }

    WorkerScript {
        id: worker
        source: "script.js"
        onMessage: {
            item.done = true
        }
    }
}
