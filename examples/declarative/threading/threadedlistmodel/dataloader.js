// ![0]
WorkerScript.onMessage = function(msg) {
    if (msg.action == 'appendCurrentTime') {
        var data = {'time': new Date().toTimeString()};
        msg.model.append(data);
        msg.model.sync();   // updates the changes to the list
    }
}
// ![0]
