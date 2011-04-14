// Commit: a5c3c11e3e2204da6c8be9af98b38929366fafb8
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsgtext_p.h"
#include "qsgtext_p_p.h"

#include <private/qsgdistancefieldglyphcache_p.h>
#include <private/qsgcontext_p.h>
#include <private/qsgadaptationlayer_p.h>
#include "qsgtextnode_p.h"
#include "qsgimage_p_p.h"
#include <private/qsgtexture_p.h>

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qabstracttextdocumentlayout.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextdocument.h>
#include <QtGui/qtextobject.h>
#include <QtGui/qtextcursor.h>
#include <QtGui/qapplication.h>

#include <private/qdeclarativestyledtext_p.h>
#include <private/qdeclarativepixmapcache_p.h>

#include <qmath.h>
#include <limits.h>

QT_BEGIN_NAMESPACE

extern Q_GUI_EXPORT bool qt_applefontsmoothing_enabled;

class QSGTextDocumentWithImageResources : public QTextDocument {
    Q_OBJECT

public:
    QSGTextDocumentWithImageResources(QSGText *parent);
    virtual ~QSGTextDocumentWithImageResources();

    void setText(const QString &);
    int resourcesLoading() const { return outstanding; }

protected:
    QVariant loadResource(int type, const QUrl &name);

private slots:
    void requestFinished();

private:
    QHash<QUrl, QDeclarativePixmap *> m_resources;

    int outstanding;
    static QSet<QUrl> errors;
};

DEFINE_BOOL_CONFIG_OPTION(enableImageCache, QML_ENABLE_TEXT_IMAGE_CACHE);

QString QSGTextPrivate::elideChar = QString(0x2026);

QSGTextPrivate::QSGTextPrivate()
: color((QRgb)0), style(QSGText::Normal), hAlign(QSGText::AlignLeft),
  vAlign(QSGText::AlignTop), elideMode(QSGText::ElideNone),
  format(QSGText::AutoText), wrapMode(QSGText::NoWrap), lineHeight(1),
  lineHeightMode(QSGText::ProportionalHeight), lineCount(1), maximumLineCount(INT_MAX),
  maximumLineCountValid(false),
  texture(0),
  imageCacheDirty(true), updateOnComponentComplete(true),
  richText(false), singleline(false), cacheAllTextAsImage(true), internalWidthUpdate(false),
  requireImplicitWidth(false), truncated(false), hAlignImplicit(true), rightToLeftText(false),
  naturalWidth(0), doc(0), nodeType(NodeIsNull)
{
    cacheAllTextAsImage = enableImageCache();
}

void QSGTextPrivate::init()
{
    Q_Q(QSGText);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFlag(QSGItem::ItemHasContents);
}

QSGTextDocumentWithImageResources::QSGTextDocumentWithImageResources(QSGText *parent)
: QTextDocument(parent), outstanding(0)
{
    setUndoRedoEnabled(false);
}

QSGTextDocumentWithImageResources::~QSGTextDocumentWithImageResources()
{
    if (!m_resources.isEmpty())
        qDeleteAll(m_resources);
}

QVariant QSGTextDocumentWithImageResources::loadResource(int type, const QUrl &name)
{
    QDeclarativeContext *context = qmlContext(parent());
    QUrl url = context->resolvedUrl(name);

    if (type == QTextDocument::ImageResource) {
        QHash<QUrl, QDeclarativePixmap *>::Iterator iter = m_resources.find(url);

        if (iter == m_resources.end()) {
            QDeclarativePixmap *p = new QDeclarativePixmap(context->engine(), url);
            iter = m_resources.insert(name, p);

            if (p->isLoading()) {
                p->connectFinished(this, SLOT(requestFinished()));
                outstanding++;
            }
        }

        QDeclarativePixmap *p = *iter;
        if (p->isReady()) {
            return p->pixmap();
        } else if (p->isError()) {
            if (!errors.contains(url)) {
                errors.insert(url);
                qmlInfo(parent()) << p->error();
            }
        }
    }

    return QTextDocument::loadResource(type,url); // The *resolved* URL
}

void QSGTextDocumentWithImageResources::requestFinished()
{
    outstanding--;
    if (outstanding == 0) {
        QSGText *textItem = static_cast<QSGText*>(parent());
        QString text = textItem->text();
#ifndef QT_NO_TEXTHTMLPARSER
        setHtml(text);
#else
        setPlainText(text);
#endif
        QSGTextPrivate *d = QSGTextPrivate::get(textItem);
        d->updateLayout();
    }
}

