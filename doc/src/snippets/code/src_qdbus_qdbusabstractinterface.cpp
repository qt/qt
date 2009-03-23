//! [0]
QString value = retrieveValue();
QDBusMessage reply;

QDBusReply<int> api = interface->call(QLatin1String("GetAPIVersion"));
if (api >= 14)
  reply = interface->call(QLatin1String("ProcessWorkUnicode"), value);
else
  reply = interface->call(QLatin1String("ProcessWork"), QLatin1String("UTF-8"), value.toUtf8());
//! [0]

//! [1]
QString value = retrieveValue();
QDBusPendingCall pcall = interface->asyncCall(QLatin1String("Process"), value);

QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);

QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 this, SLOT(callFinishedSlot(QDBusPendingCallWatcher*)));
//! [1]
