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

#include <QtCore/qdebug.h>
#include <QtCore/qmap.h>

#include <QtMediaServices/qmediaservice.h>
#include <QtMediaServices/qmediaserviceprovider.h>
#include <QtMediaServices/qmediaserviceproviderplugin.h>
#include "qmediapluginloader_p.h"
#include <QtMediaServices/qmediaplayer.h>

QT_BEGIN_NAMESPACE

class QMediaServiceProviderHintPrivate : public QSharedData
{
public:
    QMediaServiceProviderHintPrivate(QMediaServiceProviderHint::Type type)
        :type(type), features(0)
    {
    }

    QMediaServiceProviderHintPrivate(const QMediaServiceProviderHintPrivate &other)
        :QSharedData(other),
        type(other.type),
        device(other.device),
        mimeType(other.mimeType),
        codecs(other.codecs),
        features(other.features)
    {
    }

    ~QMediaServiceProviderHintPrivate()
    {
    }

    QMediaServiceProviderHint::Type type;
    QByteArray device;
    QString mimeType;
    QStringList codecs;
    QMediaServiceProviderHint::Features features;
};

/*!
    \class QMediaServiceProviderHint
    \preliminary
    \since 4.7
    \brief The QMediaServiceProviderHint class describes what is required of a QMediaService.
    
    \ingroup multimedia-serv

    The QMediaServiceProvider class uses hints to select an appropriate media service.
*/

/*!
    \enum QMediaServiceProviderHint::Feature

    Enumerates features a media service may provide.

    \value LowLatencyPlayback
            The service is expected to play simple audio formats,
            but playback should start without significant delay.
            Such playback service can be used for beeps, ringtones, etc.

    \value RecordingSupport
            The service provides audio or video recording functions.

    \value StreamPlayback
            The service is capable of playing QIODevice based streams.
*/

/*!
    \enum QMediaServiceProviderHint::Type

    Enumerates the possible types of media service provider hint.

    \value Null               En empty hint, use the default service.
    \value ContentType        Select media service most suitable for certain content type.
    \value Device             Select media service which supports certain device.
    \value SupportedFeatures  Select media service supporting the set of optional features.
*/


/*!
    Constructs an empty media service provider hint.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint()
    :d(new QMediaServiceProviderHintPrivate(Null))
{
}

/*!
    Constructs a ContentType media service provider hint.

    This type of hint describes a service that is able to play content of a specific MIME \a type
    encoded with one or more of the listed \a codecs.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(const QString &type, const QStringList& codecs)
    :d(new QMediaServiceProviderHintPrivate(ContentType))
{
    d->mimeType = type;
    d->codecs = codecs;
}

/*!
  Constructs a Device media service provider hint.

  This type of hint describes a media service that utilizes a specific \a device.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(const QByteArray &device)
    :d(new QMediaServiceProviderHintPrivate(Device))
{
    d->device = device;
}

/*!
    Constructs a SupportedFeatures media service provider hint.

    This type of hint describes a service which supports a specific set of \a features.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(QMediaServiceProviderHint::Features features)
    :d(new QMediaServiceProviderHintPrivate(SupportedFeatures))
{
    d->features = features;
}

/*!
    Constructs a copy of the media service provider hint \a other.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(const QMediaServiceProviderHint &other)
    :d(other.d)
{
}

/*!
    Destroys a media service provider hint.
*/
QMediaServiceProviderHint::~QMediaServiceProviderHint()
{
}

/*!
    Assigns the value \a other to a media service provider hint.
*/
QMediaServiceProviderHint& QMediaServiceProviderHint::operator=(const QMediaServiceProviderHint &other)
{
    d = other.d;
    return *this;
}

/*!
    Identifies if \a other is of equal value to a media service provider hint.

    Returns true if the hints are equal, and false if they are not.
*/
bool QMediaServiceProviderHint::operator == (const QMediaServiceProviderHint &other) const
{
    return (d == other.d) ||
           (d->type == other.d->type &&
            d->device == other.d->device &&
            d->mimeType == other.d->mimeType &&
            d->codecs == other.d->codecs &&
            d->features == other.d->features);
}