void QSGTextDocumentWithImageResources::setText(const QString &text)
{
    if (!m_resources.isEmpty()) {
        qDeleteAll(m_resources);
        m_resources.clear();
        outstanding = 0;
    }

#ifndef QT_NO_TEXTHTMLPARSER
    setHtml(text);
#else
    setPlainText(text);
#endif
}

QSet<QUrl> QSGTextDocumentWithImageResources::errors;

QSGTextPrivate::~QSGTextPrivate()
{
}

qreal QSGTextPrivate::getImplicitWidth() const
{
    if (!requireImplicitWidth) {
        // We don't calculate implicitWidth unless it is required.
        // We need to force a size update now to ensure implicitWidth is calculated
        QSGTextPrivate *me = const_cast<QSGTextPrivate*>(this);
        me->requireImplicitWidth = true;
        me->updateSize();
    }
    return implicitWidth;
}

void QSGTextPrivate::updateLayout()
{
    Q_Q(QSGText);
    if (!q->isComponentComplete()) {
        updateOnComponentComplete = true;
        return;
    }

    // Setup instance of QTextLayout for all cases other than richtext
    if (!richText) {
        layout.clearLayout();
        layout.setFont(font);
        if (format != QSGText::StyledText) {
            QString tmp = text;
            tmp.replace(QLatin1Char('\n'), QChar::LineSeparator);
            singleline = !tmp.contains(QChar::LineSeparator);
            if (singleline && !maximumLineCountValid && elideMode != QSGText::ElideNone && q->widthValid()) {
                QFontMetrics fm(font);
                tmp = fm.elidedText(tmp,(Qt::TextElideMode)elideMode,q->width()); // XXX still worth layout...?
                if (tmp != text && !truncated) {
                    truncated = true;
                    emit q->truncatedChanged();
                }
            }
            layout.setText(tmp);
        } else {
            singleline = false;
            QDeclarativeStyledText::parse(text, layout);
        }
    } else {
        ensureDoc();
        QTextBlockFormat::LineHeightTypes type;
        type = lineHeightMode == QSGText::FixedHeight ? QTextBlockFormat::FixedHeight : QTextBlockFormat::ProportionalHeight;
        QTextBlockFormat blockFormat;
        blockFormat.setLineHeight((lineHeightMode == QSGText::FixedHeight ? lineHeight : lineHeight * 100), type);
        for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next()) {
            QTextCursor cursor(it);
            cursor.setBlockFormat(blockFormat);
        }
    }

    updateSize();
}

void QSGTextPrivate::updateSize()
{
    Q_Q(QSGText);

    if (!q->isComponentComplete()) {
        updateOnComponentComplete = true;
        return;
    }

    if (!requireImplicitWidth) {
        emit q->implicitWidthChanged();
        // if the implicitWidth is used, then updateSize() has already been called (recursively)
        if (requireImplicitWidth)
            return;
    }

    invalidateImageCache();

    QFontMetrics fm(font);
    if (text.isEmpty()) {
        q->setImplicitWidth(0);
        q->setImplicitHeight(fm.height());
        paintedSize = QSize(0, fm.height());
        emit q->paintedSizeChanged();
        q->update();
        return;
    }

    int dy = q->height();
    QSize size(0, 0);

    //setup instance of QTextLayout for all cases other than richtext
    if (!richText) {
        QRect textRect = setupTextLayout();
        layedOutTextRect = textRect;
        size = textRect.size();
        dy -= size.height();
    } else {
        singleline = false; // richtext can't elide or be optimized for single-line case
        ensureDoc();
        doc->setDefaultFont(font);
        QSGText::HAlignment horizontalAlignment = q->effectiveHAlign();
        if (rightToLeftText) {
            if (horizontalAlignment == QSGText::AlignLeft)
                horizontalAlignment = QSGText::AlignRight;
            else if (horizontalAlignment == QSGText::AlignRight)
                horizontalAlignment = QSGText::AlignLeft;
        }
        QTextOption option;
        option.setAlignment((Qt::Alignment)int(horizontalAlignment | vAlign));
        option.setWrapMode(QTextOption::WrapMode(wrapMode));
        doc->setDefaultTextOption(option);
        if (requireImplicitWidth && q->widthValid()) {
            doc->setTextWidth(-1);
            naturalWidth = doc->idealWidth();
        }
        if (wrapMode != QSGText::NoWrap && q->widthValid())
            doc->setTextWidth(q->width());
        else
            doc->setTextWidth(doc->idealWidth()); // ### Text does not align if width is not set (QTextDoc bug)
        dy -= (int)doc->size().height();
        QSize dsize = doc->size().toSize();
        layedOutTextRect = QRect(QPoint(0,0), dsize);
        size = QSize(int(doc->idealWidth()),dsize.height());
    }
    int yoff = 0;

    if (q->heightValid()) {
        if (vAlign == QSGText::AlignBottom)
            yoff = dy;
        else if (vAlign == QSGText::AlignVCenter)
            yoff = dy/2;
    }
    q->setBaselineOffset(fm.ascent() + yoff);

    //### need to comfirm cost of always setting these for richText
    internalWidthUpdate = true;
    if (!q->widthValid())
        q->setImplicitWidth(size.width());
    else if (requireImplicitWidth)
        q->setImplicitWidth(naturalWidth);
    internalWidthUpdate = false;

    q->setImplicitHeight(size.height());
    if (paintedSize != size) {
        paintedSize = size;
        emit q->paintedSizeChanged();
    }
    q->update();
}

