/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "audiooutput.h"
#include "mediaobject.h"
#include "videowidget.h"

#include "backend.h"

#include <QtCore/QSet>
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>

QT_BEGIN_NAMESPACE

Q_EXPORT_PLUGIN2(phonon_dummy, Phonon::Dummy::Backend)

namespace Phonon
{
namespace Dummy
{

Backend::Backend(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    qWarning()<<"Using TEST Phonon backend";
}

Backend::~Backend()
{
}

/***
 * !reimp
 */
QObject *Backend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &args)
{
    Q_UNUSED(args)

    switch (c) {
    case MediaObjectClass:
        return new MediaObject(this, parent);

    case AudioOutputClass: {
            AudioOutput *ao = new AudioOutput(this, parent);
            m_audioOutputs.append(ao);
            return ao;
        }
    case VideoWidgetClass: {
            QWidget *widget =  qobject_cast<QWidget*>(parent);
            return new VideoWidget(this, widget);
        }
    default:
        qWarning("createObject() : Backend object not available");
    }
    return 0;
}

/***
 * !reimp
 */
QStringList Backend::availableMimeTypes() const
{
    QStringList availableMimeTypes;
    // audio *.wav and *.mp3 files
    availableMimeTypes << QLatin1String("audio/x-mp3");
    availableMimeTypes << QLatin1String("audio/x-wav");

    // video *.ogv, *.mp4, *.avi (some)

    availableMimeTypes << QLatin1String("video/mpeg");
    availableMimeTypes << QLatin1String("video/ogg");
    availableMimeTypes << QLatin1String("video/mp4");

    return availableMimeTypes;
}

/***
 * !reimp
 */
QList<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const
{
    QList<int> list;

    if(type == Phonon::AudioOutputDeviceType)
        list.append(0);

    return list;
}

/***
 * !reimp
 */
QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const
{
    Q_UNUSED(index);
    QHash<QByteArray, QVariant> ret;

    if(type == Phonon::AudioOutputDeviceType)
        ret["name"] = QLatin1String("default audio device");

    return ret;
}

/***
 * !reimp
 */
bool Backend::startConnectionChange(QSet<QObject *> objects)
{
    Q_UNUSED(objects)

    return true;
}

/***
 * !reimp
 */
bool Backend::connectNodes(QObject *source, QObject *sink)
{
    Q_UNUSED(source)
    Q_UNUSED(sink)

    return true;
}

/***
 * !reimp
 */
bool Backend::disconnectNodes(QObject *source, QObject *sink)
{
    Q_UNUSED(source)
    Q_UNUSED(sink)

    return true;
}

/***
 * !reimp
 */
bool Backend::endConnectionChange(QSet<QObject *> objects)
{
    Q_UNUSED(objects)

    return true;
}

}
}

QT_END_NAMESPACE

#include "moc_backend.cpp"
