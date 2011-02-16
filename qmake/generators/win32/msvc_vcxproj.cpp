/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "msvc_vcxproj.h"
#include "msbuild_objectmodel.h"
#include <qdir.h>
#include <qdiriterator.h>
#include <quuid.h>


QT_BEGIN_NAMESPACE
// Filter GUIDs (Do NOT change these!) ------------------------------
const char _GUIDSourceFiles[]          = "{4FC737F1-C7A5-4376-A066-2A32D752A2FF}";
const char _GUIDHeaderFiles[]          = "{93995380-89BD-4b04-88EB-625FBE52EBFB}";
const char _GUIDGeneratedFiles[]       = "{71ED8ED8-ACB9-4CE9-BBE1-E00B30144E11}";
const char _GUIDResourceFiles[]        = "{D9D6E242-F8AF-46E4-B9FD-80ECBC20BA3E}";
const char _GUIDLexYaccFiles[]         = "{E12AE0D2-192F-4d59-BD23-7D3FA58D3183}";
const char _GUIDTranslationFiles[]     = "{639EADAA-A684-42e4-A9AD-28FC9BCB8F7C}";
const char _GUIDFormFiles[]            = "{99349809-55BA-4b9d-BF79-8FDBB0286EB3}";
const char _GUIDExtraCompilerFiles[]   = "{E0D8C965-CC5F-43d7-AD63-FAEF0BBC0F85}";
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE


VcxprojGenerator::VcxprojGenerator() : VcprojGenerator()
{
}
bool VcxprojGenerator::writeMakefile(QTextStream &t)
{
    initProject(); // Fills the whole project with proper data

    // Generate solution file
    if(project->first("TEMPLATE") == "vcsubdirs") {
        if (!project->isActiveConfig("build_pass")) {
            debug_msg(1, "Generator: MSVC.NET: Writing solution file");
            writeSubDirs(t);
        } else {
            debug_msg(1, "Generator: MSVC.NET: Not writing solution file for build_pass configs");
        }
        return true;
    } else
    // Generate single configuration project file
    if (project->first("TEMPLATE") == "vcapp" ||
        project->first("TEMPLATE") == "vclib") {
        if(!project->isActiveConfig("build_pass")) {
            debug_msg(1, "Generator: MSVC.NET: Writing single configuration project file");
            XmlOutput xmlOut(t);
            xmlOut << vcxProject;
        }
        return true;
    }
    return project->isActiveConfig("build_pass");
}


void VcxprojGenerator::initProject()
{
    // Initialize XML sub elements
    // - Do this first since project elements may need
    // - to know of certain configuration options
    initConfiguration();
    initRootFiles();
    initSourceFiles();
    initHeaderFiles();
    initGeneratedFiles();
    initLexYaccFiles();
    initTranslationFiles();
    initFormFiles();
    initResourceFiles();
    initExtraCompilerOutputs();

    // Own elements -----------------------------
    vcxProject.Name = unescapeFilePath(project->first("QMAKE_ORIG_TARGET"));

    vcxProject.Keyword = project->first("VCPROJ_KEYWORD");
    if (project->isEmpty("CE_SDK") || project->isEmpty("CE_ARCH")) {
        vcxProject.PlatformName = vcxProject.Configuration.idl.TargetEnvironment;
        if ( vcxProject.Configuration.idl.TargetEnvironment.isEmpty() )
            vcxProject.PlatformName = "Win32";
    } else {
        vcxProject.PlatformName = project->values("CE_SDK").join(" ") + " (" + project->first("CE_ARCH") + ")";
    }
    // These are not used by Qt, but may be used by customers
    vcxProject.SccProjectName = project->first("SCCPROJECTNAME");
    vcxProject.SccLocalPath = project->first("SCCLOCALPATH");
    vcxProject.flat_files = project->isActiveConfig("flat");
}