/*!
    Lays out the QSGTextPrivate::layout QTextLayout in the constraints of the QSGText.

    Returns the size of the final text.  This can be used to position the text vertically (the text is
    already absolutely positioned horizontally).
*/
QRect QSGTextPrivate::setupTextLayout()
{
    // ### text layout handling should be profiled and optimized as needed
    // what about QStackTextEngine engine(tmp, d->font.font()); QTextLayout textLayout(&engine);
    Q_Q(QSGText);
    layout.setCacheEnabled(true);

    qreal lineWidth = 0;
    int visibleCount = 0;

    //set manual width
    if (q->widthValid())
        lineWidth = q->width();

    QTextOption textOption = layout.textOption();
    textOption.setAlignment(Qt::Alignment(q->effectiveHAlign()));
    textOption.setWrapMode(QTextOption::WrapMode(wrapMode));
    layout.setTextOption(textOption);

    bool elideText = false;
    bool truncate = false;

    QFontMetrics fm(layout.font());
    elidePos = QPointF();

    if (requireImplicitWidth && q->widthValid()) {
        // requires an extra layout
        layout.beginLayout();
        forever {
            QTextLine line = layout.createLine();
            if (!line.isValid())
                break;
        }
        layout.endLayout();
        QRectF br;
        for (int i = 0; i < layout.lineCount(); ++i) {
            QTextLine line = layout.lineAt(i);
            br = br.united(line.naturalTextRect());
        }
        naturalWidth = br.width();
    }

    if (maximumLineCountValid) {
        layout.beginLayout();
        if (!lineWidth)
            lineWidth = INT_MAX;
        int linesLeft = maximumLineCount;
        int visibleTextLength = 0;
        while (linesLeft > 0) {
            QTextLine line = layout.createLine();
            if (!line.isValid())
                break;

            visibleCount++;
            if (lineWidth)
                line.setLineWidth(lineWidth);
            visibleTextLength += line.textLength();

            if (--linesLeft == 0) {
                if (visibleTextLength < text.length()) {
                    truncate = true;
                    if (elideMode==QSGText::ElideRight && q->widthValid()) {
                        qreal elideWidth = fm.width(elideChar);
                        // Need to correct for alignment
                        line.setLineWidth(lineWidth-elideWidth);
                        if (layout.text().mid(line.textStart(), line.textLength()).isRightToLeft()) {
                            line.setPosition(QPointF(line.position().x() + elideWidth, line.position().y()));
                            elidePos.setX(line.naturalTextRect().left() - elideWidth);
                        } else {
                            elidePos.setX(line.naturalTextRect().right());
                        }
                        elideText = true;
                    }
                }
            }
        }
        layout.endLayout();

        //Update truncated
        if (truncated != truncate) {
            truncated = truncate;
            emit q->truncatedChanged();
        }
    } else {
        layout.beginLayout();
        forever {
            QTextLine line = layout.createLine();
            if (!line.isValid())
                break;
            visibleCount++;
            if (lineWidth)
                line.setLineWidth(lineWidth);
        }
        layout.endLayout();
    }

    qreal height = 0;
    QRectF br;
    for (int i = 0; i < layout.lineCount(); ++i) {
        QTextLine line = layout.lineAt(i);
        // set line spacing
        line.setPosition(QPointF(line.position().x(), height));
        if (elideText && i == layout.lineCount()-1) {
            elidePos.setY(height + fm.ascent());
            br = br.united(QRectF(elidePos, QSizeF(fm.width(elideChar), fm.ascent())));
        }
        br = br.united(line.naturalTextRect());
        height += (lineHeightMode == QSGText::FixedHeight) ? lineHeight : line.height() * lineHeight;
    }
    br.setHeight(height);

    if (!q->widthValid())
        naturalWidth = br.width();

    //Update the number of visible lines
    if (lineCount != visibleCount) {
        lineCount = visibleCount;
        emit q->lineCountChanged();
    }

    return QRect(qRound(br.x()), qRound(br.y()), qCeil(br.width()), qCeil(br.height()));
}

