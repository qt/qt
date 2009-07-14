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

#ifndef QMLPALETTE_H
#define QMLPALETTE_H

#include <QtCore/qobject.h>
#include <QtDeclarative/qml.h>
#include <QPalette>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlPalettePrivate;
class Q_DECLARATIVE_EXPORT QmlPalette : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlPalette)

public:
    QmlPalette(QObject *parent=0);
    ~QmlPalette();

    Q_PROPERTY(QColor window READ window CONSTANT)
    Q_PROPERTY(QColor windowText READ windowText CONSTANT)
    Q_PROPERTY(QColor base READ base CONSTANT)
    Q_PROPERTY(QColor alternateBase READ alternateBase CONSTANT)
    Q_PROPERTY(QColor button READ button CONSTANT)
    Q_PROPERTY(QColor buttonText READ buttonText CONSTANT)
    Q_PROPERTY(QColor light READ light CONSTANT)
    Q_PROPERTY(QColor midlight READ midlight CONSTANT)
    Q_PROPERTY(QColor dark READ dark CONSTANT)
    Q_PROPERTY(QColor mid READ mid CONSTANT)
    Q_PROPERTY(QColor shadow READ shadow CONSTANT)
    Q_PROPERTY(QColor highlight READ highlight CONSTANT)
    Q_PROPERTY(QColor highlightedText READ highlightedText CONSTANT)

    QColor window() const;
    QColor windowText() const;

    QColor base() const;
    QColor alternateBase() const;

    QColor button() const;
    QColor buttonText() const;

    QColor light() const;
    QColor midlight() const;
    QColor dark() const;
    QColor mid() const;
    QColor shadow() const;

    QColor highlight() const;
    QColor highlightedText() const;

    QPalette palette() const;

    void setColorGroup(QPalette::ColorGroup);

Q_SIGNALS:
    void updated();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlPalette)

QT_END_HEADER

#endif // QMLPALETTE_H