void VcxprojGenerator::initConfiguration()
{
    // Initialize XML sub elements
    // - Do this first since main configuration elements may need
    // - to know of certain compiler/linker options
    VCXConfiguration &conf = vcxProject.Configuration;

    initCompilerTool();

    // Only on configuration per build
    bool isDebug = project->isActiveConfig("debug");

    if(projectTarget == StaticLib)
        initLibrarianTool();
    else {
        conf.linker.GenerateDebugInformation = isDebug ? _True : _False;
        initLinkerTool();
    }
    initResourceTool();
    initIDLTool();

    // Own elements -----------------------------
    QString temp = project->first("BuildBrowserInformation");
    switch (projectTarget) {
    case SharedLib:
        conf.ConfigurationType = "DynamicLibrary";
        break;
    case StaticLib:
        conf.ConfigurationType = "StaticLibrary";
        break;
    case Application:
    default:
        conf.ConfigurationType = "Application";
        break;
    }

    conf.OutputDirectory = project->first("DESTDIR");

    if(conf.OutputDirectory.isEmpty())
        conf.OutputDirectory = ".\\";

    if(!conf.OutputDirectory.endsWith("\\"))
        conf.OutputDirectory += '\\';

    // The target name could have been changed.
    conf.TargetName = project->first("TARGET");
    if ( !conf.TargetName.isEmpty() && !project->first("TARGET_VERSION_EXT").isEmpty() && project->isActiveConfig("shared"))
        conf.TargetName.append(project->first("TARGET_VERSION_EXT"));

    conf.Name = project->values("BUILD_NAME").join(" ");
    if (conf.Name.isEmpty())
        conf.Name = isDebug ? "Debug" : "Release";
    conf.ConfigurationName = conf.Name;
    if (project->isEmpty("CE_SDK") || project->isEmpty("CE_ARCH")) {
        conf.Name += (conf.idl.TargetEnvironment == "Win64" ? "|Win64" : "|Win32");
    } else {
        conf.Name += "|" + project->values("CE_SDK").join(" ") + " (" + project->first("CE_ARCH") + ")";
    }
    conf.ATLMinimizesCRunTimeLibraryUsage = (project->first("ATLMinimizesCRunTimeLibraryUsage").isEmpty() ? _False : _True);
    conf.BuildBrowserInformation = triState(temp.isEmpty() ? (short)unset : temp.toShort());
    temp = project->first("CharacterSet");
    if (!temp.isEmpty())
    {
        switch (charSet(temp.toShort())) {

            case charSetMBCS:
                conf.CharacterSet = "MultiByte";
                break;
            case charSetUnicode:
                conf.CharacterSet = "Unicode";
                break;
            case charSetNotSet:
            default:
                conf.CharacterSet = "NotSet";
                break;
        }
    }
    conf.DeleteExtensionsOnClean = project->first("DeleteExtensionsOnClean");
    conf.ImportLibrary = conf.linker.ImportLibrary;
    conf.IntermediateDirectory = project->first("OBJECTS_DIR");
    //conf.OutputDirectory = ".";
    conf.PrimaryOutput = project->first("PrimaryOutput");
    conf.WholeProgramOptimization = conf.compiler.WholeProgramOptimization;
    temp = project->first("UseOfATL");
    if(!temp.isEmpty())
    {
        switch (useOfATL(temp.toShort())) {

            case useATLStatic:
                conf.UseOfATL = "Static";
                break;
            case useATLDynamic:
                conf.UseOfATL = "Dynamic";
                break;
            case useATLNotSet:
            default:
                conf.UseOfATL = "false";
                break;
        }
    }
    temp = project->first("UseOfMfc");
    if(!temp.isEmpty())
    {
        switch (useOfMfc(temp.toShort())) {

            case useMfcStatic:
                conf.UseOfMfc = "Static";
                break;
            case useMfcDynamic:
                conf.UseOfMfc = "Dynamic";
                break;
            case useMfcStdWin:
            default:
                conf.UseOfMfc = "false";
                break;
        }
    }

    // Configuration does not need parameters from
    // these sub XML items;
    initCustomBuildTool();
    initPreBuildEventTools();
    initPostBuildEventTools();
    // Only deploy for CE projects
    if (!project->isEmpty("CE_SDK") && !project->isEmpty("CE_ARCH"))
        initDeploymentTool();
    initPreLinkEventTools();

    // Set definite values in both configurations
    if (isDebug) {
        conf.compiler.PreprocessorDefinitions.removeAll("NDEBUG");
    } else {
        conf.compiler.PreprocessorDefinitions += "NDEBUG";
    }
}


