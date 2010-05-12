/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "private/qdeclarativetext_p.h"
#include "private/qdeclarativetext_p_p.h"
#include <qdeclarativestyledtext_p.h>
#include <qdeclarativeinfo.h>
#include <qdeclarativepixmapcache_p.h>

#include <QSet>
#include <QTextLayout>
#include <QTextLine>
#include <QTextDocument>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <qmath.h>

QT_BEGIN_NAMESPACE

class QTextDocumentWithImageResources : public QTextDocument {
    Q_OBJECT

public:
    QTextDocumentWithImageResources(QDeclarativeText *parent) :
        QTextDocument(parent),
        outstanding(0)
    {
    }

    int resourcesLoading() const { return outstanding; }

protected:
    QVariant loadResource(int type, const QUrl &name)
    {
        QUrl url = qmlContext(parent())->resolvedUrl(name);

        if (type == QTextDocument::ImageResource) {
            QPixmap pm;
            QString errorString;
            QDeclarativePixmapReply::Status status = QDeclarativePixmapCache::get(url, &pm, &errorString, 0, false, 0, 0);
            if (status == QDeclarativePixmapReply::Ready)
                return pm;
            if (status == QDeclarativePixmapReply::Error) {
                if (!errors.contains(url)) {
                    errors.insert(url);
                    qmlInfo(parent()) << errorString;
                }
            } else {
                QDeclarativePixmapReply *reply = QDeclarativePixmapCache::request(qmlEngine(parent()), url);
                connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
                outstanding++;
            }
        }

        return QTextDocument::loadResource(type,url); // The *resolved* URL
    }

private slots:
    void requestFinished()
    {
        outstanding--;
        if (outstanding == 0)
            static_cast<QDeclarativeText*>(parent())->reloadWithResources();
    }

private:
    int outstanding;
    static QSet<QUrl> errors;
};

QSet<QUrl> QTextDocumentWithImageResources::errors;

/*!
    \qmlclass Text QDeclarativeText
  \since 4.7
    \brief The Text item allows you to add formatted text to a scene.
    \inherits Item

    A Text item can display both plain and rich text. For example:

    \qml
    Text { text: "Hello World!"; font.family: "Helvetica"; font.pointSize: 24; color: "red" }
    Text { text: "<b>Hello</b> <i>World!</i>" }
    \endqml

    \image declarative-text.png

    If height and width are not explicitly set, Text will attempt to determine how
    much room is needed and set it accordingly. Unless \c wrapMode is set, it will always
    prefer width to height (all text will be placed on a single line).

    The \c elide property can alternatively be used to fit a single line of
    plain text to a set width.

    Note that the \l{Supported HTML Subset} is limited. Also, if the text contains
    HTML img tags that load remote images, the text is reloaded.

    Text provides read-only text. For editable text, see \l TextEdit.
*/

/*!
    \internal
    \class QDeclarativeText
    \qmlclass Text

    \brief The QDeclarativeText class provides a formatted text item that you can add to a QDeclarativeView.

    Text was designed for read-only text; it does not allow for any text editing.
    It can display both plain and rich text. For example:

    \qml
    Text { text: "Hello World!"; font.family: "Helvetica"; font.pointSize: 24; color: "red" }
    Text { text: "<b>Hello</b> <i>World!</i>" }
    \endqml

    \image text.png

    If height and width are not explicitly set, Text will attempt to determine how
    much room is needed and set it accordingly. Unless \c wrapMode is set, it will always
    prefer width to height (all text will be placed on a single line).

    The \c elide property can alternatively be used to fit a line of plain text to a set width.

    A QDeclarativeText object can be instantiated in Qml using the tag \c Text.
*/
QDeclarativeText::QDeclarativeText(QDeclarativeItem *parent)
  : QDeclarativeItem(*(new QDeclarativeTextPrivate), parent)
{
}

QDeclarativeText::~QDeclarativeText()
{
}


QDeclarativeTextPrivate::~QDeclarativeTextPrivate()
{
}

