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

#include "qsgtextnode_p.h"
#include "qsgsimplerectnode.h"
#include <private/qsgadaptationlayer_p.h>
#include <private/qsgdistancefieldglyphcache_p.h>
#include <private/qsgdistancefieldglyphnode_p.h>

#include <private/qsgcontext_p.h>

#include <qmath.h>
#include <qtextdocument.h>
#include <qtextlayout.h>
#include <qabstracttextdocumentlayout.h>
#include <qxmlstream.h>
#include <qrawfont.h>
#include <private/qdeclarativestyledtext_p.h>
#include <private/qfont_p.h>
#include <private/qfontengine_p.h>
#include <private/qrawfont_p.h>

QT_BEGIN_NAMESPACE

/*!
  Creates an empty QSGTextNode
*/
QSGTextNode::QSGTextNode(QSGContext *context)
: m_context(context)
{
#if defined(QML_RUNTIME_TESTING)
    description = QLatin1String("text");
#endif
}

QSGTextNode::~QSGTextNode()
{
}

#if 0
void QSGTextNode::setColor(const QColor &color)
{
    if (m_usePixmapCache) {
        setUpdateFlag(UpdateNodes);
    } else {
        for (int i=0; i<childCount(); ++i) {
            QSGNode *childNode = childAtIndex(i);
            if (childNode->subType() == GlyphNodeSubType) {
                QSGGlyphNode *glyphNode = static_cast<QSGGlyphNode *>(childNode);
                if (glyphNode->color() == m_color)
                    glyphNode->setColor(color);
            } else if (childNode->subType() == SolidRectNodeSubType) {
                QSGSimpleRectNode *solidRectNode = static_cast<QSGSimpleRectNode *>(childNode);
                if (solidRectNode->color() == m_color)
                    solidRectNode->setColor(color);
            }
        }
    }
    m_color = color;
}

void QSGTextNode::setStyleColor(const QColor &styleColor)
{
    if (m_textStyle != QSGTextNode::NormalTextStyle) {
        if (m_usePixmapCache) {
            setUpdateFlag(UpdateNodes);
        } else {
            for (int i=0; i<childCount(); ++i) {
                QSGNode *childNode = childAtIndex(i);
                if (childNode->subType() == GlyphNodeSubType) {
                    QSGGlyphNode *glyphNode = static_cast<QSGGlyphNode *>(childNode);
                    if (glyphNode->color() == m_styleColor)
                        glyphNode->setColor(styleColor);
                } else if (childNode->subType() == SolidRectNodeSubType) {
                    QSGSimpleRectNode *solidRectNode = static_cast<QSGSimpleRectNode *>(childNode);
                    if (solidRectNode->color() == m_styleColor)
                        solidRectNode->setColor(styleColor);
                }
            }
        }
    }
    m_styleColor = styleColor;
}
#endif

void QSGTextNode::addTextDecorations(const QPointF &position, const QRawFont &font, const QColor &color,
                                     qreal width, bool hasOverline, bool hasStrikeOut, bool hasUnderline)
{
    Q_ASSERT(font.isValid());
    QRawFontPrivate *dptrFont = QRawFontPrivate::get(font);
    QFontEngine *fontEngine = dptrFont->fontEngine;

    qreal lineThickness = fontEngine->lineThickness().toReal();

    QRectF line(position.x(), position.y() - lineThickness / 2.0, width, lineThickness);

    if (hasUnderline) {
        int underlinePosition = fontEngine->underlinePosition().ceil().toInt();
        QRectF underline(line);
        underline.translate(0.0, underlinePosition);
        appendChildNode(new QSGSimpleRectNode(underline, color));
    }

    qreal ascent = font.ascent();
    if (hasOverline) {
        QRectF overline(line);
        overline.translate(0.0, -ascent);
        appendChildNode(new QSGSimpleRectNode(overline, color));
    }

    if (hasStrikeOut) {
        QRectF strikeOut(line);
        strikeOut.translate(0.0, ascent / -3.0);
        appendChildNode(new QSGSimpleRectNode(strikeOut, color));
    }
}

