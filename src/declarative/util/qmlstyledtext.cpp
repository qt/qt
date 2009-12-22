/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QXmlStreamReader>
#include <QStack>
#include <QVector>
#include <QPainter>
#include <QTextLayout>
#include <QDebug>
#include <qmath.h>
#include "qmlstyledtext_p.h"

/*
    QmlStyledText supports few tags:

    <b></b> - bold
    <br> - new line
    <font color="color_name"></font>

    The opening and closing tags may be correctly nested.
*/

class QmlStyledTextPrivate
{
public:
    QmlStyledTextPrivate(const QString &t, QTextLayout &l) : text(t), layout(l) {}

    void parse();
    bool parseTag(const QChar *&ch, const QString &textIn, QString &textOut, QTextCharFormat &format);
    bool parseCloseTag(const QChar *&ch, const QString &textIn);
    bool parseFontAttributes(const QChar *&ch, const QString &textIn, QTextCharFormat &format);
    QPair<QStringRef,QStringRef> parseAttribute(const QChar *&ch, const QString &textIn);
    QStringRef parseValue(const QChar *&ch, const QString &textIn);

    inline void skipSpace(const QChar *&ch) {
        while (ch->isSpace() && !ch->isNull())
            ++ch;
    }

    QString text;
    QTextLayout &layout;

    static const QChar lessThan;
    static const QChar greaterThan;
    static const QChar equals;
    static const QChar singleQuote;
    static const QChar doubleQuote;
    static const QChar slash;
};

const QChar QmlStyledTextPrivate::lessThan(QLatin1Char('<'));
const QChar QmlStyledTextPrivate::greaterThan(QLatin1Char('>'));
const QChar QmlStyledTextPrivate::equals(QLatin1Char('='));
const QChar QmlStyledTextPrivate::singleQuote(QLatin1Char('\''));
const QChar QmlStyledTextPrivate::doubleQuote(QLatin1Char('\"'));
const QChar QmlStyledTextPrivate::slash(QLatin1Char('/'));

QmlStyledText::QmlStyledText(const QString &string, QTextLayout &layout)
: d(new QmlStyledTextPrivate(string, layout))
{
}

QmlStyledText::~QmlStyledText()
{
    delete d;
}

void QmlStyledText::parse(const QString &string, QTextLayout &layout)
{
    QmlStyledText styledText(string, layout);
    styledText.d->parse();
}

#define CUSTOM_PARSER

#ifndef CUSTOM_PARSER
void QmlStyledTextPrivate::parse()
{
    QList<QTextLayout::FormatRange> ranges;
    QStack<QTextCharFormat> formatStack;

    QString drawText;
    drawText.reserve(text.count());

    QXmlStreamReader xml("<html>" + text + "</html>");
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            QStringRef name = xml.name();
            if (name == "b") {
                QTextCharFormat format;
                if (formatStack.count())
                    format = formatStack.top();
                else
                    format.setFont(layout.font());
                format.setFontWeight(QFont::Bold);
                formatStack.push(format);
            } else if (name == "br") {
                drawText.append(QChar(QChar::LineSeparator));
            } else if (name == "font") {
                QTextCharFormat format;
                if (formatStack.count())
                    format = formatStack.top();
                else
                    format.setFont(layout.font());
                QStringRef col = xml.attributes().value("color");
                if (!col.isEmpty()) {
                    format.setForeground(QColor(col.toString()));
                    formatStack.push(format);
                }
            }
        } else if (xml.tokenType() == QXmlStreamReader::EndElement) {
            if (formatStack.count() > 1) {
                QStringRef name = xml.name();
                if (name == "b" || name == "font" || name == "br")
                    formatStack.pop();
            }
        } else if (xml.tokenType() == QXmlStreamReader::Characters) {
            if (formatStack.count() > 1) {
                QTextLayout::FormatRange formatRange;
                formatRange.format = formatStack.top();
                formatRange.start = drawText.length();
                formatRange.length = xml.text().length();
                ranges.append(formatRange);
            }
            drawText.append(xml.text());
        }
    }

    layout.setText(drawText);
    layout.setAdditionalFormats(ranges);
}
#else
void QmlStyledTextPrivate::parse()
{
    QList<QTextLayout::FormatRange> ranges;
    QStack<QTextCharFormat> formatStack;

    QString drawText;
    drawText.reserve(text.count());

    int textStart = 0;
    int textLength = 0;
    const QChar *ch = text.constData();
    while (!ch->isNull()) {
        if (*ch == lessThan) {
            if (textLength) {
                if (formatStack.count()) {
                    QTextLayout::FormatRange formatRange;
                    formatRange.format = formatStack.top();
                    formatRange.start = drawText.length();
                    formatRange.length = textLength;
                    ranges.append(formatRange);
                }
                drawText.append(QStringRef(&text, textStart, textLength));
            }
            ++ch;
            if (*ch == slash) {
                ++ch;
                if (parseCloseTag(ch, text))
                    formatStack.pop();
            } else {
                QTextCharFormat format;
                if (formatStack.count())
                    format = formatStack.top();
                else
                    format.setFont(layout.font());
                if (parseTag(ch, text, drawText, format))
                    formatStack.push(format);
            }
            textStart = ch - text.constData() + 1;
            textLength = 0;
        } else {
            ++textLength;
        }
        ++ch;
    }
    if (textLength) {
        if (formatStack.count()) {
            QTextLayout::FormatRange formatRange;
            formatRange.format = formatStack.top();
            formatRange.start = drawText.length();
            formatRange.length = textLength;
            ranges.append(formatRange);
        }
        drawText.append(QStringRef(&text, textStart, textLength));
    }

    layout.setText(drawText);
    layout.setAdditionalFormats(ranges);
}
#endif


