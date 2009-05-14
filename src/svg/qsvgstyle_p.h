/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSvg module of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSVGSTYLE_P_H
#define QSVGSTYLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qpainter.h"

#ifndef QT_NO_SVG

#include "QtGui/qpen.h"
#include "QtGui/qbrush.h"
#include "QtGui/qmatrix.h"
#include "QtGui/qcolor.h"
#include "QtGui/qfont.h"
#include <qdebug.h>

QT_BEGIN_NAMESPACE

class QPainter;
class QSvgNode;
class QSvgFont;
class QSvgTinyDocument;

template <class T> class QSvgRefCounter
{
public:
    QSvgRefCounter() { t = 0; }
    QSvgRefCounter(T *_t)
    {
        t = _t;
        if (t)
            t->ref();
    }
    QSvgRefCounter(const QSvgRefCounter &other)
    {
        t = other.t;
        if (t)
            t->ref();
    }
    QSvgRefCounter &operator =(T *_t)
    {
        if(_t)
            _t->ref();
        if (t)
            t->deref();
        t = _t;
        return *this;
    }
    QSvgRefCounter &operator =(const QSvgRefCounter &other)
    {
        if(other.t)
            other.t->ref();
        if (t)
            t->deref();
        t = other.t;
        return *this;
    }
    ~QSvgRefCounter()
    {
        if (t)
            t->deref();
    }

    inline T *operator->() const { return t; }
    inline operator T*() const { return t; }

private:
    T *t;
};

class QSvgRefCounted
{
public:
    QSvgRefCounted() { _ref = 0; }
    virtual ~QSvgRefCounted() {}
    void ref() {
        ++_ref;
//        qDebug() << this << ": adding ref, now " << _ref;
    }
    void deref() {
//        qDebug() << this << ": removing ref, now " << _ref;
        if(!--_ref) {
//            qDebug("     deleting");
            delete this;
        }
    }
private:
    int _ref;
};

struct QSvgExtraStates
{
    QSvgExtraStates();

    qreal fillOpacity;
    QSvgFont *svgFont;
    Qt::Alignment textAnchor;
    int fontWeight;
};

class QSvgStyleProperty : public QSvgRefCounted
{
public:
    enum Type
    {
        QUALITY,
        FILL,
        VIEWPORT_FILL,
        FONT,
        STROKE,
        SOLID_COLOR,
        GRADIENT,
        TRANSFORM,
        ANIMATE_TRANSFORM,
        ANIMATE_COLOR,
        OPACITY,
        COMP_OP
    };
public:
    virtual ~QSvgStyleProperty();
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states)  =0;
    virtual void revert(QPainter *p, QSvgExtraStates &states) =0;
    virtual Type type() const=0;
};

class QSvgQualityStyle : public QSvgStyleProperty
{
public:
    QSvgQualityStyle(int color);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;
private:
    // color-render ing v 	v 	'auto' | 'optimizeSpeed' |
    //                                  'optimizeQuality' | 'inherit'
    int m_colorRendering;

    // shape-rendering v 	v 	'auto' | 'optimizeSpeed' | 'crispEdges' |
    //                                  'geometricPrecision' | 'inherit'
    //QSvgShapeRendering m_shapeRendering;


    // text-rendering    v 	v 	'auto' | 'optimizeSpeed' | 'optimizeLegibility'
    //                                | 'geometricPrecision' | 'inherit'
    //QSvgTextRendering m_textRendering;


    // vector-effect         v 	x 	'default' | 'non-scaling-stroke' | 'inherit'
    //QSvgVectorEffect m_vectorEffect;

    // image-rendering v 	v 	'auto' | 'optimizeSpeed' | 'optimizeQuality' |
    //                                      'inherit'
    //QSvgImageRendering m_imageRendering;
};



class QSvgOpacityStyle : public QSvgStyleProperty
{
public:
    QSvgOpacityStyle(qreal opacity);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;
private:
    qreal m_opacity;
    qreal m_oldOpacity;
};

class QSvgFillStyle : public QSvgStyleProperty
{
public:
    QSvgFillStyle(const QBrush &brush);
    QSvgFillStyle(QSvgStyleProperty *style);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;

