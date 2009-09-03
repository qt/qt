/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#ifndef QS60MAINDOCUMENT_P_H
#define QS60MAINDOCUMENT_P_H

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
#include <akndoc.h>

#include <qglobal.h>

class CEikApplication;

QT_BEGIN_NAMESPACE

// FORWARD DECLARATIONS
class QS60MainAppUi;

// CLASS DECLARATION

/**
* QS60MainDocument application class.
* An instance of class QS60MainDocument is the Document part of the
* AVKON application framework for the QtS60Main application.
*/
class QS60MainDocument : public CAknDocument
{
public: // Constructors and destructor

    /**
     * NewL.
     * Two-phased constructor.
     * Construct a QS60MainDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of QS60MainDocument.
     */
    static QS60MainDocument* NewL( CEikApplication& aApp );

    /**
     * NewLC.
     * Two-phased constructor.
     * Construct a QS60MainDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of QS60MainDocument.
     */
    static QS60MainDocument* NewLC( CEikApplication& aApp );

    /**
     * ~QS60MainDocument
     * Virtual Destructor.
     */
    virtual ~QS60MainDocument();

public: // Functions from base classes

    /**
     * CreateAppUiL
     * From CEikDocument, CreateAppUiL.
     * Create a QS60MainAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();

private: // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * QS60MainDocument.
     * C++ default constructor.
     * @param aApp Application creating this document.
     */
    QS60MainDocument( CEikApplication& aApp );

};

QT_END_NAMESPACE

#endif // QS60MAINDOCUMENT_P_H

// End of File
