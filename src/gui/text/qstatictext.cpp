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
#include <private/qfontengine_p.h>

#include <QtGui/qapplication.h>

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

    The class primarily provides an optimization for cases where text and the transformations on
    the painter are static over several paint events. If the text or its layout is changed
    regularly, QPainter::drawText() is the more efficient alternative. Translating the painter
    will not cause the layout of the text to be recalculated, but will cause a very small
    performance impact on drawStaticText(). Altering any other parts of the painter's
    transformation or the painter's font will cause the layout of the static text to be
    recalculated. This should be avoided as often as possible to maximize the performance
    benefit of using QStaticText.

    In addition, only affine transformations are supported by drawStaticText(). Calling
    drawStaticText() on a projected painter will perform slightly worse than using the regular
    drawText() call, so this should be avoided.

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
    be clipped at the given size. The position of the text is decided by the argument
    passed to QPainter::drawStaticText() and can change from call to call with a minimal impact
    on performance.

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
QStaticText::QStaticText(const QString &text, const QSizeF &size)
    : d_ptr(new QStaticTextPrivate) 
{    
    d_ptr->text = text;
    d_ptr->size = size;
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
  Prepares the QStaticText object for being painted with the given \a matrix and the given
  \a font to avoid overhead when the actual drawStaticText() call is made.

  When drawStaticText() is called, the layout of the QStaticText will be recalculated if the
  painter's font or matrix is different from the one used for the currently cached layout. By
  default, QStaticText will use a default constructed QFont and an identity matrix to create
  its layout.

  To avoid the overhead of creating the layout the first time you draw the QStaticText with
  a painter whose matrix or font are different from the defaults, you can use the prepare()
  function and pass in the matrix and font you expect to use when drawing the text.

  \sa QPainter::setFont(), QPainter::setMatrix()
*/
void QStaticText::prepare(const QTransform &matrix, const QFont &font)
{
    d_ptr->matrix = matrix;
    d_ptr->font = font;
    d_ptr->init();
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
            || (d_ptr->text == other.d_ptr->text
                && d_ptr->font == other.d_ptr->font
                && d_ptr->size == other.d_ptr->size));
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
    Sets the maximum size of the QStaticText to \a maximumSize.

    \note This function will cause the layout of the text to be recalculated.

    \sa maximumSize()
*/
void QStaticText::setMaximumSize(const QSizeF &size)
{
    detach();
    d_ptr->size = size;
    d_ptr->init();
}

/*!
    Returns the maximum size of the QStaticText.

    \sa setMaximumSize()
*/
QSizeF QStaticText::maximumSize() const
{
    return d_ptr->size;
}

/*!
   Returns true if the text of the QStaticText is empty, and false if not.

   \sa text()
*/
bool QStaticText::isEmpty() const
{
    return d_ptr->text.isEmpty();
}

QStaticTextPrivate::QStaticTextPrivate()
        : items(0), itemCount(0), glyphPool(0), positionPool(0), needsClipRect(false)
{
    ref = 1;    
}

QStaticTextPrivate::~QStaticTextPrivate()
{
    delete[] items;    
    delete[] glyphPool;
    delete[] positionPool;
}

QStaticTextPrivate *QStaticTextPrivate::get(const QStaticText *q)
{
    return q->d_ptr;
}


extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

namespace {

    class DrawTextItemRecorder: public QPaintEngine
    {
    public:
        DrawTextItemRecorder(int expectedItemCount, QStaticTextItem *items,
                             int expectedGlyphCount, QFixedPoint *positionPool, glyph_t *glyphPool)
                : m_items(items),
                  m_expectedItemCount(expectedItemCount),
                  m_expectedGlyphCount(expectedGlyphCount),
                  m_itemCount(0), m_glyphCount(0),
                  m_positionPool(positionPool),
                  m_glyphPool(glyphPool)
        {
        }

        virtual void drawTextItem(const QPointF &position, const QTextItem &textItem)
        {
            const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);          

            m_itemCount++;
            m_glyphCount += ti.glyphs.numGlyphs;
            if (m_items == 0)
                return;

            Q_ASSERT(m_itemCount <= m_expectedItemCount);
            Q_ASSERT(m_glyphCount <= m_expectedGlyphCount);

            QStaticTextItem *currentItem = (m_items + (m_itemCount - 1));
            currentItem->fontEngine = ti.fontEngine;
            currentItem->font = ti.font();
            currentItem->chars = ti.chars;
            currentItem->numChars = ti.num_chars;
            currentItem->numGlyphs = ti.glyphs.numGlyphs;
            currentItem->glyphs = m_glyphPool;
            currentItem->glyphPositions = m_positionPool;

            QTransform matrix = state->transform();
            matrix.translate(position.x(), position.y());

            QVarLengthArray<glyph_t> glyphs;
            QVarLengthArray<QFixedPoint> positions;
            ti.fontEngine->getGlyphPositions(ti.glyphs, matrix, ti.flags, glyphs, positions);

            int size = glyphs.size();
            Q_ASSERT(size == ti.glyphs.numGlyphs);
            Q_ASSERT(size == positions.size());

            memmove(currentItem->glyphs, glyphs.constData(), sizeof(glyph_t) * size);
            memmove(currentItem->glyphPositions, positions.constData(), sizeof(QFixedPoint) * size);

            m_glyphPool += size;
            m_positionPool += size;
        }                


        virtual bool begin(QPaintDevice *)  { return true; }
        virtual bool end() { return true; }
        virtual void updateState(const QPaintEngineState &) {}
        virtual void drawPixmap(const QRectF &, const QPixmap &, const QRectF &) {}
        virtual Type type() const
        {
            return User;
        }

        int itemCount() const
        {
            return m_itemCount;
        }

        int glyphCount() const
        {
            return m_glyphCount;
        }

    private:
        QStaticTextItem *m_items;
        int m_itemCount;
        int m_glyphCount;
        int m_expectedItemCount;
        int m_expectedGlyphCount;

        glyph_t *m_glyphPool;
        QFixedPoint *m_positionPool;
    };

    class DrawTextItemDevice: public QPaintDevice
    {
    public:
        DrawTextItemDevice(int expectedItemCount = -1,  QStaticTextItem *items = 0,
                           int expectedGlyphCount = -1, QFixedPoint *positionPool = 0,
                           glyph_t *glyphPool = 0)
        {
            m_paintEngine = new DrawTextItemRecorder(expectedItemCount, items,
                                                     expectedGlyphCount, positionPool, glyphPool);
        }

        ~DrawTextItemDevice()
        {
            delete m_paintEngine;
        }

        int metric(PaintDeviceMetric m) const
        {
            int val;
            switch (m) {
            case PdmWidth:
            case PdmHeight:
            case PdmWidthMM:
            case PdmHeightMM:
                val = 0;
                break;
            case PdmDpiX:
            case PdmPhysicalDpiX:
                val = qt_defaultDpiX();
                break;
            case PdmDpiY:
            case PdmPhysicalDpiY:
                val = qt_defaultDpiY();
                break;
            case PdmNumColors:
                val = 16777216;
                break;
            case PdmDepth:
                val = 24;
                break;
            default:
                val = 0;
                qWarning("DrawTextItemDevice::metric: Invalid metric command");
            }
            return val;
        }

        virtual QPaintEngine *paintEngine() const
        {
            return m_paintEngine;
        }

        int itemCount() const
        {
            return m_paintEngine->itemCount();
        }

        int glyphCount() const
        {
            return m_paintEngine->glyphCount();
        }

    private:
        DrawTextItemRecorder *m_paintEngine;
    };

}

