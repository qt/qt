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

//#define QBBVIRTUALKEYBOARD_DEBUG

#include "qbbvirtualkeyboardbps.h"

#include <QDebug>

#include <bps/event.h>
#include <bps/locale.h>
#include <bps/virtualkeyboard.h>

QT_BEGIN_NAMESPACE

QBBVirtualKeyboardBps::QBBVirtualKeyboardBps(QObject *parent)
    : QBBAbstractVirtualKeyboard(parent)
{
    if (locale_request_events(0) != BPS_SUCCESS)
        qWarning("QBB: Failed to register for locale events");

    if (virtualkeyboard_request_events(0) != BPS_SUCCESS)
        qWarning("QBB: Failed to register for virtual keyboard events");

    int height = 0;
    if (virtualkeyboard_get_height(&height) != BPS_SUCCESS)
        qWarning("QBB: Failed to get virtual keyboard height");

    setHeight(height);
}

bool QBBVirtualKeyboardBps::handleEvent(bps_event_t *event)
{
    const int eventDomain = bps_event_get_domain(event);
    if (eventDomain == locale_get_domain())
        return handleLocaleEvent(event);

    if (eventDomain == virtualkeyboard_get_domain())
        return handleVirtualKeyboardEvent(event);

    return false;
}

bool QBBVirtualKeyboardBps::showKeyboard()
{
#if defined(QBBVIRTUALKEYBOARD_DEBUG)
    qDebug() << Q_FUNC_INFO << "current visibility=" << isVisible();
#endif

    virtualkeyboard_show();
    return true;
}

bool QBBVirtualKeyboardBps::hideKeyboard()
{
#if defined(QBBVIRTUALKEYBOARD_DEBUG)
    qDebug() << Q_FUNC_INFO << "current visibility=" << isVisible();
#endif

    virtualkeyboard_hide();
    return true;
}

void QBBVirtualKeyboardBps::applyKeyboardMode(KeyboardMode mode)
{
    virtualkeyboard_layout_t layout = VIRTUALKEYBOARD_LAYOUT_DEFAULT;

    switch (mode) {
    case Url:
        layout = VIRTUALKEYBOARD_LAYOUT_URL;
        break;

    case Email:
        layout = VIRTUALKEYBOARD_LAYOUT_EMAIL;
        break;

    case Web:
        layout = VIRTUALKEYBOARD_LAYOUT_WEB;
        break;

    case NumPunc:
        layout = VIRTUALKEYBOARD_LAYOUT_NUM_PUNC;
        break;

    case Symbol:
        layout = VIRTUALKEYBOARD_LAYOUT_SYMBOL;
        break;

    case Phone:
        layout = VIRTUALKEYBOARD_LAYOUT_PHONE;
        break;

    case Pin:
        layout = VIRTUALKEYBOARD_LAYOUT_PIN;
        break;

    case Default: // fall through
    default:
        layout = VIRTUALKEYBOARD_LAYOUT_DEFAULT;
        break;
    }

#if defined(QBBVIRTUALKEYBOARD_DEBUG)
    qDebug() << Q_FUNC_INFO << "mode=" << mode;
#endif

    virtualkeyboard_change_options(layout, VIRTUALKEYBOARD_ENTER_DEFAULT);
}

bool QBBVirtualKeyboardBps::handleLocaleEvent(bps_event_t *event)
{
    if (bps_event_get_code(event) == LOCALE_INFO) {
        const QString language = QString::fromAscii(locale_event_get_language(event));
        const QString country  = QString::fromAscii(locale_event_get_country(event));

#if defined(QBBVIRTUALKEYBOARD_DEBUG)
        qDebug() << Q_FUNC_INFO << "current language/country" << languageId() << "/" << countryId()
                 << "new language/country=" << language << "/" << country;
#endif
        setLanguage(language);
        setCountry(country);
        return true;
    }

#if defined(QBBVIRTUALKEYBOARD_DEBUG)
    qDebug() << "QBB: Unhandled locale event. code=" << bps_event_get_code(event);
#endif

    return false;
}

bool QBBVirtualKeyboardBps::handleVirtualKeyboardEvent(bps_event_t *event)
{
    switch (bps_event_get_code(event)) {
    case VIRTUALKEYBOARD_EVENT_VISIBLE:
        #if defined(QBBVIRTUALKEYBOARD_DEBUG)
        qDebug() << Q_FUNC_INFO << "EVENT VISIBLE: current visibility=" << isVisible();
        #endif

        setVisible(true);
        break;

    case VIRTUALKEYBOARD_EVENT_HIDDEN:
        #if defined(QBBVIRTUALKEYBOARD_DEBUG)
        qDebug() << Q_FUNC_INFO << "EVENT HIDDEN: current visibility=" << isVisible();
        #endif

        setVisible(false);
        break;

    case VIRTUALKEYBOARD_EVENT_INFO: {
        const int newHeight = virtualkeyboard_event_get_height(event);

        #if defined(QBBVIRTUALKEYBOARD_DEBUG)
        qDebug() << Q_FUNC_INFO << "EVENT INFO: current height=" << getHeight() << "new height=" << newHeight;
        #endif

        setHeight(newHeight);
        break;
    }

    default:
        #if defined(QBBVIRTUALKEYBOARD_DEBUG)
        qDebug() << "QBB: Unhandled virtual keyboard event. code=" << bps_event_get_code(event);
        #endif

        return false;
    }

    return true;
}

QT_END_NAMESPACE
