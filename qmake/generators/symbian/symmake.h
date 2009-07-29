/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SYMMAKEFILE_H
#define SYMMAKEFILE_H

#include <makefile.h>

QT_BEGIN_NAMESPACE

// In the Qt evaluation and educational version, we have a postfix in the
// library name (e.g. qtmteval301.dll). QTDLL_POSTFIX is used for this.
// A script modifies these lines when building eval/edu version, so be careful
// when changing them.
#ifndef QTDLL_POSTFIX
#define QTDLL_POSTFIX ""
#endif

#define BLD_INF_FILENAME "bld.inf"
#define MAKEFILE_DEPENDENCY_SEPARATOR " \\\n\t"

#define QT_EXTRA_INCLUDE_DIR "tmp"

class SymbianMakefileGenerator : public MakefileGenerator {

protected:

    QString platform;
    QString uid1;
    QString uid2;
    QString uid3;
    QString privateDirUid;
    QString targetType;
    QMap<QString, QStringList> sources;
    QMap<QString, QStringList> systeminclude;
    QMap<QString, QStringList> library;
    // (output file) (source , command)
    QMap<QString, QStringList> makmakeCommands;

    QStringList generatedFiles;
    QStringList generatedDirs;
    QHash<QString, QString> qt2S60LangMapTable;

    void removeSpecialCharacters(QString& str);
    QString fixPathForMmp(const QString& origPath, const QDir& parentDir);
    QString canonizePath(const QString& origPath);

    virtual bool writeMakefile(QTextStream &t);
    bool generatePkgFile(const QString &compiler, const QString &config, const QString &iconFile);
    bool containsStartWithItem(const QChar &c, const QStringList& src);

    virtual void init();

    QString getTargetExtension();
    bool isConfigSetToSymbian();

    QString generateUID1();
    QString generateUID2();
    QString generateUID3();

    bool initMmpVariables();

    void writeHeader(QTextStream &t);
    bool writeBldInfContent(QTextStream& t, bool addDeploymentExtension);

    static bool removeDuplicatedStrings(QStringList& stringList);

    bool writeMmpFileHeader(QTextStream &t);
    bool writeMmpFile(QString &filename, QStringList &symbianLangCodes);
    bool writeMmpFileMacrosPart(QTextStream& t);
    bool addMacro(QTextStream& t, const QString& value);
    bool writeMmpFileTargetPart(QTextStream& t);
    bool writeMmpFileResourcePart(QTextStream& t, QStringList &symbianLangCodes);
    bool writeMmpFileSystemIncludePart(QTextStream& t);
    bool writeMmpFileIncludePart(QTextStream& t);
    bool writeMmpFileLibraryPart(QTextStream& t);
    bool writeMmpFileCapabilityPart(QTextStream& t);
    bool writeMmpFileCompilerOptionPart(QTextStream& t);
    bool writeMmpFileBinaryVersionPart(QTextStream& t);
    bool writeMmpFileRulesPart(QTextStream& t);

    bool writeRegRssFile(QString &appname, QStringList &useritems);
    bool writeRssFile(QString &appName, QString &numberOfIcons, QString &iconfile);
    bool writeLocFile(QString &appName, QStringList &symbianLangCodes);
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

    bool writeCustomDefFile();

    // Subclass implements
    virtual bool writeBldInfExtensionRulesPart(QTextStream& t) = 0;
    virtual void writeBldInfMkFilePart(QTextStream& t, bool addDeploymentExtension) = 0;
    virtual bool writeMkFile(const QString& wrapperFileName, bool deploymentOnly) = 0;
    virtual void writeWrapperMakefile(QFile& wrapperFile, bool isPrimaryMakefile) = 0;

public:

    SymbianMakefileGenerator();
    ~SymbianMakefileGenerator();

};

#endif // SYMMAKEFILE_H