QSGGlyphNode *QSGTextNode::addGlyphs(const QPointF &position, const QGlyphs &glyphs, const QColor &color,
                                           QSGText::TextStyle style, const QColor &styleColor)
{
    QSGGlyphNode *node = m_context->createGlyphNode();
    if (QSGDistanceFieldGlyphCache::distanceFieldEnabled()) {
        QSGDistanceFieldGlyphNode *dfNode = static_cast<QSGDistanceFieldGlyphNode *>(node);
        dfNode->setStyle(style);
        dfNode->setStyleColor(styleColor);
    }
    node->setGlyphs(position, glyphs);
    node->setColor(color);

    appendChildNode(node);

    return node;
}

void QSGTextNode::addTextDocument(const QPointF &position, QTextDocument *textDocument, const QColor &color,
                                  QSGText::TextStyle style, const QColor &styleColor)
{
    Q_UNUSED(position)
    QTextFrame *textFrame = textDocument->rootFrame();
    QPointF p = textDocument->documentLayout()->frameBoundingRect(textFrame).topLeft();

    QTextFrame::iterator it = textFrame->begin();
    while (!it.atEnd()) {
        addTextBlock(p, textDocument, it.currentBlock(), color, style, styleColor);
        ++it;
    }
}

void QSGTextNode::addTextLayout(const QPointF &position, QTextLayout *textLayout, const QColor &color,
                                QSGText::TextStyle style, const QColor &styleColor)
{
    QList<QGlyphs> glyphsList(textLayout->glyphs());
    for (int i=0; i<glyphsList.size(); ++i)
        addGlyphs(position, glyphsList.at(i), color, style, styleColor);

    QFont font = textLayout->font();
    QRawFont rawFont = QRawFont::fromFont(font);
    if (font.strikeOut() || font.underline() || font.overline()) {
        addTextDecorations(position, rawFont, color, textLayout->boundingRect().width(),
                           font.overline(), font.strikeOut(), font.underline());
    }
}


