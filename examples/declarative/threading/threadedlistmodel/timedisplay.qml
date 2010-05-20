// ![0]
import Qt 4.7

ListView {
    width: 200
    height: 300

    model: listModel
    delegate: Component {
        Text { text: time }
    }

    ListModel { id: listModel }

    WorkerScript {
        id: worker
        source: "dataloader.js"
    }

    Timer {
        id: timer
        interval: 2000; repeat: true
        running: true
        triggeredOnStart: true

        onTriggered: {
            var msg = {'action': 'appendCurrentTime', 'model': listModel};
            worker.sendMessage(msg);
        }
    }
}
// ![0]