void VcxprojGenerator::initCompilerTool()
{
    QString placement = project->first("OBJECTS_DIR");
    if(placement.isEmpty())
        placement = ".\\";

    VCXConfiguration &conf = vcxProject.Configuration;
    conf.compiler.AssemblerListingLocation = placement ;
    conf.compiler.ProgramDataBaseFileName = ".\\" ;
    conf.compiler.ObjectFileName = placement ;
    conf.compiler.ExceptionHandling = "false";
    // PCH
    if (usePCH) {
        conf.compiler.PrecompiledHeader             = "Use";
        conf.compiler.PrecompiledHeaderOutputFile   = "$(IntDir)\\" + precompPch;
        conf.compiler.PrecompiledHeaderFile         = project->first("PRECOMPILED_HEADER");
        conf.compiler.ForcedIncludeFiles            = project->values("PRECOMPILED_HEADER");
        conf.compiler.PreprocessToFile              = _False;
        conf.compiler.PreprocessSuppressLineNumbers = _False;
        // Minimal build option triggers an Internal Compiler Error
        // when used in conjunction with /FI and /Yu, so remove it
        project->values("QMAKE_CFLAGS_DEBUG").removeAll("-Gm");
        project->values("QMAKE_CFLAGS_DEBUG").removeAll("/Gm");
        project->values("QMAKE_CXXFLAGS_DEBUG").removeAll("-Gm");
        project->values("QMAKE_CXXFLAGS_DEBUG").removeAll("/Gm");
    }

    conf.compiler.parseOptions(project->values("QMAKE_CXXFLAGS"));
    if(project->isActiveConfig("debug")){
        // Debug version
        if((projectTarget == Application) || (projectTarget == StaticLib))
            conf.compiler.parseOptions(project->values("QMAKE_CXXFLAGS_MT_DBG"));
        else
            conf.compiler.parseOptions(project->values("QMAKE_CXXFLAGS_MT_DLLDBG"));
    } else {
        // Release version
        conf.compiler.PreprocessorDefinitions += "QT_NO_DEBUG";
        conf.compiler.PreprocessorDefinitions += "NDEBUG";
        if((projectTarget == Application) || (projectTarget == StaticLib))
            conf.compiler.parseOptions(project->values("QMAKE_CXXFLAGS_MT"));
        else
            conf.compiler.parseOptions(project->values("QMAKE_CXXFLAGS_MT_DLL"));
    }

    // Common for both release and debug
    if(project->isActiveConfig("windows"))
        conf.compiler.PreprocessorDefinitions += project->values("MSVCPROJ_WINCONDEF");

    // Can this be set for ALL configs?
    // If so, use qmake.conf!
    if(projectTarget == SharedLib)
        conf.compiler.PreprocessorDefinitions += "_WINDOWS";

    conf.compiler.PreprocessorDefinitions += project->values("DEFINES");
    conf.compiler.PreprocessorDefinitions += project->values("PRL_EXPORT_DEFINES");
    conf.compiler.parseOptions(project->values("MSVCPROJ_INCPATH"));
}

void VcxprojGenerator::initLinkerTool()
{
    findLibraries(); // Need to add the highest version of the libs
    VCXConfiguration &conf = vcxProject.Configuration;
    conf.linker.parseOptions(project->values("MSVCPROJ_LFLAGS"));

    foreach(QString libs, project->values("MSVCPROJ_LIBS")) {
        if (libs.left(9).toUpper() == "/LIBPATH:") {
            QStringList l = QStringList(libs);
            conf.linker.parseOptions(l);
        } else {
            conf.linker.AdditionalDependencies += libs;
        }
    }

    switch (projectTarget) {
    case Application:
        conf.linker.OutputFile = project->first("DESTDIR");
        break;
    case SharedLib:
        conf.linker.parseOptions(project->values("MSVCPROJ_LIBOPTIONS"));
        conf.linker.OutputFile = project->first("DESTDIR");
        break;
    case StaticLib: //unhandled - added to remove warnings..
        break;
    }

    if(conf.linker.OutputFile.isEmpty())
        conf.linker.OutputFile = ".\\";

    if(!conf.linker.OutputFile.endsWith("\\"))
        conf.linker.OutputFile += '\\';

    conf.linker.OutputFile += project->first("MSVCPROJ_TARGET");

    if(project->isActiveConfig("dll")){
        conf.linker.parseOptions(project->values("QMAKE_LFLAGS_QT_DLL"));
    }
}