/*!
  Returns true if \a text contains any HTML tags, attributes or CSS properties which are unrelated
   to text, fonts or text layout. Otherwise the function returns false. If the return value is
  false, \a text is considered to be easily representable in the scenegraph. If it returns true,
  then the text should be prerendered into a pixmap before it's displayed on screen.
*/
bool QSGTextNode::isComplexRichText(QTextDocument *doc)
{
    if (doc == 0)
        return false;

    static QSet<QString> supportedTags;
    if (supportedTags.isEmpty()) {
        supportedTags.insert(QLatin1String("i"));
        supportedTags.insert(QLatin1String("b"));
        supportedTags.insert(QLatin1String("u"));
        supportedTags.insert(QLatin1String("div"));
        supportedTags.insert(QLatin1String("big"));
        supportedTags.insert(QLatin1String("blockquote"));
        supportedTags.insert(QLatin1String("body"));
        supportedTags.insert(QLatin1String("br"));
        supportedTags.insert(QLatin1String("center"));
        supportedTags.insert(QLatin1String("cite"));
        supportedTags.insert(QLatin1String("code"));
        supportedTags.insert(QLatin1String("tt"));
        supportedTags.insert(QLatin1String("dd"));
        supportedTags.insert(QLatin1String("dfn"));
        supportedTags.insert(QLatin1String("em"));
        supportedTags.insert(QLatin1String("font"));
        supportedTags.insert(QLatin1String("h1"));
        supportedTags.insert(QLatin1String("h2"));
        supportedTags.insert(QLatin1String("h3"));
        supportedTags.insert(QLatin1String("h4"));
        supportedTags.insert(QLatin1String("h5"));
        supportedTags.insert(QLatin1String("h6"));
        supportedTags.insert(QLatin1String("head"));
        supportedTags.insert(QLatin1String("html"));
        supportedTags.insert(QLatin1String("meta"));
        supportedTags.insert(QLatin1String("nobr"));
        supportedTags.insert(QLatin1String("p"));
        supportedTags.insert(QLatin1String("pre"));
        supportedTags.insert(QLatin1String("qt"));
        supportedTags.insert(QLatin1String("s"));
        supportedTags.insert(QLatin1String("samp"));
        supportedTags.insert(QLatin1String("small"));
        supportedTags.insert(QLatin1String("span"));
        supportedTags.insert(QLatin1String("strong"));
        supportedTags.insert(QLatin1String("sub"));
        supportedTags.insert(QLatin1String("sup"));
        supportedTags.insert(QLatin1String("title"));
        supportedTags.insert(QLatin1String("var"));
        supportedTags.insert(QLatin1String("style"));
    }

    static QSet<QCss::Property> supportedCssProperties;
    if (supportedCssProperties.isEmpty()) {
        supportedCssProperties.insert(QCss::Color);
        supportedCssProperties.insert(QCss::Float);
        supportedCssProperties.insert(QCss::Font);
        supportedCssProperties.insert(QCss::FontFamily);
        supportedCssProperties.insert(QCss::FontSize);
        supportedCssProperties.insert(QCss::FontStyle);
        supportedCssProperties.insert(QCss::FontWeight);
        supportedCssProperties.insert(QCss::Margin);
        supportedCssProperties.insert(QCss::MarginBottom);
        supportedCssProperties.insert(QCss::MarginLeft);
        supportedCssProperties.insert(QCss::MarginRight);
        supportedCssProperties.insert(QCss::MarginTop);
        supportedCssProperties.insert(QCss::TextDecoration);
        supportedCssProperties.insert(QCss::TextIndent);
        supportedCssProperties.insert(QCss::TextUnderlineStyle);
        supportedCssProperties.insert(QCss::VerticalAlignment);
        supportedCssProperties.insert(QCss::Whitespace);
        supportedCssProperties.insert(QCss::Padding);
        supportedCssProperties.insert(QCss::PaddingLeft);
        supportedCssProperties.insert(QCss::PaddingRight);
        supportedCssProperties.insert(QCss::PaddingTop);
        supportedCssProperties.insert(QCss::PaddingBottom);
        supportedCssProperties.insert(QCss::PageBreakBefore);
        supportedCssProperties.insert(QCss::PageBreakAfter);
        supportedCssProperties.insert(QCss::Width);
        supportedCssProperties.insert(QCss::Height);
        supportedCssProperties.insert(QCss::MinimumWidth);
        supportedCssProperties.insert(QCss::MinimumHeight);
        supportedCssProperties.insert(QCss::MaximumWidth);
        supportedCssProperties.insert(QCss::MaximumHeight);
        supportedCssProperties.insert(QCss::Left);
        supportedCssProperties.insert(QCss::Right);
        supportedCssProperties.insert(QCss::Top);
        supportedCssProperties.insert(QCss::Bottom);
        supportedCssProperties.insert(QCss::Position);
        supportedCssProperties.insert(QCss::TextAlignment);
        supportedCssProperties.insert(QCss::FontVariant);
    }

    QXmlStreamReader reader(doc->toHtml("utf-8"));
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            if (!supportedTags.contains(reader.name().toString().toLower()))
                return true;

            QXmlStreamAttributes attributes = reader.attributes();
            if (attributes.hasAttribute(QLatin1String("bgcolor")))
                return true;
            if (attributes.hasAttribute(QLatin1String("style"))) {
                QCss::StyleSheet styleSheet;
                QCss::Parser(attributes.value(QLatin1String("style")).toString()).parse(&styleSheet);

                QVector<QCss::Declaration> decls;
                for (int i=0; i<styleSheet.pageRules.size(); ++i)
                    decls += styleSheet.pageRules.at(i).declarations;

                QVector<QCss::StyleRule> styleRules =
                        styleSheet.styleRules
                        + styleSheet.idIndex.values().toVector()
                        + styleSheet.nameIndex.values().toVector();
                for (int i=0; i<styleSheet.mediaRules.size(); ++i)
                    styleRules += styleSheet.mediaRules.at(i).styleRules;

                for (int i=0; i<styleRules.size(); ++i)
                    decls += styleRules.at(i).declarations;

                for (int i=0; i<decls.size(); ++i) {
                    if (!supportedCssProperties.contains(decls.at(i).d->propertyId))
                        return true;
                }

            }
        }
    }

    return reader.hasError();
}