/*!
    \qmlproperty string Text::font.family

    Sets the family name of the font.

    The family name is case insensitive and may optionally include a foundry name, e.g. "Helvetica [Cronyx]".
    If the family is available from more than one foundry and the foundry isn't specified, an arbitrary foundry is chosen.
    If the family isn't available a family will be set using the font matching algorithm.
*/

/*!
    \qmlproperty bool Text::font.bold

    Sets whether the font weight is bold.
*/

/*!
    \qmlproperty enumeration Text::font.weight

    Sets the font's weight.

    The weight can be one of:
    \list
    \o Font.Light
    \o Font.Normal - the default
    \o Font.DemiBold
    \o Font.Bold
    \o Font.Black
    \endlist

    \qml
    Text { text: "Hello"; font.weight: Font.DemiBold }
    \endqml
*/

/*!
    \qmlproperty bool Text::font.italic

    Sets whether the font has an italic style.
*/

/*!
    \qmlproperty bool Text::font.underline

    Sets whether the text is underlined.
*/

/*!
    \qmlproperty bool Text::font.outline

    Sets whether the font has an outline style.
*/

/*!
    \qmlproperty bool Text::font.strikeout

    Sets whether the font has a strikeout style.
*/

/*!
    \qmlproperty real Text::font.pointSize

    Sets the font size in points. The point size must be greater than zero.
*/

/*!
    \qmlproperty int Text::font.pixelSize

    Sets the font size in pixels.

    Using this function makes the font device dependent.
    Use \c pointSize to set the size of the font in a device independent manner.
*/

/*!
    \qmlproperty real Text::font.letterSpacing

    Sets the letter spacing for the font.

    Letter spacing changes the default spacing between individual letters in the font.
    A value of 100 will keep the spacing unchanged; a value of 200 will enlarge the spacing after a character by
    the width of the character itself.
*/

/*!
    \qmlproperty real Text::font.wordSpacing

    Sets the word spacing for the font.

    Word spacing changes the default spacing between individual words.
    A positive value increases the word spacing by a corresponding amount of pixels,
    while a negative value decreases the inter-word spacing accordingly.
*/

/*!
    \qmlproperty enumeration Text::font.capitalization

    Sets the capitalization for the text.

    \list
    \o Font.MixedCase - This is the normal text rendering option where no capitalization change is applied.
    \o Font.AllUppercase - This alters the text to be rendered in all uppercase type.
    \o Font.AllLowercase	 - This alters the text to be rendered in all lowercase type.
    \o Font.SmallCaps -	This alters the text to be rendered in small-caps type.
    \o Font.Capitalize - This alters the text to be rendered with the first character of each word as an uppercase character.
    \endlist

    \qml
    Text { text: "Hello"; font.capitalization: Font.AllLowercase }
    \endqml
*/

QFont QDeclarativeText::font() const
{
    Q_D(const QDeclarativeText);
    return d->font;
}

void QDeclarativeText::setFont(const QFont &font)
{
    Q_D(QDeclarativeText);
    if (d->font == font)
        return;

    d->font = font;

    d->updateLayout();
    d->markImgDirty();
    emit fontChanged(d->font);
}

void QDeclarativeText::setText(const QString &n)
{
    Q_D(QDeclarativeText);
    if (d->text == n)
        return;

    d->richText = d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(n));
    if (d->richText) {
        if (isComponentComplete()) {
            d->ensureDoc();
            d->doc->setHtml(n);
        }
    }

    d->text = n;
    d->updateLayout();
    d->markImgDirty();
    emit textChanged(d->text);
}

/*!
    \qmlproperty string Text::text

    The text to display. Text supports both plain and rich text strings.

    The item will try to automatically determine whether the text should
    be treated as rich text. This determination is made using Qt::mightBeRichText().
*/
QString QDeclarativeText::text() const
{
    Q_D(const QDeclarativeText);
    return d->text;
}

