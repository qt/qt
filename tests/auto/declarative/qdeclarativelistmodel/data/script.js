WorkerScript.onMessage = function(msg) {
    try {
        for (var i=0; i<msg.commands.length; i++) {
            var c = 'msg.model.' + msg.commands[i]
            eval(c)
        }
        msg.model.sync()
    } catch(e) { }
    WorkerScript.sendMessage({'done': true})
}


