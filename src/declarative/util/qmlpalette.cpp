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
#include "qmlpalette.h"
#include <QApplication>

QT_BEGIN_NAMESPACE

class QmlPalettePrivate : public QObjectPrivate
{
public:
    QPalette palette;
    QPalette::ColorGroup group;
};

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Palette,QmlPalette)

/*!
    \qmlclass Palette QmlPalette
    \ingroup group_utility
    \brief The Palette item gives access to the Qt palettes.
    \sa QPalette

    Example:
    \code
    Palette { id: MyPalette; colorGroup: Qt.Active }

    Rectangle {
        width: 640; height: 480
        color: MyPalette.window
        Text {
            anchors.fill: parent
            text: "Hello!"; color: MyPalette.windowText
        }
    }
    \endcode
*/
QmlPalette::QmlPalette(QObject *parent)
    : QObject(*(new QmlPalettePrivate), parent)
{
    Q_D(QmlPalette);
    d->palette = qApp->palette();
    d->group = QPalette::Active;
    qApp->installEventFilter(this);
}

QmlPalette::~QmlPalette()
{
}

/*!
    \qmlproperty color Palette::window
    The window (general background) color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::window() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Window);
}

/*!
    \qmlproperty color Palette::windowText
    The window text (general foreground) color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::windowText() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::WindowText);
}

/*!
    \qmlproperty color Palette::base
    The base color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::base() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Base);
}

/*!
    \qmlproperty color Palette::text
    The text color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::text() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Text);
}

/*!
    \qmlproperty color Palette::alternateBase
    The alternate base color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::alternateBase() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::AlternateBase);
}

/*!
    \qmlproperty color Palette::button
    The button color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::button() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Button);
}

/*!
    \qmlproperty color Palette::buttonText
    The button text foreground color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::buttonText() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::ButtonText);
}

/*!
    \qmlproperty color Palette::light
    The light color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::light() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Light);
}

/*!
    \qmlproperty color Palette::midlight
    The midlight color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::midlight() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Midlight);
}

/*!
    \qmlproperty color Palette::dark
    The dark color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::dark() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Dark);
}

/*!
    \qmlproperty color Palette::mid
    The mid color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::mid() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Mid);
}

/*!
    \qmlproperty color Palette::shadow
    The shadow color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::shadow() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Shadow);
}

/*!
    \qmlproperty color Palette::highlight
    The highlight color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::highlight() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::Highlight);
}

/*!
    \qmlproperty color Palette::highlightedText
    The highlighted text color of the current color group.

    \sa QPalette::ColorRole
*/
QColor QmlPalette::highlightedText() const
{
    Q_D(const QmlPalette);
    return d->palette.color(d->group, QPalette::HighlightedText);
}

/*!
    \qmlproperty color Palette::lighter
*/
QColor QmlPalette::lighter(const QColor& color) const
{
    return color.lighter();
}

/*!
    \qmlproperty color Palette::darker
*/
QColor QmlPalette::darker(const QColor& color) const
{
    return color.darker();
}

/*!
    \qmlproperty QPalette::ColorGroup Palette::colorGroup

    The color group of the palette. It can be Active, Inactive or Disabled.
    Active is the default.

    \sa QPalette::ColorGroup
*/
QPalette::ColorGroup QmlPalette::colorGroup() const
{
    Q_D(const QmlPalette);
    return d->group;
}

void QmlPalette::setColorGroup(QPalette::ColorGroup colorGroup)
{
    Q_D(QmlPalette);
    d->group = colorGroup;
    emit paletteChanged();
}

bool QmlPalette::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == qApp) {
        if (event->type() == QEvent::ApplicationPaletteChange) {
            QApplication::postEvent(this, new QEvent(QEvent::ApplicationPaletteChange));
            return false;
        }
    }
    return QObject::eventFilter(watched, event);
}

bool QmlPalette::event(QEvent *event)
{
    Q_D(QmlPalette);
    if (event->type() == QEvent::ApplicationPaletteChange) {
        d->palette = qApp->palette();
        emit paletteChanged();
        return true;
    }
    return QObject::event(event);
}

QT_END_NAMESPACE
