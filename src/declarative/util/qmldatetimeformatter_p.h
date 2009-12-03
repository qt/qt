/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QMLDATETIMEFORMATTER_H
#define QMLDATETIMEFORMATTER_H

#include <qml.h>

#include <QtCore/qdatetime.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlDateTimeFormatterPrivate;
class Q_DECLARATIVE_EXPORT QmlDateTimeFormatter : public QObject, public QmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QmlParserStatus)

    Q_PROPERTY(QString dateText READ dateText NOTIFY textChanged)
    Q_PROPERTY(QString timeText READ timeText NOTIFY textChanged)
    Q_PROPERTY(QString dateTimeText READ dateTimeText NOTIFY textChanged)
    Q_PROPERTY(QDate date READ date WRITE setDate)
    Q_PROPERTY(QTime time READ time WRITE setTime)
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)
    Q_PROPERTY(QString dateFormat READ dateFormat WRITE setDateFormat)
    Q_PROPERTY(QString timeFormat READ timeFormat WRITE setTimeFormat)
    Q_PROPERTY(QString dateTimeFormat READ dateTimeFormat WRITE setDateTimeFormat)
    Q_PROPERTY(bool longStyle READ longStyle WRITE setLongStyle)
public:
    QmlDateTimeFormatter(QObject *parent=0);
    ~QmlDateTimeFormatter();

    QString dateTimeText() const;
    QString dateText() const;
    QString timeText() const;

    QDate date() const;
    void setDate(const QDate &);

    QTime time() const;
    void setTime(const QTime &);

    QDateTime dateTime() const;
    void setDateTime(const QDateTime &);

    QString dateTimeFormat() const;
    void setDateTimeFormat(const QString &);

    QString dateFormat() const;
    void setDateFormat(const QString &);

    QString timeFormat() const;
    void setTimeFormat(const QString &);

    bool longStyle() const;
    void setLongStyle(bool);

    virtual void classBegin();
    virtual void componentComplete();

Q_SIGNALS:
    void textChanged();

private:
    Q_DISABLE_COPY(QmlDateTimeFormatter)
    Q_DECLARE_PRIVATE(QmlDateTimeFormatter)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlDateTimeFormatter)

QT_END_HEADER

#endif
