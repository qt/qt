/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7 

QtObject {
    property bool onreadystatechange: false
    property bool readyState: false
    property bool status: false
    property bool statusText: false
    property bool responseText: false
    property bool responseXML: false

    property bool open: false
    property bool setRequestHeader: false
    property bool send: false
    property bool abort: false
    property bool getResponseHeader: false
    property bool getAllResponseHeaders: false

    Component.onCompleted: {
        var o = 10;

        try {
            XMLHttpRequest.prototype.onreadystatechange
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            onreadystatechange = true;
        }
        try {
            XMLHttpRequest.prototype.readyState
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            readyState = true;
        }
        try {
            XMLHttpRequest.prototype.status
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            status = true;
        }
        try {
            XMLHttpRequest.prototype.statusText
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            statusText = true;
        }
        try {
            XMLHttpRequest.prototype.responseText
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            responseText = true;
        }
        try {
            XMLHttpRequest.prototype.responseXML
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            responseXML = true;
        }

        try {
            XMLHttpRequest.prototype.open.call(o);
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            open = true;
        }

        try {
            XMLHttpRequest.prototype.setRequestHeader.call(o);
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            setRequestHeader = true;
        }

        try {
            XMLHttpRequest.prototype.send.call(o);
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            send = true;
        }

        try {
            XMLHttpRequest.prototype.abort.call(o);
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            abort = true;
        }

        try {
            XMLHttpRequest.prototype.getResponseHeader.call(o);
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            getResponseHeader = true;
        }

        try {
            XMLHttpRequest.prototype.getAllResponseHeaders.call(o);
        } catch (e) {
            if (!(e instanceof ReferenceError))
                return;

            if (e.message != "Not an XMLHttpRequest object")
                return;

            getAllResponseHeaders = true;
        }
    }
}