void VcxprojGenerator::initResourceTool()
{
    VCXConfiguration &conf = vcxProject.Configuration;
    conf.resource.PreprocessorDefinitions = conf.compiler.PreprocessorDefinitions;

    // We need to add _DEBUG for the debug version of the project, since the normal compiler defines
    // do not contain it. (The compiler defines this symbol automatically, which is wy we don't need
    // to add it for the compiler) However, the resource tool does not do this.
    if(project->isActiveConfig("debug"))
        conf.resource.PreprocessorDefinitions += "_DEBUG";
    if(project->isActiveConfig("staticlib"))
        conf.resource.ResourceOutputFileName = project->first("DESTDIR") + "/$(InputName).res";
}


void VcxprojGenerator::initPostBuildEventTools()
{
    VCXConfiguration &conf = vcxProject.Configuration;
    if(!project->values("QMAKE_POST_LINK").isEmpty()) {
        QStringList cmdline = VCToolBase::fixCommandLine(var("QMAKE_POST_LINK"));
        conf.postBuild.CommandLine = cmdline;
        conf.postBuild.Description = cmdline.join(QLatin1String("\r\n"));
        conf.postBuild.UseInBuild = _True;
    }

    QString signature = !project->isEmpty("SIGNATURE_FILE") ? var("SIGNATURE_FILE") : var("DEFAULT_SIGNATURE");
    bool useSignature = !signature.isEmpty() && !project->isActiveConfig("staticlib") &&
                        !project->isEmpty("CE_SDK") && !project->isEmpty("CE_ARCH");
    if(useSignature) {
        conf.postBuild.CommandLine.prepend(
                QLatin1String("signtool sign /F ") + signature + QLatin1String(" \"$(TargetPath)\""));
        conf.postBuild.UseInBuild = _True;
    }

    if(!project->values("MSVCPROJ_COPY_DLL").isEmpty()) {
        conf.postBuild.Description += var("MSVCPROJ_COPY_DLL_DESC");
        conf.postBuild.CommandLine += var("MSVCPROJ_COPY_DLL");
        conf.postBuild.UseInBuild = _True;
    }
}


