/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaServices module of the Qt Toolkit.
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

#ifndef QMEDIASERVICEPROVIDER_H
#define QMEDIASERVICEPROVIDER_H

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>

#include <QtMediaServices/qtmedianamespace.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)


class QMediaService;

class QMediaServiceProviderHintPrivate;
class Q_MEDIASERVICES_EXPORT QMediaServiceProviderHint
{
public:
    enum Type { Null, ContentType, Device, SupportedFeatures };

    enum Feature {
        LowLatencyPlayback = 0x01,
        RecordingSupport = 0x02,
        StreamPlayback = 0x04
    };
    Q_DECLARE_FLAGS(Features, Feature)

    QMediaServiceProviderHint();
    QMediaServiceProviderHint(const QString &mimeType, const QStringList& codecs);
    QMediaServiceProviderHint(const QByteArray &device);
    QMediaServiceProviderHint(Features features);
    QMediaServiceProviderHint(const QMediaServiceProviderHint &other);
    ~QMediaServiceProviderHint();

    QMediaServiceProviderHint& operator=(const QMediaServiceProviderHint &other);

    bool operator == (const QMediaServiceProviderHint &other) const;
    bool operator != (const QMediaServiceProviderHint &other) const;

    bool isNull() const;

    Type type() const;

    QString mimeType() const;
    QStringList codecs() const;

    QByteArray device() const;

    Features features() const;

    //to be extended, if necessary

private:
    QSharedDataPointer<QMediaServiceProviderHintPrivate> d;
};

class Q_MEDIASERVICES_EXPORT QMediaServiceProvider : public QObject
{
    Q_OBJECT

public:
    virtual QMediaService* requestService(const QByteArray &type, const QMediaServiceProviderHint &hint = QMediaServiceProviderHint()) = 0;
    virtual void releaseService(QMediaService *service) = 0;

    virtual QtMediaServices::SupportEstimate hasSupport(const QByteArray &serviceType,
                                             const QString &mimeType,
                                             const QStringList& codecs,
                                             int flags = 0) const;
    virtual QStringList supportedMimeTypes(const QByteArray &serviceType, int flags = 0) const;

    virtual QList<QByteArray> devices(const QByteArray &serviceType) const;
    virtual QString deviceDescription(const QByteArray &serviceType, const QByteArray &device);

    static QMediaServiceProvider* defaultServiceProvider();

#ifdef QT_BUILD_INTERNAL
    static void setDefaultServiceProvider(QMediaServiceProvider *provider);
#endif
};

/*!
    Service with support for media playback
    Required Controls: QMediaPlayerControl
    Optional Controls: QMediaPlaylistControl, QAudioDeviceControl
    Video Output Controls (used by QWideoWidget and QGraphicsVideoItem):
                        Required: QVideoOutputControl
                        Optional: QVideoWindowControl, QVideoRendererControl, QVideoWidgetControl
*/
#define Q_MEDIASERVICE_MEDIAPLAYER "com.nokia.qt.mediaplayer"

/*!
   Service with support for recording from audio sources
   Required Controls: QAudioDeviceControl
   Recording Controls (QMediaRecorder):
                        Required: QMediaRecorderControl
                        Recommended: QAudioEncoderControl
                        Optional: QMediaContainerControl
*/
#define Q_MEDIASERVICE_AUDIOSOURCE "com.nokia.qt.audiosource"

/*!
    Service with support for camera use.
    Required Controls: QCameraControl
    Optional Controls: QCameraExposureControl, QCameraFocusControl, QImageProcessingControl
    Still Capture Controls: QImageCaptureControl
    Recording Controls (QMediaRecorder):
                        Required: QMediaRecorderControl
                        Recommended: QAudioEncoderControl, QVideoEncoderControl, QMediaContainerControl
    Viewfinder Video Output Controls (used by QWideoWidget and QGraphicsVideoItem):
                        Required: QVideoOutputControl
                        Optional: QVideoWindowControl, QVideoRendererControl, QVideoWidgetControl
*/
#define Q_MEDIASERVICE_CAMERA "com.nokia.qt.camera"

/*!
    Service with support for radio tuning.
    Required Controls: QRadioTunerControl
    Recording Controls (Optional, used by QMediaRecorder):
                        Required: QMediaRecorderControl
                        Recommended: QAudioEncoderControl
                        Optional: QMediaContainerControl
*/
#define Q_MEDIASERVICE_RADIO "com.nokia.qt.radio"


QT_END_NAMESPACE

QT_END_HEADER

#endif  // QMEDIASERVICEPROVIDER_H
