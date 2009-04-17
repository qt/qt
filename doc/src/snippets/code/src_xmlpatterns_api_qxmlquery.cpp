//! [0]
    QXmlNamePool namePool(query.namePool());
    query.bindVariable(QXmlName(namePool, localName), value);
//! [0]


{
//! [1]
    QByteArray myDocument;
    QBuffer buffer(&myDocument); // This is a QIODevice.
    buffer.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("myDocument", &buffer);
    query.setQuery("doc($myDocument)");
//! [1]
}


{
    QIODevice *device = 0;
//! [2]
    QXmlNamePool namePool(query.namePool());
    query.bindVariable(QXmlName(namePool, localName), device);
//! [2]

}

{
    QIODevice *myOutputDevice = 0;
//! [3]
    QFile xq("myquery.xq");

    QXmlQuery query;
    query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));

    QXmlSerializer serializer(query, myOutputDevice);
    query.evaluateTo(&serializer);
//! [3]
}

{
    QIODevice *myOutputDevice = 0;
//! [4]
    QFile xq("myquery.xq");
    QString fileName("the filename");
    QString publisherName("the publisher");
    qlonglong year = 1234;

    QXmlQuery query;

    query.bindVariable("file", QVariant(fileName));
    query.bindVariable("publisher", QVariant(publisherName));
    query.bindVariable("year", QVariant(year));

    query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));

    QXmlSerializer serializer(query, myOutputDevice);
    query.evaluateTo(&serializer);
//! [4]
}

{
//! [5]
    QFile xq("myquery.xq");
    QString fileName("the filename");
    QString publisherName("the publisher");
    qlonglong year = 1234;

    QXmlQuery query;

    query.bindVariable("file", QVariant(fileName));
    query.bindVariable("publisher", QVariant(publisherName));
    query.bindVariable("year", QVariant(year));

    query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));

    QXmlResultItems result;
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull()) {
        if (item.isAtomicValue()) {
            QVariant v = item.toAtomicValue();
            switch (v.type()) {
                case QVariant::LongLong:
                    // xs:integer
                    break;
                case QVariant::String:
                    // xs:string
                    break;
                default:
                    // error
                    break;
            }
        }
        else if (item.isNode()) {
            QXmlNodeModelIndex i = item.toNodeModelIndex();
            // process node
        }
        item = result.next();
    }
//! [5]
}

{
//! [6]
    QFile xq("myquery.xq");

    QXmlQuery query;
    query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));

    QXmlResultItems result;
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    while (!item.isNull()) {
        if (item.isAtomicValue()) {
            QVariant v = item.toAtomicValue();
            switch (v.type()) {
                case QVariant::LongLong:
                    // xs:integer
                    break;
                case QVariant::String:
                    // xs:string
                    break;
                default:
                    if (v.userType() == qMetaTypeId<QXmlName>()) {
                        QXmlName n = qVariantValue<QXmlName>(v);
                        // process QXmlName n...
                    }
                    else {
                        // error
                    }
                    break;
            }
        }
        else if (item.isNode()) {
            QXmlNodeModelIndex i = item.toNodeModelIndex();
            // process node
        }
        item = result.next();
    }
//! [6]
}

{
    QIODevice *out = 0;
//! [7]
    QXmlQuery query(QXmlQuery::XSLT20);
    query.setFocus(QUrl("myInput.xml"));
    query.setQuery(QUrl("myStylesheet.xsl"));
    query.evaluateTo(out);
//! [7]
}
