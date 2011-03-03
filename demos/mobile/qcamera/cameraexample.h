/**
 * Copyright (c) 2011 Nokia Corporation. All rights reserved.
 *
 * Nokia and Nokia Connecting People are registered trademarks of Nokia
 * Corporation. Oracle and Java are registered trademarks of Oracle and/or its
 * affiliates. Other product and company names mentioned herein may be
 * trademarks or trade names of their respective owners.
 *
 *
 * Subject to the conditions below, you may, without charge:
 *
 *  *  Use, copy, modify and/or merge copies of this software and associated
 *     documentation files (the "Software")
 *
 *  *  Publish, distribute, sub-licence and/or sell new software derived from
 *     or incorporating the Software.
 *
 *
 *
 * This file, unmodified, shall be included with all copies or substantial
 * portions of the Software that are distributed in source code form.
 *
 * The Software cannot constitute the primary value of any new software derived
 * from or incorporating the Software.
 *
 * Any person dealing with the Software shall not misrepresent the source of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
