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


QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(QFxPen,Pen);

/*!
    \internal
    \class QFxPen
    \ingroup group_utility
    \brief The QFxPen class provides a pen used for drawing rect borders on a QFxView.

    Example:
    \qml
    Rect { pen.width: 2; pen.color: "red" ... }
    \endqml
*/

/*! \property QFxPen::width
    \brief the width of the pen.

    The default width is 1. If the width is less than 1 the pen is considered invalid
    and won't be used.
*/

/*!
    \property QFxPen::color
    \brief the color of the pen.

    color is most commonly specified in hexidecimal notation (#RRGGBB)
    or as an \l {http://www.w3.org/TR/SVG/types.html#ColorKeywords}{SVG color keyword name}
     (as defined by the World Wide Web Consortium). For example:
    \qml
    // rect with green border using hexidecimal notation
    Rect { pen.color: "#00FF00" }

    // rect with steelblue border using SVG color name
    Rect { pen.color: "steelblue" }
    \endqml

    For the full set of ways to specify color, see Qt's QColor::setNamedColor documentation.
*/

void QFxPen::setColor(const QColor &c)
{
    _color = c;
    emit updated();
    _valid = _color.alpha() ? true : false;
}



QML_DEFINE_TYPE(QFxRect,Rect);

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
        pen.color: "black"
        pen.width: 5
        radius: 10
    }
    \endqml

    \image declarative-rect.png
*/

/*!
    \internal
    \class QFxRect
    \brief The QFxRect class provides a rect item that you can add to a QFxView.

    A Rect is painted having a solid fill (color) and an optional border (pen).
    You can also create rounded rectangles using the radius property.

    \qml
    Rect {
        width: 100
        height: 100
        color: "red"
        pen.color: "black"
        pen.width: 5
        radius: 10
    }
    \endqml

    \image declarative-rect.png

    A QFxRect object can be instantiated in Qml using the tag \l Rect.

    \ingroup group_coreitems
*/
QFxRect::QFxRect(QFxItem *parent)
  : QFxItem(*(new QFxRectPrivate), parent)
{
    Q_D(QFxRect);
    d->init();
    setOptions(HasContents, true);
}

QFxRect::QFxRect(QFxRectPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    Q_D(QFxRect);
    d->init();
    setOptions(HasContents, true);
}

void QFxRect::doUpdate()
{
#if defined(QFX_RENDER_QPAINTER)
    Q_D(QFxRect);
    d->_rectImage = QSimpleCanvasConfig::Image();
#endif
#if defined(QFX_RENDER_OPENGL)
    Q_D(QFxRect);
    d->_rectTexture.clear();
#endif
    update();
}

/*!
    \qmlproperty int Rect::pen.width
    \qmlproperty color Rect::pen.color

    The pen used to draw the border of the rect.
*/
/*!
    \property QFxRect::pen
    \brief the pen used to draw the border of the rect.
*/
QFxPen *QFxRect::pen()
{
    Q_D(QFxRect);
    return d->pen();
}

/*!
    \qmlproperty real Rect::radius
    This property holds the corner radius used to draw a rounded rect.

    If radius is non-zero, the rect will be painted as a rounded rectangle, otherwise it will be
    painted as a normal rectangle. The same radius is used by all 4 corners; there is currently 
    no way to specify different radii for different corners.
*/

/*!
    \property QFxRect::radius
    \brief the corner radius used to draw a rounded rect.
*/
qreal QFxRect::radius() const
{
    Q_D(const QFxRect);
    return d->_radius;
}

void QFxRect::setRadius(qreal radius)
{
    Q_D(QFxRect);
    if (d->_radius == radius)
        return;

    d->_radius = radius;
#if defined(QFX_RENDER_QPAINTER)
    d->_rectImage = QSimpleCanvasConfig::Image();
#elif defined(QFX_RENDER_OPENGL)
    d->_rectTexture.clear();
#endif
    update();
}