/*!
    Returns a painted version of the QSGTextPrivate::layout QTextLayout.
    If \a drawStyle is true, the style color overrides all colors in the document.
*/
QPixmap QSGTextPrivate::textLayoutImage(bool drawStyle)
{
    QSize size = layedOutTextRect.size();

    //paint text
    QPixmap img(size);
    if (!size.isEmpty()) {
        img.fill(Qt::transparent);
#ifdef Q_WS_MAC
        bool oldSmooth = qt_applefontsmoothing_enabled;
        qt_applefontsmoothing_enabled = false;
#endif
        QPainter p(&img);
#ifdef Q_WS_MAC
        qt_applefontsmoothing_enabled = oldSmooth;
#endif
        drawTextLayout(&p, QPointF(-layedOutTextRect.x(),0), drawStyle);
    }
    return img;
}

/*!
    Paints the QSGTextPrivate::layout QTextLayout into \a painter at \a pos.  If
    \a drawStyle is true, the style color overrides all colors in the document.
*/
void QSGTextPrivate::drawTextLayout(QPainter *painter, const QPointF &pos, bool drawStyle)
{
    if (drawStyle)
        painter->setPen(styleColor);
    else
        painter->setPen(color);
    painter->setFont(font);
    layout.draw(painter, pos);
    if (!elidePos.isNull())
        painter->drawText(pos + elidePos, elideChar);
}

/*!
    Returns a painted version of the QSGTextPrivate::doc QTextDocument.
    If \a drawStyle is true, the style color overrides all colors in the document.
*/
QPixmap QSGTextPrivate::textDocumentImage(bool drawStyle)
{
    QSize size = doc->size().toSize();

    //paint text
    QPixmap img(size);
    img.fill(Qt::transparent);
#ifdef Q_WS_MAC
    bool oldSmooth = qt_applefontsmoothing_enabled;
    qt_applefontsmoothing_enabled = false;
#endif
    QPainter p(&img);
#ifdef Q_WS_MAC
    qt_applefontsmoothing_enabled = oldSmooth;
#endif

    QAbstractTextDocumentLayout::PaintContext context;

    QTextOption oldOption(doc->defaultTextOption());
    if (drawStyle) {
        context.palette.setColor(QPalette::Text, styleColor);
        QTextOption colorOption(doc->defaultTextOption());
        colorOption.setFlags(QTextOption::SuppressColors);
        doc->setDefaultTextOption(colorOption);
    } else {
        context.palette.setColor(QPalette::Text, color);
    }
    doc->documentLayout()->draw(&p, context);
    if (drawStyle)
        doc->setDefaultTextOption(oldOption);
    return img;
}

/*!
    Mark the image cache as dirty.
*/
void QSGTextPrivate::invalidateImageCache()
{
    Q_Q(QSGText);

    if(cacheAllTextAsImage || (!QSGDistanceFieldGlyphCache::distanceFieldEnabled() && style != QSGText::Normal)){//If actually using the image cache
        if (imageCacheDirty)
            return;

        imageCacheDirty = true;
        imageCache = QPixmap();
    }
    if (q->isComponentComplete())
        q->update();
}

