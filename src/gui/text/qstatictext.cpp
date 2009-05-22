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
QStaticText::QStaticText(const QString &text, const QFont &font, const QSizeF &sz)
    : d_ptr(new QStaticTextPrivate) 
{
    Q_D(QStaticText);
    
    d->textLayout->setText(text);
    d->textLayout->setFont(font);
    d->size = sz;

    d->init();
}

/*!
    Destroys the QStaticText.
*/
QStaticText::~QStaticText()
{
    Q_D(QStaticText);
    delete d;
}

/*!
    Sets the text of the QStaticText to \a text.

    \note This function will cause the layout of the text to be recalculated.

    \sa text()
*/
void QStaticText::setText(const QString &text) 
{
    Q_D(QStaticText);
    d->textLayout->setText(text);
    d->init();
}

/*!
    Returns the text of the QStaticText.

    \sa setText()
*/
QString QStaticText::text() const 
{
    Q_D(const QStaticText);
    return d->textLayout->text();
}

/*!
    Sets the font of the QStaticText to \a font.

    \note This function will cause the layout of the text to be recalculated.

    \sa font()
*/
void QStaticText::setFont(const QFont &font)
{
    Q_D(QStaticText);
    d->textLayout->setFont(font);
    d->init();
}

/*!
    Returns the font of the QStaticText.

    \sa setFont()
*/
QFont QStaticText::font() const
{
    Q_D(const QStaticText);
    return d->textLayout->font();
}

/*!
    Sets the maximum size of the QStaticText to \a maximumSize. If a valid maximum size is set for 
    the QStaticText, it will be formatted to fit within its width, and clipped by its height.

    \note This function will cause the layout of the text to be recalculated.

    \sa maximumSize()
*/
void QStaticText::setMaximumSize(const QSizeF &maximumSize)
{
    Q_D(QStaticText);
    d->size = maximumSize;
    d->init();
}

/*!
    Returns the maximum size of the QStaticText.

    \sa setMaximumSize()
*/
QSizeF QStaticText::maximumSize() const
{
    Q_D(const QStaticText);
    return d->size;
}

QStaticTextPrivate::QStaticTextPrivate()
    : textLayout(new QTextLayout())
{
}

QStaticTextPrivate::~QStaticTextPrivate()
{
    delete textLayout;
}

QStaticTextPrivate *QStaticTextPrivate::get(const QStaticText *q)
{
    return q->d_ptr;
}

void QStaticTextPrivate::init()
{
    Q_ASSERT(textLayout != 0);
    textLayout->setCacheEnabled(true);

    QFontMetrics fontMetrics(textLayout->font());

    textLayout->beginLayout();
    int h = size.isValid() ? 0 : -fontMetrics.ascent();

    QTextLine line;
    qreal lineWidth = size.isValid() ? size.width() : fontMetrics.width(textLayout->text());
    while ((line = textLayout->createLine()).isValid()) {
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, h));
        h += line.height();
    }
    textLayout->endLayout();
}

QT_END_NAMESPACE
