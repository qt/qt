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

#include "qfxtext.h"
#include "qfxtext_p.h"

#include <private/qtextcontrol_p.h>
#include <private/qfxperf_p.h>
#include <QTextLayout>
#include <QTextLine>
#include <QTextDocument>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Text,QFxText)

/*!
    \qmlclass Text QFxText
    \brief The Text item allows you to add formatted text to a scene.
    \inherits Item

    It can display both plain and rich text. For example:

    \qml
    Text { text: "Hello World!"; font.family: "Helvetica"; font.size: 24; color: "red" }
    Text { text: "<b>Hello</b> <i>World!</i>" }
    \endqml

    \image declarative-text.png

    If height and width are not explicitly set, Text will attempt to determine how
    much room is needed and set it accordingly. Unless \c wrap is set, it will always
    prefer width to height (all text will be placed on a single line).

    The \c elide property can alternatively be used to fit a single line of
    plain text to a set width.

    Text provides read-only text. For editable text, see \l TextEdit.
*/

/*!
    \internal
    \class QFxText
    \qmlclass Text
    \ingroup group_coreitems

    \brief The QFxText class provides a formatted text item that you can add to a QFxView.

    Text was designed for read-only text; it does not allow for any text editing.
    It can display both plain and rich text. For example:

    \qml
    Text { text: "Hello World!"; font.family: "Helvetica"; font.size: 24; color: "red" }
    Text { text: "<b>Hello</b> <i>World!</i>" }
    \endqml

    \image text.png

    If height and width are not explicitly set, Text will attempt to determine how
    much room is needed and set it accordingly. Unless \c wrap is set, it will always
    prefer width to height (all text will be placed on a single line).

    The \c elide property can alternatively be used to fit a line of plain text to a set width.

    A QFxText object can be instantiated in Qml using the tag \c Text.
*/
QFxText::QFxText(QFxItem *parent)
  : QFxItem(*(new QFxTextPrivate), parent)
{
    Q_D(QFxText);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QFxText::QFxText(QFxTextPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    Q_D(QFxText);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QFxText::~QFxText()
{
}

/*!
    \qmlproperty string Text::font.family
    \qmlproperty bool Text::font.bold
    \qmlproperty bool Text::font.italic
    \qmlproperty real Text::font.size

    Set the Text's font attributes.

    \note \c font.size sets the font's point size (not pixel size).
*/

/*!
    \property QFxText::font
    \brief the font used to display the text.
*/

QFont QFxText::font() const
{
    Q_D(const QFxText);
    return d->font;
}

void QFxText::setFont(const QFont &font)
{
    Q_D(QFxText);
    d->font = font;

    d->imgDirty = true;
    d->updateSize();
    update();
}

void QFxText::setText(const QString &n)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::QFxText_setText> st;
#endif
    Q_D(QFxText);
    if (d->text == n)
        return;

    d->richText = d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(n));
    if (d->richText) {
        if (!d->doc)
        {
            d->control = new QTextControl(this);
            d->control->setTextInteractionFlags(Qt::TextBrowserInteraction);
            d->doc = d->control->document();
            d->doc->setDocumentMargin(0);
        }
        d->doc->setHtml(n);
    }

    d->text = n;
    d->imgDirty = true;
    d->updateSize();
    emit textChanged(d->text);
    update();
}

/*!
    \qmlproperty string Text::text

    The text to display.  Text supports both plain and rich text strings.

    The item will try to automatically determine whether the text should
    be treated as rich text. This determination is made using Qt::mightBeRichText().
*/
QString QFxText::text() const
{
    Q_D(const QFxText);
    return d->text;
}

void QFxText::setColor(const QColor &color)
{
    Q_D(QFxText);
    if (d->color == color)
        return;

    d->imgDirty = true;
    d->color = color;
    update();
}

/*!
    \qmlproperty color Text::color

    The text color.

    \qml
    //green text using hexadecimal notation
    Text { color: "#00FF00"; ... }

    //steelblue text using SVG color name
    Text { color: "steelblue"; ... }
    \endqml
*/

QColor QFxText::color() const
{
    Q_D(const QFxText);
    return d->color;
}