void QSGTextNode::addTextBlock(const QPointF &position, QTextDocument *textDocument, const QTextBlock &block,
                               const QColor &overrideColor, QSGText::TextStyle style, const QColor &styleColor)
{
    if (!block.isValid())
        return;

    QPointF blockPosition = textDocument->documentLayout()->blockBoundingRect(block).topLeft();

    QTextBlock::iterator it = block.begin();
    while (!it.atEnd()) {
        QTextFragment fragment = it.fragment();
        if (!fragment.text().isEmpty()) {
            QTextCharFormat charFormat = fragment.charFormat();
            QColor color = overrideColor.isValid()
                    ? overrideColor
                    : charFormat.foreground().color();

            QFontMetricsF fm(fragment.charFormat().font());
            QPointF ascent(0, fm.ascent());

            QList<QGlyphs> glyphsList = fragment.glyphs();
            for (int i=0; i<glyphsList.size(); ++i) {
                QGlyphs glyphs = glyphsList.at(i);
                QSGGlyphNode *glyphNode = addGlyphs(position + blockPosition + ascent, glyphs,
                                                          color, style, styleColor);

                QRawFont font = glyphs.font();
                QPointF baseLine = glyphNode->baseLine();
                qreal width = glyphNode->boundingRect().width();
                addTextDecorations(baseLine, font, color, width,
                                   glyphs.overline(), glyphs.strikeOut(), glyphs.underline());
            }
        }

        ++it;
    }
}

void QSGTextNode::deleteContent()
{
    while (childCount() > 0)
        delete childAtIndex(0);
}

#if 0
void QSGTextNode::updateNodes()
{
    return;
    deleteContent();
    if (m_text.isEmpty())
        return;

    if (m_usePixmapCache) {
        // ### gunnar: port properly
//        QPixmap pixmap = generatedPixmap();
//        if (pixmap.isNull())
//            return;

//        QSGImageNode *pixmapNode = m_context->createImageNode();
//        pixmapNode->setRect(pixmap.rect());
//        pixmapNode->setSourceRect(pixmap.rect());
//        pixmapNode->setOpacity(m_opacity);
//        pixmapNode->setClampToEdge(true);
//        pixmapNode->setLinearFiltering(m_linearFiltering);

//        appendChildNode(pixmapNode);
    } else {
        if (m_text.isEmpty())
            return;

        // Implement styling by drawing text several times at slight shifts. shiftForStyle
        // contains the sequence of shifted positions at which to draw the text. All except
        // the last will be drawn with styleColor.
        QList<QPointF> shiftForStyle;
        switch (m_textStyle) {
        case OutlineTextStyle:
            // ### Should be made faster by implementing outline material
            shiftForStyle << QPointF(-1, 0);
            shiftForStyle << QPointF(0, -1);
            shiftForStyle << QPointF(1, 0);
            shiftForStyle << QPointF(0, 1);
            break;
        case SunkenTextStyle:
            shiftForStyle << QPointF(0, -1);
            break;
        case RaisedTextStyle:
            shiftForStyle << QPointF(0, 1);
            break;
        default:
            break;
        }

        shiftForStyle << QPointF(0, 0); // Regular position
        while (!shiftForStyle.isEmpty()) {
            QPointF shift = shiftForStyle.takeFirst();

            // Use styleColor for all but last shift
            if (m_richText) {
                QColor overrideColor = shiftForStyle.isEmpty() ? QColor() : m_styleColor;

                QTextFrame *textFrame = m_textDocument->rootFrame();
                QPointF p = m_textDocument->documentLayout()->frameBoundingRect(textFrame).topLeft();

                QTextFrame::iterator it = textFrame->begin();
                while (!it.atEnd()) {
                    addTextBlock(shift + p, it.currentBlock(), overrideColor);
                    ++it;
                }
            } else {
                addTextLayout(shift, m_textLayout, shiftForStyle.isEmpty()
                                                   ? m_color
                                                   : m_styleColor);
            }
        }
    }
}
#endif

QT_END_NAMESPACE
