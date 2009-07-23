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

#include "private/qobject_p.h"
#include "qfont.h"
#include "qmlfont.h"

QT_BEGIN_NAMESPACE

class QmlFontPrivate : public QObjectPrivate
{
public:
    QFont font;
};

QML_DEFINE_TYPE(Qt,4.6,Font,QmlFont)

/*!
    \internal
    \class QmlFont
    \ingroup group_utility
    \brief The QmlFont class provides a font used for drawing text on a QFxView.
*/
QmlFont::QmlFont(QObject *parent)
    : QObject(*(new QmlFontPrivate), parent)
{
}

QmlFont::~QmlFont()
{
}

/*!
    \property QmlFont::family
    \brief the family of the font.
*/
QString QmlFont::family() const
{
    Q_D(const QmlFont);
    return d->font.family();
}

void QmlFont::setFamily(const QString &family)
{
    Q_D(QmlFont);
    d->font.setFamily(family);
    emit updated();
}

/*!
    \property QmlFont::bold
    \brief whether the font should be bold.
*/
bool QmlFont::bold() const
{
    Q_D(const QmlFont);
    return d->font.bold();
}

void QmlFont::setBold(bool b)
{
    Q_D(QmlFont);
    d->font.setBold(b);
    emit updated();
}

/*!
    \property QmlFont::italic
    \brief whether the font should be italic.
*/
bool QmlFont::italic() const
{
    Q_D(const QmlFont);
    return d->font.italic();
}

void QmlFont::setItalic(bool b)
{
    Q_D(QmlFont);
    d->font.setItalic(b);
    emit updated();
}

/*!
    \property QmlFont::size
    \brief the size of the font in points.
*/
qreal QmlFont::size() const
{
    Q_D(const QmlFont);
    return d->font.pointSizeF();
}

void QmlFont::setSize(qreal size)
{
    Q_D(QmlFont);
    d->font.setPointSizeF(size);
    emit updated();
}

/*!
    \brief Returns a QFont representation of the font.
*/
QFont QmlFont::font() const
{
    Q_D(const QmlFont);
    return d->font;
}

QT_END_NAMESPACE
