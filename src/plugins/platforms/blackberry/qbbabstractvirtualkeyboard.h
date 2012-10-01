/****************************************************************************
**
** Copyright (C) 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBBABSTRACTVIRTUALKEYBOARD_H
#define QBBABSTRACTVIRTUALKEYBOARD_H

#include <QObject>

QT_BEGIN_NAMESPACE

class QBBAbstractVirtualKeyboard : public QObject
{
    Q_OBJECT
public:
    // NOTE:  Not all the following keyboard modes are currently used.
    // Default - Regular Keyboard
    // Url/Email - Enhanced keys for each types.
    // Web - Regular keyboard with two blank keys, currently unused.
    // NumPunc - Numbers & Punctionation, alternate to Symbol
    // Symbol - All symbols, alternate to NumPunc, currently unused.
    // Phone - Phone enhanced keyboard - currently unused as no alternate keyboard available to access a-zA-Z
    // Pin - Keyboard for entering Pins (Hex values) currently unused.
    //
    // SPECIAL NOTE: Usage of NumPunc may have to be removed, ABC button is non-functional.
    //
    enum KeyboardMode { Default, Url, Email, Web, NumPunc, Symbol, Phone, Pin };

    explicit QBBAbstractVirtualKeyboard(QObject *parent = 0);

    virtual bool showKeyboard() = 0;
    virtual bool hideKeyboard() = 0;

    int  getHeight() const { return mVisible ? mHeight : 0; }

    void setKeyboardMode(KeyboardMode mode);
    void setInputHintsFromWidget(QWidget *focusWidget);
    KeyboardMode keyboardMode() const { return mKeyboardMode; }

    bool isVisible() const { return mVisible; }
    QString languageId() const { return mLanguageId; }
    QString countryId() const { return mCountryId; }

Q_SIGNALS:
    void heightChanged(int height);

protected:
    virtual void applyKeyboardMode(KeyboardMode mode) = 0;

    void setVisible(bool visible);
    void setHeight(int height);
    void setLanguage(const QString &language);
    void setCountry(const QString &country);

private:
    int          mHeight;
    KeyboardMode mKeyboardMode;
    bool         mVisible;
    QString      mLanguageId;
    QString      mCountryId;
};

QT_END_NAMESPACE

#endif // QBBABSTRACTVIRTUALKEYBOARD_H
