/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <private/qcore_unix_p.h> // overrides QT_OPEN

#include <qvfbhdr.h>
#include <qsocketnotifier.h>

#include "qgraphicssystem_qvfb.h"
#include "qwindowsurface_qvfb.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtCore/qdebug.h>
#include <QMouseEvent>

#include <qsocketnotifier.h>
#include <QApplication>
#include <QWindowSystemInterface>

QT_BEGIN_NAMESPACE


class QVFbGraphicsSystemScreenKeyboardHandler : public QObject
{
    Q_OBJECT
public:
    QVFbGraphicsSystemScreenKeyboardHandler();
    ~QVFbGraphicsSystemScreenKeyboardHandler();

private slots:
    void readKeyboardData();

private:
    int kbdFD;
    int kbdIdx;
    int kbdBufferLen;
    unsigned char *kbdBuffer;
    QSocketNotifier *keyNotifier;
};

QVFbGraphicsSystemScreenKeyboardHandler::QVFbGraphicsSystemScreenKeyboardHandler()
{
    int displayId = 0; //TODO displayId
    const QString keyboardDev = QT_VFB_KEYBOARD_PIPE(displayId);


    kbdFD = -1;
    kbdIdx = 0;
    kbdBufferLen = sizeof(QVFbKeyData) * 5;
    kbdBuffer = new unsigned char [kbdBufferLen];

    kbdFD = QT_OPEN(keyboardDev.toLatin1().constData(), O_RDWR | O_NDELAY);

    if (kbdFD == -1) {
        perror("QVFbGraphicsSystemScreenKeyboardHandler");
        qWarning("QVFbGraphicsSystemScreenKeyboardHandler: Unable to open device %s",
                 qPrintable(keyboardDev));
        return;
    }

    // Clear pending input
    char buf[2];
    while (QT_READ(kbdFD, buf, 1) > 0) { }

    keyNotifier = new QSocketNotifier(kbdFD, QSocketNotifier::Read, this);
    connect(keyNotifier, SIGNAL(activated(int)),this, SLOT(readKeyboardData()));

}

QVFbGraphicsSystemScreenKeyboardHandler::~QVFbGraphicsSystemScreenKeyboardHandler()
{
    if (kbdFD >= 0)
        QT_CLOSE(kbdFD);
    delete [] kbdBuffer;
}


void QVFbGraphicsSystemScreenKeyboardHandler::readKeyboardData()
{
    int n;
    do {
        n  = QT_READ(kbdFD, kbdBuffer+kbdIdx, kbdBufferLen - kbdIdx);
        if (n > 0)
            kbdIdx += n;
    } while (n > 0);

    int idx = 0;
    while (kbdIdx - idx >= (int)sizeof(QVFbKeyData)) {
        QVFbKeyData *kd = (QVFbKeyData *)(kbdBuffer + idx);
        if (kd->unicode == 0 && kd->keycode == 0 && kd->modifiers == 0 && kd->press) {
            // magic exit key
            qWarning("Instructed to quit by Virtual Keyboard");
            qApp->quit();
        }

        //QWSServer::processKeyEvent(kd->unicode ? kd->unicode : 0xffff, kd->keycode, kd->modifiers, kd->press, kd->repeat);

        QEvent::Type type = kd->press ? QEvent::KeyPress : QEvent::KeyRelease;

        QString text;
        if (kd->unicode && kd->unicode != 0xffff)
            text += QChar(kd->unicode);

//        qDebug() << "readKeyboardData" << type << hex << kd->keycode << kd->modifiers << text;

        QWindowSystemInterface::handleKeyEvent(0, type, kd->keycode, kd->modifiers, text, kd->repeat, int(text.length()));
        idx += sizeof(QVFbKeyData);
    }

    int surplus = kbdIdx - idx;
    for (int i = 0; i < surplus; i++)
        kbdBuffer[i] = kbdBuffer[idx+i];
    kbdIdx = surplus;
}




class QVFbGraphicsSystemScreenMouseHandler : public QObject
{
    Q_OBJECT
public:
    QVFbGraphicsSystemScreenMouseHandler();
    ~QVFbGraphicsSystemScreenMouseHandler();

private slots:
    void readMouseData();

private:
    int mouseFD;
    int mouseIdx;
    enum {mouseBufSize = 128};
    uchar mouseBuf[mouseBufSize];
    QSocketNotifier *mouseNotifier;

    int oldButtonState;
};

QVFbGraphicsSystemScreenMouseHandler::QVFbGraphicsSystemScreenMouseHandler()
{
    int displayId = 0; //TODO: displayId
    QString mouseDev = QT_VFB_MOUSE_PIPE(displayId);

    mouseFD = QT_OPEN(mouseDev.toLatin1().constData(), O_RDWR | O_NDELAY);

        if (mouseFD == -1) {
        perror("QVFbMouseHandler::QVFbMouseHandler");
        qWarning("QVFbMouseHander: Unable to open device %s",
                 qPrintable(mouseDev));
        return;
    }

    // Clear pending input
    char buf[2];
    while (QT_READ(mouseFD, buf, 1) > 0) { }

    mouseIdx = 0;
    oldButtonState = 0;
    mouseNotifier = new QSocketNotifier(mouseFD, QSocketNotifier::Read, this);
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
}


QVFbGraphicsSystemScreenMouseHandler::~QVFbGraphicsSystemScreenMouseHandler()
{
    if (mouseFD >= 0)
        QT_CLOSE(mouseFD);
}