void VcxprojGenerator::initDeploymentTool()
{
    VCXConfiguration &conf = vcxProject.Configuration;
    QString targetPath = project->values("deploy.path").join(" ");
    if (targetPath.isEmpty())
        targetPath = QString("%CSIDL_PROGRAM_FILES%\\") + project->first("TARGET");
    if (targetPath.endsWith("/") || targetPath.endsWith("\\"))
        targetPath.chop(1);

    // Only deploy Qt libs for shared build
    if (!project->values("QMAKE_QT_DLL").isEmpty()) {
        QStringList& arg = project->values("MSVCPROJ_LIBS");
        for (QStringList::ConstIterator it = arg.constBegin(); it != arg.constEnd(); ++it) {
            if (it->contains(project->first("QMAKE_LIBDIR"))) {
                QString dllName = *it;

                if (dllName.contains(QLatin1String("QAxContainer"))
                    || dllName.contains(QLatin1String("qtmain"))
                    || dllName.contains(QLatin1String("QtUiTools")))
                    continue;
                dllName.replace(QLatin1String(".lib") , QLatin1String(".dll"));
                QFileInfo info(dllName);
                conf.deployment.AdditionalFiles += info.fileName()
                                                + "|" + QDir::toNativeSeparators(info.absolutePath())
                                                + "|" + targetPath
                                                + "|0;";
            }
        }
    }

    // C-runtime deployment
    QString runtime = project->values("QT_CE_C_RUNTIME").join(QLatin1String(" "));
    if (!runtime.isEmpty() && (runtime != QLatin1String("no"))) {
        QString runtimeVersion = QLatin1String("msvcr");
        QString mkspec = project->first("QMAKESPEC");
        // If no .qmake.cache has been found, we fallback to the original mkspec
        if (mkspec.isEmpty())
            mkspec = project->first("QMAKESPEC_ORIGINAL");

        if (!mkspec.isEmpty()) {
            if (mkspec.endsWith("2010"))
                runtimeVersion.append("100");
            else if (mkspec.endsWith("2008"))
                runtimeVersion.append("90");
            else
                runtimeVersion.append("80");
            if (project->isActiveConfig("debug"))
                runtimeVersion.append("d");
            runtimeVersion.append(".dll");

            if (runtime == "yes") {
                // Auto-find C-runtime
                QString vcInstallDir = qgetenv("VCINSTALLDIR");
                if (!vcInstallDir.isEmpty()) {
                    vcInstallDir += "\\ce\\dll\\";
                    vcInstallDir += project->values("CE_ARCH").join(QLatin1String(" "));
                    if (!QFileInfo(vcInstallDir + QDir::separator() + runtimeVersion).exists())
                        runtime.clear();
                    else
                        runtime = vcInstallDir;
                }
            }
        }

        if (!runtime.isEmpty() && runtime != QLatin1String("yes")) {
            conf.deployment.AdditionalFiles += runtimeVersion
                                            + "|" + QDir::toNativeSeparators(runtime)
                                            + "|" + targetPath
                                            + "|0;";
        }
    }

    // foreach item in DEPLOYMENT
    foreach(QString item, project->values("DEPLOYMENT")) {
        // get item.path
        QString devicePath = project->first(item + ".path");
        if (devicePath.isEmpty())
            devicePath = targetPath;
        // check if item.path is relative (! either /,\ or %)
        if (!(devicePath.at(0) == QLatin1Char('/')
            || devicePath.at(0) == QLatin1Char('\\')
            || devicePath.at(0) == QLatin1Char('%'))) {
            // create output path
            devicePath = Option::fixPathToLocalOS(QDir::cleanPath(targetPath + QLatin1Char('\\') + devicePath));
        }
        // foreach d in item.sources
        foreach(QString source, project->values(item + ".sources")) {
            QString itemDevicePath = devicePath;
            source = Option::fixPathToLocalOS(source);
            QString nameFilter;
            QFileInfo info(source);
            QString searchPath;
            if (info.isDir()) {
                nameFilter = QLatin1String("*");
                itemDevicePath += "\\" + info.fileName();
                searchPath = info.absoluteFilePath();
            } else {
                nameFilter = source.split('\\').last();
                searchPath = info.absolutePath();
            }

            int pathSize = searchPath.size();
            QDirIterator iterator(searchPath, QStringList() << nameFilter
                                  , QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks
                                  , QDirIterator::Subdirectories);
            // foreach dirIterator-entry in d
            while(iterator.hasNext()) {
                iterator.next();
                QString absoluteItemPath = Option::fixPathToLocalOS(QFileInfo(iterator.filePath()).absolutePath());
                // Identify if it is just another subdir
                int diffSize = absoluteItemPath.size() - pathSize;
                // write out rules
                conf.deployment.AdditionalFiles += iterator.fileName()
                    + "|" + absoluteItemPath
                    + "|" + itemDevicePath + (diffSize ? (absoluteItemPath.right(diffSize)) : QLatin1String(""))
                    + "|0;";
            }
        }
    }
}

void VcxprojGenerator::initPreLinkEventTools()
{
    VCXConfiguration &conf = vcxProject.Configuration;
    if(!project->values("QMAKE_PRE_LINK").isEmpty()) {
        QStringList cmdline = VCToolBase::fixCommandLine(var("QMAKE_PRE_LINK"));
        conf.preLink.CommandLine = cmdline;
        conf.preLink.Description = cmdline.join(QLatin1String("\r\n"));
        conf.preLink.UseInBuild = _True;
    }
}

void VcxprojGenerator::initRootFiles()
{
    vcxProject.RootFiles.addFiles(project->values("RC_FILE"));
    vcxProject.RootFiles.Project = this;
    vcxProject.RootFiles.Config = &(vcxProject.Configuration);
    vcxProject.RootFiles.CustomBuild = none;
}