/*!
    Identifies if \a other is not of equal value to a media service provider hint.

    Returns true if the hints are not equal, and false if they are.
*/
bool QMediaServiceProviderHint::operator != (const QMediaServiceProviderHint &other) const
{
    return !(*this == other);
}

/*!
    Returns true if a media service provider is null.
*/
bool QMediaServiceProviderHint::isNull() const
{
    return d->type == Null;
}

/*!
    Returns the type of a media service provider hint.
*/
QMediaServiceProviderHint::Type QMediaServiceProviderHint::type() const
{
    return d->type;
}

/*!
    Returns the mime type of the media a service is expected to be able play.
*/
QString QMediaServiceProviderHint::mimeType() const
{
    return d->mimeType;
}

/*!
    Returns a list of codes a media service is expected to be able to decode.
*/
QStringList QMediaServiceProviderHint::codecs() const
{
    return d->codecs;
}

/*!
    Returns the name of a device a media service is expected to utilize.
*/
QByteArray QMediaServiceProviderHint::device() const
{
    return d->device;
}

/*!
    Returns a set of features a media service is expected to provide.
*/
QMediaServiceProviderHint::Features QMediaServiceProviderHint::features() const
{
    return d->features;
}


Q_GLOBAL_STATIC_WITH_ARGS(QMediaPluginLoader, loader,
        (QMediaServiceProviderFactoryInterface_iid, QLatin1String("/mediaservices"), Qt::CaseInsensitive))


class QPluginServiceProvider : public QMediaServiceProvider
{
    QMap<QMediaService*, QMediaServiceProviderPlugin*> pluginMap;

public:
    QMediaService* requestService(const QByteArray &type, const QMediaServiceProviderHint &hint)
    {
        QString key(QString::fromLatin1(type.constData(),type.length()));

        QList<QMediaServiceProviderPlugin *>plugins;
        foreach (QObject *obj, loader()->instances(key)) {
            QMediaServiceProviderPlugin *plugin =
                qobject_cast<QMediaServiceProviderPlugin*>(obj);
            if (plugin)
                plugins << plugin;
        }

        if (!plugins.isEmpty()) {
            QMediaServiceProviderPlugin *plugin = 0;

            switch (hint.type()) {
            case QMediaServiceProviderHint::Null:
                plugin = plugins[0];
                //special case for media player, if low latency was not asked,
                //prefer services not offering it, since they are likely to support
                //more formats
                if (type == QByteArray(Q_MEDIASERVICE_MEDIAPLAYER)) {
                    foreach (QMediaServiceProviderPlugin *currentPlugin, plugins) {
                        QMediaServiceFeaturesInterface *iface =
                                qobject_cast<QMediaServiceFeaturesInterface*>(currentPlugin);

                        if (!iface || !(iface->supportedFeatures(type) &
                                        QMediaServiceProviderHint::LowLatencyPlayback)) {
                            plugin = currentPlugin;
                            break;
                        }

                    }
                }
                break;
            case QMediaServiceProviderHint::SupportedFeatures:
                plugin = plugins[0];
                foreach (QMediaServiceProviderPlugin *currentPlugin, plugins) {
                    QMediaServiceFeaturesInterface *iface =
                            qobject_cast<QMediaServiceFeaturesInterface*>(currentPlugin);

                    if (iface) {
                        if ((iface->supportedFeatures(type) & hint.features()) == hint.features()) {
                            plugin = currentPlugin;
                            break;
                        }
                    }
                }
                break;
            case QMediaServiceProviderHint::Device: {
                    foreach (QMediaServiceProviderPlugin *currentPlugin, plugins) {
                        QMediaServiceSupportedDevicesInterface *iface =
                                qobject_cast<QMediaServiceSupportedDevicesInterface*>(currentPlugin);

                        if (!iface) {
                            // the plugin may support the device,
                            // but this choice still can be overridden
                            plugin = currentPlugin;
                        } else {
                            if (iface->devices(type).contains(hint.device())) {
                                plugin = currentPlugin;
                                break;
                            }
                        }
                    }
                }
                break;
            case QMediaServiceProviderHint::ContentType: {
                    QtMediaServices::SupportEstimate estimate = QtMediaServices::NotSupported;
                    foreach (QMediaServiceProviderPlugin *currentPlugin, plugins) {
                        QtMediaServices::SupportEstimate currentEstimate = QtMediaServices::MaybeSupported;
                        QMediaServiceSupportedFormatsInterface *iface =
                                qobject_cast<QMediaServiceSupportedFormatsInterface*>(currentPlugin);

                        if (iface)
                            currentEstimate = iface->hasSupport(hint.mimeType(), hint.codecs());

                        if (currentEstimate > estimate) {
                            estimate = currentEstimate;
                            plugin = currentPlugin;

                            if (currentEstimate == QtMediaServices::PreferredService)
                                break;
                        }
                    }
                }
                break;
            }

            if (plugin != 0) {
                QMediaService *service = plugin->create(key);
                if (service != 0)
                    pluginMap.insert(service, plugin);

                return service;
            }
        }

        qWarning() << "defaultServiceProvider::requestService(): no service found for -" << key;
        return 0;
    }

