{
//! [0]
    QDBusPendingCall async = iface->asyncCall("RemoteMethod", value1, value2);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(callFinishedSlot(QDBusPendingCallWatcher*)));
//! [0]

}

//! [1]
void MyClass::callFinishedSlot(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString, QByteArray> reply = *call;
    if (reply.isError()) {
        showError();
    } else {
        QString text = reply.argumentAt<0>();
        QByteArray data = reply.argumentAt<1>();
        showReply(text, data);
    }
}
//! [1]