/*!
    \qmlproperty enumeration Text::style

    Set an additional text style.

    Supported text styles are \c Normal, \c Outline, \c Raised and \c Sunken.

    \qml
    HorizontalPositioner {
        Text { font.size: 24; text: "Normal" }
        Text { font.size: 24; text: "Raised";  style: "Raised";  styleColor: "#AAAAAA" }
        Text { font.size: 24; text: "Outline"; style: "Outline"; styleColor: "red" }
        Text { font.size: 24; text: "Sunken";  style: "Sunken";  styleColor: "#AAAAAA" }
    }
    \endqml

    \image declarative-textstyle.png
*/

/*!
    \property QFxText::style
    \brief an additional style of the text to display.

    By default, the text style is Normal.

    \note This property is used to support text styles not natively
    handled by QFont or QPainter::drawText().
*/
QFxText::TextStyle QFxText::style() const
{
    Q_D(const QFxText);
    return d->style;
}

void QFxText::setStyle(QFxText::TextStyle style)
{
    Q_D(QFxText);
    if (d->style == style)
        return;

    d->imgDirty = true;
    d->style = style;
    update();
}

void QFxText::setStyleColor(const QColor &color)
{
    Q_D(QFxText);
    if (d->styleColor == color)
        return;

    d->imgDirty = true;
    d->styleColor = color;
    update();
}

/*!
    \qmlproperty color Text::styleColor

    Defines the secondary color used by text styles.

    \c styleColor is used as the outline color for outlined text, and as the
    shadow color for raised or sunken text. If no style has been set, it is not
    used at all.
 */
QColor QFxText::styleColor() const
{
    Q_D(const QFxText);
    return d->styleColor;
}

/*!
    \qmlproperty enumeration Text::hAlign
    \qmlproperty enumeration Text::vAlign

    Sets the horizontal and vertical alignment of the text within the Text items
    width and height.  By default, the text is top-left aligned.

    The valid values for \c hAlign are \c AlignLeft, \c AlignRight and
    \c AlignHCenter.  The valid values for \c vAlign are \c AlignTop, \c AlignBottom
    and \c AlignVCenter.
*/

/*!
    \property QFxText::hAlign
    \brief the horizontal alignment of the text.

    Valid values are \c AlignLeft, \c AlignRight, and \c AlignHCenter. The default value is \c AlignLeft.
*/
QFxText::HAlignment QFxText::hAlign() const
{
    Q_D(const QFxText);
    return d->hAlign;
}

void QFxText::setHAlign(HAlignment align)
{
    Q_D(QFxText);
    d->hAlign = align;
}

/*!
    \property QFxText::vAlign
    \brief the vertical alignment of the text.

    Valid values are \c AlignTop, \c AlignBottom, and \c AlignVCenter. The default value is \c AlignTop.
*/
QFxText::VAlignment QFxText::vAlign() const
{
    Q_D(const QFxText);
    return d->vAlign;
}

void QFxText::setVAlign(VAlignment align)
{
    Q_D(QFxText);
    d->vAlign = align;
}

/*!
    \qmlproperty bool Text::wrap

    Set this property to wrap the text to the Text item's width.  The text will only
    wrap if an explicit width has been set.

    Wrapping is done on word boundaries (i.e. it is a "word-wrap"). If the text cannot be
    word-wrapped to the specified width it will be partially drawn outside of the item's bounds.
    If this is undesirable then enable clipping on the item (Item::clip).

    Wrapping is off by default.
*/
//### Future may provide choice of wrap modes, such as QTextOption::WrapAtWordBoundaryOrAnywhere
bool QFxText::wrap() const
{
    Q_D(const QFxText);
    return d->wrap;
}

void QFxText::setWrap(bool w)
{
    Q_D(QFxText);
    if (w == d->wrap)
        return;

    d->wrap = w;

    d->imgDirty = true;
    d->updateSize();
}

/*!
    \qmlproperty enumeration Text::textFormat

    The way the text property should be displayed.

    Supported text formats are \c AutoText, \c PlainText and \c RichText.

    The default is AutoText.  If the text format is AutoText the text element
    will automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().

    \table
    \row
    \o
    \qml
VerticalPositioner {
    TextEdit {
        font.size: 24
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.size: 24
        textFormat: "RichText"
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.size: 24
        textFormat: "PlainText"
        text: "<b>Hello</b> <i>World!</i>"
    }
}
    \endqml
    \o \image declarative-textformat.png
    \endtable
*/

