/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qglyphs.h"
#include "qglyphs_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGlyphs
    \brief the QGlyphs class provides direct access to the internal glyphs in a font
    \since 4.8

    \ingroup text
    \mainclass

    When Qt displays a string of text encoded in Unicode, it will first convert the Unicode points
    into a list of glyph indexes and a list of positions based on one or more fonts. The Unicode
    representation of the text and the QFont object will in this case serve as a convenient
    abstraction that hides the details of what actually takes place when displaying the text
    on-screen. For instance, by the time the text actually reaches the screen, it may be represented
    by a set of fonts in addition to the one specified by the user, e.g. in case the originally
    selected font did not support all the writing systems contained in the text.

    Under certain circumstances, it can be useful as an application developer to have more low-level
    control over which glyphs in a specific font are drawn to the screen. This could for instance
    be the case in applications that use an external font engine and text shaper together with Qt.
    QGlyphs provides an interface to the raw data needed to get text on the screen. It
    contains a list of glyph indexes, a position for each glyph and a font.

    It is the user's responsibility to ensure that the selected font actually contains the
    provided glyph indexes.

    QTextLayout::glyphs() can be used to convert unicode encoded text into a list of QGlyphs
    objects, and QPainter::drawGlyphs() can be used to draw the glyphs.
*/


/*!
    Constructs an empty QGlyphs object.
*/
QGlyphs::QGlyphs() : d(new QGlyphsPrivate)
{
}

/*!
    Constructs a QGlyphs object which is a copy of \a other.
*/
QGlyphs::QGlyphs(const QGlyphs &other)
{
    d = other.d;
}

/*!
    Destroys the QGlyphs.
*/
QGlyphs::~QGlyphs()
{
    // Required for QExplicitlySharedDataPointer
}

/*!
    \internal
*/
void QGlyphs::detach()
{
    if (d->ref != 1)
        d.detach();
}

/*!
    Assigns \a other to this QGlyphs object.
*/
QGlyphs &QGlyphs::operator=(const QGlyphs &other)
{
    d = other.d;
    return *this;
}

/*!
    Compares \a other to this QGlyphs object. Returns true if the list of glyph indexes,
    the list of positions and the font are all equal, otherwise returns false.
*/
bool QGlyphs::operator==(const QGlyphs &other) const
{
    return ((d == other.d)
            || (d->glyphIndexes == other.d->glyphIndexes
                && d->glyphPositions == other.d->glyphPositions
                && d->font == other.d->font));
}

/*!
    Compares \a other to this QGlyphs object. Returns true if any of the list of glyph
    indexes, the list of positions or the font are different, otherwise returns false.
*/
bool QGlyphs::operator!=(const QGlyphs &other) const
{
    return !(*this == other);
}

/*!
    \internal

    Adds together the lists of glyph indexes and positions in \a other and this QGlyphs
    object and returns the result. The font in the returned QGlyphs will be the same as in
    this QGlyphs object.
*/
QGlyphs QGlyphs::operator+(const QGlyphs &other) const
{
    QGlyphs ret(*this);
    ret += other;
    return ret;
}

/*!
    \internal

    Appends the glyph indexes and positions in \a other to this QGlyphs object and returns
    a reference to the current object.
*/
QGlyphs &QGlyphs::operator+=(const QGlyphs &other)
{
    detach();

    d->glyphIndexes += other.d->glyphIndexes;
    d->glyphPositions += other.d->glyphPositions;

    return *this;
}

/*!
    Returns the font selected for this QGlyphs object.

    \sa setFont()
*/
QFont QGlyphs::font() const
{
    return d->font;
}

/*!
    Sets the font in which to look up the glyph indexes to \a font. This must be an explicitly
    resolvable font which defines glyphs for the specified glyph indexes.

    \sa font(), setGlyphIndexes()
*/
void QGlyphs::setFont(const QFont &font)
{
    detach();
    d->font = font;
}

/*!
    Returns the glyph indexes for this QGlyphs object.

    \sa setGlyphIndexes(), setPositions()
*/
QVector<quint32> QGlyphs::glyphIndexes() const
{
    return d->glyphIndexes;
}

/*!
    Set the glyph indexes for this QGlyphs object to \a glyphIndexes. The glyph indexes must
    be valid for the selected font.
*/
void QGlyphs::setGlyphIndexes(const QVector<quint32> &glyphIndexes)
{
    detach();
    d->glyphIndexes = glyphIndexes;
}

/*!
    Returns the position of the edge of the baseline for each glyph in this set of glyph indexes.
*/
QVector<QPointF> QGlyphs::positions() const
{
    return d->glyphPositions;
}

/*!
    Sets the positions of the edge of the baseline for each glyph in this set of glyph indexes to
    \a positions.
*/
void QGlyphs::setPositions(const QVector<QPointF> &positions)
{
    detach();
    d->glyphPositions = positions;
}

/*!
    Clears all data in the QGlyphs object.
*/
void QGlyphs::clear()
{
    detach();
    d->glyphPositions = QVector<QPointF>();
    d->glyphIndexes = QVector<quint32>();
    d->font = QFont();
}

QT_END_NAMESPACE
