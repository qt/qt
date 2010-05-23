import Qt 4.7

WorkerScript {
    id: worker
    source: "script.js"

    property variant response

    signal done()

    function testSend(value) {
        worker.sendMessage(value)
    }

    function compareLiteralResponse(expected) {
        var e = eval('(' + expected + ')')
        return worker.response == e
    }

    onMessage: {
        worker.response = messageObject
        worker.done()
    }
}
