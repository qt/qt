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
    property string url
    property string expectedStatus

    property bool unsentException: false;
    property bool openedException: false;
    property bool sentException: false;

    property bool headersReceived: false
    property bool loading: false
    property bool done: false

    property bool resetException: false

    property bool dataOK: false

    Component.onCompleted: {
        var x = new XMLHttpRequest;

	try {
	    var a = x.statusText;
	} catch (e) {
	    if (e.code == DOMException.INVALID_STATE_ERR)
		unsentException = true;
	}

        x.open("GET", url);
        x.setRequestHeader("Accept-Language", "en-US");

	try {
	    var a = x.statusText;
	} catch (e) {
	    if (e.code == DOMException.INVALID_STATE_ERR)
		openedException = true;
	}

        // Test to the end
        x.onreadystatechange = function() {
            if (x.readyState == XMLHttpRequest.HEADERS_RECEIVED) {
                if (x.statusText == expectedStatus)
                    headersReceived = true;
            } else if (x.readyState == XMLHttpRequest.LOADING) {
                if (x.statusText == expectedStatus)
                    loading = true;
            } else if (x.readyState == XMLHttpRequest.DONE) {
                if (x.statusText == expectedStatus)
                    done = true;

                if (expectedStatus != "OK") {
                    dataOK = (x.responseText == "");
                } else {
                    dataOK = (x.responseText == "QML Rocks!\n");
                }

                x.open("GET", url);
                x.setRequestHeader("Accept-Language", "en-US");

                try {
                    var a = x.statusText;
                } catch (e) {
                    if (e.code == DOMException.INVALID_STATE_ERR)
                        resetException = true;
                }

            }
        }

        x.send()

	try {
	    var a = x.statusText;
	} catch (e) {
	    if (e.code == DOMException.INVALID_STATE_ERR)
		sentException = true;
	}
    }
}
