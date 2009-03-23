//! [0]
foreach (QSslCertificate cert, QSslCertificate::fromPath("C:/ssl/certificate.*.pem", QSsl::Pem,
                                                         QRegExp::Wildcard)) {
    qDebug() << cert.issuerInfo(QSslCertificate::Organization);
}
//! [0]
