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
    property int index_size_err: DOMException.INDEX_SIZE_ERR
    property int domstring_size_err: DOMException.DOMSTRING_SIZE_ERR
    property int hierarchy_request_err: DOMException.HIERARCHY_REQUEST_ERR
    property int wrong_document_err: DOMException.WRONG_DOCUMENT_ERR
    property int invalid_character_err: DOMException.INVALID_CHARACTER_ERR
    property int no_data_allowed_err: DOMException.NO_DATA_ALLOWED_ERR
    property int no_modification_allowed_err: DOMException.NO_MODIFICATION_ALLOWED_ERR
    property int not_found_err: DOMException.NOT_FOUND_ERR
    property int not_supported_err: DOMException.NOT_SUPPORTED_ERR
    property int inuse_attribute_err: DOMException.INUSE_ATTRIBUTE_ERR
    property int invalid_state_err: DOMException.INVALID_STATE_ERR
    property int syntax_err: DOMException.SYNTAX_ERR
    property int invalid_modification_err: DOMException.INVALID_MODIFICATION_ERR
    property int namespace_err: DOMException.NAMESPACE_ERR
    property int invalid_access_err: DOMException.INVALID_ACCESS_ERR
    property int validation_err: DOMException.VALIDATION_ERR
    property int type_mismatch_err: DOMException.TYPE_MISMATCH_ERR

    Component.onCompleted: {
        // Attempt to overwrite and delete values
        DOMException.INDEX_SIZE_ERR = 44;
        DOMException.DOMSTRING_SIZE_ERR = 44;
        DOMException.HIERARCHY_REQUEST_ERR = 44;
        DOMException.WRONG_DOCUMENT_ERR = 44;
        DOMException.INVALID_CHARACTER_ERR = 44;
        DOMException.NO_DATA_ALLOWED_ERR = 44;
        DOMException.NO_MODIFICATION_ALLOWED_ERR = 44;
        DOMException.NOT_FOUND_ERR = 44;
        DOMException.NOT_SUPPORTED_ERR = 44;
        DOMException.INUSE_ATTRIBUTE_ERR = 44;
        DOMException.INVALID_STATE_ERR = 44;
        DOMException.SYNTAX_ERR = 44;
        DOMException.INVALID_MODIFICATION_ERR = 44;
        DOMException.NAMESPACE_ERR = 44;
        DOMException.INVALID_ACCESS_ERR = 44;
        DOMException.VALIDATION_ERR = 44;
        DOMException.TYPE_MISMATCH_ERR = 44;

        delete DOMException.INDEX_SIZE_ERR;
        delete DOMException.DOMSTRING_SIZE_ERR;
        delete DOMException.HIERARCHY_REQUEST_ERR;
        delete DOMException.WRONG_DOCUMENT_ERR;
        delete DOMException.INVALID_CHARACTER_ERR;
        delete DOMException.NO_DATA_ALLOWED_ERR;
        delete DOMException.NO_MODIFICATION_ALLOWED_ERR;
        delete DOMException.NOT_FOUND_ERR;
        delete DOMException.NOT_SUPPORTED_ERR;
        delete DOMException.INUSE_ATTRIBUTE_ERR;
        delete DOMException.INVALID_STATE_ERR;
        delete DOMException.SYNTAX_ERR;
        delete DOMException.INVALID_MODIFICATION_ERR;
        delete DOMException.NAMESPACE_ERR;
        delete DOMException.INVALID_ACCESS_ERR;
        delete DOMException.VALIDATION_ERR;
        delete DOMException.TYPE_MISMATCH_ERR;
    }
}
