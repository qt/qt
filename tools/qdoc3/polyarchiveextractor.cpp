/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*
  polyarchiveextractor.cpp
*/

#include "command.h"
#include "polyarchiveextractor.h"

QT_BEGIN_NAMESPACE

/*!
  \class PolyArchiveExtractor
  
  \brief The PolyArchiveExtractor class is a class for unpacking
  archive files.

  This subclass of ArchiveExtractor contains a parameterized
  command for doing the archive extraction.

  It has an extractArchive() function you call to do the
  actual archive extraction.
 */

/*!
  The constructor takes the list of filename \a extensions,
  which it passes to the base class, and the \a commandFormat,
  which it stores locally. The \a commandFormat is a command
  template string.
 */
PolyArchiveExtractor::PolyArchiveExtractor( const QStringList& extensions,
					    const QString& commandFormat )
    : ArchiveExtractor( extensions ), cmd( commandFormat )
{
}

/*!
  The destructor doesn't have to do anything.
 */
PolyArchiveExtractor::~PolyArchiveExtractor()
{
}

/*!
  Call this function to do the actual archive extraction. It calls
  the executeCommand() function to do the work. That's all it does.
 */
void PolyArchiveExtractor::extractArchive( const Location& location,
					   const QString& filePath,
					   const QString& outputDir )
{
    executeCommand( location, cmd, QStringList() << filePath << outputDir );
}

QT_END_NAMESPACE
