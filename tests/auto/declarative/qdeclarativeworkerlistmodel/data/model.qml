import Qt 4.6

Item {
    property alias model: model

    WorkerListModel { id: model }
    
    function workerModifyModel(cmd) { worker.sendMessage({'command': cmd, 'model': model}) }

    WorkerScript {
        id: worker
        source: "script.js"
    }
}