/*!
    Tests if the image cache is dirty, and repaints it if it is.
*/
void QSGTextPrivate::checkImageCache()
{
    if (!imageCacheDirty)
        return;

    if (text.isEmpty()) {

        imageCache = QPixmap();

    } else {

        QPixmap textImage;
        QPixmap styledImage;

        if (richText) {
            textImage = textDocumentImage(false);
            if (style != QSGText::Normal)
                styledImage = textDocumentImage(true); //### should use styleColor
        } else {
            textImage = textLayoutImage(false);
            if (style != QSGText::Normal)
                styledImage = textLayoutImage(true); //### should use styleColor
        }

        switch (style) {
        case QSGText::Outline:
            imageCache = drawOutline(textImage, styledImage);
            break;
        case QSGText::Sunken:
            imageCache = drawOutline(textImage, styledImage, -1);
            break;
        case QSGText::Raised:
            imageCache = drawOutline(textImage, styledImage, 1);
            break;
        default:
            imageCache = textImage;
            break;
        }

    }

    imageCacheDirty = false;
}

/*!
    Ensures the QSGTextPrivate::doc variable is set to a valid text document
*/
void QSGTextPrivate::ensureDoc()
{
    if (!doc) {
        Q_Q(QSGText);
        doc = new QSGTextDocumentWithImageResources(q);
        doc->setDocumentMargin(0);
    }
}

/*!
    Draw \a styleSource as an outline around \a source and return the new image.
*/
QPixmap QSGTextPrivate::drawOutline(const QPixmap &source, const QPixmap &styleSource)
{
    QPixmap img = QPixmap(styleSource.width() + 2, styleSource.height() + 2);
    img.fill(Qt::transparent);

    QPainter ppm(&img);

    QPoint pos(0, 0);
    pos += QPoint(-1, 0);
    ppm.drawPixmap(pos, styleSource);
    pos += QPoint(2, 0);
    ppm.drawPixmap(pos, styleSource);
    pos += QPoint(-1, -1);
    ppm.drawPixmap(pos, styleSource);
    pos += QPoint(0, 2);
    ppm.drawPixmap(pos, styleSource);

    pos += QPoint(0, -1);
    ppm.drawPixmap(pos, source);
    ppm.end();

    return img;
}

/*!
    Draw \a styleSource below \a source at \a yOffset and return the new image.
*/
QPixmap QSGTextPrivate::drawOutline(const QPixmap &source, const QPixmap &styleSource, int yOffset)
{
    QPixmap img = QPixmap(styleSource.width() + 2, styleSource.height() + 2);
    img.fill(Qt::transparent);

    QPainter ppm(&img);

    ppm.drawPixmap(QPoint(0, yOffset), styleSource);
    ppm.drawPixmap(0, 0, source);

    ppm.end();

    return img;
}

QSGText::QSGText(QSGItem *parent)
: QSGImplicitSizeItem(*(new QSGTextPrivate), parent)
{
    Q_D(QSGText);
    d->init();
}

QSGText::~QSGText()
{
}

QFont QSGText::font() const
{
    Q_D(const QSGText);
    return d->sourceFont;
}

void QSGText::setFont(const QFont &font)
{
    Q_D(QSGText);
    if (d->sourceFont == font)
        return;

    d->sourceFont = font;
    QFont oldFont = d->font;
    d->font = font;
    if (QSGDistanceFieldGlyphCache::distanceFieldEnabled())
        d->font.setHintingPreference(QFont::PreferNoHinting);

    if (d->font.pointSizeF() != -1) {
        // 0.5pt resolution
        qreal size = qRound(d->font.pointSizeF()*2.0);
        d->font.setPointSizeF(size/2.0);
    }

    if (oldFont != d->font)
        d->updateLayout();

    emit fontChanged(d->sourceFont);
}

QString QSGText::text() const
{
    Q_D(const QSGText);
    return d->text;
}

void QSGText::setText(const QString &n)
{
    Q_D(QSGText);
    if (d->text == n)
        return;

    d->richText = d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(n));
    d->text = n;
    if (isComponentComplete()) {
        if (d->richText) {
            d->ensureDoc();
            d->doc->setText(n);
            d->rightToLeftText = d->doc->toPlainText().isRightToLeft();
        } else {
            d->rightToLeftText = d->text.isRightToLeft();
        }
        d->determineHorizontalAlignment();
    }
    d->updateLayout();
    emit textChanged(d->text);
}