    void releaseService(QMediaService *service)
    {
        if (service != 0) {
            QMediaServiceProviderPlugin *plugin = pluginMap.take(service);

            if (plugin != 0)
                plugin->release(service);
        }
    }

    QtMediaServices::SupportEstimate hasSupport(const QByteArray &serviceType,
                                     const QString &mimeType,
                                     const QStringList& codecs,
                                     int flags) const
    {
        QList<QObject*> instances = loader()->instances(
                QString::fromLatin1(serviceType.constData(),serviceType.length()));

        if (instances.isEmpty())
            return QtMediaServices::NotSupported;

        bool allServicesProvideInterface = true;
        QtMediaServices::SupportEstimate supportEstimate = QtMediaServices::NotSupported;

        foreach(QObject *obj, instances) {
            QMediaServiceSupportedFormatsInterface *iface =
                    qobject_cast<QMediaServiceSupportedFormatsInterface*>(obj);


            if (flags) {
                QMediaServiceFeaturesInterface *iface =
                        qobject_cast<QMediaServiceFeaturesInterface*>(obj);

                if (iface) {
                    QMediaServiceProviderHint::Features features = iface->supportedFeatures(serviceType);

                    //if low latency playback was asked, skip services known
                    //not to provide low latency playback
                    if ((flags & QMediaPlayer::LowLatency) &&
                        !(features & QMediaServiceProviderHint::LowLatencyPlayback))
                            continue;

                    //the same for QIODevice based streams support
                    if ((flags & QMediaPlayer::StreamPlayback) &&
                        !(features & QMediaServiceProviderHint::StreamPlayback))
                            continue;
                }
            }

            if (iface)
                supportEstimate = qMax(supportEstimate, iface->hasSupport(mimeType, codecs));
            else
                allServicesProvideInterface = false;
        }

        //don't return PreferredService
        supportEstimate = qMin(supportEstimate, QtMediaServices::ProbablySupported);

        //Return NotSupported only if no services are available of serviceType
        //or all the services returned NotSupported, otherwise return at least MaybeSupported
        if (!allServicesProvideInterface)
            supportEstimate = qMax(QtMediaServices::MaybeSupported, supportEstimate);

        return supportEstimate;
    }

    QStringList supportedMimeTypes(const QByteArray &serviceType, int flags) const
    {
        QList<QObject*> instances = loader()->instances(
                QString::fromLatin1(serviceType.constData(),serviceType.length()));

        QStringList supportedTypes;

        foreach(QObject *obj, instances) {
            QMediaServiceSupportedFormatsInterface *iface =
                    qobject_cast<QMediaServiceSupportedFormatsInterface*>(obj);


            if (flags & QMediaPlayer::LowLatency) {
                QMediaServiceFeaturesInterface *iface =
                        qobject_cast<QMediaServiceFeaturesInterface*>(obj);

                if (iface) {
                    QMediaServiceProviderHint::Features features = iface->supportedFeatures(serviceType);

                    // If low latency playback was asked for, skip MIME types from services known
                    // not to provide low latency playback
                    if ((flags & QMediaPlayer::LowLatency) &&
                        !(features & QMediaServiceProviderHint::LowLatencyPlayback))
                        continue;

                    //the same for QIODevice based streams support
                    if ((flags & QMediaPlayer::StreamPlayback) &&
                        !(features & QMediaServiceProviderHint::StreamPlayback))
                            continue;
                }
            }

            if (iface) {
                supportedTypes << iface->supportedMimeTypes();
            }
        }

        // Multiple services may support the same MIME type
        supportedTypes.removeDuplicates();

        return supportedTypes;
    }

