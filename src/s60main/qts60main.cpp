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

// INCLUDE FILES
#include <eikstart.h>
#include "qts60mainapplication.h"

/**
 * factory function to create the QtS60Main application class
 */
LOCAL_C CApaApplication* NewApplication()
	{
	return new CQtS60MainApplication;
	}

/**
 * A normal Symbian OS executable provides an E32Main() function which is
 * called by the operating system to start the program.
 */
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

