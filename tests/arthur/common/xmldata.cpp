/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "xmldata.h"


bool XMLReader::startElement(const QString &, const QString &localName,
                             const QString &, const QXmlAttributes &attributes)
{
    if (localName == "arthur" ) {
        QString engineName   = attributes.value("engine");
        QString defaultStr   = attributes.value("default");
        QString foreignStr   = attributes.value("foreign");
        QString referenceStr = attributes.value("reference");
        QString genDate      = attributes.value("generationDate");
        engine = new XMLEngine(engineName, defaultStr == "true");
        engine->foreignEngine   = (foreignStr == "true");
        engine->referenceEngine = (referenceStr == "true");
        if (!genDate.isEmpty())
            engine->generationDate  = QDateTime::fromString(genDate);
        else
            engine->generationDate  = QDateTime::currentDateTime();
    } else if (localName == "suite") {
        QString suiteName = attributes.value("dir");
        suite = new XMLSuite(suiteName);
    } else if (localName == "file") {
        QString testName = attributes.value("name");
        QString outputName = attributes.value("output");
        file = new XMLFile(testName, outputName);
    } else if (localName == "data") {
        QString dateStr = attributes.value("date");
        QString timeStr = attributes.value("time_to_render");
        QString itrStr = attributes.value("iterations");
        QString detailsStr = attributes.value("details");
        QString maxElapsedStr = attributes.value("maxElapsed");
        QString minElapsedStr = attributes.value("minElapsed");
        XMLData data(dateStr, timeStr.toInt(),
                     (!itrStr.isEmpty())?itrStr.toInt():1);
        data.details = detailsStr;
        if (maxElapsedStr.isEmpty())
            data.maxElapsed = data.timeToRender;
        else
            data.maxElapsed = maxElapsedStr.toInt();
        if (minElapsedStr.isEmpty())
            data.minElapsed = data.timeToRender;
        else
            data.minElapsed = minElapsedStr.toInt();

        file->data.append(data);
    } else {
        qDebug()<<"Error while parsing element :"<<localName;
        return false;
    }
    return true;
}

bool XMLReader::endElement(const QString &, const QString &localName,
                           const QString &)
{
    if (localName == "arthur" ) {
        //qDebug()<<"done";
    } else if (localName == "suite") {
        engine->suites.insert(suite->name, suite);
    } else if (localName == "file") {
        suite->files.insert(file->name, file);
    }
    return true;
}

bool XMLReader::fatalError(const QXmlParseException &)
{
    return true;
}
