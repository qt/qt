// ![0]
WorkerScript.onMessage = function(msg) {
    console.log("Worker told to", msg.action);

    if (msg.action == 'appendCurrentTime') {
        var data = {'time': new Date().toTimeString()};
        msg.model.append(data);
        msg.model.sync();   // updates the changes to the list

        var msgToSend = {'msg': 'Model updated!'};
        WorkerScript.sendMessage(msgToSend);
    }
}
// ![0]
