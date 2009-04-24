/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#ifndef __QTS60MAINDOCUMENT_H__
#define __QTS60MAINDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CQtS60MainAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CQtS60MainDocument application class.
* An instance of class CQtS60MainDocument is the Document part of the
* AVKON application framework for the QtS60Main application.
*/
class CQtS60MainDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CQtS60MainDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CQtS60MainDocument.
        */
        static CQtS60MainDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CQtS60MainDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CQtS60MainDocument.
        */
        static CQtS60MainDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CQtS60MainDocument
        * Virtual Destructor.
        */
        virtual ~CQtS60MainDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CQtS60MainAppUi object and return a pointer to it.
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
        * CQtS60MainDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CQtS60MainDocument( CEikApplication& aApp );

    };

#endif // __QTS60MAINDOCUMENT_H__

// End of File

