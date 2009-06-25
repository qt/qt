/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <qstyle.h>
#include <private/qstyle_p.h>
#include <private/qproxystyle_p.h>
#include <private/qapplication_p.h>
#include "qproxystyle.h"
#include "qstylefactory.h"

#if !defined(QT_NO_STYLE_PROXY) || defined(QT_PLUGIN)

QT_BEGIN_NAMESPACE

/*!
    \class QProxyStyle

    \brief The QProxyStyle is a convenience class that simplifies
    the creation of proxy styles in Qt.

    \since 4.6

    A proxy style is a style that wraps a different,
    usually the default system style, to override the painting or
    behavior of only specific parts.

    Here's an example allowing you to override the shortcut underline
    behavior on all platforms:

    \snippet doc/src/snippets/code/src_gui_proxystyle.cpp 0

    Warning: Note that even though Qt's internal styles should respect this.
    hint, there is no guarantee that it will work for all styles.
    The example above would for instance not work on Mac since menus are
    handled by the operating system.

    \sa QStyle
*/
void QProxyStylePrivate::ensureBaseStyle() const
{
    Q_Q(const QProxyStyle);

    if (baseStyle)
        return;

    if (!baseStyle && !QApplicationPrivate::styleOverride.isEmpty()) {
        baseStyle = QStyleFactory::create(QApplicationPrivate::styleOverride);
        if (baseStyle) {
            // If baseStyle is an instance of the same proxyStyle
            // we destroy it and fall back to the desktop style
            if (qstrcmp(baseStyle->metaObject()->className(),
                        q->metaObject()->className()) == 0) {
                delete baseStyle;
                baseStyle = 0;
            }
        }
    } 

    if (!baseStyle) // Use application desktop style
        baseStyle = QStyleFactory::create(QApplicationPrivate::desktopStyleKey());

    if (!baseStyle) // Fallback to windows style
        baseStyle = QStyleFactory::create(QLatin1String("windows"));

    baseStyle->setProxy(const_cast<QProxyStyle*>(q));
    baseStyle->setParent(const_cast<QProxyStyle*>(q)); // Take ownership
}

/*!
    Constructs a QProxyStyle object.

    If no base style is provided, the current application style
    will be used as the base style.

    Ownership of \style is transferred to QProxyStyle.
*/
QProxyStyle::QProxyStyle(QStyle *style) :
    QCommonStyle(*new QProxyStylePrivate())
{
    Q_D(QProxyStyle);
    if (style) {
        style->setProxy(this);
        style->setParent(this); // Take ownership
        d->baseStyle = style;
    }
}

/*!
    Destroys the QProxyStyle object.
*/
QProxyStyle::~QProxyStyle()
{
}

/*!
    Returns the proxy base style object. If no base style
    is set on the proxy style, QProxyStyle will create
    an instance of the application style instead.

    \sa setBaseStyle(), QStyle
*/
QStyle *QProxyStyle::baseStyle() const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle;
}

/*!
    Sets the base style that should be proxied.

    Ownership of \style is transferred to QProxyStyle.

    If style is zero, a desktop-dependant style will be
    assigned automatically.
*/
void QProxyStyle::setBaseStyle(QStyle *style)
{
    Q_D (QProxyStyle);

    if (d->baseStyle && d->baseStyle->parent() == this)
        d->baseStyle->deleteLater();

    d->baseStyle = style;

    if (d->baseStyle) {
        d->baseStyle->setProxy(this);
        d->baseStyle->setParent(this);
    }
}

/*! reimp */
void QProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->drawPrimitive(element, option, painter, widget);
}

/*! reimp */
void QProxyStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->drawControl(element, option, painter, widget);
}

/*! reimp */
void QProxyStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->drawComplexControl(control, option, painter, widget);
}

/*! reimp */
void QProxyStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled,
                               const QString &text, QPalette::ColorRole textRole) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->drawItemText(painter, rect, flags, pal, enabled, text, textRole);
}

/*! reimp */
void QProxyStyle::drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->drawItemPixmap(painter, rect, alignment, pixmap);
}

/*! reimp */
QSize QProxyStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->sizeFromContents(type, option, size, widget);
}

/*! reimp */
QRect QProxyStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->subElementRect(element, option, widget);
}

/*! reimp */
QRect QProxyStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *option, SubControl sc, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->subControlRect(cc, option, sc, widget);
}

/*! reimp */
QRect QProxyStyle::itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled, const QString &text) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->itemTextRect(fm, r, flags, enabled, text);
}

/*! reimp */
QRect QProxyStyle::itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->itemPixmapRect(r, flags, pixmap);
}

/*! reimp */
QStyle::SubControl QProxyStyle::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &pos, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->hitTestComplexControl(control, option, pos, widget);
}

/*! reimp */
int QProxyStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->styleHint(hint, option, widget, returnData);
}

/*! reimp */
int QProxyStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->pixelMetric(metric, option, widget);
}

/*! reimp */
QPixmap QProxyStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->standardPixmap(standardPixmap, opt, widget);
}

/*! reimp */
QPixmap QProxyStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *opt) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->generatedIconPixmap(iconMode, pixmap, opt);
}

/*! reimp */
QPalette QProxyStyle::standardPalette() const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->standardPalette();
}

/*! reimp */
void QProxyStyle::polish(QWidget *widget)
{
    Q_D (QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->polish(widget);
}

/*! reimp */
void QProxyStyle::polish(QPalette &pal)
{
    Q_D (QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->polish(pal);
}

/*! reimp */
void QProxyStyle::polish(QApplication *app)
{
    Q_D (QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->polish(app);
}

/*! reimp */
void QProxyStyle::unpolish(QWidget *widget)
{
    Q_D (QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->unpolish(widget);
}

/*! reimp */
void QProxyStyle::unpolish(QApplication *app)
{
    Q_D (QProxyStyle);
    d->ensureBaseStyle();
    d->baseStyle->unpolish(app);
}

/*! reimp */
bool QProxyStyle::event(QEvent *e)
{
    Q_D (QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->event(e);
}

/*! reimp */
QIcon QProxyStyle::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->standardIcon(standardIcon, option, widget);
}

/*! reimp */
int QProxyStyle::layoutSpacingImplementation(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
                                    Qt::Orientation orientation, const QStyleOption *option, const QWidget *widget) const
{
    Q_D (const QProxyStyle);
    d->ensureBaseStyle();
    return d->baseStyle->layoutSpacing(control1, control2, orientation, option, widget);
}

QT_END_NAMESPACE

#endif // QT_NO_STYLE_PROXY
