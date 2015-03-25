/****************************************************************************
**
** Copyright (C) 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qbbabstractvirtualkeyboard.h"

#include <QWidget>

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

void QBBAbstractVirtualKeyboard::setInputHintsFromWidget(QWidget *focusWidget)
{
    if (focusWidget) {
        const Qt::InputMethodHints hints = focusWidget->inputMethodHints();
        if (hints & Qt::ImhEmailCharactersOnly)
            setKeyboardMode(QBBAbstractVirtualKeyboard::Email);
        else if (hints & Qt::ImhDialableCharactersOnly)
            setKeyboardMode(QBBAbstractVirtualKeyboard::Phone);
        else if (hints & Qt::ImhUrlCharactersOnly)
            setKeyboardMode(QBBAbstractVirtualKeyboard::Web);
        else if (hints & Qt::ImhFormattedNumbersOnly || hints & Qt::ImhDigitsOnly)
            setKeyboardMode(QBBAbstractVirtualKeyboard::NumPunc);
        else
            setKeyboardMode(QBBAbstractVirtualKeyboard::Default);
    } else {
        setKeyboardMode(QBBAbstractVirtualKeyboard::Default);
    }
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

void QBBAbstractVirtualKeyboard::setHeight(int height)
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
