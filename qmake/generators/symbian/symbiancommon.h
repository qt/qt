/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef SYMBIANCOMMON_H
#define SYMBIANCOMMON_H

#include <project.h>
#include <makefile.h>
#include "initprojectdeploy_symbian.h"

#define PRINT_FILE_CREATE_ERROR(filename) fprintf(stderr, "Error: Could not create '%s'\n", qPrintable(filename));

class SymbianCommonGenerator
{
public:
    enum TargetType {
        TypeExe,
        TypeDll,
        TypeLib,
        TypePlugin,
        TypeSubdirs
    };

    SymbianCommonGenerator(MakefileGenerator *generator);

    virtual void init();

protected:

    QString removePathSeparators(QString &file);
    void removeSpecialCharacters(QString& str);
    void removeEpocSpecialCharacters(QString& str);
    void generatePkgFile(const QString &iconFile, bool epocBuild);
    bool containsStartWithItem(const QChar &c, const QStringList& src);

    void writeRegRssFile(QMap<QString, QStringList> &useritems);
    void writeRegRssList(QTextStream &t, QStringList &userList,
                         const QString &listTag,
                         const QString &listItem);
    void writeRssFile(QString &numberOfIcons, QString &iconfile);
    void writeLocFile(QStringList &symbianLangCodes);
    void readRssRules(QString &numberOfIcons,
                      QString &iconFile,
                      QMap<QString, QStringList> &userRssRules);

    void writeCustomDefFile();

    QStringList symbianLangCodesFromTsFiles();
    void fillQt2S60LangMapTable();

protected:
    MakefileGenerator *generator;

    QStringList generatedFiles;
    QStringList generatedDirs;
    QString fixedTarget;
    QString privateDirUid;
    QString uid3;
    TargetType targetType;

    QHash<QString, QString> qt2S60LangMapTable;
};

#endif // SYMBIANCOMMON_H