void QVFbGraphicsSystemScreenMouseHandler::readMouseData()
{
   int n;
    do {
        n = QT_READ(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx);
        if (n > 0)
            mouseIdx += n;
    } while (n > 0);

    int idx = 0;
    static const int packetsize = sizeof(QPoint) + 2*sizeof(int);
    while (mouseIdx-idx >= packetsize) {
        uchar *mb = mouseBuf+idx;
        QPoint mousePos = *reinterpret_cast<QPoint *>(mb);
        mb += sizeof(QPoint);
        int bstate = *reinterpret_cast<int *>(mb);
        mb += sizeof(int);
        int wheel = *reinterpret_cast<int *>(mb);

        int button = bstate ^ oldButtonState;
        QEvent::Type type = QEvent::MouseMove;

        if (button) {
            type = (button & bstate) ? QEvent::MouseButtonPress : QEvent::MouseButtonRelease;
        }
        QWindowSystemInterface::handleMouseEvent(0, mousePos, mousePos, Qt::MouseButtons(bstate));

//        qDebug() << "readMouseData" << mousePos << button << bstate << oldButtonState << type;

        oldButtonState = bstate;

        idx += packetsize;
    }

    int surplus = mouseIdx - idx;
    for (int i = 0; i < surplus; i++)
        mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;

}


class QVFbGraphicsSystemScreenPrivate
{
public:
    QVFbGraphicsSystemScreenPrivate(QVFbGraphicsSystemScreen *)
        : shmrgn(0), hdr(0), data(0), mouseHandler(0), keyboardHandler(0)
        {
            connect(0); //for now we only handle one screen
        }

    ~QVFbGraphicsSystemScreenPrivate() { disconnect(); }
    void setDirty(const QRect &r);

    bool connect(int displayId);
    void disconnect();

    QImage *screenImage() { return &img; }
    QSize screenSize() { return img.size(); }

    int depth() const { return img.depth(); }
    QImage::Format format() const { return img.format(); }

private:
    unsigned char *shmrgn;
    QVFbHeader *hdr;
    uchar *data;
    QVFbGraphicsSystemScreenMouseHandler *mouseHandler;
    QVFbGraphicsSystemScreenKeyboardHandler *keyboardHandler;


    QImage img;
};


void QVFbGraphicsSystemScreenPrivate::setDirty(const QRect &r)
{
    hdr->dirty = true;
    hdr->update = hdr->update.united(r);
}


bool QVFbGraphicsSystemScreenPrivate::connect(int displayId)
{

    key_t key = ftok(QT_VFB_MOUSE_PIPE(displayId).toLatin1(), 'b');

    if (key == -1)
        return false;


    int shmId = shmget(key, 0, 0);
    if (shmId != -1)
        shmrgn = (unsigned char *)shmat(shmId, 0, 0);
    else
        return false;

    if ((long)shmrgn == -1 || shmrgn == 0) {
        qDebug("No shmrgn %ld", (long)shmrgn);
        return false;
    }

    hdr = (QVFbHeader *)shmrgn;
    data = shmrgn + hdr->dataoffset;

    int w = hdr->width;
    int h = hdr->height;
    int d = hdr->depth;
    int lstep = hdr->linestep;

    QImage::Format format = QImage::Format_Invalid;
    if (d == 32)
        format = QImage::Format_ARGB32_Premultiplied;
    else if (d == 16)
        format = QImage::Format_RGB16;


    if (format == QImage::Format_Invalid) {
        img = QImage();
        return false;
    }

    img = QImage(data, w, h, lstep, format);

    qDebug("connected %dx%d %d bpp", w, h, d);


    mouseHandler = new QVFbGraphicsSystemScreenMouseHandler;
    keyboardHandler = new QVFbGraphicsSystemScreenKeyboardHandler;
    return true;
}

void QVFbGraphicsSystemScreenPrivate::disconnect()
{
    if ((long)shmrgn != -1 && shmrgn) {
        shmdt((char*)shmrgn);
        shmrgn = 0;
    }
    delete mouseHandler;
    mouseHandler = 0;
    delete keyboardHandler;
    keyboardHandler = 0;
}


QVFbGraphicsSystemScreen::QVFbGraphicsSystemScreen()
{
    d_ptr = new QVFbGraphicsSystemScreenPrivate(this);
}


QVFbGraphicsSystemScreen::~QVFbGraphicsSystemScreen()
{
    delete d_ptr;
}

void QVFbGraphicsSystemScreen::setDirty(const QRect &rect)
{
    d_ptr->setDirty(rect);
}



QRect QVFbGraphicsSystemScreen::geometry() const {
    return QRect(QPoint(), d_ptr->screenSize());
}


int QVFbGraphicsSystemScreen::depth() const
{
    return d_ptr->depth();
}

QImage::Format QVFbGraphicsSystemScreen::format() const
{
    return d_ptr->format();
}

QSize QVFbGraphicsSystemScreen::physicalSize() const {
    return (d_ptr->screenSize()*254)/720;
}

#if 0
int QVFbGraphicsSystemScreen::linestep() const {
    return d_ptr->screenImage() ? d_ptr->screenImage()->bytesPerLine() : 0;
}

uchar *QVFbGraphicsSystemScreen::base() const {
    return d_ptr->screenImage() ? d_ptr->screenImage()->bits() : 0;
}
#endif

QImage *QVFbGraphicsSystemScreen::screenImage()
{
    return d_ptr->screenImage();
}

QVFbGraphicsSystem::QVFbGraphicsSystem()
{
    mPrimaryScreen = new QVFbGraphicsSystemScreen();

    mScreens.append(mPrimaryScreen);
}

QPixmapData *QVFbGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QWindowSurface *QVFbGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    if (widget->windowType() == Qt::Desktop)
        return 0;   // Don't create an explicit window surface for the destkop.
    return new QVFbWindowSurface
        (const_cast<QVFbGraphicsSystem *>(this), mPrimaryScreen, widget);
}

QT_END_NAMESPACE

#include "qgraphicssystem_qvfb.moc"
