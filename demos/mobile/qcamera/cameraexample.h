/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCAMERAEXAMPLE_H
#define QCAMERAEXAMPLE_H

#include <QtGui>

// Multimedia API in QtMobility API
// Unlike the other APIs in Qt Mobility,
// the Multimedia API is not in the QtMobility namespace "QTM_USE_NAMESPACE"
#include <QCamera>
#include <QCameraImageCapture>

// QtMobility API
#include <QSystemScreenSaver>
QTM_USE_NAMESPACE


#include <QAbstractVideoSurface>
#include <QVideoRendererControl>
#include <QVideoSurfaceFormat>

/*****************************************************************************
* MyVideoSurface
*/
class VideoIF
{
public:
    virtual void updateVideo() = 0;
};
class MyVideoSurface: public QAbstractVideoSurface
{
Q_OBJECT

public:
    MyVideoSurface(QWidget* widget, VideoIF* target, QObject * parent = 0);
    ~MyVideoSurface();

    bool start(const QVideoSurfaceFormat &format);

    bool present(const QVideoFrame &frame);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
                QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    void paint(QPainter*);

private:
    QWidget* m_targetWidget;
    VideoIF* m_target;
    QVideoFrame m_frame;
    QImage::Format m_imageFormat;
    QVideoSurfaceFormat m_videoFormat;
};



/*****************************************************************************
* CameraExample
*/
class Message;
class ContactsDialog;
class Button;
class BusinessCardHandling;
class CameraExample: public QMainWindow, public VideoIF
{
Q_OBJECT

public:
    CameraExample(QWidget *parent = 0);
    ~CameraExample();

    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *);

    void updateVideo();


public slots:
    void enableCamera();
    void lockStatusChanged(QCamera::LockStatus status, QCamera::LockChangeReason reason);
    void searchAndLock();
    void captureImage();
    void imageCaptured(int id, const QImage &preview);
    void error(QCamera::Error);

    void openContactsDlg();
    void contactSelected(QString phoneNumber);

    void messageStateChanged(int error);
    void messageReceived(QString phoneNumber, QString filename, QPixmap pixmap);

    void sendMMS();

private:
    QWidget*                m_videoWidget;
    QCamera*                m_camera;
    QCameraImageCapture*    m_stillImageCapture;

    QStackedWidget*         m_stackedWidget;
    Button*                 m_exit;
    Button*                 m_cameraBtn;
    Button*                 m_mms;
    QImage                  m_capturedImage;
    QString                 m_imageName;
    QString                 m_focusMessage;
    bool                    m_focusing;
    QString                 m_phoneNumber;

    Message*                m_message;
    QPointer<ContactsDialog> m_contactsDialog;
    BusinessCardHandling*   m_businessCardHandling;
    bool                    pictureCaptured;
    bool                    showViewFinder;
    MyVideoSurface*         m_myVideoSurface;
    QSystemScreenSaver*     m_systemScreenSaver;
};

#endif // QCAMERA_H