bool QmlStyledTextPrivate::parseTag(const QChar *&ch, const QString &textIn, QString &textOut, QTextCharFormat &format)
{
    skipSpace(ch);

    int tagStart = ch - textIn.constData();
    int tagLength = 0;
    while (!ch->isNull()) {
        if (*ch == greaterThan) {
            QStringRef tag(&textIn, tagStart, tagLength);
            if (tag.at(0) == QLatin1Char('b')) {
                if (tagLength == 1) {
                    format.setFontWeight(QFont::Bold);
                    return true;
                } else if (tagLength == 2 && tag.at(1) == QLatin1Char('r')) {
                    textOut.append(QChar(QChar::LineSeparator));
                    return true;
                }
            }
            return false;
        } else if (ch->isSpace()) {
            // may have params.
            QStringRef tag(&textIn, tagStart, tagLength);
            if (tag == QLatin1String("font"))
                return parseFontAttributes(ch, textIn, format);
            if (*ch == greaterThan || ch->isNull())
                continue;
        } else if (*ch != slash){
            tagLength++;
        }
        ++ch;
    }

    return false;
}

bool QmlStyledTextPrivate::parseCloseTag(const QChar *&ch, const QString &textIn)
{
    skipSpace(ch);

    int tagStart = ch - textIn.constData();
    int tagLength = 0;
    while (!ch->isNull()) {
        if (*ch == greaterThan) {
            QStringRef tag(&textIn, tagStart, tagLength);
            if (tag.at(0) == QLatin1Char('b')) {
                if (tagLength == 1)
                    return true;
                else if (tag.at(1) == QLatin1Char('r') && tagLength == 2)
                    return true;
            } else if (tag == QLatin1String("font")) {
                return true;
            }
            return false;
        } else if (!ch->isSpace()){
            tagLength++;
        }
        ++ch;
    }

    return false;
}

bool QmlStyledTextPrivate::parseFontAttributes(const QChar *&ch, const QString &textIn, QTextCharFormat &format)
{
    bool valid = false;
    QPair<QStringRef,QStringRef> attr;
    do {
        attr = parseAttribute(ch, textIn);
        if (attr.first == QLatin1String("color")) {
            valid = true;
            format.setForeground(QColor(attr.second.toString()));
        }
    } while (!ch->isNull() && !attr.first.isEmpty());

    return valid;
}

QPair<QStringRef,QStringRef> QmlStyledTextPrivate::parseAttribute(const QChar *&ch, const QString &textIn)
{
    skipSpace(ch);

    int attrStart = ch - textIn.constData();
    int attrLength = 0;
    while (!ch->isNull()) {
        if (*ch == greaterThan) {
            break;
        } else if (*ch == equals) {
            ++ch;
            if (*ch != singleQuote && *ch != doubleQuote) {
                while (*ch != greaterThan && !ch->isNull())
                    ++ch;
                break;
            }
            ++ch;
            if (!attrLength)
                break;
            QStringRef attr(&textIn, attrStart, attrLength);
            QStringRef val = parseValue(ch, textIn);
            if (!val.isEmpty())
                return QPair<QStringRef,QStringRef>(attr,val);
            break;
        } else {
            ++attrLength;
        }
        ++ch;
    }

    return QPair<QStringRef,QStringRef>();
}

QStringRef QmlStyledTextPrivate::parseValue(const QChar *&ch, const QString &textIn)
{
    int valStart = ch - textIn.constData();
    int valLength = 0;
    while (*ch != singleQuote && *ch != doubleQuote && !ch->isNull()) {
        ++valLength;
        ++ch;
    }
    if (ch->isNull())
        return QStringRef();
    ++ch; // skip quote

    return QStringRef(&textIn, valStart, valLength);
}
