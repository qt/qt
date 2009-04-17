{
//! [0]
    QDBusPendingReply<QString> reply = interface->asyncCall("RemoteMethod");
    reply.waitForFinished();
    if (reply.isError())
        // call failed. Show an error condition.
        showError(reply.error());
    else
        // use the returned value
        useValue(reply.value());
//! [0]

//! [2]
    QDBusPendingReply<bool, QString> reply = interface->asyncCall("RemoteMethod");
    reply.waitForFinished();
    if (!reply.isError()) {
        if (reply.argumentAt<0>())
            showSuccess(reply.argumentAt<1>());
        else
            showFailure(reply.argumentAt<1>());
    }
//! [2]
}
