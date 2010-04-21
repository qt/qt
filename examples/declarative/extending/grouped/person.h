/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QColor>

class ShoeDescription : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int size READ size WRITE setSize)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QString brand READ brand WRITE setBrand)
    Q_PROPERTY(qreal price READ price WRITE setPrice)
public:
    ShoeDescription(QObject *parent = 0);

    int size() const;
    void setSize(int);

    QColor color() const;
    void setColor(const QColor &);

    QString brand() const;
    void setBrand(const QString &);

    qreal price() const;
    void setPrice(qreal);
private:
    int m_size;
    QColor m_color;
    QString m_brand;
    qreal m_price;
};

class Person : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
// ![1]
    Q_PROPERTY(ShoeDescription *shoe READ shoe)
// ![1]
public:
    Person(QObject *parent = 0);

    QString name() const;
    void setName(const QString &);

    ShoeDescription *shoe();
private:
    QString m_name;
    ShoeDescription m_shoe;
};

class Boy : public Person
{
    Q_OBJECT
public:
    Boy(QObject * parent = 0);
};

class Girl : public Person
{
    Q_OBJECT
public:
    Girl(QObject * parent = 0);
};

#endif // PERSON_H
