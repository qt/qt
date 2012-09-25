/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qostdevice.h"
#include <e32base.h>

#include "usbostcomm.h"

class QOstDevicePrivate : public CActive
{
    QOstDevice* q_ptr;
    Q_DECLARE_PUBLIC(QOstDevice)

public:
    QOstDevicePrivate() : CActive(CActive::EPriorityStandard) {
        CActiveScheduler::Add(this);
    }
    ~QOstDevicePrivate() {
        Cancel();
    }

    TInt& AoFlags() { return ((TInt*)&iStatus)[1]; }

private:
    void RunL();
    void DoCancel();

private:
    RUsbOstComm ost;
    TBuf8<4096> readBuf;
    QByteArray dataBuf;
    TBool inReadyRead;
};

QOstDevice::QOstDevice(QObject *parent) :
    QIODevice(parent), d_ptr(new QOstDevicePrivate)
{
    d_ptr->q_ptr = this;
}

QOstDevice::~QOstDevice()
{
    close();
    delete d_ptr;
}

bool QOstDevice::open(int ostProtocolId)
{
    if (isOpen())
        return false;

    Q_D(QOstDevice);
    TInt err = d->ost.Connect();
    if (!err) err = d->ost.Open();
    const TVersion KRequiredVersion(1,1,0);
    TVersion version = d->ost.Version();
    if (version.iMajor < KRequiredVersion.iMajor ||
            (version.iMajor == KRequiredVersion.iMajor && version.iMinor < KRequiredVersion.iMinor)) {
        setErrorString("CODA version too old. At least version 4.0.18 (without TRK) is required.");
        return false;
    }

    if (!err) err = d->ost.RegisterProtocolID((TOstProtIds)ostProtocolId, EFalse);
    if (!err) {
        d->ost.ReadMessage(d->iStatus, d->readBuf);
        d->SetActive();
        return QIODevice::open(ReadWrite | Unbuffered);
    }
    return false;
}

void QOstDevicePrivate::RunL()
{
    Q_Q(QOstDevice);
    //qDebug("QOstDevice received %d bytes q=%x", readBuf.Size(), q);
    if (iStatus == KErrNone) {
        QByteArray data = QByteArray::fromRawData((const char*)readBuf.Ptr(), readBuf.Size());
        dataBuf.append(data);

        readBuf.Zero();
        ost.ReadMessage(iStatus, readBuf);
        SetActive();

        if (!inReadyRead) {
            inReadyRead = true;
            emit q->readyRead();
            inReadyRead = false;
        }
    } else {
        q->setErrorString(QString("Error %1 from RUsbOstComm::ReadMessage()").arg(iStatus.Int()));
    }
    //qDebug("-QOstDevicePrivate RunL");
}

void QOstDevicePrivate::DoCancel()
{
    ost.ReadCancel();
}

void QOstDevice::close()
{
    Q_D(QOstDevice);
    QIODevice::close();
    d->Cancel();
    // RDbgTrcComm::Close isn't safe to call when not open, sigh
    if (d->ost.Handle()) {
        d->ost.Close();
    }
}

qint64 QOstDevice::readData(char *data, qint64 maxSize)
{
    Q_D(QOstDevice);
    if (d->dataBuf.length() == 0 && !d->IsActive())
        return -1;
    qint64 available = qMin(maxSize, (qint64)d->dataBuf.length());
    memcpy(data, d->dataBuf.constData(), available);
    d->dataBuf.remove(0, available);
    return available;
}

static const TInt KMaxOstPacketLen = 4096;

qint64 QOstDevice::writeData(const char *data, qint64 maxSize)
{
    Q_D(QOstDevice);
    TPtrC8 ptr((const TUint8*)data, (TInt)maxSize);
    while (ptr.Length()) {
        TPtrC8 fragment = ptr.Left(qMin(ptr.Length(), KMaxOstPacketLen));
        //qDebug("QOstDevice writing %d bytes", fragment.Length());
        TRequestStatus stat;
        d->ost.WriteMessage(stat, fragment);
        User::WaitForRequest(stat);
        if (stat.Int() != KErrNone) {
            setErrorString(QString("Error %1 from RUsbOstComm::WriteMessage()").arg(stat.Int()));
            return -1;
        }
        ptr.Set(ptr.Mid(fragment.Length()));
    }
    emit bytesWritten(maxSize); //TODO does it matter this is emitted synchronously?
    //qDebug("QOstDevice wrote %d bytes", ptr.Size());
    return maxSize;
}

qint64 QOstDevice::bytesAvailable() const
{
    Q_D(const QOstDevice);
    return d->dataBuf.length();
}

bool QOstDevice::waitForReadyRead(int msecs)
{
    Q_D(QOstDevice);
    if (msecs >= 0) {
        RTimer timer;
        TInt err = timer.CreateLocal();
        if (err) return false;
        TRequestStatus timeoutStat;
        timer.After(timeoutStat, msecs*1000);
        User::WaitForRequest(timeoutStat, d->iStatus);
        if (timeoutStat != KRequestPending) {
            // Timed out
            timer.Close();
            return false;
        } else {
            // We got data, so cancel timer
            timer.Cancel();
            User::WaitForRequest(timeoutStat);
            timer.Close();
            // And drop through
        }
    } else {
        // Just wait forever for data
        User::WaitForRequest(d->iStatus);
    }

    // If we get here we have data
    TInt err = d->iStatus.Int();
    d->AoFlags() &= ~3; // This is necessary to clean up the scheduler as you're not supposed to bypass it like this
    TRAP_IGNORE(d->RunL());
    return err == KErrNone;
}
