/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QFXRECT_H
#define QFXRECT_H

#include <QtDeclarative/qfxitem.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class Q_DECLARATIVE_EXPORT QFxPen : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    QFxPen(QObject *parent=0)
        : QObject(parent), _width(1), _color("#000000"), _valid(false)
    {}

    int width() const { return _width; }
    void setWidth(int w);

    QColor color() const { return _color; }
    void setColor(const QColor &c);

    bool isValid() { return _valid; };

Q_SIGNALS:
    void updated();

private:
    int _width;
    QColor _color;
    bool _valid;
};
QML_DECLARE_TYPE(QFxPen)

class Q_DECLARATIVE_EXPORT QFxGradientStop : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal position READ position WRITE setPosition)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    QFxGradientStop(QObject *parent=0) : QObject(parent) {}

    qreal position() const { return m_position; }
    void setPosition(qreal position) { m_position = position; updateGradient(); }

    QColor color() const { return m_color; }
    void setColor(const QColor &color) { m_color = color; updateGradient(); }

private:
    void updateGradient();

private:
    qreal m_position;
    QColor m_color;
};
QML_DECLARE_TYPE(QFxGradientStop)

class Q_DECLARATIVE_EXPORT QFxGradient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QFxGradientStop *> *stops READ stops)
    Q_CLASSINFO("DefaultProperty", "stops")

public:
    QFxGradient(QObject *parent=0) : QObject(parent), m_gradient(0) {}
    ~QFxGradient() { delete m_gradient; }

    QList<QFxGradientStop *> *stops() { return &m_stops; }

    const QGradient *gradient() const;

Q_SIGNALS:
    void updated();

private:
    void doUpdate();

private:
    QList<QFxGradientStop *> m_stops;
    mutable QGradient *m_gradient;
    friend class QFxGradientStop;
};
QML_DECLARE_TYPE(QFxGradient)

class QFxRectPrivate;
class Q_DECLARATIVE_EXPORT QFxRect : public QFxItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor tintColor READ tintColor WRITE setTintColor)
    Q_PROPERTY(QFxGradient *gradient READ gradient WRITE setGradient)
    Q_PROPERTY(QFxPen * pen READ pen)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)
public:
    QFxRect(QFxItem *parent=0);

    QColor color() const;
    void setColor(const QColor &);

    QColor tintColor() const;
    void setTintColor(const QColor &);

    QFxPen *pen();

    QFxGradient *gradient() const;
    void setGradient(QFxGradient *gradient);

    qreal radius() const;
    void setRadius(qreal radius);

    virtual void dump(int depth);
#if defined(QFX_RENDER_QPAINTER)
    void paintContents(QPainter &painter);
#endif

#if defined(QFX_RENDER_OPENGL)
    void paintGLContents(GLPainter &);
#endif

private Q_SLOTS:
    void doUpdate();

private:
    void generateRoundedRect();
    void generateBorderedRect();
#if defined(QFX_RENDER_QPAINTER)
    void drawRect(QPainter &painter);
#endif
protected:
    QFxRect(QFxRectPrivate &dd, QFxItem *parent);

private:
    Q_DISABLE_COPY(QFxRect)
    Q_DECLARE_PRIVATE(QFxRect)
};
QML_DECLARE_TYPE(QFxRect)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFXRECT_H