    QList<QByteArray> devices(const QByteArray &serviceType) const
    {
        QList<QByteArray> res;

        foreach(QObject *obj, loader()->instances(
                    QString::fromLatin1(serviceType.constData(),serviceType.length()))) {
            QMediaServiceSupportedDevicesInterface *iface =
                    qobject_cast<QMediaServiceSupportedDevicesInterface*>(obj);

            if (iface) {
                res.append(iface->devices(serviceType));
            }
        }

        return res;
    }

    QString deviceDescription(const QByteArray &serviceType, const QByteArray &device)
    {
        foreach(QObject *obj, loader()->instances(
                    QString::fromLatin1(serviceType.constData(),serviceType.length()))) {
            QMediaServiceSupportedDevicesInterface *iface =
                    qobject_cast<QMediaServiceSupportedDevicesInterface*>(obj);

            if (iface) {
                if (iface->devices(serviceType).contains(device))
                    return iface->deviceDescription(serviceType, device);
            }
        }

        return QString();
    }
};

Q_GLOBAL_STATIC(QPluginServiceProvider, pluginProvider);

/*!
    \class QMediaServiceProvider
    \preliminary
    \since 4.7
    \brief The QMediaServiceProvider class provides an abstract allocator for media services.
*/

/*!
    \fn QMediaServiceProvider::requestService(const QByteArray &type, const QMediaServiceProviderHint &hint)

    Requests an instance of a \a type service which best matches the given \a hint.

    Returns a pointer to the requested service, or a null pointer if there is no suitable service.

    The returned service must be released with releaseService when it is finished with.
*/

/*!
    \fn QMediaServiceProvider::releaseService(QMediaService *service)

    Releases a media \a service requested with requestService().
*/

/*!
    \fn QtMediaServices::SupportEstimate QMediaServiceProvider::hasSupport(const QByteArray &serviceType, const QString &mimeType, const QStringList& codecs, int flags) const

    Returns how confident a media service provider is that is can provide a \a serviceType
    service that is able to play media of a specific \a mimeType that is encoded using the listed
    \a codecs while adhearing to constraints identified in \a flags.
*/
QtMediaServices::SupportEstimate QMediaServiceProvider::hasSupport(const QByteArray &serviceType,
                                                        const QString &mimeType,
                                                        const QStringList& codecs,
                                                        int flags) const
{
    Q_UNUSED(serviceType);
    Q_UNUSED(mimeType);
    Q_UNUSED(codecs);
    Q_UNUSED(flags);

    return QtMediaServices::MaybeSupported;
}

/*!
    \fn QStringList QMediaServiceProvider::supportedMimeTypes(const QByteArray &serviceType, int flags) const

    Returns a list of MIME types supported by the service provider for the specified \a serviceType.

    The resultant list is restricted to MIME types which can be supported given the constraints in \a flags.
*/
QStringList QMediaServiceProvider::supportedMimeTypes(const QByteArray &serviceType, int flags) const
{
    Q_UNUSED(serviceType);
    Q_UNUSED(flags);

    return QStringList();
}

/*!
  Returns the list of devices related to \a service type.
*/
QList<QByteArray> QMediaServiceProvider::devices(const QByteArray &service) const
{
    Q_UNUSED(service);
    return QList<QByteArray>();
}

/*!
    Returns the description of \a device related to \a serviceType,
    suitable to be displayed to user.
*/
QString QMediaServiceProvider::deviceDescription(const QByteArray &serviceType, const QByteArray &device)
{
    Q_UNUSED(serviceType);
    Q_UNUSED(device);
    return QString();
}


