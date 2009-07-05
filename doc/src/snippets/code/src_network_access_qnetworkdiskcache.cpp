//! [0]
QNetworkAccessManager *manager = new QNetworkAccessManager(this);
QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
diskCache->setCacheDirectory("cacheDir");
manager->setCache(diskCache);
//! [0]

//! [1]
// do a normal request (preferred from network, as this is the default)
QNetworkRequest request(QUrl(QString("http://www.qtsoftware.com")));
manager->get(request);

// do a request preferred from cache
QNetworkRequest request2(QUrl(QString("http://www.qtsoftware.com")));
request2.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
manager->get(request2);
//! [1]

//! [2]
void replyFinished(QNetworkReply *reply) {
    QVariant fromCache = reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);
    qDebug() << "page from cache?" << fromCache.toBool();
}
//! [2]
