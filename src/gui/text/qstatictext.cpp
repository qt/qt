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

#include "qstatictext.h"
#include "qstatictext_p.h"
#include <private/qtextengine_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QStaticText
    \brief The QStaticText class enables optimized drawing of text when the text and its layout
    is updated rarely.

    \ingroup multimedia
    \ingroup text
    \mainclass

    QStaticText provides a way to cache layout data for a block of text so that it can be drawn
    more efficiently than by using QPainter::drawText() in which the layout information is 
    recalculated with every call. 

    The class primarily provides an optimization for cases where text is static over several paint
    events. If the text or its layout is changed regularly, QPainter::drawText() is the more 
    efficient alternative.

    \code
    class MyWidget: public QWidget
    {
    public:
        MyWidget(QWidget *parent = 0) : QWidget(parent), m_staticText("This is static text")

    protected:
        void paintEvent(QPaintEvent *)
        {
            QPainter painter(this);
            painter.drawStaticText(0, 0, m_staticText);
        }

    private:
        QStaticText m_staticText;
    };
    \endcode

    The QStaticText class can be used to mimic the behavior of QPainter::drawText() to a specific
    point with no boundaries, and also when QPainter::drawText() is called with a bounding 
    rectangle. 

    If a bounding rectangle is not required, create a QStaticText object without setting a maximum 
    size. The text will then occupy a single line. 

    If you set a maximum size on the QStaticText object, this will bound the text. The text will
    be formatted so that no line exceeds the given width. When the object is painted, it will 
    be clipped vertically at the given height. The position of the text is decided by the argument
    passed to QPainter::drawStaticText() and can change from call to call without affecting 
    performance.

    \sa QPainter::drawText(), QPainter::drawStaticText(), QTextLayout, QTextDocument
*/

/*!
    Constructs an empty QStaticText
*/
QStaticText::QStaticText()
    : d_ptr(new QStaticTextPrivate)
{
}

/*!
    \fn QStaticText::QStaticText(const QString &text, const QFont &font, const QSizeF &maximumSize)

    Constructs a QStaticText object with the given \a text which is to be rendered in the given
    \a font and bounded by the given \a maximumSize. If an invalid size is passed for \a maximumSize
    the text will be unbounded.         
*/
QStaticText::QStaticText(const QString &text, const QFont &font)
    : d_ptr(new QStaticTextPrivate) 
{    
    d_ptr->text = text;
    d_ptr->font = font;
    d_ptr->init();
}

/*!
    Constructs a QStaticText object which is a copy of \a other.
*/
QStaticText::QStaticText(const QStaticText &other)    
{
    d_ptr = other.d_ptr;
    d_ptr->ref.ref();
}

/*!
    Destroys the QStaticText.
*/
QStaticText::~QStaticText()
{    
    if (!d_ptr->ref.deref())
        delete d_ptr;        
}

/*!
    \internal
*/
void QStaticText::detach()
{
    if (d_ptr->ref != 1)
        qAtomicDetach(d_ptr);    
}

/*!
    Assigns \a other to this QStaticText.
*/
QStaticText &QStaticText::operator=(const QStaticText &other)
{
    qAtomicAssign(d_ptr, other.d_ptr);
    return *this;
}

/*!
    Compares \a other to this QStaticText. Returns true if the texts, fonts and maximum sizes
    are equal.
*/
bool QStaticText::operator==(const QStaticText &other) const
{
    return (d_ptr == other.d_ptr
            || (d_ptr->text == other.d_ptr->text && d_ptr->font == other.d_ptr->font));
}

/*!
    Compares \a other to this QStaticText. Returns true if the texts, fonts or maximum sizes
    are different.
*/
bool QStaticText::operator!=(const QStaticText &other) const
{
    return !(*this == other);
}

/*!
    Sets the text of the QStaticText to \a text.

    \note This function will cause the layout of the text to be recalculated.

    \sa text()
*/
void QStaticText::setText(const QString &text) 
{
    detach();
    d_ptr->text = text;
    d_ptr->init();
}

/*!
    Returns the text of the QStaticText.

    \sa setText()
*/
QString QStaticText::text() const 
{
    return d_ptr->text;
}

/*!
    Sets the font of the QStaticText to \a font.

    \note This function will cause the layout of the text to be recalculated.

    \sa font()
*/
void QStaticText::setFont(const QFont &font)
{
    detach();
    d_ptr->font = font;
    d_ptr->init();
}

/*!
    Returns the font of the QStaticText.

    \sa setFont()
*/
QFont QStaticText::font() const
{
    return d_ptr->font;
}

QString QStaticText::toString() const
{
    return text();
}

bool QStaticText::isEmpty() const
{
    return d_ptr->text.isEmpty();
}

QStaticTextPrivate::QStaticTextPrivate()
        : glyphLayoutMemory(0), logClusterMemory(0), items(0), itemCount(0)
{
    ref = 1;    
}

QStaticTextPrivate::QStaticTextPrivate(const QStaticTextPrivate &other)
{
    ref = 1;
    text = other.text;
    font = other.font;
    init();
}

QStaticTextPrivate::~QStaticTextPrivate()
{
    delete[] glyphLayoutMemory;
    delete[] logClusterMemory;
    delete[] items;
}

QStaticTextPrivate *QStaticTextPrivate::get(const QStaticText *q)
{
    return q->d_ptr;
}

void QStaticTextPrivate::init()
{
    delete[] glyphLayoutMemory;
    delete[] logClusterMemory;
    delete[] items;

    QStackTextEngine engine(text, font);
    engine.itemize();

    engine.option.setTextDirection(QApplication::layoutDirection());
    QScriptLine line;
    line.length = text.length();
    engine.shapeLine(line);

    int nItems = engine.layoutData->items.size();
    QVarLengthArray<int> visualOrder(nItems);
    QVarLengthArray<uchar> levels(nItems);
    for (int i = 0; i < nItems; ++i)
        levels[i] = engine.layoutData->items[i].analysis.bidiLevel;
    QTextEngine::bidiReorder(nItems, levels.data(), visualOrder.data());

    int numGlyphs = engine.layoutData->used;
    glyphLayoutMemory = new char[QGlyphLayout::spaceNeededForGlyphLayout(numGlyphs)];
    logClusterMemory = new unsigned short[numGlyphs];
    items = new QTextItemInt[nItems];
    itemCount = nItems;

    char *currentGlyphLayout = glyphLayoutMemory;
    unsigned short *currentLogCluster = logClusterMemory;
    for (int i = 0; i < nItems; ++i) {
        int item = visualOrder[i];
        const QScriptItem &si = engine.layoutData->items.at(item);

        QFont f = engine.font(si);
        if (si.analysis.flags >= QScriptAnalysis::TabOrObject) {
            items[i].width = si.width;
            continue;
        }        

        items[i].init(si, &f);

        QGlyphLayout l = engine.shapedGlyphs(&si);
        items[i].glyphs = l.clone(currentGlyphLayout);
        currentGlyphLayout += QGlyphLayout::spaceNeededForGlyphLayout(l.numGlyphs);

        items[i].chars = text.unicode() + si.position;
        items[i].num_chars = engine.length(item);
        items[i].width = si.width;

        memmove(currentLogCluster, engine.logClusters(&si), sizeof(unsigned short) * l.numGlyphs);
        items[i].logClusters = currentLogCluster;
        currentLogCluster += l.numGlyphs;        
    }
}

QT_END_NAMESPACE