void VcxprojGenerator::initSourceFiles()
{
    vcxProject.SourceFiles.Name = "Source Files";
    vcxProject.SourceFiles.Filter = "cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx";
    vcxProject.SourceFiles.Guid = _GUIDSourceFiles;

    vcxProject.SourceFiles.addFiles(project->values("SOURCES"));

    vcxProject.SourceFiles.Project = this;
    vcxProject.SourceFiles.Config = &(vcxProject.Configuration);
    vcxProject.SourceFiles.CustomBuild = none;
}

void VcxprojGenerator::initHeaderFiles()
{
    vcxProject.HeaderFiles.Name = "Header Files";
    vcxProject.HeaderFiles.Filter = "h;hpp;hxx;hm;inl;inc;xsd";
    vcxProject.HeaderFiles.Guid = _GUIDHeaderFiles;

    vcxProject.HeaderFiles.addFiles(project->values("HEADERS"));
    if (usePCH) // Generated PCH cpp file
        vcxProject.HeaderFiles.addFile(precompH);

    vcxProject.HeaderFiles.Project = this;
    vcxProject.HeaderFiles.Config = &(vcxProject.Configuration);
}

void VcxprojGenerator::initGeneratedFiles()
{
    vcxProject.GeneratedFiles.Name = "Generated Files";
    vcxProject.GeneratedFiles.Filter = "cpp;c;cxx;moc;h;def;odl;idl;res";
    vcxProject.GeneratedFiles.Guid = _GUIDGeneratedFiles;

    // ### These cannot have CustomBuild (mocSrc)!!
    vcxProject.GeneratedFiles.addFiles(project->values("GENERATED_SOURCES"));
    vcxProject.GeneratedFiles.addFiles(project->values("GENERATED_FILES"));
    vcxProject.GeneratedFiles.addFiles(project->values("IDLSOURCES"));
    vcxProject.GeneratedFiles.addFiles(project->values("RES_FILE"));
    vcxProject.GeneratedFiles.addFiles(project->values("QMAKE_IMAGE_COLLECTION"));   // compat
    if(!extraCompilerOutputs.isEmpty())
        vcxProject.GeneratedFiles.addFiles(extraCompilerOutputs.keys());

    vcxProject.GeneratedFiles.Project = this;
    vcxProject.GeneratedFiles.Config = &(vcxProject.Configuration);
}

void VcxprojGenerator::initLexYaccFiles()
{
    vcxProject.LexYaccFiles.Name = "Lex / Yacc Files";
    vcxProject.LexYaccFiles.ParseFiles = _False;
    vcxProject.LexYaccFiles.Filter = "l;y";
    vcxProject.LexYaccFiles.Guid = _GUIDLexYaccFiles;

    vcxProject.LexYaccFiles.addFiles(project->values("LEXSOURCES"));
    vcxProject.LexYaccFiles.addFiles(project->values("YACCSOURCES"));

    vcxProject.LexYaccFiles.Project = this;
    vcxProject.LexYaccFiles.Config = &(vcxProject.Configuration);
    vcxProject.LexYaccFiles.CustomBuild = lexyacc;
}

void VcxprojGenerator::initTranslationFiles()
{
    vcxProject.TranslationFiles.Name = "Translation Files";
    vcxProject.TranslationFiles.ParseFiles = _False;
    vcxProject.TranslationFiles.Filter = "ts;xlf";
    vcxProject.TranslationFiles.Guid = _GUIDTranslationFiles;

    vcxProject.TranslationFiles.addFiles(project->values("TRANSLATIONS"));

    vcxProject.TranslationFiles.Project = this;
    vcxProject.TranslationFiles.Config = &(vcxProject.Configuration);
    vcxProject.TranslationFiles.CustomBuild = none;
}


void VcxprojGenerator::initFormFiles()
{
    vcxProject.FormFiles.Name = "Form Files";
    vcxProject.FormFiles.ParseFiles = _False;
    vcxProject.FormFiles.Filter = "ui";
    vcxProject.FormFiles.Guid = _GUIDFormFiles;

    vcxProject.FormFiles.addFiles(project->values("FORMS"));
    vcxProject.FormFiles.addFiles(project->values("FORMS3"));

    vcxProject.FormFiles.Project = this;
    vcxProject.FormFiles.Config = &(vcxProject.Configuration);
    vcxProject.FormFiles.CustomBuild = none;
}


