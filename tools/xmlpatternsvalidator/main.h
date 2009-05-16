/****************************************************************************
 **
 ** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 ** Contact: Qt Software Information (qt-info@nokia.com)
 **
 ** This file is part of the Patternist project on Trolltech Labs.  * **
 ** $TROLLTECH_DUAL_LICENSE$
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_main_h
#define Patternist_main_h

#include <QtCore/QCoreApplication>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QXmlPatternistCLI
{
public:
        Q_DECLARE_TR_FUNCTIONS(QXmlPatternistCLI)
private:
    inline QXmlPatternistCLI();
    Q_DISABLE_COPY(QXmlPatternistCLI)
};

QT_END_NAMESPACE

QT_END_HEADER 

#endif
