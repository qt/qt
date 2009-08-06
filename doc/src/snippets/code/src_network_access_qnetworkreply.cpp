//! [0]
QList<QSslCertificate> cert = QSslCertificate::fromPath(QLatin1String("server-certificate.pem"));
QSslError error(QSslError::SelfSignedCertificate, cert.at(0));
QList<QSslError> expectedSslErrors;
expectedSslErrors.append(error);

QNetworkReply *reply = manager.get(QNetworkRequest(QUrl("https://server.tld/index.html")));
reply->ignoreSslErrors(expectedSslErrors);
// here connect signals etc.
//! [0]