    void setFillRule(Qt::FillRule f);
    void setFillOpacity(qreal opacity);
    void setFillStyle(QSvgStyleProperty* style);
    void setBrush(QBrush brush);

    const QBrush & qbrush() const
    {
        return m_fill;
    }

    qreal fillOpacity() const
    {
        return m_fillOpacity;
    }

    Qt::FillRule fillRule() const
    {
        return m_fillRule;
    }

    QSvgStyleProperty* style() const
    {
        return m_style;
    }

    void setGradientId(const QString &Id)
    {
        m_gradientId = Id;
    }

    QString getGradientId() const
    {
        return m_gradientId;
    }

    void setGradientResolved(bool resolved)
    {
        m_gradientResolved = resolved;
    }

    bool isGradientResolved() const
    {
        return m_gradientResolved;
    }

private:
    // fill            v 	v 	'inherit' | <Paint.datatype>
    // fill-opacity    v 	v 	'inherit' | <OpacityValue.datatype>
    QBrush m_fill;
    QBrush m_oldFill;
    QSvgStyleProperty *m_style;

    bool         m_fillRuleSet;
    Qt::FillRule m_fillRule;
    bool m_fillOpacitySet;
    qreal m_fillOpacity;
    qreal m_oldOpacity;
    QString m_gradientId;
    bool m_gradientResolved;
    bool m_fillSet;
};

class QSvgViewportFillStyle : public QSvgStyleProperty
{
public:
    QSvgViewportFillStyle(const QBrush &brush);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;

    const QBrush & qbrush() const
    {
        return m_viewportFill;
    }
private:
    // viewport-fill         v 	x 	'inherit' | <Paint.datatype>
    // viewport-fill-opacity 	v 	x 	'inherit' | <OpacityValue.datatype>
    QBrush m_viewportFill;

    QBrush m_oldFill;
};

class QSvgFontStyle : public QSvgStyleProperty
{
public:
    static const int LIGHTER = -1;
    static const int BOLDER = 1;

    QSvgFontStyle(QSvgFont *font, QSvgTinyDocument *doc);
    QSvgFontStyle();
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;

    void setSize(qreal size)
    {
        // Store the _pixel_ size in the font. Since QFont::setPixelSize() only takes an int, call
        // QFont::SetPointSize() instead. Set proper font size just before rendering.
        m_qfont.setPointSize(size);
        m_sizeSet = 1;
    }

    void setTextAnchor(Qt::Alignment anchor)
    {
        m_textAnchor = anchor;
        m_textAnchorSet = 1;
    }

    void setFamily(const QString &family)
    {
        m_qfont.setFamily(family);
        m_familySet = 1;
    }

    void setStyle(QFont::Style fontStyle) {
        m_qfont.setStyle(fontStyle);
        m_styleSet = 1;
    }

    void setVariant(QFont::Capitalization fontVariant)
    {
        m_qfont.setCapitalization(fontVariant);
        m_variantSet = 1;
    }

    static int SVGToQtWeight(int weight);

    void setWeight(int weight)
    {
        m_weight = weight;
        m_weightSet = 1;
    }

    QSvgFont * svgFont() const
    {
        return m_svgFont;
    }

    const QFont &qfont() const
    {
        return m_qfont;
    }

    QSvgTinyDocument *doc() const {return m_doc;}

private:
    QSvgFont *m_svgFont;
    QSvgTinyDocument *m_doc;
    QFont m_qfont;

    int m_weight;
    Qt::Alignment m_textAnchor;

    QSvgFont *m_oldSvgFont;
    QFont m_oldQFont;
    Qt::Alignment m_oldTextAnchor;
    int m_oldWeight;

    unsigned m_familySet : 1;
    unsigned m_sizeSet : 1;
    unsigned m_styleSet : 1;
    unsigned m_variantSet : 1;
    unsigned m_weightSet : 1;
    unsigned m_textAnchorSet : 1;
};

class QSvgStrokeStyle : public QSvgStyleProperty
{
public:
    QSvgStrokeStyle(const QPen &pen);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;

    void setStroke(bool stroke)
    {
        m_strokePresent = stroke;
    }

    bool strokePresent() const
    {
        return m_strokePresent;
    }