QFxText::TextFormat QFxText::textFormat() const
{
    Q_D(const QFxText);
    return d->format;
}

void QFxText::setTextFormat(TextFormat format)
{
    Q_D(QFxText);
    if (format == d->format)
        return;
    bool wasRich = d->richText;
    d->richText = format == RichText || (format == AutoText && Qt::mightBeRichText(d->text));

    if (wasRich && !d->richText) {
        //### delete control? (and vice-versa below)
        d->imgDirty = true;
        d->updateSize();
        update();
    } else if (!wasRich && d->richText) {
        if (!d->doc)
        {
            d->control = new QTextControl(this);
            d->control->setTextInteractionFlags(Qt::TextBrowserInteraction);
            d->doc = d->control->document();
            d->doc->setDocumentMargin(0);
        }
        d->doc->setHtml(d->text);
        d->imgDirty = true;
        d->updateSize();
        update();
    }
    d->format = format;
}

/*!
    \qmlproperty Qt::TextElideMode Text::elide

    Set this property to elide parts of the text fit to the Text item's width.
    The text will only elide if an explicit width has been set.

    This property cannot be used with wrap enabled or with rich text.

    Eliding can be ElideNone, ElideLeft, ElideMiddle, or ElideRight.

    ElideNone is the default.
*/
Qt::TextElideMode QFxText::elideMode() const
{
    Q_D(const QFxText);
    return d->elideMode;
}

void QFxText::setElideMode(Qt::TextElideMode mode)
{
    Q_D(QFxText);
    if (mode == d->elideMode)
        return;

    d->elideMode = mode;

    d->imgDirty = true;
    d->updateSize();
}

void QFxText::geometryChanged(const QRectF &newGeometry,
                              const QRectF &oldGeometry)
{
    Q_D(QFxText);
    if (newGeometry.width() != oldGeometry.width()) {
        if (d->wrap || d->elideMode != Qt::ElideNone) {
            d->imgDirty = true;
            d->updateSize();
        }
    }
    QFxItem::geometryChanged(newGeometry, oldGeometry);
}

void QFxTextPrivate::updateSize()
{
    Q_Q(QFxText);
    if (q->isComponentComplete()) {
        if (text.isEmpty()) {
            return;
        }
        QFontMetrics fm(font);
        int dy = q->height();

        QString tmp;
        QSize size(0, 0);

        //setup instance of QTextLayout for all cases other than richtext
        if (!richText)
            {
                tmp = text;
                tmp.replace(QLatin1Char('\n'), QChar::LineSeparator);
                singleline = !tmp.contains(QChar::LineSeparator);
                if (singleline && elideMode != Qt::ElideNone && q->widthValid())
                    tmp = fm.elidedText(tmp,elideMode,q->width()); // XXX still worth layout...?
                layout.clearLayout();
                layout.setFont(font);
                layout.setText(tmp);
                size = setupTextLayout(&layout);
                cachedLayoutSize = size;
            }
        if (richText) {
            singleline = false; // richtext can't elide or be optimized for single-line case
            doc->setDefaultFont(font);
            QTextOption option((Qt::Alignment)int(hAlign | vAlign));
            if (wrap)
                option.setWrapMode(QTextOption::WordWrap);
            else
                option.setWrapMode(QTextOption::NoWrap);
            doc->setDefaultTextOption(option);
            if (wrap && !q->heightValid() && q->widthValid())
                doc->setTextWidth(q->width());
            else
                doc->setTextWidth(doc->idealWidth()); // ### Text does not align if width is not set (QTextDoc bug)
            dy -= (int)doc->size().height();
        } else {
            dy -= size.height();
        }
        int yoff = 0;

        if (q->heightValid()) {
            if (vAlign == QFxText::AlignBottom)
                yoff = dy;
            else if (vAlign == QFxText::AlignVCenter)
                yoff = dy/2;
        }
        q->setBaselineOffset(fm.ascent() + yoff);

        if (!q->widthValid()) {
            int newWidth = (richText ? (int)doc->idealWidth() : size.width());
            q->setImplicitWidth(newWidth);
        }
        if (!q->heightValid()) {
            if (richText) {
                q->setImplicitHeight((int)doc->size().height());
            } else {
                q->setImplicitHeight(size.height());
            }
        }
    } else {
        dirty = true;
    }
}

