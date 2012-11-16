/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef PROJECTPORTER_H
#define PROJECTPORTER_H

#include "fileporter.h"
#include "preprocessorcontrol.h"
#include <QString>
#include <QSet>

QT_BEGIN_NAMESPACE

class ProjectPorter : public QObject
{
Q_OBJECT
public:
    ProjectPorter(QString basePath, QStringList includeDirectories, QStringList qt3HeadersFilenames = QStringList());
    void enableCppParsing(bool enable);
    void enableMissingFilesWarnings(bool enable);
    void portProject(QString filePath);
    void portFile(QString filePath);
private slots:
    void error(QString type, QString text);
private:
    void portProject(QString inPath, QString proFileName);
    void portProFile(QString fileName, QMap<QString, QString> tagMap);
    void portFiles(QString basePath, QStringList fileNames);
    void enableAttributes(const IncludeFiles &includeFiles, const QString &fileName);

    QSet<QString> processedFilesSet;
    QString basePath;
    QStringList includeDirectories;
    PreprocessorCache preprocessorCache;
    Rpp::DefineMap *defaultDefinitions;
    FilePorter filePorter;
    QStringList qt3HeadersFilenames;
    bool analyze;
    bool warnings;
};

QT_END_NAMESPACE

#endif
