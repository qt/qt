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

#include <QBuffer>
#include <qml.h>
#include "qfxscalegrid_p.h"


QT_BEGIN_NAMESPACE
/*!
    \internal
    \class QFxScaleGrid
    \brief The QFxScaleGrid class allows you to specify a 3x3 grid to use in scaling an image.

    A scale grid uses 4 grid lines (2 horizontal and 2 vertical) to break an image into 9 sections, as shown below:
    \image scalegrid.png

    When the image is scaled:
    \list
    \i the corners (sections 1, 3, 7, and 9) are not scaled at all
    \i the middle (section 5) is scaled both horizontally and vertically
    \i sections 2 and 8 are scaled horizontally
    \i sections 4 and 6 are scaled vertically
    \endlist

    A common way of specifying a scale grid is to create an sci file. An sci file uses a simple
    text-based format that specifies each grid line, as well as the associated image file. An example of an sci file's contents:
    \code
    gridLeft: 10
    gridTop: 10
    gridBottom: 10
    gridRight: 10
    imageFile: picture.png
    \endcode
*/
QML_DEFINE_NOCREATE_TYPE(QFxScaleGrid)

QFxScaleGrid::QFxScaleGrid() : QObject(), _left(0), _top(0), _right(0), _bottom(0)
{
}

QFxScaleGrid::~QFxScaleGrid()
{
}

bool QFxScaleGrid::isNull() const
{
    return !_left && !_top && !_right && !_bottom;
}

/*!
    \property QFxScaleGrid::left
    \brief the position of the left grid line as an offset from the left side of the image.
*/
void QFxScaleGrid::setLeft(int pos)
{
    _left = pos;
}

/*!
    \property QFxScaleGrid::top
    \brief the position of the top grid line as an offset from the top of the image.
*/
void QFxScaleGrid::setTop(int pos)
{
    _top = pos;
}

/*!
    \property QFxScaleGrid::right
    \brief the position of the right grid line as an offset from the right side of the image.
*/
void QFxScaleGrid::setRight(int pos)
{
    _right = pos;
}

/*!
    \property QFxScaleGrid::bottom
    \brief the position of the bottom grid line as an offset from the bottom of the image.
*/
void QFxScaleGrid::setBottom(int pos)
{
    _bottom = pos;
}

QFxGridScaledImage::QFxGridScaledImage()
: _l(-1), _r(-1), _t(-1), _b(-1)
{
}

QFxGridScaledImage::QFxGridScaledImage(const QFxGridScaledImage &o)
: _l(o._l), _r(o._r), _t(o._t), _b(o._b), _pix(o._pix)
{
}

QFxGridScaledImage &QFxGridScaledImage::operator=(const QFxGridScaledImage &o)
{
    _l = o._l;
    _r = o._r;
    _t = o._t;
    _b = o._b;
    _pix = o._pix;
    return *this;
}

QFxGridScaledImage::QFxGridScaledImage(QIODevice *data)
: _l(-1), _r(-1), _t(-1), _b(-1)
{
    int l = -1;
    int r = -1; 
    int t = -1; 
    int b = -1;
    QString imgFile;

    while(!data->atEnd()) {
        QString line = QString::fromUtf8(data->readLine().trimmed());
        if (line.isEmpty() || line.startsWith(QLatin1String("#"))) 
            continue;

        QStringList list = line.split(QLatin1Char(':'));
        if (list.count() != 2)
            return;

        list[0] = list[0].trimmed();
        list[1] = list[1].trimmed();

        if (list[0] == QLatin1String("gridLeft"))
            l = list[1].toInt();
        else if (list[0] == QLatin1String("gridRight"))
            r = list[1].toInt();
        else if (list[0] == QLatin1String("gridTop"))
            t = list[1].toInt();
        else if (list[0] == QLatin1String("gridBottom"))
            b = list[1].toInt();
        else if (list[0] == QLatin1String("imageFile"))
            imgFile = list[1];
    }

    if (l < 0 || r < 0 || t < 0 || b < 0 || imgFile.isEmpty())
        return;

    _l = l; _r = r; _t = t; _b = b;

    _pix = imgFile;
}

bool QFxGridScaledImage::isValid() const
{
    return _l >= 0;
}

int QFxGridScaledImage::gridLeft() const
{
    return _l;
}

int QFxGridScaledImage::gridRight() const
{
    return _r;
}

int QFxGridScaledImage::gridTop() const
{
    return _t;
}

int QFxGridScaledImage::gridBottom() const
{
    return _b;
}

QString QFxGridScaledImage::pixmapUrl() const
{
    return _pix;
}

QT_END_NAMESPACE
