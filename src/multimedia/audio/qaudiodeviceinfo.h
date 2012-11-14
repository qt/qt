/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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


#ifndef QAUDIODEVICEINFO_H
#define QAUDIODEVICEINFO_H

#include <QtCore/qobject.h>
#include <QtCore/qglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qlist.h>

#include <QtMultimedia/qaudio.h>
#include <QtMultimedia/qaudioformat.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)

class QAudioDeviceFactory;

class QAudioDeviceInfoPrivate;
class Q_MULTIMEDIA_EXPORT QAudioDeviceInfo
{
    friend class QAudioDeviceFactory;

public:
    QAudioDeviceInfo();
    QAudioDeviceInfo(const QAudioDeviceInfo& other);
    ~QAudioDeviceInfo();

    QAudioDeviceInfo& operator=(const QAudioDeviceInfo& other);

    bool isNull() const;

    QString deviceName() const;

    bool isFormatSupported(const QAudioFormat &format) const;
    QAudioFormat preferredFormat() const;
    QAudioFormat nearestFormat(const QAudioFormat &format) const;

    QStringList supportedCodecs() const;
    QList<int> supportedFrequencies() const;
    QList<int> supportedSampleRates() const;
    QList<int> supportedChannels() const;
    QList<int> supportedChannelCounts() const;
    QList<int> supportedSampleSizes() const;
    QList<QAudioFormat::Endian> supportedByteOrders() const;
    QList<QAudioFormat::SampleType> supportedSampleTypes() const;

    static QAudioDeviceInfo defaultInputDevice();
    static QAudioDeviceInfo defaultOutputDevice();

    static QList<QAudioDeviceInfo> availableDevices(QAudio::Mode mode);

private:
    QAudioDeviceInfo(const QString &realm, const QByteArray &handle, QAudio::Mode mode);
    QString realm() const;
    QByteArray handle() const;
    QAudio::Mode mode() const;

    QSharedDataPointer<QAudioDeviceInfoPrivate> d;
};

QT_END_NAMESPACE

QT_END_HEADER

Q_DECLARE_METATYPE(QAudioDeviceInfo)

#endif // QAUDIODEVICEINFO_H