// ### text layout handling should be profiled and optimized as needed
// what about QStackTextEngine engine(tmp, d->font.font()); QTextLayout textLayout(&engine);

void QFxTextPrivate::drawOutline()
{
    QPixmap img = QPixmap(imgCache.size());
    img.fill(Qt::transparent);

    QPainter ppm(&img);

    QPoint pos(imgCache.rect().topLeft());
    pos += QPoint(-1, 0);
    ppm.drawPixmap(pos, imgStyleCache);
    pos += QPoint(2, 0);
    ppm.drawPixmap(pos, imgStyleCache);
    pos += QPoint(-1, -1);
    ppm.drawPixmap(pos, imgStyleCache);
    pos += QPoint(0, 2);
    ppm.drawPixmap(pos, imgStyleCache);

    pos += QPoint(0, -1);
    ppm.drawPixmap(pos, imgCache);
    ppm.end();

    imgCache = img;
}

void QFxTextPrivate::drawOutline(int yOffset)
{
    QPixmap img = QPixmap(imgCache.size());
    img.fill(Qt::transparent);

    QPainter ppm(&img);

    QPoint pos(imgCache.rect().topLeft());
    pos += QPoint(0, yOffset);
    ppm.drawPixmap(pos, imgStyleCache);

    pos += QPoint(0, -yOffset);
    ppm.drawPixmap(pos, imgCache);
    ppm.end();

    imgCache = img;
}

QSize QFxTextPrivate::setupTextLayout(QTextLayout *layout)
{
    Q_Q(QFxText);
    layout->setCacheEnabled(true);

    QFontMetrics fm = QFontMetrics(font);

    int height = 0;
    qreal widthUsed = 0;
    qreal lineWidth = 0;

    //set manual width
    if ((wrap || elideMode != Qt::ElideNone) && q->widthValid())
        lineWidth = q->width();

    layout->beginLayout();

    while (1) {
        QTextLine line = layout->createLine();
        if (!line.isValid())
            break;

        if ((wrap || elideMode != Qt::ElideNone) && q->widthValid())
            line.setLineWidth(lineWidth);
    }
    layout->endLayout();

    for (int i = 0; i < layout->lineCount(); ++i) {
        QTextLine line = layout->lineAt(i);
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
        line.setPosition(QPointF(0, height));
        height += int(line.height());
    }
    return QSize((int)widthUsed, height);
}

QPixmap QFxTextPrivate::wrappedTextImage(bool drawStyle)
{
    //do layout
    QSize size = cachedLayoutSize;

    int x = 0;
    for (int i = 0; i < layout.lineCount(); ++i) {
        QTextLine line = layout.lineAt(i);
        if (hAlign == QFxText::AlignLeft) {
            x = 0;
        } else if (hAlign == QFxText::AlignRight) {
            x = size.width() - (int)line.naturalTextWidth();
        } else if (hAlign == QFxText::AlignHCenter) {
            x = (size.width() - (int)line.naturalTextWidth()) / 2;
        }
        line.setPosition(QPoint(x, (int)line.y()));
    }

    //paint text
    QPixmap img(size);
    img.fill(Qt::transparent);
    QPainter p(&img);
    if (drawStyle) {
        p.setPen(styleColor);
    }
    else
        p.setPen(color);
    p.setFont(font);
    layout.draw(&p, QPointF(0, 0));
    return img;
}

QPixmap QFxTextPrivate::richTextImage(bool drawStyle)
{
    QSize size = doc->size().toSize();

    //paint text
    QPixmap img(size);
    img.fill(Qt::transparent);
    QPainter p(&img);

    if (drawStyle) {
        QPalette pal = control->palette();
        pal.setColor(QPalette::Text, styleColor);
        control->setPalette(pal);
        QTextOption colorOption;
        colorOption.setFlags(QTextOption::SuppressColors);
        doc->setDefaultTextOption(colorOption);
    } else {
        QPalette pal = control->palette();
        pal.setColor(QPalette::Text, color);
        control->setPalette(pal);
    }
    control->drawContents(&p, QRectF(QPointF(0, 0), QSizeF(size)));
    if (drawStyle)
        doc->setDefaultTextOption(QTextOption());
    return img;
}

