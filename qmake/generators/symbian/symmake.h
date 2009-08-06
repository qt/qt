/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SYMMAKEFILE_H
#define SYMMAKEFILE_H

#include <makefile.h>

QT_BEGIN_NAMESPACE

#define BLD_INF_FILENAME "bld.inf"
#define MAKEFILE_DEPENDENCY_SEPARATOR " \\\n\t"

#define QT_EXTRA_INCLUDE_DIR "tmp"

class SymbianMakefileGenerator : public MakefileGenerator
{
protected:
    enum TargetType {
        TypeExe,
        TypeDll,
        TypeLib,
        TypePlugin,
        TypeSubdirs
    };

    QString platform;
    QString uid2;
    QString uid3;
    QString privateDirUid;
    TargetType targetType;
    QMap<QString, QStringList> sources;
    QMap<QString, QStringList> systeminclude;
    QMap<QString, QStringList> library;
    // (output file) (source , command)
    QMap<QString, QStringList> makmakeCommands;

    QStringList generatedFiles;
    QStringList generatedDirs;
    QHash<QString, QString> qt2S60LangMapTable;

    QString fixedTarget;

    void removeSpecialCharacters(QString& str);
    QString fixPathForMmp(const QString& origPath, const QDir& parentDir);
    QString canonizePath(const QString& origPath);

    virtual bool writeMakefile(QTextStream &t);
    void generatePkgFile(const QString &compiler, const QString &config, const QString &iconFile);
    bool containsStartWithItem(const QChar &c, const QStringList& src);

    virtual void init();

    QString getTargetExtension();

    QString generateUID3();

    void initMmpVariables();

    void writeHeader(QTextStream &t);
    void writeBldInfContent(QTextStream& t, bool addDeploymentExtension);

    static bool removeDuplicatedStrings(QStringList& stringList);

    void writeMmpFileHeader(QTextStream &t);
    void writeMmpFile(QString &filename, QStringList &symbianLangCodes);
    void writeMmpFileMacrosPart(QTextStream& t);
    void addMacro(QTextStream& t, const QString& value);
    void writeMmpFileTargetPart(QTextStream& t);
    void writeMmpFileResourcePart(QTextStream& t, QStringList &symbianLangCodes);
    void writeMmpFileSystemIncludePart(QTextStream& t);
    void writeMmpFileIncludePart(QTextStream& t);
    void writeMmpFileLibraryPart(QTextStream& t);
    void writeMmpFileCapabilityPart(QTextStream& t);
    void writeMmpFileCompilerOptionPart(QTextStream& t);
    void writeMmpFileBinaryVersionPart(QTextStream& t);
    void writeMmpFileRulesPart(QTextStream& t);

    void writeCustomDefFile();

    void writeRegRssFile(QString &appname, QStringList &useritems);
    void writeRssFile(QString &appName, QString &numberOfIcons, QString &iconfile);
    void writeLocFile(QString &appName, QStringList &symbianLangCodes);
    void readRssRules(QString &numberOfIcons, QString &iconFile, QStringList &userRssRules);

    QStringList symbianLangCodesFromTsFiles();
    void fillQt2S60LangMapTable();

    void appendIfnotExist(QStringList &list, QString value);
    void appendIfnotExist(QStringList &list, QStringList values);

    QString removePathSeparators(QString &file);
    QString removeTrailingPathSeparators(QString &file);
    void generateCleanCommands(QTextStream& t,
                               const QStringList& toClean,
                               const QString& cmd,
                               const QString& cmdOptions,
                               const QString& itemPrefix,
                               const QString& itemSuffix);

    void generateDistcleanTargets(QTextStream& t);

    // Subclass implements
    virtual void writeBldInfExtensionRulesPart(QTextStream& t) = 0;
    virtual void writeBldInfMkFilePart(QTextStream& t, bool addDeploymentExtension) = 0;
    virtual void writeMkFile(const QString& wrapperFileName, bool deploymentOnly) = 0;
    virtual void writeWrapperMakefile(QFile& wrapperFile, bool isPrimaryMakefile) = 0;

public:

    SymbianMakefileGenerator();
    ~SymbianMakefileGenerator();
};

#endif // SYMMAKEFILE_H
