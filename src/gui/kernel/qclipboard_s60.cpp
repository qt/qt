/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#include "qclipboard.h"

#ifndef QT_NO_CLIPBOARD

#include "qapplication.h"
#include "qbitmap.h"
#include "qdatetime.h"
#include "qbuffer.h"
#include "qwidget.h"
#include "qevent.h"
#include <QtDebug>

// Symbian's clipboard
#include <baclipb.h>
QT_BEGIN_NAMESPACE

//###  Mime Type mapping to UIDs

const TUid KQtCbDataStream = {0x666777};


class QClipboardData
{
public:
    QClipboardData();
   ~QClipboardData();

    void setSource(QMimeData* s)
    {
        if (s == src)
            return;
        delete src;
        src = s;
    }
    QMimeData* source()
    { return src; }
    bool connected()
    { return connection; }    
    void clear();
    RFs fsSession();
    

private:
    QMimeData* src;
    RFs iFs;
    bool connection;
};

QClipboardData::QClipboardData():src(0),connection(true)
{
    clear();
    if (KErrNone != iFs.Connect())
    {
        qWarning("QClipboardData::fileserver connnect failed");
        connection = false;
    }
}

QClipboardData::~QClipboardData()
{
    iFs.Close();
    connection = false;
    delete src;
}

void QClipboardData::clear()
{
    QMimeData* newSrc = new QMimeData;
    delete src;
    src = newSrc;
}
RFs QClipboardData::fsSession()
{
    return iFs;
}

static QClipboardData *internalCbData = 0;

static void cleanupClipboardData()
{
    delete internalCbData;
    internalCbData = 0;
}

static QClipboardData *clipboardData()
{
    if (internalCbData == 0) {
        internalCbData = new QClipboardData;
        if (internalCbData)
        {
            if (!internalCbData->connected())
            {
                delete internalCbData;
                internalCbData = 0;
            }
            else 
            {
                qAddPostRoutine(cleanupClipboardData);
            }
        }
    }
    return internalCbData;
}

void writeToStream(const QMimeData* aData, RWriteStream& aStream)
{
    QStringList headers = aData->formats();
    aStream << TCardinality(headers.count());
    for (QStringList::const_iterator iter= headers.constBegin();iter != headers.constEnd();iter++)
    {
        HBufC* stringData = TPtrC(reinterpret_cast<const TUint16*>((*iter).utf16())).AllocLC();
        QByteArray ba = aData->data((*iter));
        qDebug() << "copy to clipboard mime: " << *iter << " data: " << ba;
        // mime type
        aStream << TCardinality(stringData->Size());
        aStream << *(stringData);
        // mime data
        aStream << TCardinality(ba.size());
        aStream.WriteL(reinterpret_cast<const uchar*>(ba.constData()),ba.size());
        CleanupStack::PopAndDestroy(stringData);
    }    
}

void readFromStream(QMimeData* aData,RReadStream& aStream)
{
    TCardinality mimeTypeCount;
    aStream >> mimeTypeCount;    
    for (int i = 0; i< mimeTypeCount;i++)
    {
        // mime type
        TCardinality mimeTypeSize;
        aStream >> mimeTypeSize;
        HBufC* mimeTypeBuf = HBufC::NewLC(aStream,mimeTypeSize);
        QString mimeType = QString::fromUtf16(mimeTypeBuf->Des().Ptr(),mimeTypeBuf->Length());
        // mime data
        TCardinality dataSize;
        aStream >> dataSize;
        QByteArray ba;
        ba.reserve(dataSize);
        aStream.ReadL(reinterpret_cast<uchar*>(ba.data_ptr()->data),dataSize);
        ba.data_ptr()->size = dataSize;
        qDebug() << "paste from clipboard mime: " << mimeType << " data: " << ba;
        aData->setData(mimeType,ba);
        CleanupStack::PopAndDestroy(mimeTypeBuf);

    }
}


/*****************************************************************************
  QClipboard member functions
 *****************************************************************************/

void QClipboard::clear(Mode mode)
{
    setText(QString(), mode);
}
const QMimeData* QClipboard::mimeData(Mode mode) const
{
    if (mode != Clipboard) return 0;
    QClipboardData *d = clipboardData();
    if (d)
    {
        //###fixme when exceptions are added to Qt
        TRAPD(err,{
            RFs fs = d->fsSession();
            CClipboard* cb = CClipboard::NewForReadingLC(fs);
            Q_ASSERT(cb);
            RStoreReadStream stream;
            TStreamId stid = (cb->StreamDictionary()).At(KQtCbDataStream);
            stream.OpenLC(cb->Store(),stid);
            readFromStream(d->source(),stream);
            CleanupStack::PopAndDestroy(2,cb);
            return d->source();
        });
        if (err != KErrNone){
            qDebug()<< "clipboard is empty/err: " << err;
        }
            
    }
    return 0;
}


void QClipboard::setMimeData(QMimeData* src, Mode mode)
{
    if (mode != Clipboard) return;
    QClipboardData *d = clipboardData();
    if (d)
    {
        //###fixme when exceptions are added to Qt
        TRAPD(err,{
            RFs fs = d->fsSession();
            CClipboard* cb = CClipboard::NewForWritingLC(fs);
            RStoreWriteStream  stream;
            TStreamId stid = stream.CreateLC(cb->Store());
            writeToStream(src,stream);
            d->setSource(src);
            stream.CommitL();
            (cb->StreamDictionary()).AssignL(KQtCbDataStream,stid);
            cb->CommitL();
            CleanupStack::PopAndDestroy(2,cb);
        });
        if (err != KErrNone){
            qDebug()<< "clipboard write err :" << err;
        }
    }
    emitChanged(QClipboard::Clipboard);      
}

bool QClipboard::supportsMode(Mode mode) const
{
    return (mode == Clipboard);
}

bool QClipboard::ownsMode(Mode mode) const
{
    if (mode == Clipboard)
        qWarning("QClipboard::ownsClipboard: UNIMPLEMENTED!");
    return false;
}

bool QClipboard::event(QEvent *e)
{
    return true;
}

void QClipboard::connectNotify( const char * )
{
}

void QClipboard::ownerDestroyed()
{
}
QT_END_NAMESPACE
#endif // QT_NO_CLIPBOARD