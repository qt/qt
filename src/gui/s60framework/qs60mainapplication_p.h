/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
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

#ifndef QS60MAINAPPLICATION_P_H
#define QS60MAINAPPLICATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

// INCLUDES
#include <aknapp.h>

#include <qglobal.h>

// CLASS DECLARATION

QT_BEGIN_NAMESPACE

CApaApplication* NewApplication();

static TUid ProcessUid()
{
    RProcess me;
    TSecureId securId = me.SecureId();
    me.Close();
    return securId.operator TUid();
}

/**
* QS60MainApplication application class.
* Provides factory to create concrete document object.
* An instance of QS60MainApplication is the application part of the
* AVKON application framework for the QtS60Main example application.
*/
class QS60MainApplication : public CAknApplication
{
public: // Functions from base classes

    /**
     * From CApaApplication, AppDllUid.
     * @return Application's UID (KUidQtS60MainApp).
     */
    TUid AppDllUid() const;

    /**
     * From CApaApplication, ResourceFileName
     * @return Application's resource filename (KUidQtS60MainApp).
     */
    TFileName ResourceFileName() const;

protected: // Functions from base classes

    /**
     * From CApaApplication, CreateDocumentL.
     * Creates QS60MainDocument document object. The returned
     * pointer in not owned by the QS60MainApplication object.
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();
};

QT_END_NAMESPACE

#endif // QS60MAINAPPLICATION_P_H

// End of File