void QFxRect::dump(int depth)
{
    Q_D(QFxRect);
    QByteArray ba(depth * 4, ' ');
    qWarning() << ba.constData() << "QFxRect:" << d->_color;
    QFxItem::dump(depth);
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
*/

/*!
    \property QFxRect::color
    \brief the color used to fill the rect.
*/
QColor QFxRect::color() const
{
    Q_D(const QFxRect);
    return d->_color;
}

void QFxRect::setColor(const QColor &c)
{
    Q_D(QFxRect);
    if (d->_color == c)
        return;

    d->_color = c;
#if defined(QFX_RENDER_QPAINTER)
    d->_rectImage = QSimpleCanvasConfig::Image();
#endif
#if defined(QFX_RENDER_OPENGL)
    d->_rectTexture.clear();
#endif
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

/*!
    \property QFxRect::tintColor
    \brief The color to tint the rectangle.
*/
QColor QFxRect::tintColor() const
{
    Q_D(const QFxRect);
    return d->_tintColor;
}

void QFxRect::setTintColor(const QColor &c)
{
    Q_D(QFxRect);
    if (d->_tintColor == c)
        return;

    d->_tintColor = c;
    update();
}

QColor QFxRectPrivate::getColor()
{
    if (_tintColor.isValid()) {
        int a = _tintColor.alpha();
        if (a == 0xFF)
            return _tintColor;
        else if (a == 0x00)
            return _color;
        else {
            uint src = _tintColor.rgba();
            uint dest = _color.rgba();

            uint res = (((a * (src & 0xFF00FF)) +
                        ((0xFF - a) * (dest & 0xFF00FF))) >> 8) & 0xFF00FF;
            res |= (((a * ((src >> 8) & 0xFF00FF)) +
                    ((0xFF - a) * ((dest >> 8) & 0xFF00FF)))) & 0xFF00FF00;
            if ((src & 0xFF000000) == 0xFF000000)
                res |= 0xFF000000;

            return QColor::fromRgba(res);
        }
    } else {
        return _color;
    }
}

/*!
    \qmlproperty color Rect::gradientColor
    This property holds the color to use at the base of the rectangle and blend upwards.

    This property allows for the easy construction of simple horizontal gradients. Other gradients may by formed by adding rotation to the rect. The gradient will blend linearly from the rect's main color to the color specified for gradient color.

    \qml
    Rect { y: 0; width: 80; height: 80; color: "lightsteelblue" }
    Rect { y: 100; width: 80; height: 80; color: "lightsteelblue"; gradientColor="blue" }
    Rect { rotation: 90; x: 80; y: 200; width: 80; height: 80; color="lightsteelblue"
            gradientColor: "blue" }
    // The x offset is needed because the rotation is from the top left corner
    \endqml
    \image declarative-rect_gradient.png
*/

/*!
    \property QFxRect::gradientColor
    \brief The color to use at the base of the rectangle and blend upwards.
*/

QColor QFxRect::gradientColor() const
{
    Q_D(const QFxRect);
    return d->_gradcolor;
}

void QFxRect::setGradientColor(const QColor &c)
{
    Q_D(QFxRect);
    if (d->_gradcolor == c)
        return;

    d->_gradcolor = c;
    update();
}

#if defined(QFX_RENDER_QPAINTER)
void QFxRect::generateRoundedRect()
{
    Q_D(QFxRect);
    if (d->_rectImage.isNull()) {
        d->_rectImage = QImage(d->_radius*2 + 1, d->_radius*2 + 1, QImage::Format_ARGB32_Premultiplied);
        d->_rectImage.fill(0);
        QPainter p(&(d->_rectImage));
        QPen pn(QColor(pen()->color()), pen()->width());
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(pn);
        p.setBrush(d->_color);
        p.drawRoundedRect(0, 0, d->_rectImage.width(), d->_rectImage.height(), d->_radius, d->_radius);
    }
}

void QFxRect::generateBorderedRect()
{
    Q_D(QFxRect);
    if (d->_rectImage.isNull()) {
        d->_rectImage = QImage(d->pen()->width()*2 + 1, d->pen()->width()*2 + 1, QImage::Format_ARGB32_Premultiplied);
        d->_rectImage.fill(0);
        QPainter p(&(d->_rectImage));
        QPen pn(QColor(pen()->color()), pen()->width());
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(pn);
        p.setBrush(d->_color);
        p.drawRect(0, 0, d->_rectImage.width(), d->_rectImage.height());
    }
}
#elif defined(QFX_RENDER_OPENGL)
void QFxRect::generateRoundedRect()
{
    Q_D(QFxRect);
    if (d->_rectTexture.isNull()) {
        QImage roundRect(int(d->_radius*2 + 1), int(d->_radius*2 + 1), QImage::Format_ARGB32);
        roundRect.fill(0);
        QPainter p(&roundRect);
        QPen pn(QColor(pen()->color()), pen()->width());
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(pn);
        p.setBrush(d->_color);
        p.drawRoundedRect(0, 0, roundRect.width(), roundRect.height(), d->_radius, d->_radius);
        d->_rectTexture.setImage(roundRect);
    }
}

void QFxRect::generateBorderedRect()
{
    Q_D(QFxRect);
    if (d->_rectTexture.isNull()) {
        QImage borderedRect(d->pen()->width()*2 + 1, d->pen()->width()*2 + 1, QImage::Format_ARGB32_Premultiplied);
        borderedRect.fill(0);
        QPainter p(&(borderedRect));
        QPen pn(QColor(pen()->color()), pen()->width());
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(pn);
        p.setBrush(d->_color);
        p.drawRect(0, 0, borderedRect.width(), borderedRect.height());
        d->_rectTexture.setImage(borderedRect);
    }
}
#endif


#if defined(QFX_RENDER_QPAINTER)
void QFxRect::paintContents(QPainter &p)
{
    Q_D(QFxRect);
    if (d->_radius > 0 || (d->_pen && d->_pen->isValid())
           || d->_gradcolor.isValid())
        drawRect(p);
    /*
        QLinearGradient grad(0, 0, 0, height());
        grad.setColorAt(0, d->_color);
        grad.setColorAt(1, d->_gradcolor);
        p.setBrush(grad);
        p.drawRect(0, 0, width(), height());
        p.setBrush(QBrush());
        */
    else
        p.fillRect(QRect(0, 0, width(), height()), d->getColor());
}

void QFxRect::drawRect(QPainter &p)
{
    Q_D(QFxRect);
    if (d->_gradcolor.isValid() /*|| p.usingQt() */) {
        // XXX This path is still slower than the image path
        // Image path won't work for gradients though
        p.save();
        QPen pn(QColor(pen()->color()), pen()->width());
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(pn);
        if (d->_gradcolor.isValid()){
            QLinearGradient grad(0, 0, 0, height());
            grad.setColorAt(0, d->_color);
            grad.setColorAt(1, d->_gradcolor);
            p.setBrush(grad);
        }else{
            p.setBrush(d->_color);
        }
        if (d->_radius)
            p.drawRoundedRect(0, 0, width(), height(), d->_radius, d->_radius);
        else
            p.drawRect(0, 0, width(), height());
        p.restore();
    } else {
        int offset = 0;
        if (d->_radius > 0) {
            generateRoundedRect();
            //### implicit conversion to int
            offset = d->_radius;
        } else {
            generateBorderedRect();
            offset = d->pen()->width();
        }

        //basically same code as QFxImage uses to paint sci images
        int xSide = qMin(offset * 2, int(width()));
        int ySide = qMin(offset * 2, int(height()));;

        // Upper left
        p.drawImage(QRect(0, 0, offset, offset), d->_rectImage, QRect(0, 0, offset, offset));

        // Upper middle
        if (d->_rectImage.width() - xSide)
            p.drawImage(QRect(offset, 0, width() - xSide, offset), d->_rectImage,
                                  QRect(offset, 0, d->_rectImage.width() - xSide, offset));
        // Upper right
        if (d->_rectImage.width() - offset) {
            p.drawImage(QPoint(width()-offset, 0), d->_rectImage,
                                  QRect(d->_rectImage.width()-offset, 0, offset, offset));
        }
        // Middle left
        if (d->_rectImage.height() - ySide)
            p.drawImage(QRect(0, offset, offset, height() - ySide), d->_rectImage,
                                  QRect(0, offset, offset, d->_rectImage.height() - ySide));

        // Middle
        if (d->_rectImage.width() - xSide && d->_rectImage.height() - ySide)
            p.drawImage(QRect(offset, offset, width() - xSide, height() - ySide), d->_rectImage,
                                QRect(offset, offset, d->_rectImage.width() - xSide, d->_rectImage.height() - ySide));
        // Midlle right
        if (d->_rectImage.height() - ySide)
            p.drawImage(QRect(width()-offset, offset, offset, height() - ySide), d->_rectImage,
                                QRect(d->_rectImage.width()-offset, offset, offset, d->_rectImage.height() - ySide));
        // Lower left 
        p.drawImage(QPoint(0, height() - offset), d->_rectImage, QRect(0, d->_rectImage.height() - offset, offset, offset));

        // Lower Middle
        if (d->_rectImage.width() - xSide)
            p.drawImage(QRect(offset, height() - offset, width() - xSide, offset), d->_rectImage,
                                QRect(offset, d->_rectImage.height() - offset, d->_rectImage.width() - xSide, offset));
        // Lower Right
        if (d->_rectImage.width() - offset)
            p.drawImage(QPoint(width()-offset, height() - offset), d->_rectImage,
                            QRect(d->_rectImage.width()-offset, d->_rectImage.height() - offset, offset, offset));
    }
}
#endif

#if defined(QFX_RENDER_OPENGL2)
#include "glbasicshaders.h"

void QFxRect::paintGLContents(GLPainter &p)
{
    Q_D(QFxRect);
    if (d->_radius == 0 && (!d->_pen || !d->_pen->isValid())) {
        if (d->_gradcolor.isValid()) {
            float widthV = width();
            float heightV = height();

            GLfloat vertices[] = { 0, heightV,
                                   widthV, heightV,
                                   0, 0,
                                   widthV, 0 };

            float r = d->_color.redF();
            float g = d->_color.greenF();
            float b = d->_color.blueF();
            float a = d->_color.alphaF() * p.activeOpacity;

            float r2 = d->_gradcolor.redF();
            float g2 = d->_gradcolor.greenF();
            float b2 = d->_gradcolor.blueF();
            float a2 = d->_gradcolor.alphaF() * p.activeOpacity;

            GLfloat colors[] = { r2, g2, b2, a2,
                                 r2, g2, b2, a2,
                                 r, g, b, a,
                                 r, g, b, a };

            ColorShader *shader = basicShaders()->color();
            shader->enable();
            shader->setTransform(p.activeTransform);

            shader->setAttributeArray(ColorShader::Vertices, vertices, 2);
            shader->setAttributeArray(ColorShader::Colors, colors, 4);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            shader->disableAttributeArray(ColorShader::Vertices);
            shader->disableAttributeArray(ColorShader::Colors);
        } else {
            QGLShaderProgram *shader = p.useColorShader(d->getColor());

            float widthV = width();
            float heightV = height();

            GLfloat vertices[] = { 0, heightV,
                                   widthV, heightV,
                                   0, 0,
                                   widthV, 0 };

            shader->setAttributeArray(ConstantColorShader::Vertices, vertices, 2);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            shader->disableAttributeArray(ConstantColorShader::Vertices);
        }
    } else {
        qreal offset = 0;
        if (d->_radius > 0) {
            generateRoundedRect();
            offset = d->_radius;
        } else {
            generateBorderedRect();
            offset = d->pen()->width();
        }

        QGLShaderProgram *shader = p.useTextureShader();

        float imgWidth = d->_rectTexture.width();
        float imgHeight = d->_rectTexture.height();
        if (!imgWidth || !imgHeight)
            return;

        float widthV = width();
        float heightV = height();

        float texleft = 0;
        float texright = 1;
        float textop = 1;
        float texbottom = 0;
        float imgleft = 0;
        float imgright = widthV;
        float imgtop = 0;
        float imgbottom = heightV;

        texleft = float(offset) / imgWidth;
        imgleft = offset;
        texright = 1. - float(offset) / imgWidth;
        imgright = widthV - offset;
        textop = 1. - float(offset) / imgHeight;
        imgtop = offset;
        texbottom = float(offset) / imgHeight;
        imgbottom = heightV - offset;

        //Bug 231768: Inappropriate interpolation was occuring on 3x3 textures
        if (offset==1)
            texleft=texright=textop=texbottom=0.5;

        float vert1[] = { 0, 0, 
                          0, imgtop,
                          imgleft, 0, 
                          imgleft, imgtop,
                          imgright, 0,
                          imgright, imgtop,
                          widthV, 0,
                          widthV, imgtop };
        float tex1[] = { 0, 0, 
                         0, textop,
                         texleft, 0,
                         texleft, textop,
                         texright, 0,
                         texright, textop,
                         1, 0,
                         1, textop };
        float vert2[] = { 0, imgtop,
                          0, imgbottom,
                          imgleft, imgtop,
                          imgleft, imgbottom,
                          imgright, imgtop,
                          imgright, imgbottom,
                          widthV, imgtop,
                          widthV, imgbottom };
        float tex2[] = { 0, textop,
                         0, texbottom,
                         texleft, textop,
                         texleft, texbottom,
                         texright, textop,
                         texright, texbottom,
                         1, textop,
                         1, texbottom };
        float vert3[] = { 0, imgbottom,
                          0, heightV,
                          imgleft, imgbottom,
                          imgleft, heightV,
                          imgright, imgbottom,
                          imgright, heightV,
                          widthV, imgbottom,
                          widthV, heightV };
        float tex3[] = { 0, texbottom,
                         0, 0,
                         texleft, texbottom,
                         texleft, 0,
                         texright, texbottom,
                         texright, 0,
                         1, texbottom,
                         1, 0 };

        glBindTexture(GL_TEXTURE_2D, d->_rectTexture.texture());

        shader->setAttributeArray(SingleTextureShader::Vertices, vert1, 2);
        shader->setAttributeArray(SingleTextureShader::TextureCoords, tex1, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        shader->setAttributeArray(SingleTextureShader::Vertices, vert2, 2);
        shader->setAttributeArray(SingleTextureShader::TextureCoords, tex2, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        shader->setAttributeArray(SingleTextureShader::Vertices, vert3, 2);
        shader->setAttributeArray(SingleTextureShader::TextureCoords, tex3, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

        shader->disableAttributeArray(SingleTextureShader::Vertices);
        shader->disableAttributeArray(SingleTextureShader::TextureCoords);
    }
}
#elif defined(QFX_RENDER_OPENGL1)
void QFxRect::paintGLContents(GLPainter &p)
{
    Q_D(QFxRect);

    float widthV = width();
    float heightV = height();
        
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(p.activeTransform.data());
    
    if (d->_radius == 0 && (!d->_pen || !d->_pen->isValid())) {
        GLfloat vertices[] = { 0, heightV,
                               widthV, heightV,
                               0, 0,
                               widthV, 0 };

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2,GL_FLOAT,0,vertices);

        QColor c;
        if (d->_gradcolor.isValid())
            c = d->_color;
        else
            c = d->getColor();
        float r = c.redF();
        float g = c.greenF();
        float b = c.blueF();
        float a = c.alphaF() * p.activeOpacity;

        float r2 = r; float g2 = g; float b2 = b; float a2 = a;

        if (d->_gradcolor.isValid()) {
            r2 = d->_gradcolor.redF();
            g2 = d->_gradcolor.greenF();
            b2 = d->_gradcolor.blueF();
            a2 = d->_gradcolor.alphaF() * p.activeOpacity;
        }

        GLfloat colors[] = { r2, g2, b2, a2,
                             r2, g2, b2, a2,
                             r, g, b, a,
                             r, g, b, a };

        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4,GL_FLOAT,0,colors);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    } else {
        qreal offset = 0;
        if (d->_radius > 0) {
            generateRoundedRect();
            offset = d->_radius;
        } else {
            generateBorderedRect();
            offset = d->pen()->width();
        }

        if (p.activeOpacity == 1.) {
            GLint i = GL_REPLACE;
            glTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &i);
        } else {
            GLint i = GL_MODULATE;
            glTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &i);
            glColor4f(1, 1, 1, p.activeOpacity);
        }

        float imgWidth = d->_rectTexture.width();
        float imgHeight = d->_rectTexture.height();
        if (!imgWidth || !imgHeight)
            return;

        float widthV = width();
        float heightV = height();

        float texleft = 0;
        float texright = 1;
        float textop = 1;
        float texbottom = 0;
        float imgleft = 0;
        float imgright = widthV;
        float imgtop = 0;
        float imgbottom = heightV;

        texleft = float(offset) / imgWidth;
        imgleft = offset;
        texright = 1. - float(offset) / imgWidth;
        imgright = widthV - offset;
        textop = 1. - float(offset) / imgHeight;
        imgtop = offset;
        texbottom = float(offset) / imgHeight;
        imgbottom = heightV - offset;

        float vert1[] = { 0, 0, 
                          0, imgtop,
                          imgleft, 0, 
                          imgleft, imgtop,
                          imgright, 0,
                          imgright, imgtop,
                          widthV, 0,
                          widthV, imgtop };
        float tex1[] = { 0, 1, 
                         0, textop,
                         texleft, 1,
                         texleft, textop,
                         texright, 1,
                         texright, textop,
                         1, 1,
                         1, textop };
        float vert2[] = { 0, imgtop,
                          0, imgbottom,
                          imgleft, imgtop,
                          imgleft, imgbottom,
                          imgright, imgtop,
                          imgright, imgbottom,
                          widthV, imgtop,
                          widthV, imgbottom };
        float tex2[] = { 0, textop,
                         0, texbottom,
                         texleft, textop,
                         texleft, texbottom,
                         texright, textop,
                         texright, texbottom,
                         1, textop,
                         1, texbottom };
        float vert3[] = { 0, imgbottom,
                          0, heightV,
                          imgleft, imgbottom,
                          imgleft, heightV,
                          imgright, imgbottom,
                          imgright, heightV,
                          widthV, imgbottom,
                          widthV, heightV };
        float tex3[] = { 0, texbottom,
                         0, 0,
                         texleft, texbottom,
                         texleft, 0,
                         texright, texbottom,
                         texright, 0,
                         1, texbottom,
                         1, 0 };

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, d->_rectTexture.texture());

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, vert1);
        glTexCoordPointer(2, GL_FLOAT, 0, tex1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

        glVertexPointer(2, GL_FLOAT, 0, vert2);
        glTexCoordPointer(2, GL_FLOAT, 0, tex2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

        glVertexPointer(2, GL_FLOAT, 0, vert3);
        glTexCoordPointer(2, GL_FLOAT, 0, tex3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
}
#endif

QT_END_NAMESPACE