#ifdef QT_BUILD_INTERNAL

static QMediaServiceProvider *qt_defaultMediaServiceProvider = 0;

/*!
    Sets a media service \a provider as the default.

    \internal
*/
void QMediaServiceProvider::setDefaultServiceProvider(QMediaServiceProvider *provider)
{
    qt_defaultMediaServiceProvider = provider;
}

#endif

/*!
    Returns a default provider of media services.
*/
QMediaServiceProvider *QMediaServiceProvider::defaultServiceProvider()
{
#ifdef QT_BUILD_INTERNAL
    return qt_defaultMediaServiceProvider != 0
            ? qt_defaultMediaServiceProvider
            : static_cast<QMediaServiceProvider *>(pluginProvider());
#else
    return pluginProvider();
#endif
}

/*!
    \class QMediaServiceProviderPlugin
    \preliminary
    \since 4.7
    \brief The QMediaServiceProviderPlugin class interface provides an interface for QMediaService
    plug-ins.

    A media service provider plug-in may implement one or more of
    QMediaServiceSupportedFormatsInterface, QMediaServiceSupportedDevicesInterface,
    and QMediaServiceFeaturesInterface to identify the features it supports.
*/

/*!
    \fn QMediaServiceProviderPlugin::keys() const

    Returns a list of keys for media services a plug-in can create.
*/

/*!
    \fn QMediaServiceProviderPlugin::create(const QString &key)

    Constructs a new instance of the QMediaService identified by \a key.

    The QMediaService returned must be destroyed with release().
*/

/*!
    \fn QMediaServiceProviderPlugin::release(QMediaService *service)

    Destroys a media \a service constructed with create().
*/


/*!
    \class QMediaServiceSupportedFormatsInterface
    \brief The QMediaServiceSupportedFormatsInterface class interface
    identifies if a media service plug-in supports a media format.
    \since 4.7

    A QMediaServiceProviderPlugin may implement this interface.
*/

/*!
    \fn QMediaServiceSupportedFormatsInterface::~QMediaServiceSupportedFormatsInterface()

    Destroys a media service supported formats interface.
*/

/*!
    \fn QMediaServiceSupportedFormatsInterface::hasSupport(const QString &mimeType, const QStringList& codecs) const

    Returns the level of support a media service plug-in has for a \a mimeType and set of \a codecs.
*/

/*!
    \fn QMediaServiceSupportedFormatsInterface::supportedMimeTypes() const

    Returns a list of MIME types supported by the media service plug-in.
*/

/*!
    \class QMediaServiceSupportedDevicesInterface
    \brief The QMediaServiceSupportedDevicesInterface class interface
    identifies the devices supported by a media service plug-in.
    \since 4.7

    A QMediaServiceProviderPlugin may implement this interface.
*/

/*!
    \fn QMediaServiceSupportedDevicesInterface::~QMediaServiceSupportedDevicesInterface()

    Destroys a media service supported devices interface.
*/

/*!
    \fn QMediaServiceSupportedDevicesInterface::devices(const QByteArray &service) const

    Returns a list of devices supported by a plug-in \a service.
*/

/*!
    \fn QMediaServiceSupportedDevicesInterface::deviceDescription(const QByteArray &service, const QByteArray &device)

    Returns a description of a \a device supported by a plug-in \a service.
*/

/*!
    \class QMediaServiceFeaturesInterface
    \brief The QMediaServiceFeaturesInterface class interface identifies
    features supported by a media service plug-in.
    \since 4.7

    A QMediaServiceProviderPlugin may implement this interface.
*/

/*!
    \fn QMediaServiceFeaturesInterface::~QMediaServiceFeaturesInterface()

    Destroys a media service features interface.
*/
/*!
    \fn QMediaServiceFeaturesInterface::supportedFeatures(const QByteArray &service) const

    Returns a set of features supported by a plug-in \a service.
*/

QT_END_NAMESPACE

#include "moc_qmediaserviceprovider.cpp"
#include "moc_qmediaserviceproviderplugin.cpp"
