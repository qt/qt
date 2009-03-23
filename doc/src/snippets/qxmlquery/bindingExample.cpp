//! [0]
    QBuffer device;
    device.setData(myQString.toUtf8());
    device.open(QIODevice::ReadOnly);

    QXmlQuery query;
    query.setQuery("doc($inputDocument)/query[theDocument]");
    query.bindVariable("inputDocument", &device);
//! [0]
