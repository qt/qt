import Qt 4.6

WorkerScript {
    id: worker
    source: "script.js"

    property bool done : false
    property var response

    function testSend(value) {
        worker.sendMessage(value)
    }

    function testSendLiteral(value) {
        eval('worker.sendMessage(' + value +')')
    }

    function compareLiteralResponse(expected) {
        var e = eval('(' + expected + ')')
        return worker.response == e
    }

    onMessage: {
        worker.done = true
        worker.response = messageObject
    }
}
