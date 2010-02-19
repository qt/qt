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

// This is just a dummy file to include the documentation

/*!
    \qmlclass Script QmlScript
    \brief The Script element provides a way to add JavaScript code snippets in QML.
    \ingroup group_utility

    The Script element is used to add convenient JavaScript "glue" methods to
    your Qt Declarative application or component. 

    An example: 

    \qml
    Script {
        function debugMyComponent() {
            console.log(text.text);
            console.log(otherinterestingitem.property);
        }
    }
    MouseRegion { onClicked: debugMyComponent() }
    \endqml

    \note While it is possible to use any JavaScript code within a Script element,
    it is recommended that the code be limited to defining functions. The Script
    element executes JavaScript as soon as it is specified, so
    when defining a component, this may be done before the execution context is
    fully specified.  As a result, some properties or items may not be
    accessible. You can avoid this problem by limiting your JavaScript to
    defining functions that are only executed later once the context is fully
    defined.

    \sa {JavaScript Blocks}
*/

/*!
    \qmlproperty string Script::script
    \default
    The JavaScript code to be executed.
*/

/*!
    \qmlproperty url Script::source

    Specifies a source file containing JavaScript code. This can be used instead
    of providing inline JavaScript code in the Script element.
*/