QColor QSGText::color() const
{
    Q_D(const QSGText);
    return d->color;
}

void QSGText::setColor(const QColor &color)
{
    Q_D(QSGText);
    if (d->color == color)
        return;

    d->color = color;
    d->invalidateImageCache();
    emit colorChanged(d->color);
}

QSGText::TextStyle QSGText::style() const
{
    Q_D(const QSGText);
    return d->style;
}

void QSGText::setStyle(QSGText::TextStyle style)
{
    Q_D(QSGText);
    if (d->style == style)
        return;

    // changing to/from Normal requires the boundingRect() to change
    if (isComponentComplete() && (d->style == Normal || style == Normal))
        update();
    d->style = style;
    d->invalidateImageCache();
    emit styleChanged(d->style);
}

QColor QSGText::styleColor() const
{
    Q_D(const QSGText);
    return d->styleColor;
}

void QSGText::setStyleColor(const QColor &color)
{
    Q_D(QSGText);
    if (d->styleColor == color)
        return;

    d->styleColor = color;
    d->invalidateImageCache();
    emit styleColorChanged(d->styleColor);
}

QSGText::HAlignment QSGText::hAlign() const
{
    Q_D(const QSGText);
    return d->hAlign;
}

void QSGText::setHAlign(HAlignment align)
{
    Q_D(QSGText);
    bool forceAlign = d->hAlignImplicit && d->effectiveLayoutMirror;
    d->hAlignImplicit = false;
    if (d->setHAlign(align, forceAlign) && isComponentComplete())
        d->updateLayout();
}

void QSGText::resetHAlign()
{
    Q_D(QSGText);
    d->hAlignImplicit = true;
    if (d->determineHorizontalAlignment() && isComponentComplete())
        d->updateLayout();
}

QSGText::HAlignment QSGText::effectiveHAlign() const
{
    Q_D(const QSGText);
    QSGText::HAlignment effectiveAlignment = d->hAlign;
    if (!d->hAlignImplicit && d->effectiveLayoutMirror) {
        switch (d->hAlign) {
        case QSGText::AlignLeft:
            effectiveAlignment = QSGText::AlignRight;
            break;
        case QSGText::AlignRight:
            effectiveAlignment = QSGText::AlignLeft;
            break;
        default:
            break;
        }
    }
    return effectiveAlignment;
}

bool QSGTextPrivate::setHAlign(QSGText::HAlignment alignment, bool forceAlign)
{
    Q_Q(QSGText);
    if (hAlign != alignment || forceAlign) {
        QSGText::HAlignment oldEffectiveHAlign = q->effectiveHAlign();
        hAlign = alignment;

        emit q->horizontalAlignmentChanged(hAlign);
        if (oldEffectiveHAlign != q->effectiveHAlign())
            emit q->effectiveHorizontalAlignmentChanged();
        return true;
    }
    return false;
}

bool QSGTextPrivate::determineHorizontalAlignment()
{
    Q_Q(QSGText);
    if (hAlignImplicit && q->isComponentComplete()) {
        bool alignToRight = text.isEmpty() ? QApplication::keyboardInputDirection() == Qt::RightToLeft : rightToLeftText;
        return setHAlign(alignToRight ? QSGText::AlignRight : QSGText::AlignLeft);
    }
    return false;
}

void QSGTextPrivate::mirrorChange()
{
    Q_Q(QSGText);
    if (q->isComponentComplete()) {
        if (!hAlignImplicit && (hAlign == QSGText::AlignRight || hAlign == QSGText::AlignLeft)) {
            updateLayout();
            emit q->effectiveHorizontalAlignmentChanged();
        }
    }
}

QTextDocument *QSGTextPrivate::textDocument()
{
    return doc;
}

QSGText::VAlignment QSGText::vAlign() const
{
    Q_D(const QSGText);
    return d->vAlign;
}

void QSGText::setVAlign(VAlignment align)
{
    Q_D(QSGText);
    if (d->vAlign == align)
        return;

    d->vAlign = align;
    emit verticalAlignmentChanged(align);
}

QSGText::WrapMode QSGText::wrapMode() const
{
    Q_D(const QSGText);
    return d->wrapMode;
}

void QSGText::setWrapMode(WrapMode mode)
{
    Q_D(QSGText);
    if (mode == d->wrapMode)
        return;

    d->wrapMode = mode;
    d->updateLayout();

    emit wrapModeChanged();
}