void QDeclarativeText::setColor(const QColor &color)
{
    Q_D(QDeclarativeText);
    if (d->color == color)
        return;

    d->color = color;
    d->markImgDirty();
    emit colorChanged(d->color);
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

QColor QDeclarativeText::color() const
{
    Q_D(const QDeclarativeText);
    return d->color;
}

/*!
    \qmlproperty enumeration Text::style

    Set an additional text style.

    Supported text styles are:
    \list
    \o Text.Normal - the default
    \o Text.Outline
    \o Text.Raised
    \o Text.Sunken
    \endlist

    \qml
    Row {
        Text { font.pointSize: 24; text: "Normal" }
        Text { font.pointSize: 24; text: "Raised"; style: Text.Raised; styleColor: "#AAAAAA" }
        Text { font.pointSize: 24; text: "Outline";style: Text.Outline; styleColor: "red" }
        Text { font.pointSize: 24; text: "Sunken"; style: Text.Sunken; styleColor: "#AAAAAA" }
    }
    \endqml

    \image declarative-textstyle.png
*/
QDeclarativeText::TextStyle QDeclarativeText::style() const
{
    Q_D(const QDeclarativeText);
    return d->style;
}

void QDeclarativeText::setStyle(QDeclarativeText::TextStyle style)
{
    Q_D(QDeclarativeText);
    if (d->style == style)
        return;

    d->style = style;
    d->markImgDirty();
    emit styleChanged(d->style);
}

void QDeclarativeText::setStyleColor(const QColor &color)
{
    Q_D(QDeclarativeText);
    if (d->styleColor == color)
        return;

    d->styleColor = color;
    d->markImgDirty();
    emit styleColorChanged(d->styleColor);
}

/*!
    \qmlproperty color Text::styleColor

    Defines the secondary color used by text styles.

    \c styleColor is used as the outline color for outlined text, and as the
    shadow color for raised or sunken text. If no style has been set, it is not
    used at all.

    \qml
    Text { font.pointSize: 18; text: "hello"; style: Text.Raised; styleColor: "gray" }
    \endqml
 */
QColor QDeclarativeText::styleColor() const
{
    Q_D(const QDeclarativeText);
    return d->styleColor;
}

/*!
    \qmlproperty enumeration Text::horizontalAlignment
    \qmlproperty enumeration Text::verticalAlignment

    Sets the horizontal and vertical alignment of the text within the Text items
    width and height.  By default, the text is top-left aligned.

    The valid values for \c horizontalAlignment are \c Text.AlignLeft, \c Text.AlignRight and
    \c Text.AlignHCenter.  The valid values for \c verticalAlignment are \c Text.AlignTop, \c Text.AlignBottom
    and \c Text.AlignVCenter.

    Note that for a single line of text, the size of the text is the area of the text. In this common case,
    all alignments are equivalent. If you want the text to be, say, centered in it parent, then you will
    need to either modify the Item::anchors, or set horizontalAlignment to Text.AlignHCenter and bind the width to 
    that of the parent.
*/
QDeclarativeText::HAlignment QDeclarativeText::hAlign() const
{
    Q_D(const QDeclarativeText);
    return d->hAlign;
}

void QDeclarativeText::setHAlign(HAlignment align)
{
    Q_D(QDeclarativeText);
    if (d->hAlign == align)
        return;

    d->hAlign = align;
    emit horizontalAlignmentChanged(align);
}

QDeclarativeText::VAlignment QDeclarativeText::vAlign() const
{
    Q_D(const QDeclarativeText);
    return d->vAlign;
}

void QDeclarativeText::setVAlign(VAlignment align)
{
    Q_D(QDeclarativeText);
    if (d->vAlign == align)
        return;

    d->vAlign = align;
    emit verticalAlignmentChanged(align);
}

/*!
    \qmlproperty enumeration Text::wrapMode

    Set this property to wrap the text to the Text item's width.  The text will only
    wrap if an explicit width has been set.  wrapMode can be one of:

    \list
    \o Text.NoWrap - no wrapping will be performed.
    \o Text.WordWrap - wrapping is done on word boundaries. If the text cannot be
    word-wrapped to the specified width it will be partially drawn outside of the item's bounds.
    If this is undesirable then enable clipping on the item (Item::clip).
    \o Text.WrapAnywhere - Text can be wrapped at any point on a line, even if it occurs in the middle of a word.
    \o Text.WrapAtWordBoundaryOrAnywhere - If possible, wrapping occurs at a word boundary; otherwise it
       will occur at the appropriate point on the line, even in the middle of a word.
    \endlist

    The default is Text.NoWrap.
*/
QDeclarativeText::WrapMode QDeclarativeText::wrapMode() const
{
    Q_D(const QDeclarativeText);
    return d->wrapMode;
}

void QDeclarativeText::setWrapMode(WrapMode mode)
{
    Q_D(QDeclarativeText);
    if (mode == d->wrapMode)
        return;

    d->wrapMode = mode;

    d->updateLayout();
    d->markImgDirty();
    emit wrapModeChanged();
}


/*!
    \qmlproperty enumeration Text::textFormat

    The way the text property should be displayed.

    Supported text formats are:
    
    \list
    \o Text.AutoText
    \o Text.PlainText
    \o Text.RichText
    \o Text.StyledText
    \endlist

    The default is Text.AutoText.  If the text format is Text.AutoText the text element
    will automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().

    Text.StyledText is an optimized format supporting some basic text
    styling markup, in the style of html 3.2:

    \code
    <font size="4" color="#ff0000">font size and color</font>
    <b>bold</b>
    <i>italic</i>
    <br>
    &gt; &lt; &amp;
    \endcode

    \c Text.StyledText parser is strict, requiring tags to be correctly nested.

    \table
    \row
    \o
    \qml
Column {
    Text {
        font.pointSize: 24
        text: "<b>Hello</b> <i>World!</i>"
    }
    Text {
        font.pointSize: 24
        textFormat: Text.RichText
        text: "<b>Hello</b> <i>World!</i>"
    }
    Text {
        font.pointSize: 24
        textFormat: Text.PlainText
        text: "<b>Hello</b> <i>World!</i>"
    }
}
    \endqml
    \o \image declarative-textformat.png
    \endtable
*/

QDeclarativeText::TextFormat QDeclarativeText::textFormat() const
{
    Q_D(const QDeclarativeText);
    return d->format;
}

void QDeclarativeText::setTextFormat(TextFormat format)
{
    Q_D(QDeclarativeText);
    if (format == d->format)
        return;
    d->format = format;
    bool wasRich = d->richText;
    d->richText = format == RichText || (format == AutoText && Qt::mightBeRichText(d->text));

    if (wasRich && !d->richText) {
        //### delete control? (and vice-versa below)
        d->updateLayout();
        d->markImgDirty();
    } else if (!wasRich && d->richText) {
        if (isComponentComplete()) {
            d->ensureDoc();
            d->doc->setHtml(d->text);
        }
        d->updateLayout();
        d->markImgDirty();
    }

    emit textFormatChanged(d->format);
}

/*!
    \qmlproperty enumeration Text::elide

    Set this property to elide parts of the text fit to the Text item's width.
    The text will only elide if an explicit width has been set.

    This property cannot be used with wrapping enabled or with rich text.

    Eliding can be:
    \list
    \o Text.ElideNone  - the default
    \o Text.ElideLeft
    \o Text.ElideMiddle
    \o Text.ElideRight
    \endlist

    If the text is a multi-length string, and the mode is not \c Text.ElideNone,
    the first string that fits will be used, otherwise the last will be elided.

    Multi-length strings are ordered from longest to shortest, separated by the
    Unicode "String Terminator" character \c U009C (write this in QML with \c{"\u009C"} or \c{"\x9C"}).
*/
QDeclarativeText::TextElideMode QDeclarativeText::elideMode() const
{
    Q_D(const QDeclarativeText);
    return d->elideMode;
}

void QDeclarativeText::setElideMode(QDeclarativeText::TextElideMode mode)
{
    Q_D(QDeclarativeText);
    if (mode == d->elideMode)
        return;

    d->elideMode = mode;

    d->updateLayout();
    d->markImgDirty();
    emit elideModeChanged(d->elideMode);
}

void QDeclarativeText::geometryChanged(const QRectF &newGeometry,
                              const QRectF &oldGeometry)
{
    Q_D(QDeclarativeText);
    if (newGeometry.width() != oldGeometry.width()) {
        if (d->wrapMode != QDeclarativeText::NoWrap || d->elideMode != QDeclarativeText::ElideNone) {
            //re-elide if needed
            if (d->singleline && d->elideMode != QDeclarativeText::ElideNone &&
                isComponentComplete() && widthValid()) {

                QFontMetrics fm(d->font);
                QString tmp = fm.elidedText(d->text,(Qt::TextElideMode)d->elideMode,width()); // XXX still worth layout...?
                d->layout.setText(tmp);
            }

            d->imgDirty = true;
            d->updateSize();
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void QDeclarativeTextPrivate::updateLayout()
{
    Q_Q(QDeclarativeText);
    if (q->isComponentComplete()) {
        //setup instance of QTextLayout for all cases other than richtext
        if (!richText) {
            layout.clearLayout();
            layout.setFont(font);
            if (format != QDeclarativeText::StyledText) {
                QString tmp = text;
                tmp.replace(QLatin1Char('\n'), QChar::LineSeparator);
                singleline = !tmp.contains(QChar::LineSeparator);
                if (singleline && elideMode != QDeclarativeText::ElideNone && q->widthValid()) {
                    QFontMetrics fm(font);
                    tmp = fm.elidedText(tmp,(Qt::TextElideMode)elideMode,q->width()); // XXX still worth layout...?
                }
                layout.setText(tmp);
            } else {
                singleline = false;
                QDeclarativeStyledText::parse(text, layout);
            }
        }
        updateSize();
    } else {
        dirty = true;
    }
}

void QDeclarativeTextPrivate::updateSize()
{
    Q_Q(QDeclarativeText);
    if (q->isComponentComplete()) {
        QFontMetrics fm(font);
        if (text.isEmpty()) {
            q->setImplicitHeight(fm.height());
            return;
        }

        int dy = q->height();
        QSize size(0, 0);

        //setup instance of QTextLayout for all cases other than richtext
        if (!richText) {
            size = setupTextLayout(&layout);
            cachedLayoutSize = size;
            dy -= size.height();
        } else {
            singleline = false; // richtext can't elide or be optimized for single-line case
            ensureDoc();
            doc->setDefaultFont(font);
            QTextOption option((Qt::Alignment)int(hAlign | vAlign));
            option.setWrapMode(QTextOption::WrapMode(wrapMode));
            doc->setDefaultTextOption(option);
            if (wrapMode != QDeclarativeText::NoWrap && q->widthValid())
                doc->setTextWidth(q->width());
            else
                doc->setTextWidth(doc->idealWidth()); // ### Text does not align if width is not set (QTextDoc bug)
            dy -= (int)doc->size().height();
            cachedLayoutSize = doc->size().toSize();
        }
        int yoff = 0;

        if (q->heightValid()) {
            if (vAlign == QDeclarativeText::AlignBottom)
                yoff = dy;
            else if (vAlign == QDeclarativeText::AlignVCenter)
                yoff = dy/2;
        }
        q->setBaselineOffset(fm.ascent() + yoff);

        //### need to comfirm cost of always setting these for richText
        q->setImplicitWidth(richText ? (int)doc->idealWidth() : size.width());
        q->setImplicitHeight(richText ? (int)doc->size().height() : size.height());
    } else {
        dirty = true;
    }
}

// ### text layout handling should be profiled and optimized as needed
// what about QStackTextEngine engine(tmp, d->font.font()); QTextLayout textLayout(&engine);

void QDeclarativeTextPrivate::drawOutline()
{
    QPixmap img = QPixmap(imgStyleCache.width()+2,imgStyleCache.height()+2);
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

void QDeclarativeTextPrivate::drawOutline(int yOffset)
{
    QPixmap img = QPixmap(imgStyleCache.width()+2,imgStyleCache.height()+2);
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

QSize QDeclarativeTextPrivate::setupTextLayout(QTextLayout *layout)
{
    Q_Q(QDeclarativeText);
    layout->setCacheEnabled(true);

    int height = 0;
    qreal widthUsed = 0;
    qreal lineWidth = 0;

    //set manual width
    if ((wrapMode != QDeclarativeText::NoWrap || elideMode != QDeclarativeText::ElideNone) && q->widthValid())
        lineWidth = q->width();

    QTextOption textOption = layout->textOption();
    textOption.setWrapMode(QTextOption::WrapMode(wrapMode));
    layout->setTextOption(textOption);

    layout->beginLayout();

    while (1) {
        QTextLine line = layout->createLine();
        if (!line.isValid())
            break;

        if ((wrapMode != QDeclarativeText::NoWrap || elideMode != QDeclarativeText::ElideNone) && q->widthValid())
            line.setLineWidth(lineWidth);
    }
    layout->endLayout();

    int x = 0;
    for (int i = 0; i < layout->lineCount(); ++i) {
        QTextLine line = layout->lineAt(i);
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
        line.setPosition(QPointF(0, height));
        height += int(line.height());

        if (!cache) {
            if (hAlign == QDeclarativeText::AlignLeft) {
                x = 0;
            } else if (hAlign == QDeclarativeText::AlignRight) {
                x = q->width() - (int)line.naturalTextWidth();
            } else if (hAlign == QDeclarativeText::AlignHCenter) {
                x = (q->width() - (int)line.naturalTextWidth()) / 2;
            }
            line.setPosition(QPoint(x, (int)line.y()));
        }
    }

    return QSize(qCeil(widthUsed), height);
}

QPixmap QDeclarativeTextPrivate::wrappedTextImage(bool drawStyle)
{
    //do layout
    QSize size = cachedLayoutSize;

    int x = 0;
    for (int i = 0; i < layout.lineCount(); ++i) {
        QTextLine line = layout.lineAt(i);
        if (hAlign == QDeclarativeText::AlignLeft) {
            x = 0;
        } else if (hAlign == QDeclarativeText::AlignRight) {
            x = size.width() - (int)line.naturalTextWidth();
        } else if (hAlign == QDeclarativeText::AlignHCenter) {
            x = (size.width() - (int)line.naturalTextWidth()) / 2;
        }
        line.setPosition(QPoint(x, (int)line.y()));
    }

    //paint text
    QPixmap img(size);
    if (!size.isEmpty()) {
        img.fill(Qt::transparent);
        QPainter p(&img);
        drawWrappedText(&p, QPointF(0,0), drawStyle);
    }
    return img;
}

void QDeclarativeTextPrivate::drawWrappedText(QPainter *p, const QPointF &pos, bool drawStyle)
{
    if (drawStyle)
        p->setPen(styleColor);
    else
        p->setPen(color);
    p->setFont(font);
    layout.draw(p, pos);
}

QPixmap QDeclarativeTextPrivate::richTextImage(bool drawStyle)
{
    QSize size = doc->size().toSize();

    //paint text
    QPixmap img(size);
    img.fill(Qt::transparent);
    QPainter p(&img);

    QAbstractTextDocumentLayout::PaintContext context;

    if (drawStyle) {
        context.palette.setColor(QPalette::Text, styleColor);
        // ### Do we really want this?
        QTextOption colorOption;
        colorOption.setFlags(QTextOption::SuppressColors);
        doc->setDefaultTextOption(colorOption);
    } else {
        context.palette.setColor(QPalette::Text, color);
    }
    doc->documentLayout()->draw(&p, context);
    if (drawStyle)
        doc->setDefaultTextOption(QTextOption());
    return img;
}

void QDeclarativeTextPrivate::checkImgCache()
{
    if (!imgDirty)
        return;

    bool empty = text.isEmpty();
    if (empty) {
        imgCache = QPixmap();
        imgStyleCache = QPixmap();
    } else if (richText) {
        imgCache = richTextImage(false);
        if (style != QDeclarativeText::Normal)
            imgStyleCache = richTextImage(true); //### should use styleColor
    } else {
        imgCache = wrappedTextImage(false);
        if (style != QDeclarativeText::Normal)
            imgStyleCache = wrappedTextImage(true); //### should use styleColor
    }
    if (!empty)
        switch (style) {
        case QDeclarativeText::Outline:
            drawOutline();
            break;
        case QDeclarativeText::Sunken:
            drawOutline(-1);
            break;
        case QDeclarativeText::Raised:
            drawOutline(1);
            break;
        default:
            break;
        }

    imgDirty = false;
}

void QDeclarativeTextPrivate::ensureDoc()
{
    if (!doc) {
        Q_Q(QDeclarativeText);
        doc = new QTextDocumentWithImageResources(q);
        doc->setDocumentMargin(0);
    }
}

void QDeclarativeText::reloadWithResources()
{
    Q_D(QDeclarativeText);
    if (!d->richText)
        return;
    d->doc->setHtml(d->text);
    d->updateLayout();
    d->markImgDirty();
}

/*!
    Returns the number of resources (images) that are being loaded asynchronously.
*/
int QDeclarativeText::resourcesLoading() const
{
    Q_D(const QDeclarativeText);
    return d->doc ? d->doc->resourcesLoading() : 0;
}

void QDeclarativeText::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(QDeclarativeText);

    if (d->cache || d->style != Normal) {
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

        bool needClip = clip() && (d->imgCache.width() > width() ||
                                   d->imgCache.height() > height());

        if (needClip) {
            p->save();
            p->setClipRect(boundingRect(), Qt::IntersectClip);
        }
        p->drawPixmap(x, y, d->imgCache);
        if (needClip)
            p->restore();

        if (d->smooth) {
            p->setRenderHint(QPainter::Antialiasing, oldAA);
            p->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
        }
    } else {
        int h = height();
        int y = 0;

        switch (d->vAlign) {
        case AlignTop:
            y = 0;
            break;
        case AlignBottom:
            y = h - d->cachedLayoutSize.height();
            break;
        case AlignVCenter:
            y = (h - d->cachedLayoutSize.height()) / 2;
            break;
        }
        bool needClip = !clip() && (d->cachedLayoutSize.width() > width() ||
                                    d->cachedLayoutSize.height() > height());

        if (needClip) {
            p->save();
            p->setClipRect(boundingRect(), Qt::IntersectClip);
        }
        if (d->richText) {
            QAbstractTextDocumentLayout::PaintContext context;
            context.palette.setColor(QPalette::Text, d->color);
            p->translate(0, y);
            d->doc->documentLayout()->draw(p, context);
            p->translate(0, -y);
        } else {
            d->drawWrappedText(p, QPointF(0,y), false);
        }
        if (needClip)
            p->restore();
    }
}

/*!
    \qmlproperty bool Text::smooth

    This property holds whether the text is smoothly scaled or transformed.

    Smooth filtering gives better visual quality, but is slower.  If
    the item is displayed at its natural size, this property has no visual or
    performance effect.

    \note Generally scaling artifacts are only visible if the item is stationary on
    the screen.  A common pattern when animating an item is to disable smooth
    filtering at the beginning of the animation and reenable it at the conclusion.
*/

void QDeclarativeText::componentComplete()
{
    Q_D(QDeclarativeText);
    QDeclarativeItem::componentComplete();
    if (d->dirty) {
        if (d->richText) {
            d->ensureDoc();
            d->doc->setHtml(d->text);
        }
        d->updateLayout();
        d->dirty = false;
    }
}

/*!
  \overload
  Handles the given mouse \a event.
 */
void QDeclarativeText::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeText);

    if (!d->richText || !d->doc || d->doc->documentLayout()->anchorAt(event->pos()).isEmpty()) {
        event->setAccepted(false);
        d->activeLink = QString();
    } else {
        d->activeLink = d->doc->documentLayout()->anchorAt(event->pos());
    }

    // ### may malfunction if two of the same links are clicked & dragged onto each other)

    if (!event->isAccepted())
        QDeclarativeItem::mousePressEvent(event);

}

/*!
    \qmlsignal Text::linkActivated(link)

    This handler is called when the user clicks on a link embedded in the text.
*/

/*!
  \overload
  Handles the given mouse \a event.
 */
void QDeclarativeText::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeText);

        // ### confirm the link, and send a signal out
    if (d->richText && d->doc && d->activeLink == d->doc->documentLayout()->anchorAt(event->pos()))
        emit linkActivated(d->activeLink);
    else
        event->setAccepted(false);

    if (!event->isAccepted())
        QDeclarativeItem::mouseReleaseEvent(event);
}

QT_END_NAMESPACE

#include "qdeclarativetext.moc"
