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

#ifndef QMETADATACONTROL_H
#define QMETADATACONTROL_H

#include <QtMediaServices/qmediacontrol.h>
#include <QtMediaServices/qmediaobject.h>
#include <QtMediaServices/qmediaresource.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)

class Q_MEDIASERVICES_EXPORT QMetaDataControl : public QMediaControl
{
    Q_OBJECT

public:
    ~QMetaDataControl();

    virtual bool isWritable() const = 0;
    virtual bool isMetaDataAvailable() const = 0;

    virtual QVariant metaData(QtMediaServices::MetaData key) const = 0;
    virtual void setMetaData(QtMediaServices::MetaData key, const QVariant &value) = 0;
    virtual QList<QtMediaServices::MetaData> availableMetaData() const = 0;

    virtual QVariant extendedMetaData(const QString &key) const = 0;
    virtual void setExtendedMetaData(const QString &key, const QVariant &value) = 0;
    virtual QStringList availableExtendedMetaData() const = 0;

Q_SIGNALS:
    void metaDataChanged();

    void writableChanged(bool writable);
    void metaDataAvailableChanged(bool available);

protected:
    QMetaDataControl(QObject *parent = 0);
};

#define QMetaDataControl_iid "com.nokia.Qt.QMetaDataControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QMetaDataControl, QMetaDataControl_iid)

QT_END_NAMESPACE

QT_END_HEADER


#endif  // QMETADATAPROVIDER_H
