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
  command.cpp
*/

#include <QProcess>

#include "command.h"

#include <stdlib.h>

QT_BEGIN_NAMESPACE

void executeCommand(const Location& location,
                    const QString& format,
                    const QStringList& args)
{
    QString actualCommand;
    for (int i = 0; i < (int) format.length(); i++) {
	int ch = format[i].unicode();
	if (ch > 0 && ch < 8) {
	    actualCommand += args[ch - 1];
	}
        else {
	    actualCommand += format[i];
	}
    }

    QString toolName = actualCommand;
    int space = toolName.indexOf(QLatin1Char(' '));
    if (space != -1)
	toolName.truncate(space);

#ifdef QT_BOOTSTRAPPED
    int status = system(qPrintable(actualCommand));
    int exitCode = WEXITSTATUS(status);
    if (status == -1 || exitCode != EXIT_SUCCESS)
        location.fatal(QString("Error executing '$1': $2").arg(toolName).arg(exitCode));
#else
    QProcess process;
    process.start(QLatin1String("sh"),
        QStringList() << QLatin1String("-c") << actualCommand);
    process.waitForFinished();

    if (process.exitCode() == 127)
	location.fatal(tr("Couldn't launch the '%1' tool")
                       .arg(toolName),
                       tr("Make sure the tool is installed and in the"
                          " path."));

    QString errors = QString::fromLocal8Bit(process.readAllStandardError());
    while (errors.endsWith(QLatin1Char('\n')))
        errors.truncate(errors.length() - 1);
    if (!errors.isEmpty())
	location.fatal(tr("The '%1' tool encountered some problems")
                       .arg(toolName),
                       tr("The tool was invoked like this:\n%1\n"
                          "It emitted these errors:\n%2")
                       .arg(actualCommand).arg(errors));
#endif
}

QT_END_NAMESPACE