    const QPen & qpen() const
    {
        return m_stroke;
    }
private:
    // stroke            v 	v 	'inherit' | <Paint.datatype>
    // stroke-dasharray  v 	v 	'inherit' | <StrokeDashArrayValue.datatype>
    // stroke-dashoffset v 	v 	'inherit' | <StrokeDashOffsetValue.datatype>
    // stroke-linecap    v 	v 	'butt' | 'round' | 'square' | 'inherit'
    // stroke-linejoin   v 	v 	'miter' | 'round' | 'bevel' | 'inherit'
    // stroke-miterlimit v 	v 	'inherit' | <StrokeMiterLimitValue.datatype>
    // stroke-opacity    v 	v 	'inherit' | <OpacityValue.datatype>
    // stroke-width      v 	v 	'inherit' | <StrokeWidthValue.datatype>
    QPen m_stroke;
    QPen m_oldStroke;
	bool m_strokePresent;
};


class QSvgSolidColorStyle : public QSvgStyleProperty
{
public:
    QSvgSolidColorStyle(const QColor &color);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;

    const QColor & qcolor() const
    {
        return m_solidColor;
    }
private:
    // solid-color       v 	x 	'inherit' | <SVGColor.datatype>
    // solid-opacity     v 	x 	'inherit' | <OpacityValue.datatype>
    QColor m_solidColor;

    QBrush m_oldFill;
    QPen   m_oldStroke;
};

class QSvgGradientStyle : public QSvgStyleProperty
{
public:
    QSvgGradientStyle(QGradient *grad);
    ~QSvgGradientStyle() { delete m_gradient; }
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;

    void setStopLink(const QString &link, QSvgTinyDocument *doc);
    QString stopLink() const { return m_link; }
    void resolveStops();

    void setMatrix(const QMatrix &matrix);
    QMatrix  qmatrix() const
    {
        return m_matrix;
    }

    QGradient *qgradient() const
    {
        return m_gradient;
    }

    void addResolve(qreal offset);

    bool gradientStopsSet() const
    {
        return m_gradientStopsSet;
    }

    void setGradientStopsSet(bool set)
    {
        m_gradientStopsSet = set;
    }
private:
    QGradient      *m_gradient;
    QList<qreal>  m_resolvePoints;

    QBrush m_oldFill;

    QMatrix m_matrix;

    QSvgTinyDocument *m_doc;
    QString           m_link;
    bool m_gradientStopsSet;
};

class QSvgTransformStyle : public QSvgStyleProperty
{
public:
    QSvgTransformStyle(const QTransform &transform);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;

    const QTransform & qtransform() const
    {
        return m_transform;
    }
private:
    //7.6 The transform  attribute
    QTransform m_transform;
    QTransform m_oldWorldTransform;
};


class QSvgAnimateTransform : public QSvgStyleProperty
{
public:
    enum TransformType
    {
        Empty,
        Translate,
        Scale,
        Rotate,
        SkewX,
        SkewY
    };
    enum Additive
    {
        Sum,
        Replace
    };
public:
    QSvgAnimateTransform(int startMs, int endMs, int by = 0);
    void setArgs(TransformType type, Additive additive, const QVector<qreal> &args);
    void setFreeze(bool freeze);
    void setRepeatCount(qreal repeatCount);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;
    QSvgAnimateTransform::Additive additiveType() const
    {
        return m_additive;
    }

    bool animFinished(qreal totalTimeElapsed)
    {
        qreal animationFrame = (totalTimeElapsed - m_from) / m_to;
        if (m_repeatCount >= 0 && m_repeatCount < animationFrame)
            return true;
        return false;
    }

    qreal animStartTime() const
    {
        return m_from;
    }

    void setTransformApplied(bool apply)
    {
        m_transformApplied = apply;
    }

    bool transformApplied() const
    {
        return m_transformApplied;
    }

    bool frozen()
    {
        return m_freeze;
    }

protected:
    void resolveMatrix(QSvgNode *node);
private:
    qreal m_from, m_to, m_by;
    qreal m_totalRunningTime;
    TransformType m_type;
    Additive m_additive;
    QVector<qreal> m_args;
    int m_count;
    QTransform m_transform;
    QTransform m_oldWorldTransform;
    bool m_finished;
    bool m_freeze;
    qreal m_repeatCount;
    bool m_transformApplied;
};