int QSGText::lineCount() const
{
    Q_D(const QSGText);
    return d->lineCount;
}

bool QSGText::truncated() const
{
    Q_D(const QSGText);
    return d->truncated;
}

int QSGText::maximumLineCount() const
{
    Q_D(const QSGText);
    return d->maximumLineCount;
}

void QSGText::setMaximumLineCount(int lines)
{
    Q_D(QSGText);

    d->maximumLineCountValid = lines==INT_MAX ? false : true;
    if (d->maximumLineCount != lines) {
        d->maximumLineCount = lines;
        d->updateLayout();
        emit maximumLineCountChanged();
    }
}

void QSGText::resetMaximumLineCount()
{
    Q_D(QSGText);
    setMaximumLineCount(INT_MAX);
    d->elidePos = QPointF();
    if (d->truncated != false) {
        d->truncated = false;
        emit truncatedChanged();
    }
}

QSGText::TextFormat QSGText::textFormat() const
{
    Q_D(const QSGText);
    return d->format;
}

void QSGText::setTextFormat(TextFormat format)
{
    Q_D(QSGText);
    if (format == d->format)
        return;
    d->format = format;
    bool wasRich = d->richText;
    d->richText = format == RichText || (format == AutoText && Qt::mightBeRichText(d->text));

    if (!wasRich && d->richText && isComponentComplete()) {
        d->ensureDoc();
        d->doc->setText(d->text);
    }

    d->updateLayout();

    emit textFormatChanged(d->format);
}

QSGText::TextElideMode QSGText::elideMode() const
{
    Q_D(const QSGText);
    return d->elideMode;
}

void QSGText::setElideMode(QSGText::TextElideMode mode)
{
    Q_D(QSGText);
    if (mode == d->elideMode)
        return;

    d->elideMode = mode;
    d->updateLayout();

    emit elideModeChanged(d->elideMode);
}

/*! \internal */
QRectF QSGText::boundingRect() const
{
    Q_D(const QSGText);

    QRect rect = d->layedOutTextRect;
    if (d->style != Normal)
        rect.adjust(-1, 0, 1, 2);

    // Could include font max left/right bearings to either side of rectangle.

    int h = height();
    switch (d->vAlign) {
    case AlignTop:
        break;
    case AlignBottom:
        rect.moveTop(h - rect.height());
        break;
    case AlignVCenter:
        rect.moveTop((h - rect.height()) / 2);
        break;
    }

    return QRectF(rect);
}

/*! \internal */
void QSGText::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_D(QSGText);
    if ((!d->internalWidthUpdate && newGeometry.width() != oldGeometry.width())
            && (d->wrapMode != QSGText::NoWrap
                || d->elideMode != QSGText::ElideNone
                || d->hAlign != QSGText::AlignLeft)) {
        if ((d->singleline || d->maximumLineCountValid) && d->elideMode != QSGText::ElideNone && widthValid()) {
            // We need to re-elide
            d->updateLayout();
        } else {
            // We just need to re-layout
            d->updateSize();
        }
    }

    QSGItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode *QSGText::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    Q_D(QSGText);

    bool richTextAsImage = false;
    if (d->richText) {
        d->ensureDoc();
        richTextAsImage = QSGTextNode::isComplexRichText(d->doc);
    }

    QRectF bounds = boundingRect();

    // XXX todo - some styled text can be done by the QSGTextNode
    if (richTextAsImage || d->cacheAllTextAsImage || (!QSGDistanceFieldGlyphCache::distanceFieldEnabled() && d->style != Normal)) {
        bool wasDirty = d->imageCacheDirty;

        d->checkImageCache();

        if (d->imageCache.isNull()) {
            delete oldNode;
            return 0;
        }

        QSGImageNode *node = 0;
        if (!oldNode || d->nodeType != QSGTextPrivate::NodeIsTexture) {
            delete oldNode;
            node = QSGItemPrivate::get(this)->sceneGraphContext()->createImageNode();
            d->texture = new QSGPlainTexture();
            wasDirty = true;
            d->nodeType = QSGTextPrivate::NodeIsTexture;
        } else {
            node = static_cast<QSGImageNode *>(oldNode);
            Q_ASSERT(d->texture);
        }

        if (wasDirty) {
            qobject_cast<QSGPlainTexture *>(d->texture)->setImage(d->imageCache.toImage());
            node->setTexture(0);
            node->setTexture(d->texture);
        }

        node->setTargetRect(QRectF(bounds.x(), bounds.y(), d->imageCache.width(), d->imageCache.height()));
        node->setSourceRect(QRectF(0, 0, 1, 1));
        node->setHorizontalWrapMode(QSGTexture::ClampToEdge);
        node->setVerticalWrapMode(QSGTexture::ClampToEdge);
        node->setFiltering(QSGTexture::Linear); // Nonsmooth text just ugly, so don't do that..
        node->update();

        return node;

    } else {
        QSGTextNode *node = 0;
        if (!oldNode || d->nodeType != QSGTextPrivate::NodeIsText) {
            delete oldNode;
            node = new QSGTextNode(QSGItemPrivate::get(this)->sceneGraphContext());
            d->nodeType = QSGTextPrivate::NodeIsText;
        } else {
            node = static_cast<QSGTextNode *>(oldNode);
        }

        node->deleteContent();
        node->setMatrix(QMatrix4x4());

        if (d->richText) {

            d->ensureDoc();
            node->addTextDocument(bounds.topLeft(), d->doc, QColor(), d->style, d->styleColor);

        } else {
            node->addTextLayout(QPoint(0, bounds.y()), &d->layout, d->color, d->style, d->styleColor);
            QMatrix4x4 m;
            m.translate(0, QFontMetricsF(d->font).ascent());
            node->setMatrix(m);
        }

        return node;
    }
}

