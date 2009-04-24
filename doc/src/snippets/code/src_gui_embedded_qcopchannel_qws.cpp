//! [0]
void MyClass::receive(const QString &message, const QByteArray &data)
{
    QDataStream in(data);
    if (message == "execute(QString,QString)") {
        QString cmd;
        QString arg;
        in >> cmd >> arg;
        ...
    } else if (message == "delete(QString)") {
        QString fileName;
        in >> fileName;
        ...
    } else {
        ...
    }
}
//! [0]


//! [1]
QByteArray data;
QDataStream out(&data, QIODevice::WriteOnly);
out << QString("cat") << QString("file.txt");
QCopChannel::send("System/Shell", "execute(QString,QString)", data);
//! [1]
