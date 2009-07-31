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

#include "qfxrect.h"
#include "qfxrect_p.h"

#include <QPainter>

QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Pen,QFxPen)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,GradientStop,QFxGradientStop)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Gradient,QFxGradient)

/*!
    \internal
    \class QFxPen
    \brief The QFxPen class provides a pen used for drawing rect borders on a QFxView.

    By default, the pen is invalid and nothing is drawn. You must either set a color (then the default
    width is 1) or a width (then the default color is black).

    A width of 1 indicates is a single-pixel line on the border of the item being painted.

    Example:
    \qml
    Rect { border.width: 2; border.color: "red" ... }
    \endqml
*/

/*! \property QFxPen::width
    \brief the width of the border.

    A width of 1 is a single-pixel line on the border of the item being painted.

    If the width is less than 1 the pen is considered invalid and won't be used.
*/

/*!
    \property QFxPen::color
    \brief the color of the border.

    color is most commonly specified in hexidecimal notation (#RRGGBB)
    or as an \l {http://www.w3.org/TR/SVG/types.html#ColorKeywords}{SVG color keyword name}
     (as defined by the World Wide Web Consortium). For example:
    \qml
    // rect with green border using hexidecimal notation
    Rect { border.color: "#00FF00" }

    // rect with steelblue border using SVG color name
    Rect { border.color: "steelblue" }
    \endqml

    For the full set of ways to specify color, see Qt's QColor::setNamedColor documentation.
*/

void QFxPen::setColor(const QColor &c)
{
    _color = c;
    _valid = _color.alpha() ? true : false;
    emit updated();
}

/*!
    \property QFxPen::width
    \brief the width of the border.

    \qml
    Rect { border.width: 4 }
    \endqml

    A width of 1 creates a thin line. For no line, use a width of 0 or a transparent color.

    To keep the border smooth (rather than blurry), odd pen widths cause the rect to be painted at
    a half-pixel offset;
*/
void QFxPen::setWidth(int w)
{
    _width = w;
    _valid = (_width < 1) ? false : true;
    emit updated();
}


/*!
    \qmlclass GradientStop QFxGradientStop
    \brief The GradientStop item defines the color at a position in a Gradient

    \sa Gradient
*/

/*!
    \qmlproperty real GradientStop::position
    \qmlproperty color GradientStop::color

    Sets a \e color at a \e position in a gradient.
*/

void QFxGradientStop::updateGradient()
{
    if (QFxGradient *grad = qobject_cast<QFxGradient*>(parent()))
        grad->doUpdate();
}

/*!
    \qmlclass Gradient QFxGradient
    \brief The Gradient item defines a gradient fill.

    A gradient is defined by two or more colors, which will be blended seemlessly.  The
    colors are specified at their position in the range 0.0 - 1.0 via
    the GradientStop item.  For example, the following code paints a
    Rect with a gradient starting with red, blending to yellow at 1/3 of the
    size of the Rect, and ending with Green:

    \table
    \row
    \o \image gradient.png
    \o \quotefile doc/src/snippets/declarative/gradient.qml
    \endtable

    \sa GradientStop
*/

/*!
    \qmlproperty list<GradientStop> Gradient::stops
    This property holds the gradient stops describing the gradient.
*/

const QGradient *QFxGradient::gradient() const
{
    if (!m_gradient && !m_stops.isEmpty()) {
        m_gradient = new QLinearGradient(0,0,0,1.0);
        for (int i = 0; i < m_stops.count(); ++i) {
            const QFxGradientStop *stop = m_stops.at(i);
            m_gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
            m_gradient->setColorAt(stop->position(), stop->color());
        }
    }

    return m_gradient;
}

void QFxGradient::doUpdate()
{
    delete m_gradient;
    m_gradient = 0;
    emit updated();
}

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Rect,QFxRect)

/*!
    \qmlclass Rect QFxRect
    \brief The Rect item allows you to add rectangles to a scene.
    \inherits Item

    A Rect is painted having a solid fill (color) and an optional border (pen).
    You can also create rounded rectangles using the radius property.

    \qml
    Rect {
        width: 100
        height: 100
        color: "red"
        border.color: "black"
        border.width: 5
        radius: 10
    }
    \endqml

    \image declarative-rect.png
*/

