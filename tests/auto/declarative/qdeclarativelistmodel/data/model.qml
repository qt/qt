import Qt 4.6

Item {
    id: item
    property var model
    property bool done: false
    property var result

    function evalExpressionViaWorker(commands) {
        done = false
        worker.sendMessage({'commands': commands, 'model': model})
    }

    WorkerScript {
        id: worker
        source: "script.js"
        onMessage: {
            item.result = messageObject.result
            item.done = true
        }
    }
}
