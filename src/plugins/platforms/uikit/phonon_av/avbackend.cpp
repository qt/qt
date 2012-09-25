/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "avbackend.h"
#include "avaudiooutput.h"
#include "avmediaobject.h"

#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <QtCore/QtPlugin>

AVBackend::AVBackend(QObject *parent)
    : QObject(parent)
{
}

AVBackend::~AVBackend()
{
}

QObject *AVBackend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &args)
{
    Q_UNUSED(args);
    switch (c)
    {
    case MediaObjectClass:
        return new AVMediaObject(parent);
    case AudioOutputClass:
        return new AVAudioOutput(parent);
    default:
        return 0;
    }
}

QList<int> AVBackend::objectDescriptionIndexes(Phonon::ObjectDescriptionType type) const
{
    if (type == Phonon::AudioOutputDeviceType)
        return QList<int>() << 0;
    return QList<int>();
}

QHash<QByteArray, QVariant> AVBackend::objectDescriptionProperties(Phonon::ObjectDescriptionType type, int index) const
{
    Q_UNUSED(index);
    QHash<QByteArray, QVariant> r;
    if (type == Phonon::AudioOutputDeviceType)
        r["name"] = QLatin1String("AVAudioPlayer");
    return r;
}

bool AVBackend::startConnectionChange(QSet<QObject *> connection)
{
    Q_UNUSED(connection)
    return true;
}

bool AVBackend::connectNodes(QObject *node1, QObject *node2)
{
    AVMediaObject *media = qobject_cast<AVMediaObject*>(node1);
    AVAudioOutput *output = qobject_cast<AVAudioOutput*>(node2);
    if (media && output)
        media->setAudioOutput(output);
    return true;
}

bool AVBackend::disconnectNodes(QObject *node1, QObject *node2)
{
    AVMediaObject *media = qobject_cast<AVMediaObject*>(node1);
    AVAudioOutput *output = qobject_cast<AVAudioOutput*>(node2);
    if (media && output)
        media->setAudioOutput(0);
    return true;
}

bool AVBackend::endConnectionChange(QSet<QObject *> connection)
{
    Q_UNUSED(connection)
    return true;
}

QStringList AVBackend::availableMimeTypes() const
{
    return QStringList();
}

Q_EXPORT_PLUGIN2(phonon_av, AVBackend)
