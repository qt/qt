/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
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

#ifndef QBBVIRTUALKEYBOARDPPS_H_
#define QBBVIRTUALKEYBOARDPPS_H_

#include "qbbabstractvirtualkeyboard.h"

#include <sys/pps.h>

QT_BEGIN_NAMESPACE

class QSocketNotifier;

/* Shamelessly copied from the browser - this should be rewritten once we have a proper PPS wrapper class */
class QBBVirtualKeyboardPps : public QBBAbstractVirtualKeyboard
{
    Q_OBJECT
public:
    QBBVirtualKeyboardPps();
    ~QBBVirtualKeyboardPps();

    bool showKeyboard();
    bool hideKeyboard();

public Q_SLOTS:
    void start();

protected:
    void applyKeyboardMode(KeyboardMode mode);

private Q_SLOTS:
    void ppsDataReady();

private:
    pps_encoder_t          *mEncoder;
    pps_decoder_t          *mDecoder;
    char                   *mBuffer;
    int                     mFd;
    QSocketNotifier        *mReadNotifier;

    // Path to keyboardManager in PPS.
    static const char  *sPPSPath;
    static const size_t sBufferSize;

    // Will be called internally if needed.
    bool connect();
    void close();
    bool queryPPSInfo();
    void handleKeyboardInfoMessage();

    void applyKeyboardModeOptions(KeyboardMode mode);
    void addDefaultModeOptions();
    void addUrlModeOptions();
    void addEmailModeOptions();
    void addWebModeOptions();
    void addNumPuncModeOptions();
    void addSymbolModeOptions();
    void addPhoneModeOptions();
    void addPinModeOptions();
};

#endif /* QBBVIRTUALKEYBOARDPPS_H_ */
