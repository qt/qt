/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#ifndef TESTTYPES_H
#define TESTTYPES_H

#include <QObject>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>
#include <QRect>
#include <QRectF>
#include <QVector3D>
#include <QFont>
#include <qdeclarative.h>
#include <QDeclarativePropertyValueSource>
#include <QDeclarativeProperty>
#include <private/qdeclarativeproperty_p.h>

class MyTypeObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPoint point READ point WRITE setPoint NOTIFY changed)
    Q_PROPERTY(QPointF pointf READ pointf WRITE setPointf NOTIFY changed)
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY changed)
    Q_PROPERTY(QSizeF sizef READ sizef WRITE setSizef NOTIFY changed)
    Q_PROPERTY(QSize sizereadonly READ size NOTIFY changed)
    Q_PROPERTY(QRect rect READ rect WRITE setRect NOTIFY changed)
    Q_PROPERTY(QRectF rectf READ rectf WRITE setRectf NOTIFY changed)
    Q_PROPERTY(QVector3D vector READ vector WRITE setVector NOTIFY changed)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY changed)

public:
    MyTypeObject() :
        m_point(10, 4),
        m_pointf(11.3, -10.9),
        m_size(1912, 1913),
        m_sizef(0.1, 100923.2),
        m_rect(2, 3, 109, 102),
        m_rectf(103.8, 99.2, 88.1, 77.6),
        m_vector(23.88, 3.1, 4.3)
    {
        m_font.setFamily("Arial");
        m_font.setBold(true);
        m_font.setWeight(QFont::DemiBold);
        m_font.setItalic(true);
        m_font.setUnderline(true);
        m_font.setOverline(true);
        m_font.setStrikeOut(true);
        m_font.setPointSize(29);
        m_font.setCapitalization(QFont::AllUppercase);
        m_font.setLetterSpacing(QFont::AbsoluteSpacing, 10.2);
        m_font.setWordSpacing(19.7);
    }

    QPoint m_point;
    QPoint point() const { return m_point; }
    void setPoint(const QPoint &v) { m_point = v; emit changed(); }

    QPointF m_pointf;
    QPointF pointf() const { return m_pointf; }
    void setPointf(const QPointF &v) { m_pointf = v; emit changed(); }

    QSize m_size;
    QSize size() const { return m_size; }
    void setSize(const QSize &v) { m_size = v; emit changed(); }

    QSizeF m_sizef;
    QSizeF sizef() const { return m_sizef; }
    void setSizef(const QSizeF &v) { m_sizef = v; emit changed(); }

    QRect m_rect;
    QRect rect() const { return m_rect; }
    void setRect(const QRect &v) { m_rect = v; emit changed(); }

    QRectF m_rectf;
    QRectF rectf() const { return m_rectf; }
    void setRectf(const QRectF &v) { m_rectf = v; emit changed(); }

    QVector3D m_vector;
    QVector3D vector() const { return m_vector; }
    void setVector(const QVector3D &v) { m_vector = v; emit changed(); }

    QFont m_font;
    QFont font() const { return m_font; }
    void setFont(const QFont &v) { m_font = v; emit changed(); }

    void emitRunScript() { emit runScript(); }

signals:
    void changed();
    void runScript();

public slots:
    QSize method() { return QSize(13, 14); }
};
QML_DECLARE_TYPE(MyTypeObject);

class MyConstantValueSource : public QObject, public QDeclarativePropertyValueSource
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativePropertyValueSource)
public:
    virtual void setTarget(const QDeclarativeProperty &p) { p.write(3345); }
};
QML_DECLARE_TYPE(MyConstantValueSource);

class MyOffsetValueInterceptor : public QObject, public QDeclarativePropertyValueInterceptor
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativePropertyValueInterceptor)
public:
    virtual void setTarget(const QDeclarativeProperty &p) { prop = p; }
    virtual void write(const QVariant &value) { QDeclarativePropertyPrivate::write(prop, value.toInt() + 13, QDeclarativePropertyPrivate::BypassInterceptor); }

private:
    QDeclarativeProperty prop;
};
QML_DECLARE_TYPE(MyOffsetValueInterceptor);

void registerTypes();

#endif // TESTTYPES_H
