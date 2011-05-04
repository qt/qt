/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
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

#ifndef PARTICLEEXTRUDER_H
#define PARTICLEEXTRUDER_H

#include <QObject>
#include <QRectF>
#include <QPointF>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class ParticleExtruder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool fill READ fill WRITE setFill NOTIFY fillChanged)//###Should this be base class, or a BoxExtruder?

public:
    explicit ParticleExtruder(QObject *parent = 0);
    virtual QPointF extrude(const QRectF &);
    virtual bool contains(const QRectF &bounds, const QPointF &point);//###Needed for follow emitter, but does it belong? Only marginally conceptually valid, and that's from user's perspective
    bool fill() const
    {
        return m_fill;
    }

signals:

    void fillChanged(bool arg);

public slots:

    void setFill(bool arg)
    {
        if (m_fill != arg) {
            m_fill = arg;
            emit fillChanged(arg);
        }
    }
protected:
    bool m_fill;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // PARTICLEEXTRUDER_H