class QSvgAnimateColor : public QSvgStyleProperty
{
public:
    QSvgAnimateColor(int startMs, int endMs, int by = 0);
    void setArgs(bool fill, const QList<QColor> &colors);
    void setFreeze(bool freeze);
    void setRepeatCount(qreal repeatCount);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;
private:
    qreal m_from, m_to, m_by;
    qreal m_totalRunningTime;
    QList<QColor> m_colors;
    QBrush m_oldBrush;
    QPen   m_oldPen;
    bool m_fill;
    bool m_finished;
    bool m_freeze;
    qreal m_repeatCount;
};


class QSvgCompOpStyle : public QSvgStyleProperty
{
public:
    QSvgCompOpStyle(QPainter::CompositionMode mode);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &states);
    virtual void revert(QPainter *p, QSvgExtraStates &states);
    virtual Type type() const;

    const QPainter::CompositionMode & compOp() const
    {
        return m_mode;
    }
private:
    //comp-op attribute
    QPainter::CompositionMode m_mode;

    QPainter::CompositionMode m_oldMode;
};


class QSvgStyle
{
public:
    QSvgStyle()
        : quality(0),
          fill(0),
          viewportFill(0),
          font(0),
          stroke(0),
          solidColor(0),
          gradient(0),
          transform(0),
          animateColor(0),
          opacity(0),
          compop(0)
    {}
    ~QSvgStyle();

    void apply(QPainter *p, const QRectF &rect, QSvgNode *node, QSvgExtraStates &states);
    void revert(QPainter *p, QSvgExtraStates &states);
    QSvgRefCounter<QSvgQualityStyle>      quality;
    QSvgRefCounter<QSvgFillStyle>         fill;
    QSvgRefCounter<QSvgViewportFillStyle> viewportFill;
    QSvgRefCounter<QSvgFontStyle>         font;
    QSvgRefCounter<QSvgStrokeStyle>       stroke;
    QSvgRefCounter<QSvgSolidColorStyle>   solidColor;
    QSvgRefCounter<QSvgGradientStyle>     gradient;
    QSvgRefCounter<QSvgTransformStyle>    transform;
    QSvgRefCounter<QSvgAnimateColor>      animateColor;
    QList<QSvgRefCounter<QSvgAnimateTransform> >   animateTransforms;
    QSvgRefCounter<QSvgOpacityStyle>      opacity;
    QSvgRefCounter<QSvgCompOpStyle>       compop;
};

/********************************************************/
// NOT implemented:

// color           v 	v 	'inherit' | <Color.datatype>
//QColor m_color;

// display         v 	x 	'inline' | 'block' | 'list-item'
//                                 | 'run-in' | 'compact' | 'marker' |
//                                 'table' | 'inline-table' |
//                                 'table-row-group' | 'table-header-group' |
//                                 'table-footer-group' | 'table-row' |
//                                 'table-column-group' | 'table-column' |
//                                 'table-cell' | 'table-caption' |
//                                 'none' | 'inherit'
//QSvgDisplayStyle m_display;

// display-align   v 	v 	'auto' | 'before' | 'center' | 'after' | 'inherit'
//QSvgDisplayAlign m_displayAlign;

// line-increment  v 	v 	'auto' | 'inherit' | <Number.datatype>
//int m_lineIncrement;

// text-anchor       v 	v 	'start' | 'middle' | 'end' | 'inherit'
//QSvgTextAnchor m_textAnchor;

// visibility 	v 	v 	'visible' | 'hidden' | 'inherit'
//QSvgVisibility m_visibility;

/******************************************************/
// the following do not make sense for us

// pointer-events  v 	v 	'visiblePainted' | 'visibleFill' | 'visibleStroke' |
//                              'visible' | 'painted' | 'fill' | 'stroke' | 'all' |
//                              'none' | 'inherit'
//QSvgPointEvents m_pointerEvents;

// audio-level     v  	x  	'inherit' | <Number.datatype>

QT_END_NAMESPACE

#endif // QT_NO_SVG
#endif // QSVGSTYLE_P_H