void VcxprojGenerator::initResourceFiles()
{
    vcxProject.ResourceFiles.Name = "Resource Files";
    vcxProject.ResourceFiles.ParseFiles = _False;
    vcxProject.ResourceFiles.Filter = "qrc;*"; //"rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;ts;xlf;qrc";
    vcxProject.ResourceFiles.Guid = _GUIDResourceFiles;

    // Bad hack, please look away -------------------------------------
    QString rcc_dep_cmd = project->values("rcc.depend_command").join(" ");
    if(!rcc_dep_cmd.isEmpty()) {
        QStringList qrc_files = project->values("RESOURCES");
        QStringList deps;
        if(!qrc_files.isEmpty()) {
            for (int i = 0; i < qrc_files.count(); ++i) {
                char buff[256];
                QString dep_cmd = replaceExtraCompilerVariables(rcc_dep_cmd, qrc_files.at(i),"");

                dep_cmd = Option::fixPathToLocalOS(dep_cmd, true, false);
                if(canExecute(dep_cmd)) {
                    if(FILE *proc = QT_POPEN(dep_cmd.toLatin1().constData(), "r")) {
                        QString indeps;
                        while(!feof(proc)) {
                            int read_in = (int)fread(buff, 1, 255, proc);
                            if(!read_in)
                                break;
                            indeps += QByteArray(buff, read_in);
                        }
                        QT_PCLOSE(proc);
                        if(!indeps.isEmpty())
                            deps += fileFixify(indeps.replace('\n', ' ').simplified().split(' '));
                    }
                }
            }
            vcxProject.ResourceFiles.addFiles(deps);
        }
    }
    // You may look again --------------------------------------------

    vcxProject.ResourceFiles.addFiles(project->values("RESOURCES"));
    vcxProject.ResourceFiles.addFiles(project->values("IMAGES"));

    vcxProject.ResourceFiles.Project = this;
    vcxProject.ResourceFiles.Config = &(vcxProject.Configuration);
    vcxProject.ResourceFiles.CustomBuild = none;
}

void VcxprojGenerator::initExtraCompilerOutputs()
{
    QStringList otherFilters;
    otherFilters << "FORMS"
                 << "FORMS3"
                 << "GENERATED_FILES"
                 << "GENERATED_SOURCES"
                 << "HEADERS"
                 << "IDLSOURCES"
                 << "IMAGES"
                 << "LEXSOURCES"
                 << "QMAKE_IMAGE_COLLECTION"
                 << "RC_FILE"
                 << "RESOURCES"
                 << "RES_FILE"
                 << "SOURCES"
                 << "TRANSLATIONS"
                 << "YACCSOURCES";
    const QStringList &quc = project->values("QMAKE_EXTRA_COMPILERS");
    for(QStringList::ConstIterator it = quc.begin(); it != quc.end(); ++it) {
        QString extracompilerName = project->first((*it) + ".name");
        if (extracompilerName.isEmpty())
            extracompilerName = (*it);

        // Create an extra compiler filter and add the files
        VCXFilter extraCompile;
        extraCompile.Name = extracompilerName;
        extraCompile.ParseFiles = _False;
        extraCompile.Filter = "";
        extraCompile.Guid = QString(_GUIDExtraCompilerFiles) + "-" + (*it);

        // If the extra compiler has a variable_out set the output file
        // is added to an other file list, and does not need its own..
        bool addOnInput = hasBuiltinCompiler(project->first((*it) + ".output"));
        QString tmp_other_out = project->first((*it) + ".variable_out");
        if (!tmp_other_out.isEmpty() && !addOnInput)
            continue;

        if (!addOnInput) {
            QString tmp_out = project->first((*it) + ".output");
            if (project->values((*it) + ".CONFIG").indexOf("combine") != -1) {
                // Combined output, only one file result
                extraCompile.addFile(
                    Option::fixPathToTargetOS(replaceExtraCompilerVariables(tmp_out, QString(), QString()), false));
            } else {
                // One output file per input
                QStringList tmp_in = project->values(project->first((*it) + ".input"));
                for (int i = 0; i < tmp_in.count(); ++i) {
                    const QString &filename = tmp_in.at(i);
                    if (extraCompilerSources.contains(filename))
                        extraCompile.addFile(
                            Option::fixPathToTargetOS(replaceExtraCompilerVariables(filename, tmp_out, QString()), false));
                }
            }
        } else {
            // In this case we the outputs have a built-in compiler, so we cannot add the custom
            // build steps there. So, we turn it around and add it to the input files instead,
            // provided that the input file variable is not handled already (those in otherFilters
            // are handled, so we avoid them).
            QStringList inputVars = project->values((*it) + ".input");
            foreach(QString inputVar, inputVars) {
                if (!otherFilters.contains(inputVar)) {
                    QStringList tmp_in = project->values(inputVar);
                    for (int i = 0; i < tmp_in.count(); ++i) {
                        const QString &filename = tmp_in.at(i);
                        if (extraCompilerSources.contains(filename))
                            extraCompile.addFile(
                                Option::fixPathToTargetOS(replaceExtraCompilerVariables(filename, QString(), QString()), false));
                    }
                }
            }
        }
        extraCompile.Project = this;
        extraCompile.Config = &(vcxProject.Configuration);
        extraCompile.CustomBuild = none;

        vcxProject.ExtraCompilersFiles.append(extraCompile);
    }
}



