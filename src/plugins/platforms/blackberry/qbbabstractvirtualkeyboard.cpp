/****************************************************************************
**
** Copyright (C) 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qbbabstractvirtualkeyboard.h"

QT_BEGIN_NAMESPACE

QBBAbstractVirtualKeyboard::QBBAbstractVirtualKeyboard(QObject *parent)
    : QObject(parent)
    , mHeight(0)
    , mKeyboardMode(Default)
    , mVisible(false)
    , mLanguageId(QString::fromLatin1("en"))
    , mCountryId(QString::fromLatin1("US"))
{
}

void QBBAbstractVirtualKeyboard::setKeyboardMode(KeyboardMode mode)
{
    if (mode == mKeyboardMode)
        return;

    mKeyboardMode = mode;

    applyKeyboardMode(mode);
}

void QBBAbstractVirtualKeyboard::setVisible(bool visible)
{
    if (visible == mVisible)
        return;

    const int effectiveHeight = getHeight();

    mVisible = visible;

    if (effectiveHeight != getHeight())
        emit heightChanged(getHeight());
}

void QBBAbstractVirtualKeyboard::setHeight(bool height)
{
    if (height == mHeight)
        return;

    const int effectiveHeight = getHeight();

    mHeight = height;

    if (effectiveHeight != getHeight())
        emit heightChanged(getHeight());
}

void QBBAbstractVirtualKeyboard::setLanguage(const QString &language)
{
    if (language == mLanguageId)
        return;

    mLanguageId = language;
}

void QBBAbstractVirtualKeyboard::setCountry(const QString &country)
{
    if (country == mCountryId)
        return;

    mCountryId = country;
}

QT_END_NAMESPACE
