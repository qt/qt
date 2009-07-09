
//! [0]
    QString foo;
    QString type = "long";

    foo->setText(QLatin1String("vector<") + type + QLatin1String(">::iterator"));

    if (foo.startsWith("(" + type + ") 0x"))
        ...
//! [0]

//! [3]
    #define QT_USE_FAST_CONCATENATION        
//! [3]

//! [4]
    #define QT_USE_FAST_CONCATENATION 
    #define QT_USE_FAST_OPERATOR_PLUS 
//! [4]

//! [5]
    #include <QStringBuilder>

    QString hello("hello");
    QStringRef el(&hello, 2, 3);
    QLatin1String world("world");
    QString message =  hello % el % world % QChar('!');
//! [5]
