
void wrapInFunction()
{

//! [0]
QTransportAuth::Data *conData;
QTransportAuth *a = QTransportAuth::getInstance();

conData = a->connectTransport(
    QTransportAuth::Trusted | QTransportAuth::UnixStreamSock,
    socketDescriptor );
//! [0]


//! [1]
// mySocket can be any QIODevice subclass
AuthDevice *ad = a->recvBuf( d, mySocket );

// proxy in the auth device where the socket would have gone
connect( ad, SIGNAL(readyRead()), this, SLOT(mySocketReadyRead()));
//! [1]


//! [2]
AuthDevice *ad = a->authBuf( d, mySocket );

ad->write( someData );
//! [2]


//! [3]
policyCheck( QTransportAuth::Data &, const QString & )
//! [3]


//! [4]
QTransportAuth::Result r = d.status & QTransportAuth::ErrMask;
qWarning( "error: %s", QTransportAuth::errorStrings[r] );
//! [4]


//! [5]
MD5(K XOR opad, MD5(K XOR ipad, text))
//! [5]

}

