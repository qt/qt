// Commit: fa8d0838dfc40ed269b30b9872cfdc2d2b16b64a
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

#include "qsgtext_p.h"
#include "qsgtext_p_p.h"

#include "distancefieldfontatlas_p.h"
#include "qsgcontext.h"
#include "adaptationlayer.h"
#include "qsgtextnode_p.h"

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qabstracttextdocumentlayout.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextdocument.h>
#include <QtGui/qtextobject.h>

#include <private/qdeclarativestyledtext_p.h>
#include <private/qdeclarativepixmapcache_p.h>

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

QSGTextPrivate::QSGTextPrivate()
: color((QRgb)0), style(QSGText::Normal), hAlign(QSGText::AlignLeft), 
  vAlign(QSGText::AlignTop), elideMode(QSGText::ElideNone),
  format(QSGText::AutoText), wrapMode(QSGText::NoWrap), imageCacheDirty(true), 
  updateOnComponentComplete(true), richText(false), singleline(false), cacheAllTextAsImage(true), 
  internalWidthUpdate(false), doc(0) 
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
            if (singleline && elideMode != QSGText::ElideNone && q->widthValid()) {
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
}

void QSGTextPrivate::updateSize()
{
    Q_Q(QSGText);

    if (!q->isComponentComplete()) {
        updateOnComponentComplete = true;
        return;
    }

    invalidateImageCache();

    QFontMetrics fm(font);
    if (text.isEmpty()) {
        q->setImplicitWidth(0);
        q->setImplicitHeight(fm.height());
        emit q->paintedSizeChanged();
        q->update();
        return;
    }

    int dy = q->height();
    QSize size(0, 0);

    //setup instance of QTextLayout for all cases other than richtext
    if (!richText) {
        size = setupTextLayout();
        if (layedOutTextSize != size) {
            layedOutTextSize = size;
        }
        dy -= size.height();
    } else {
        singleline = false; // richtext can't elide or be optimized for single-line case
        ensureDoc();
        doc->setDefaultFont(font);
        QTextOption option((Qt::Alignment)int(hAlign | vAlign));
        option.setWrapMode(QTextOption::WrapMode(wrapMode));
        doc->setDefaultTextOption(option);
        if (wrapMode != QSGText::NoWrap && q->widthValid())
            doc->setTextWidth(q->width());
        else
            doc->setTextWidth(doc->idealWidth()); // ### Text does not align if width is not set (QTextDoc bug)
        dy -= (int)doc->size().height();
        QSize dsize = doc->size().toSize();
        if (dsize != layedOutTextSize) {
            layedOutTextSize = dsize;
        }
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
    q->setImplicitWidth(size.width());
    internalWidthUpdate = false;
    q->setImplicitHeight(size.height());
    emit q->paintedSizeChanged();
    q->update();
}

/*!
    Lays out the QSGTextPrivate::layout QTextLayout in the constraints of the QSGText.

    Returns the size of the final text.  This can be used to position the text vertically (the text is
    already absolutely positioned horizontally).
*/
QSize QSGTextPrivate::setupTextLayout()
{
    // ### text layout handling should be profiled and optimized as needed
    // what about QStackTextEngine engine(tmp, d->font.font()); QTextLayout textLayout(&engine);
    Q_Q(QSGText);
    layout.setCacheEnabled(true);

    qreal height = 0;
    qreal widthUsed = 0;
    qreal lineWidth = 0;

    //set manual width
    if ((wrapMode != QSGText::NoWrap || elideMode != QSGText::ElideNone) && q->widthValid())
        lineWidth = q->width();

    QTextOption textOption = layout.textOption();
    textOption.setWrapMode(QTextOption::WrapMode(wrapMode));
    layout.setTextOption(textOption);

    layout.beginLayout();
    forever {
        QTextLine line = layout.createLine();
        if (!line.isValid())
            break;

        if (lineWidth)
            line.setLineWidth(lineWidth);
    }
    layout.endLayout();

    for (int i = 0; i < layout.lineCount(); ++i) {
        QTextLine line = layout.lineAt(i);
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
    }

    qreal layoutWidth = q->widthValid() ? q->width() : widthUsed;

    qreal x = 0;
    for (int i = 0; i < layout.lineCount(); ++i) {
        QTextLine line = layout.lineAt(i);
        line.setPosition(QPointF(0, height));
        height += line.height();

        if (!cacheAllTextAsImage) {
            if (hAlign == QSGText::AlignLeft) {
                x = 0;
            } else if (hAlign == QSGText::AlignRight) {
                x = layoutWidth - line.naturalTextWidth();
            } else if (hAlign == QSGText::AlignHCenter) {
                x = (layoutWidth - line.naturalTextWidth()) / 2;
            }
            line.setPosition(QPointF(x, line.y()));
        }
    }

    return layout.boundingRect().toAlignedRect().size();
}

/*!
    Returns a painted version of the QSGTextPrivate::layout QTextLayout.
    If \a drawStyle is true, the style color overrides all colors in the document.
*/
QPixmap QSGTextPrivate::textLayoutImage(bool drawStyle)
{
    //do layout
    QSize size = layedOutTextSize;

    qreal x = 0;
    for (int i = 0; i < layout.lineCount(); ++i) {
        QTextLine line = layout.lineAt(i);
        if (hAlign == QSGText::AlignLeft) {
            x = 0;
        } else if (hAlign == QSGText::AlignRight) {
            x = size.width() - line.naturalTextWidth();
        } else if (hAlign == QSGText::AlignHCenter) {
            x = (size.width() - line.naturalTextWidth()) / 2;
        }
        line.setPosition(QPointF(x, line.y()));
    }

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
        drawTextLayout(&p, QPointF(0,0), drawStyle);
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

    if(cacheAllTextAsImage || style != QSGText::Normal){//If actually using the image cache
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
: QSGItem(*(new QSGTextPrivate), parent)
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
    return d->font;
}

void QSGText::setFont(const QFont &font)
{
    Q_D(QSGText);
    if (d->font == font)
        return;

    d->font = font;
    if (DistanceFieldFontAtlas::useDistanceFieldForFont(d->font))
        qt_disableFontHinting(d->font);

    d->updateLayout();

    emit fontChanged(d->font);
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
    if (d->richText && isComponentComplete()) {
        d->ensureDoc();
        d->doc->setText(n);
    }

    d->text = n;
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
    if (d->hAlign == align)
        return;

    d->hAlign = align;
    d->updateLayout();

    emit horizontalAlignmentChanged(align);
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

    int w = width();
    int h = height();

    int x = 0;
    int y = 0;

    QSize size = d->layedOutTextSize;
    if (d->style != Normal)
        size += QSize(2,2);

    // Could include font max left/right bearings to either side of rectangle.

    switch (d->hAlign) {
    case AlignLeft:
        x = 0;
        break;
    case AlignRight:
        x = w - size.width();
        break;
    case AlignHCenter:
        x = (w - size.width()) / 2;
        break;
    }

    switch (d->vAlign) {
    case AlignTop:
        y = 0;
        break;
    case AlignBottom:
        y = h - size.height();
        break;
    case AlignVCenter:
        y = (h - size.height()) / 2;
        break;
    }

    return QRectF(x,y,size.width(),size.height());
}

/*! \internal */
void QSGText::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_D(QSGText);
    if ((!d->internalWidthUpdate && newGeometry.width() != oldGeometry.width())
            && (d->wrapMode != QSGText::NoWrap
                || d->elideMode != QSGText::ElideNone
                || d->hAlign != QSGText::AlignLeft)) {
        if (d->singleline && d->elideMode != QSGText::ElideNone && widthValid()) {
            // We need to re-elide
            d->updateLayout();
        } else {
            // We just need to re-layout
            d->updateSize();
        }
    }

    QSGItem::geometryChanged(newGeometry, oldGeometry);
}

Node *QSGText::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    Q_D(QSGText);

    bool richTextAsImage = false;
    if (d->richText) {
        d->ensureDoc();
        richTextAsImage = QSGTextNode::isComplexRichText(d->doc);
    }

    QRectF bounds = boundingRect();

    // XXX todo - some styled text can be done by the QSGTextNode
    if (richTextAsImage || d->cacheAllTextAsImage || d->style != Normal) {
        bool wasDirty = d->imageCacheDirty;

        d->checkImageCache();

        if (d->imageCache.isNull()) {
            delete oldNode;
            return 0;
        }

        TextureNodeInterface *node = 0;
        if (!oldNode || oldNode->subType() != Node::TextureNodeInterfaceSubType) {
            delete oldNode;
            node = QSGContext::current->createTextureNode();
            wasDirty = true;
        } else {
            node = static_cast<TextureNodeInterface *>(oldNode);
        }

        if (wasDirty) {
            QSGTextureManager *tm = QSGContext::current->textureManager();
            QSGTextureRef ref = tm->upload(d->imageCache.toImage());
            node->setTexture(ref);
        }

        node->setRect(QRectF(bounds.x(), bounds.y(), d->imageCache.width(), d->imageCache.height()));
        node->setSourceRect(QRectF(0, 0, 1., 1.));
        node->setOpacity(data->opacity);
        node->setClampToEdge(true);
        node->setLinearFiltering(d->smooth);
        node->update();

        return node;

    } else {
        QSGTextNode *node = 0;
        if (!oldNode || oldNode->subType() != Node::TextNodeSubType) {
            delete oldNode;
            node = new QSGTextNode(QSGContext::current);
        } else {
            node = static_cast<QSGTextNode *>(oldNode);
        }

        node->deleteContent();
        node->setOpacity(data->opacity);
        node->setMatrix(QMatrix4x4());

        if (d->richText) {

            d->ensureDoc();
            node->addTextDocument(bounds.topLeft(), d->doc);

        } else {
            node->addTextLayout(QPoint(0, bounds.y()), &d->layout, d->color);
            QMatrix4x4 m;
            m.translate(0, QFontMetricsF(d->font).ascent());
            node->setMatrix(m);
        }

        return node;
    }
}

qreal QSGText::paintedWidth() const
{
    return implicitWidth();
}

qreal QSGText::paintedHeight() const
{
    return implicitHeight();
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
        }
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
