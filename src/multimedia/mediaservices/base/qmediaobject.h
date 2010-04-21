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

#ifndef QABSTRACTMEDIAOBJECT_H
#define QABSTRACTMEDIAOBJECT_H

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

#include <QtMediaServices/qtmedianamespace.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)

class QMediaService;

class QMediaObjectPrivate;
class Q_MEDIASERVICES_EXPORT QMediaObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int notifyInterval READ notifyInterval WRITE setNotifyInterval NOTIFY notifyIntervalChanged)
    Q_PROPERTY(bool metaDataAvailable READ isMetaDataAvailable NOTIFY metaDataAvailableChanged)
    Q_PROPERTY(bool metaDataWritable READ isMetaDataWritable NOTIFY metaDataWritableChanged)

public:
    ~QMediaObject();

    virtual bool isAvailable() const;
    virtual QtMediaServices::AvailabilityError availabilityError() const;

    virtual QMediaService* service() const;

    int notifyInterval() const;
    void setNotifyInterval(int milliSeconds);

    virtual void bind(QObject*);
    virtual void unbind(QObject*);

    bool isMetaDataAvailable() const;
    bool isMetaDataWritable() const;

    QVariant metaData(QtMediaServices::MetaData key) const;
    void setMetaData(QtMediaServices::MetaData key, const QVariant &value);
    QList<QtMediaServices::MetaData> availableMetaData() const;

    QVariant extendedMetaData(const QString &key) const;
    void setExtendedMetaData(const QString &key, const QVariant &value);
    QStringList availableExtendedMetaData() const;

Q_SIGNALS:
    void notifyIntervalChanged(int milliSeconds);

    void metaDataAvailableChanged(bool available);
    void metaDataWritableChanged(bool writable);
    void metaDataChanged();

    void availabilityChanged(bool available);

protected:
    QMediaObject(QObject *parent, QMediaService *service);
    QMediaObject(QMediaObjectPrivate &dd, QObject *parent, QMediaService *service);

    void addPropertyWatch(QByteArray const &name);
    void removePropertyWatch(QByteArray const &name);

    QMediaObjectPrivate *d_ptr;

private:
    void setupMetaData();

    Q_DECLARE_PRIVATE(QMediaObject)
    Q_PRIVATE_SLOT(d_func(), void _q_notify())
};

QT_END_NAMESPACE

QT_END_HEADER


#endif  // QABSTRACTMEDIAOBJECT_H