void QStaticTextPrivate::init()
{
    delete[] items;
    delete[] glyphPool;
    delete[] positionPool;

    position = QPointF(0, 0);

    // Draw once to count number of items and glyphs, so that we can use as little memory
    // as possible to store the data
    DrawTextItemDevice counterDevice;
    {
        QPainter painter(&counterDevice);
        painter.setFont(font);
        painter.setTransform(matrix);

        if (size.isValid())
            painter.drawText(QRectF(QPointF(0, 0), size), text);
        else
            painter.drawText(0, 0, text);
    }

    itemCount = counterDevice.itemCount();    
    items = new QStaticTextItem[itemCount];

    int glyphCount = counterDevice.glyphCount();
    glyphPool = new glyph_t[glyphCount];
    positionPool = new QFixedPoint[glyphCount];

    // Draw again to actually record the items and glyphs
    DrawTextItemDevice recorderDevice(itemCount, items, glyphCount, positionPool, glyphPool);
    {
        QPainter painter(&recorderDevice);
        painter.setFont(font);
        painter.setTransform(matrix);

        if (size.isValid()) {
            QRectF boundingRect;
            painter.drawText(QRectF(QPointF(0, 0), size), Qt::TextWordWrap, text, &boundingRect);

            needsClipRect = boundingRect.width() > size.width()
                            || boundingRect.height() > size.height();

        } else {
            painter.drawText(0, 0, text);
            needsClipRect = false;
        }
    }

}

QT_END_NAMESPACE