/*!
    \internal
    \class QFxRect
    \brief The QFxRect class provides a rect item that you can add to a QFxView.
*/
QFxRect::QFxRect(QFxItem *parent)
  : QFxItem(*(new QFxRectPrivate), parent)
{
    Q_D(QFxRect);
    d->init();
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QFxRect::QFxRect(QFxRectPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    Q_D(QFxRect);
    d->init();
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

void QFxRect::doUpdate()
{
    Q_D(QFxRect);
    d->rectImage = QPixmap();
    const int pw = d->pen && d->pen->isValid() ? d->pen->width() : 0;
    d->setPaintMargin((pw+1)/2);
    update();
}

/*!
    \qmlproperty int Rect::border.width
    \qmlproperty color Rect::border.color

    The width and color used to draw the border of the rect.

    A width of 1 creates a thin line. For no line, use a width of 0 or a transparent color.

    To keep the border smooth (rather than blurry), odd widths cause the rect to be painted at
    a half-pixel offset;
*/
QFxPen *QFxRect::border()
{
    Q_D(QFxRect);
    return d->getPen();
}

/*!
    \qmlproperty Gradient Rect::gradient

    The gradient to use to fill the rect.

    This property allows for the construction of simple vertical gradients.
    Other gradients may by formed by adding rotation to the rect.

    \table
    \row
    \o \image declarative-rect_gradient.png
    \o
    \qml
    Rect { y: 0; width: 80; height: 80; color: "lightsteelblue" }
    Rect { y: 100; width: 80; height: 80
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightsteelblue" }
            GradientStop { position: 1.0; color: "blue" }
        }
    }
    Rect { rotation: 90; x: 80; y: 200; width: 80; height: 80
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightsteelblue" }
            GradientStop { position: 1.0; color: "blue" }
        }
    }
    // The x offset is needed because the rotation is from the top left corner
    \endqml
    \endtable

    If both a gradient and a color are specified, the gradient will be used.

    \sa Gradient, color
*/
QFxGradient *QFxRect::gradient() const
{
    Q_D(const QFxRect);
    return d->gradient;
}

void QFxRect::setGradient(QFxGradient *gradient)
{
    Q_D(QFxRect);
    if (d->gradient == gradient)
        return;
    if (d->gradient)
        disconnect(d->gradient, SIGNAL(updated()), this, SLOT(doUpdate()));
    d->gradient = gradient;
    if (d->gradient)
        connect(d->gradient, SIGNAL(updated()), this, SLOT(doUpdate()));
    update();
}


/*!
    \qmlproperty real Rect::radius
    This property holds the corner radius used to draw a rounded rect.

    If radius is non-zero, the rect will be painted as a rounded rectangle, otherwise it will be
    painted as a normal rectangle. The same radius is used by all 4 corners; there is currently
    no way to specify different radii for different corners.
*/
qreal QFxRect::radius() const
{
    Q_D(const QFxRect);
    return d->radius;
}

void QFxRect::setRadius(qreal radius)
{
    Q_D(QFxRect);
    if (d->radius == radius)
        return;

    d->radius = radius;
    d->rectImage = QPixmap();
    update();
}

/*!
    \qmlproperty color Rect::color
    This property holds the color used to fill the rect.

    \qml
    // green rect using hexidecimal notation
    Rect { color: "#00FF00" }

    // steelblue rect using SVG color name
    Rect { color: "steelblue" }
    \endqml

    The default color is white.

    If both a gradient and a color are specified, the gradient will be used.
*/
QColor QFxRect::color() const
{
    Q_D(const QFxRect);
    return d->color;
}

void QFxRect::setColor(const QColor &c)
{
    Q_D(QFxRect);
    if (d->color == c)
        return;

    d->color = c;
    d->rectImage = QPixmap();
    update();
}

/*!
    \qmlproperty color Rect::tintColor
    This property holds The color to tint the rectangle.

    This color will be drawn over the rect's color when the rect is painted. The tint color should usually be mostly transparent, or you will not be able to see the underlying color. The below example provides a slight red tint by having the tint color be pure red which is only 1/16th opaque.

    \qml
    Rect { x: 0; width: 80; height: 80; color: "lightsteelblue" }
    Rect { x: 100; width: 80; height: 80; color: "lightsteelblue"; tintColor: "#10FF0000" }
    \endqml
    \image declarative-rect_tint.png

    This attribute is not intended to be used with a single color over the lifetime of an user interface. It is most useful when a subtle change is intended to be conveyed due to some event; you can then use the tint color to more effectively tune the visible color.
*/
QColor QFxRect::tintColor() const
{
    Q_D(const QFxRect);
    return d->tintColor;
}

void QFxRect::setTintColor(const QColor &c)
{
    Q_D(QFxRect);
    if (d->tintColor == c)
        return;

    d->tintColor = c;
    update();
}

QColor QFxRectPrivate::getColor()
{
    if (tintColor.isValid()) {
        int a = tintColor.alpha();
        if (a == 0xFF)
            return tintColor;
        else if (a == 0x00)
            return color;
        else {
            uint src = tintColor.rgba();
            uint dest = color.rgba();

            uint res = (((a * (src & 0xFF00FF)) +
                        ((0xFF - a) * (dest & 0xFF00FF))) >> 8) & 0xFF00FF;
            res |= (((a * ((src >> 8) & 0xFF00FF)) +
                    ((0xFF - a) * ((dest >> 8) & 0xFF00FF)))) & 0xFF00FF00;
            if ((src & 0xFF000000) == 0xFF000000)
                res |= 0xFF000000;

            return QColor::fromRgba(res);
        }
    } else {
        return color;
    }
}


void QFxRect::generateRoundedRect()
{
    Q_D(QFxRect);
    if (d->rectImage.isNull()) {
        const int pw = d->pen && d->pen->isValid() ? d->pen->width() : 0;
        d->rectImage = QPixmap(d->radius*2 + 3 + pw*2, d->radius*2 + 3 + pw*2);
        d->rectImage.fill(Qt::transparent);
        QPainter p(&(d->rectImage));
        p.setRenderHint(QPainter::Antialiasing);
        if (d->pen && d->pen->isValid()) {
            QPen pn(QColor(d->pen->color()), d->pen->width());
            p.setPen(pn);
        } else {
            p.setPen(Qt::NoPen);
        }
        p.setBrush(d->color);
        if (pw%2)
            p.drawRoundedRect(QRectF(qreal(pw)/2+1, qreal(pw)/2+1, d->rectImage.width()-(pw+1), d->rectImage.height()-(pw+1)), d->radius, d->radius);
        else
            p.drawRoundedRect(QRectF(qreal(pw)/2, qreal(pw)/2, d->rectImage.width()-pw, d->rectImage.height()-pw), d->radius, d->radius);
    }
}

void QFxRect::generateBorderedRect()
{
    Q_D(QFxRect);
    if (d->rectImage.isNull()) {
        const int pw = d->pen && d->pen->isValid() ? d->pen->width() : 0;
        d->rectImage = QPixmap(d->getPen()->width()*2 + 3 + pw*2, d->getPen()->width()*2 + 3 + pw*2);
        d->rectImage.fill(Qt::transparent);
        QPainter p(&(d->rectImage));
        p.setRenderHint(QPainter::Antialiasing);
        if (d->pen && d->pen->isValid()) {
            QPen pn(QColor(d->pen->color()), d->pen->width());
            pn.setJoinStyle(Qt::MiterJoin);
            p.setPen(pn);
        } else {
            p.setPen(Qt::NoPen);
        }
        p.setBrush(d->color);
        if (pw%2)
            p.drawRect(QRectF(qreal(pw)/2+1, qreal(pw)/2+1, d->rectImage.width()-(pw+1), d->rectImage.height()-(pw+1)));
        else
            p.drawRect(QRectF(qreal(pw)/2, qreal(pw)/2, d->rectImage.width()-pw, d->rectImage.height()-pw));
    }
}

void QFxRect::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(QFxRect);
    if (d->radius > 0 || (d->pen && d->pen->isValid())
           || (d->gradient && d->gradient->gradient()) )
        drawRect(*p);
    else
        p->fillRect(QRect(0, 0, width(), height()), d->getColor());
}

void QFxRect::drawRect(QPainter &p)
{
    Q_D(QFxRect);
    if (d->gradient && d->gradient->gradient() /*|| p.usingQt() */) {
        // XXX This path is still slower than the image path
        // Image path won't work for gradients though
        bool oldAA = p.testRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::Antialiasing);
        if (d->pen && d->pen->isValid()) {
            QPen pn(QColor(d->pen->color()), d->pen->width());
            p.setPen(pn);
        } else {
            p.setPen(Qt::NoPen);
        }
        p.setBrush(*d->gradient->gradient());
        if (d->radius > 0.)
            p.drawRoundedRect(0, 0, width(), height(), d->radius, d->radius);
        else
            p.drawRect(0, 0, width(), height());
        p.setRenderHint(QPainter::Antialiasing, oldAA);
    } else {
        int offset = 0;
        const int pw = d->pen && d->pen->isValid() ? (d->pen->width()+1)/2*2 : 0;

        if (d->radius > 0) {
            generateRoundedRect();
            //### implicit conversion to int
            offset = int(d->radius+1.5+pw);
        } else {
            generateBorderedRect();
            offset = pw+1;
        }

        //basically same code as QFxImage uses to paint sci images
        int w = width()+pw;
        int h = height()+pw;
        int xOffset = offset;
        int xSide = xOffset * 2;
        bool xMiddles=true;
        if (xSide > w) {
            xMiddles=false;
            xOffset = w/2 + 1;
            xSide = xOffset * 2;
        }
        int yOffset = offset;
        int ySide = yOffset * 2;
        bool yMiddles=true;
        if (ySide > h) {
            yMiddles = false;
            yOffset = h/2 + 1;
            ySide = yOffset * 2;
        }

        // Upper left
        p.drawPixmap(QRect(-pw/2, -pw/2, xOffset, yOffset), d->rectImage, QRect(0, 0, xOffset, yOffset));

        // Upper middle
        if (xMiddles)
            p.drawPixmap(QRect(xOffset-pw/2, -pw/2, width() - xSide + pw, yOffset), d->rectImage,
                                  QRect(d->rectImage.width()/2, 0, 1, yOffset));
        // Upper right
        p.drawPixmap(QPoint(width()-xOffset+pw/2, -pw/2), d->rectImage,
                              QRect(d->rectImage.width()-xOffset, 0, xOffset, yOffset));
        // Middle left
        if (yMiddles)
            p.drawPixmap(QRect(-pw/2, yOffset-pw/2, xOffset, height() - ySide + pw), d->rectImage,
                                  QRect(0, d->rectImage.height()/2, xOffset, 1));

        // Middle
        if (xMiddles && yMiddles)
            // XXX paint errors in animation example
            //p.fillRect(xOffset-pw/2, yOffset-pw/2, width() - xSide + pw, height() - ySide + pw, d->getColor());
            p.drawPixmap(QRect(xOffset-pw/2, yOffset-pw/2, width() - xSide + pw, height() - ySide + pw), d->rectImage,
                                QRect(d->rectImage.width()/2, d->rectImage.height()/2, 1, 1));
        // Middle right
        if (yMiddles)
            p.drawPixmap(QRect(width()-xOffset+pw/2, yOffset-pw/2, xOffset, height() - ySide + pw), d->rectImage,
                                QRect(d->rectImage.width()-xOffset, d->rectImage.height()/2, xOffset, 1));
        // Lower left
        p.drawPixmap(QPoint(-pw/2, height() - yOffset + pw/2), d->rectImage, QRect(0, d->rectImage.height() - yOffset, xOffset, yOffset));

        // Lower Middle
        if (xMiddles)
            p.drawPixmap(QRect(xOffset-pw/2, height() - yOffset +pw/2, width() - xSide + pw, yOffset), d->rectImage,
                                QRect(d->rectImage.width()/2, d->rectImage.height() - yOffset, 1, yOffset));
        // Lower Right
        p.drawPixmap(QPoint(width()-xOffset+pw/2, height() - yOffset+pw/2), d->rectImage,
                        QRect(d->rectImage.width()-xOffset, d->rectImage.height() - yOffset, xOffset, yOffset));
    }
}

QRectF QFxRect::boundingRect() const
{
    Q_D(const QFxRect);
    return QRectF(-d->paintmargin, -d->paintmargin, d->width+d->paintmargin*2, d->height+d->paintmargin*2);
}

QT_END_NAMESPACE
