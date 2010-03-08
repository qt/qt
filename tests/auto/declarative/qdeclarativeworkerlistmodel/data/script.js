WorkerScript.onMessage = function(msg) {
    eval("msg.model." + msg.command)
    msg.model.sync()
}