bool VcxprojGenerator::writeProjectMakefile()
{
    usePlatformDir();
    QTextStream t(&Option::output);

    // Check if all requirements are fulfilled
    if(!project->values("QMAKE_FAILED_REQUIREMENTS").isEmpty()) {
        fprintf(stderr, "Project file not generated because all requirements not met:\n\t%s\n",
                var("QMAKE_FAILED_REQUIREMENTS").toLatin1().constData());
        return true;
    }

    // Generate project file
    if(project->first("TEMPLATE") == "vcapp" ||
       project->first("TEMPLATE") == "vclib") {
        if (!mergedProjects.count()) {
            warn_msg(WarnLogic, "Generator: MSVC.NET: no single configuration created, cannot output project!");
            return false;
        }

        debug_msg(1, "Generator: MSVC.NET: Writing project file");
        VCXProject mergedProject;
        for (int i = 0; i < mergedProjects.count(); ++i) {
            VCXProjectSingleConfig *singleProject = &(mergedProjects.at(i)->vcxProject);
            mergedProject.SingleProjects += *singleProject;
            for (int j = 0; j < singleProject->ExtraCompilersFiles.count(); ++j) {
                const QString &compilerName = singleProject->ExtraCompilersFiles.at(j).Name;
                if (!mergedProject.ExtraCompilers.contains(compilerName))
                    mergedProject.ExtraCompilers += compilerName;
            }
        }

        if(mergedProjects.count() > 1 &&
           mergedProjects.at(0)->vcxProject.Name ==
           mergedProjects.at(1)->vcxProject.Name)
            mergedProjects.at(0)->writePrlFile();
        mergedProject.Name = unescapeFilePath(project->first("QMAKE_ORIG_TARGET"));
        mergedProject.Version = mergedProjects.at(0)->vcxProject.Version;
        mergedProject.ProjectGUID = project->isEmpty("QMAKE_UUID") ? getProjectUUID().toString().toUpper() : project->first("QMAKE_UUID");
        mergedProject.Keyword = project->first("VCPROJ_KEYWORD");
        mergedProject.SccProjectName = mergedProjects.at(0)->vcxProject.SccProjectName;
        mergedProject.SccLocalPath = mergedProjects.at(0)->vcxProject.SccLocalPath;
        mergedProject.PlatformName = mergedProjects.at(0)->vcxProject.PlatformName;

        XmlOutput xmlOut(t);
        xmlOut << mergedProject;
        return true;
    } else if(project->first("TEMPLATE") == "vcsubdirs") {
        return writeMakefile(t);
    }
    return false;
}




bool VcxprojGenerator::mergeBuildProject(MakefileGenerator *other)
{
    VcxprojGenerator *otherVC = static_cast<VcxprojGenerator*>(other);
    if (!otherVC) {
        warn_msg(WarnLogic, "VcxprojGenerator: Cannot merge other types of projects! (ignored)");
        return false;
    }
    mergedProjects += otherVC;
    return true;
}

QT_END_NAMESPACE