qreal QSGText::paintedWidth() const
{
    Q_D(const QSGText);
    return d->paintedSize.width();
}

qreal QSGText::paintedHeight() const
{
    Q_D(const QSGText);
    return d->paintedSize.height();
}

qreal QSGText::lineHeight() const
{
    Q_D(const QSGText);
    return d->lineHeight;
}

void QSGText::setLineHeight(qreal lineHeight)
{
    Q_D(QSGText);

    if ((d->lineHeight == lineHeight) || (lineHeight < 0.0))
        return;

    d->lineHeight = lineHeight;
    d->updateLayout();
    emit lineHeightChanged(lineHeight);
}

QSGText::LineHeightMode QSGText::lineHeightMode() const
{
    Q_D(const QSGText);
    return d->lineHeightMode;
}

void QSGText::setLineHeightMode(LineHeightMode mode)
{
    Q_D(QSGText);
    if (mode == d->lineHeightMode)
        return;

    d->lineHeightMode = mode;
    d->updateLayout();

    emit lineHeightModeChanged(mode);
}

/*!
    Returns the number of resources (images) that are being loaded asynchronously.
*/
int QSGText::resourcesLoading() const
{
    Q_D(const QSGText);
    return d->doc ? d->doc->resourcesLoading() : 0;
}

/*! \internal */
void QSGText::componentComplete()
{
    Q_D(QSGText);
    QSGItem::componentComplete();
    if (d->updateOnComponentComplete) {
        d->updateOnComponentComplete = false;
        if (d->richText) {
            d->ensureDoc();
            d->doc->setText(d->text);
            d->rightToLeftText = d->doc->toPlainText().isRightToLeft();
        } else {
            d->rightToLeftText = d->text.isRightToLeft();
        }
        d->determineHorizontalAlignment();
        d->updateLayout();
    }
}

/*!  \internal */
void QSGText::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGText);

    if (!d->richText || !d->doc || d->doc->documentLayout()->anchorAt(event->pos()).isEmpty()) {
        event->setAccepted(false);
        d->activeLink.clear();
    } else {
        d->activeLink = d->doc->documentLayout()->anchorAt(event->pos());
    }

    // ### may malfunction if two of the same links are clicked & dragged onto each other)

    if (!event->isAccepted())
        QSGItem::mousePressEvent(event);

}

/*! \internal */
void QSGText::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGText);

        // ### confirm the link, and send a signal out
    if (d->richText && d->doc && d->activeLink == d->doc->documentLayout()->anchorAt(event->pos()))
        emit linkActivated(d->activeLink);
    else
        event->setAccepted(false);

    if (!event->isAccepted())
        QSGItem::mouseReleaseEvent(event);
}

QT_END_NAMESPACE

#include "qsgtext.moc"
