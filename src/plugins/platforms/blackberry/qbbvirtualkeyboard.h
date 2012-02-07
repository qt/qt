/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
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

#ifndef VIRTUALKEYBOARD_H_
#define VIRTUALKEYBOARD_H_

#include <QtGui/QPlatformScreen>
#include <QtGui/QWindowSystemInterface>
#include <QThread>

#include <stddef.h>
#include <vector>
#include <string>
#include <sys/pps.h>

QT_BEGIN_NAMESPACE

/* Shamelessly copied from the browser - this should be rewritten once we have a proper PPS wrapper class */
class QBBVirtualKeyboard : QThread
{
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

    static QBBVirtualKeyboard& instance();
    static void destroy();

    bool showKeyboard();
    bool hideKeyboard();
    int  getHeight() { return mVisible ? mHeight : 0; }
    void setKeyboardMode(KeyboardMode);
    void notifyClientActiveStateChange(bool);
    bool isVisible() const { return mVisible; }
    QString languageId() const { return mLanguageId; }
    QString countryId() const { return mCountryId; }

private:
    QBBVirtualKeyboard();
    virtual ~QBBVirtualKeyboard();

    pps_encoder_t          *mEncoder;
    pps_decoder_t          *mDecoder;
    char                   *mBuffer;
    int                     mHeight;
    int                     mFd;
    KeyboardMode            mKeyboardMode;
    bool                    mVisible;
    QString                 mLanguageId;
    QString                 mCountryId;

    // Path to keyboardManager in PPS.
    static const char  *sPPSPath;
    static const size_t sBufferSize;

    // Will be called internally if needed.
    bool connect();
    void close();
    void ppsDataReady();
    bool queryPPSInfo();
    void handleKeyboardStateChangeMessage(bool visible);
    void handleKeyboardInfoMessage();

    void applyKeyboardModeOptions();
    void addDefaultModeOptions();
    void addUrlModeOptions();
    void addEmailModeOptions();
    void addWebModeOptions();
    void addNumPuncModeOptions();
    void addSymbolModeOptions();
    void addPhoneModeOptions();
    void addPinModeOptions();

    // QThread overrides
    virtual void run();

};

#endif /* VIRTUALKEYBOARD_H_ */
