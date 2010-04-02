/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#define QT_NO_KEYWORDS
#define signals int
#define slots int
#define emit public:;
#define foreach public:;
#define forever public:;

// include all of Qt here

// core Qt
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtNetwork/QtNetwork>

// extra
#include <QtDBus/QtDBus>
#include <QtDeclarative/QtDeclarative>
#include <QtHelp/QtHelp>
#include <QtMultimedia/QtMultimedia>
#include <QtOpenGL/QtOpenGL>
#include <QtScript/QtScript>
#include <QtScriptTools/QtScriptTools>
#include <QtSql/QtSql>
#include <QtSvg/QtSvg>
#include <QtTest/QtTest>
#include <QtXml/QtXml>
#include <QtXmlPatterns/QtXmlPatterns>

// webkit:
#include <QtWebKit/QtWebKit>

// designer:
#include <QtDesigner/QtDesigner>
#include <QtUiTools/QtUiTools>

// feature dependent:
#ifndef QT_NO_OPENVG
#include <QtOpenVG/QtOpenVG>
#endif

// removed in 4.7:
//#include <QtAssistant/QtAssistant>

// can't include this since it causes a linker error
//#include <Qt3Support/Qt3Support>