void QFxTextPrivate::checkImgCache()
{
    if (!imgDirty)
        return;

    bool empty = text.isEmpty();
    if (empty) {
        imgCache = QPixmap();
        imgStyleCache = QPixmap();
    } else if (richText) {
        imgCache = richTextImage(false);
        if (style != QFxText::Normal)
            imgStyleCache = richTextImage(true); //### should use styleColor
    } else {
        imgCache = wrappedTextImage(false);
        if (style != QFxText::Normal)
            imgStyleCache = wrappedTextImage(true); //### should use styleColor
    }
    if (!empty)
        switch (style) {
        case QFxText::Outline:
            drawOutline();
            break;
        case QFxText::Sunken:
            drawOutline(-1);
            break;
        case QFxText::Raised:
            drawOutline(1);
            break;
        default:
            break;
        }

    imgDirty = false;
}

void QFxText::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(QFxText);
    d->checkImgCache();
    if (d->imgCache.isNull())
        return;

    bool oldAA = p->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = p->testRenderHint(QPainter::SmoothPixmapTransform);
    if (d->smooth)
        p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, d->smooth);

    int w = width();
    int h = height();

    int x = 0;
    int y = 0;

    switch (d->hAlign) {
    case AlignLeft:
        x = 0;
        break;
    case AlignRight:
        x = w - d->imgCache.width();
        break;
    case AlignHCenter:
        x = (w - d->imgCache.width()) / 2;
        break;
    }

    switch (d->vAlign) {
    case AlignTop:
        y = 0;
        break;
    case AlignBottom:
        y = h - d->imgCache.height();
        break;
    case AlignVCenter:
        y = (h - d->imgCache.height()) / 2;
        break;
    }

    bool needClip = !clip() && (d->imgCache.width() > width() ||
                                d->imgCache.height() > height());

    if (needClip) {
        p->save();
        p->setClipRect(boundingRect());
    }
    p->drawPixmap(x, y, d->imgCache);
    if (needClip)
        p->restore();

    if (d->smooth) {
        p->setRenderHint(QPainter::Antialiasing, oldAA);
        p->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    }
}

/*!
    \qmlproperty bool Text::smooth

    Set this property if you want the text to be smoothly scaled or
    transformed.  Smooth filtering gives better visual quality, but is slower.  If
    the item is displayed at its natural size, this property has no visual or
    performance effect.

    \note Generally scaling artifacts are only visible if the item is stationary on
    the screen.  A common pattern when animating an item is to disable smooth
    filtering at the beginning of the animation and reenable it at the conclusion.
*/

void QFxText::componentComplete()
{
    Q_D(QFxText);
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::TextComponentComplete> cc;
#endif
    QFxItem::componentComplete();
    if (d->dirty) {
        d->updateSize();
        d->dirty = false;
    }
}

/*!
  \overload
  Handles the given mouse \a event.
 */
void QFxText::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxText);

    if (!d->richText || !d->doc || d->control->anchorAt(event->pos()).isEmpty()) {
        event->setAccepted(false);
        d->activeLink = QString();
    } else {
        d->activeLink = d->control->anchorAt(event->pos());
    }

    // ### may malfunction if two of the same links are clicked & dragged onto each other)

    if (!event->isAccepted())
        QFxItem::mousePressEvent(event);

}

/*!
    \qmlsignal Text::linkActivated(link)

    This handler is called when the user clicks on a link embedded in the text.
*/

/*!
  \overload
  Handles the given mouse \a event.
 */
void QFxText::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxText);

        // ### confirm the link, and send a signal out
    if (d->richText && d->doc && d->activeLink == d->control->anchorAt(event->pos()))
        emit linkActivated(d->activeLink);
    else
        event->setAccepted(false);

    if (!event->isAccepted())
        QFxItem::mouseReleaseEvent(event);
}
QT_END_NAMESPACE
