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

#include "msvc_objectmodel.h"
#include "msbuild_objectmodel.h"
#include "msvc_vcproj.h"
#include "msvc_vcxproj.h"
#include <qstringlist.h>
#include <qfileinfo.h>

QT_BEGIN_NAMESPACE

// XML Tags ---------------------------------------------------------
const char _CLCompile[]                         = "ClCompile";
const char _Configuration[]                     = "Configuration";
const char _Configurations[]                    = "Configurations";
const char q_File[]                             = "File";
const char _FileConfiguration[]                 = "FileConfiguration";
const char q_Files[]                            = "Files";
const char _Filter[]                            = "Filter";
const char _Globals[]                           = "Globals";
const char _ItemGroup[]                         = "ItemGroup";
const char _Link[]                              = "Link";
const char _Midl[]                              = "Midl";
const char _Platform[]                          = "Platform";
const char _Platforms[]                         = "Platforms";
const char _ResourceCompile[]                   = "ResourceCompile";
const char _Tool[]                              = "Tool";
const char _VisualStudioProject[]               = "VisualStudioProject";

// XML Properties ---------------------------------------------------
const char _AddModuleNamesToAssembly[]          = "AddModuleNamesToAssembly";
const char _AdditionalDependencies[]            = "AdditionalDependencies";
const char _AdditionalFiles[]                   = "AdditionalFiles";
const char _AdditionalIncludeDirectories[]      = "AdditionalIncludeDirectories";
const char _AdditionalLibraryDirectories[]      = "AdditionalLibraryDirectories";
const char _AdditionalManifestDependencies[]    = "AdditionalManifestDependencies";
const char _AdditionalOptions[]                 = "AdditionalOptions";
const char _AdditionalUsingDirectories[]        = "AdditionalUsingDirectories";
const char _AllowIsolation[]                    = "AllowIsolation";
const char _AlwaysAppend[]                      = "AlwaysAppend";
const char _ApplicationConfigurationMode[]      = "ApplicationConfigurationMode";
const char _AssemblerListingLocation[]          = "AssemblerListingLocation";
const char _AssemblerOutput[]                   = "AssemblerOutput";
const char _AssemblyDebug[]                     = "AssemblyDebug";
const char _AssemblyLinkResource[]              = "AssemblyLinkResource";
const char _ATLMinimizesCRunTimeLibraryUsage[]  = "ATLMinimizesCRunTimeLibraryUsage";
const char _BaseAddress[]                       = "BaseAddress";
const char _BasicRuntimeChecks[]                = "BasicRuntimeChecks";
const char _BrowseInformation[]                 = "BrowseInformation";
const char _BrowseInformationFile[]             = "BrowseInformationFile";
const char _BufferSecurityCheck[]               = "BufferSecurityCheck";
const char _BuildBrowserInformation[]           = "BuildBrowserInformation";
const char _CallingConvention[]                 = "CallingConvention";
const char _CharacterSet[]                      = "CharacterSet";
const char _ClientStubFile[]                    = "ClientStubFile";
const char _CLRImageType[]                      = "CLRImageType";
const char _CLRSupportLastError[]               = "CLRSupportLastError";
const char _CLRThreadAttribute[]                = "CLRThreadAttribute";
const char _CLRUnmanagedCodeCheck[]             = "CLRUnmanagedCodeCheck";
const char _Command[]                           = "Command";
const char _CommandLine[]                       = "CommandLine";
const char _CompileAs[]                         = "CompileAs";
const char _CompileAsManaged[]                  = "CompileAsManaged";
const char _CompileForArchitecture[]            = "CompileForArchitecture";
const char _CompileOnly[]                       = "CompileOnly";
const char _ConfigurationType[]                 = "ConfigurationType";
const char _CPreprocessOptions[]                = "CPreprocessOptions";
const char _CreateHotpatchableImage[]           = "CreateHotpatchableImage";
const char _CreateHotPatchableImage[]           = "CreateHotPatchableImage";
const char _Culture[]                           = "Culture";
const char _DataExecutionPrevention[]           = "DataExecutionPrevention";
const char _DebugInformationFormat[]            = "DebugInformationFormat";
const char _DefaultCharIsUnsigned[]             = "DefaultCharIsUnsigned";
const char _DefaultCharType[]                   = "DefaultCharType";
const char _DelayLoadDLLs[]                     = "DelayLoadDLLs";
const char _DelaySign[]                         = "DelaySign";
const char _DeleteExtensionsOnClean[]           = "DeleteExtensionsOnClean";
const char _Description[]                       = "Description";
const char _Detect64BitPortabilityProblems[]    = "Detect64BitPortabilityProblems";
const char _DisableLanguageExtensions[]         = "DisableLanguageExtensions";
const char _DisableSpecificWarnings[]           = "DisableSpecificWarnings";
const char _DisplayLibrary[]                    = "DisplayLibrary";
const char _DLLDataFileName[]                   = "DLLDataFileName";
const char _Driver[]                            = "Driver";
const char _EmbedManagedResourceFile[]          = "EmbedManagedResourceFile";
const char _EnableCOMDATFolding[]               = "EnableCOMDATFolding";
const char _EnableUAC[]                         = "EnableUAC";
const char _EnableErrorChecks[]                 = "EnableErrorChecks";
const char _EnableEnhancedInstructionSet[]      = "EnableEnhancedInstructionSet";
const char _EnableFiberSafeOptimizations[]      = "EnableFiberSafeOptimizations";
const char _EnableFunctionLevelLinking[]        = "EnableFunctionLevelLinking";
const char _EnableIntrinsicFunctions[]          = "EnableIntrinsicFunctions";
const char _EnablePREfast[]                     = "EnablePREfast";
const char _EntryPointSymbol[]                  = "EntryPointSymbol";
const char _ErrorCheckAllocations[]             = "ErrorCheckAllocations";
const char _ErrorCheckBounds[]                  = "ErrorCheckBounds";
const char _ErrorCheckEnumRange[]               = "ErrorCheckEnumRange";
const char _ErrorCheckRefPointers[]             = "ErrorCheckRefPointers";
const char _ErrorCheckStubData[]                = "ErrorCheckStubData";
const char _ErrorReporting[]                    = "ErrorReporting";
const char _ExceptionHandling[]                 = "ExceptionHandling";
const char _ExcludedFromBuild[]                 = "ExcludedFromBuild";
const char _ExpandAttributedSource[]            = "ExpandAttributedSource";
const char _ExportNamedFunctions[]              = "ExportNamedFunctions";
const char _FavorSizeOrSpeed[]                  = "FavorSizeOrSpeed";
const char _FixedBaseAddress[]                  = "FixedBaseAddress";
const char _FloatingPointModel[]                = "FloatingPointModel";
const char _FloatingPointExceptions[]           = "FloatingPointExceptions";
const char _ForceConformanceInForLoopScope[]    = "ForceConformanceInForLoopScope";
const char _ForceFileOutput[]                   = "ForceFileOutput";
const char _ForceSymbolReferences[]             = "ForceSymbolReferences";
const char _ForcedIncludeFiles[]                = "ForcedIncludeFiles";
const char _ForcedUsingFiles[]                  = "ForcedUsingFiles";
const char _FullIncludePath[]                   = "FullIncludePath";
const char _FunctionLevelLinking[]              = "FunctionLevelLinking";
const char _FunctionOrder[]                     = "FunctionOrder";
const char _GenerateClientFiles[]               = "GenerateClientFiles";
const char _GenerateDebugInformation[]          = "GenerateDebugInformation";
const char _GenerateManifest[]                  = "GenerateManifest";
const char _GenerateMapFile[]                   = "GenerateMapFile";
const char _GeneratePreprocessedFile[]          = "GeneratePreprocessedFile";
const char _GenerateServerFiles[]               = "GenerateServerFiles";
const char _GenerateStublessProxies[]           = "GenerateStublessProxies";
const char _GenerateTypeLibrary[]               = "GenerateTypeLibrary";
const char _GenerateXMLDocumentationFiles[]     = "GenerateXMLDocumentationFiles";
const char _GlobalOptimizations[]               = "GlobalOptimizations";
const char _HeaderFileName[]                    = "HeaderFileName";
const char _HeapCommitSize[]                    = "HeapCommitSize";
const char _HeapReserveSize[]                   = "HeapReserveSize";
const char _IgnoreAllDefaultLibraries[]         = "IgnoreAllDefaultLibraries";
const char _IgnoreDefaultLibraryNames[]         = "IgnoreDefaultLibraryNames";
const char _IgnoreEmbeddedIDL[]                 = "IgnoreEmbeddedIDL";
const char _IgnoreImportLibrary[]               = "IgnoreImportLibrary";
const char _IgnoreSpecificDefaultLibraries[]    = "IgnoreSpecificDefaultLibraries";
const char _IgnoreStandardIncludePath[]         = "IgnoreStandardIncludePath";
const char _ImageHasSafeExceptionHandlers[]     = "ImageHasSafeExceptionHandlers";
const char _ImportLibrary[]                     = "ImportLibrary";
const char _ImproveFloatingPointConsistency[]   = "ImproveFloatingPointConsistency";
const char _InlineFunctionExpansion[]           = "InlineFunctionExpansion";
const char _IntrinsicFunctions[]                = "IntrinsicFunctions";
const char _InterfaceIdentifierFileName[]       = "InterfaceIdentifierFileName";
const char _IntermediateDirectory[]             = "IntermediateDirectory";
const char _InterworkCalls[]                    = "InterworkCalls";
const char _KeyContainer[]                      = "KeyContainer";
const char _KeyFile[]                           = "KeyFile";
const char _Keyword[]                           = "Keyword";
const char _KeepComments[]                      = "KeepComments";
const char _LargeAddressAware[]                 = "LargeAddressAware";
const char _LinkDLL[]                           = "LinkDLL";
const char _LinkErrorReporting[]                = "LinkErrorReporting";
const char _LinkIncremental[]                   = "LinkIncremental";
const char _LinkStatus[]                        = "LinkStatus";
const char _LinkTimeCodeGeneration[]            = "LinkTimeCodeGeneration";
const char _LinkToManagedResourceFile[]         = "LinkToManagedResourceFile";
const char _LocaleID[]                          = "LocaleID";
const char _ManifestFile[]                      = "ManifestFile";
const char _MapExports[]                        = "MapExports";
const char _MapFileName[]                       = "MapFileName";
const char _MapLines[]                          = "MapLines ";
const char _MergedIDLBaseFileName[]             = "MergedIDLBaseFileName";
const char _MergeSections[]                     = "MergeSections";
const char _Message[]                           = "Message";
const char _MidlCommandFile[]                   = "MidlCommandFile";
const char _MinimalRebuild[]                    = "MinimalRebuild";
const char _MkTypLibCompatible[]                = "MkTypLibCompatible";
const char _ModuleDefinitionFile[]              = "ModuleDefinitionFile";
const char _MSDOSStubFileName[]                 = "MSDOSStubFileName";
const char _MultiProcessorCompilation[]         = "MultiProcessorCompilation";
const char _Name[]                              = "Name";
const char _NoEntryPoint[]                      = "NoEntryPoint";
const char _NullTerminateStrings[]              = "NullTerminateStrings";
const char _ObjectFile[]                        = "ObjectFile";
const char _ObjectFiles[]                       = "ObjectFiles";
const char _ObjectFileName[]                    = "ObjectFileName";
const char _OmitDefaultLibName[]                = "OmitDefaultLibName";
const char _OmitFramePointers[]                 = "OmitFramePointers";
const char _OpenMP[]                            = "OpenMP";
const char _OpenMPSupport[]                     = "OpenMPSupport";
const char _Optimization[]                      = "Optimization";
const char _OptimizeForProcessor[]              = "OptimizeForProcessor";
const char _OptimizeForWindows98[]              = "OptimizeForWindows98";
const char _OptimizeForWindowsApplication[]     = "OptimizeForWindowsApplication";
const char _OptimizeReferences[]                = "OptimizeReferences";
const char _OutputDirectory[]                   = "OutputDirectory";
const char _OutputFile[]                        = "OutputFile";
const char _Outputs[]                           = "Outputs";
const char _ParseFiles[]                        = "ParseFiles";
const char _Path[]                              = "Path";
const char _PrecompiledHeader[]                 = "PrecompiledHeader";
const char _PrecompiledHeaderFile[]             = "PrecompiledHeaderFile";
const char _PrecompiledHeaderOutputFile[]       = "PrecompiledHeaderOutputFile";
const char _PrecompiledHeaderThrough[]          = "PrecompiledHeaderThrough";
const char _PreprocessorDefinitions[]           = "PreprocessorDefinitions";
const char _PreprocessKeepComments[]            = "PreprocessKeepComments";
const char _PreprocessOutputPath[]              = "PreprocessOutputPath";
const char _PreprocessSuppressLineNumbers[]     = "PreprocessSuppressLineNumbers";
const char _PreprocessToFile[]                  = "PreprocessToFile";
const char _PreventDllBinding[]                 = "PreventDllBinding";
const char _PrimaryOutput[]                     = "PrimaryOutput";
const char _Profile[]                           = "Profile";
const char _ProfileGuidedDatabase[]             = "ProfileGuidedDatabase";
const char _ProjectGUID[]                       = "ProjectGUID";
const char _ProcessorNumber[]                   = "ProcessorNumber";
const char _ProjectType[]                       = "ProjectType";
const char _ProgramDatabase[]                   = "ProgramDatabase";
const char _ProgramDataBaseFileName[]           = "ProgramDataBaseFileName";
const char _ProgramDatabaseFile[]               = "ProgramDatabaseFile";
const char _ProxyFileName[]                     = "ProxyFileName";
const char _RandomizedBaseAddress[]             = "RandomizedBaseAddress";
const char _RedirectOutputAndErrors[]           = "RedirectOutputAndErrors";
const char _RegisterOutput[]                    = "RegisterOutput";
const char _RelativePath[]                      = "RelativePath";
const char _RemoteDirectory[]                   = "RemoteDirectory";
const char _RemoveObjects[]                     = "RemoveObjects";
const char _ResourceOnlyDLL[]                   = "ResourceOnlyDLL";
const char _ResourceOutputFileName[]            = "ResourceOutputFileName";
const char _RuntimeLibrary[]                    = "RuntimeLibrary";
const char _RuntimeTypeInfo[]                   = "RuntimeTypeInfo";
const char _SccProjectName[]                    = "SccProjectName";
const char _SccLocalPath[]                      = "SccLocalPath";
const char _SectionAlignment[]                  = "SectionAlignment";
const char _ServerStubFile[]                    = "ServerStubFile";
const char _SetChecksum[]                       = "SetChecksum";
const char _ShowIncludes[]                      = "ShowIncludes";
const char _ShowProgress[]                      = "ShowProgress";
const char _SmallerTypeCheck[]                  = "SmallerTypeCheck";
const char _SpecifySectionAttributes[]          = "SpecifySectionAttributes";
const char _StackCommitSize[]                   = "StackCommitSize";
const char _StackReserveSize[]                  = "StackReserveSize";
const char _StringPooling[]                     = "StringPooling";
const char _StripPrivateSymbols[]               = "StripPrivateSymbols";
const char _StructMemberAlignment[]             = "StructMemberAlignment";
const char _SubSystem[]                         = "SubSystem";
const char _SupportNobindOfDelayLoadedDLL[]     = "SupportNobindOfDelayLoadedDLL";
const char _SupportUnloadOfDelayLoadedDLL[]     = "SupportUnloadOfDelayLoadedDLL";
const char _SuppressCompilerWarnings[]          = "SuppressCompilerWarnings";
const char _SuppressStartupBanner[]             = "SuppressStartupBanner";
const char _SwapRunFromCD[]                     = "SwapRunFromCD";
const char _SwapRunFromNet[]                    = "SwapRunFromNet";
const char _TargetEnvironment[]                 = "TargetEnvironment";
const char _TargetMachine[]                     = "TargetMachine";
const char _TerminalServerAware[]               = "TerminalServerAware";
const char _TrackerLogDirectory[]               = "TrackerLogDirectory";
const char _TreatLibWarningAsErrors[]           = "TreatLibWarningAsErrors";
const char _TreatLinkerWarningAsErrors[]        = "TreatLinkerWarningAsErrors";
const char _TreatSpecificWarningsAsErrors[]     = "TreatSpecificWarningsAsErrors";
const char _TreatWarningAsError[]               = "TreatWarningAsError";
const char _TreatWChar_tAsBuiltInType[]         = "TreatWChar_tAsBuiltInType";
const char _TurnOffAssemblyGeneration[]         = "TurnOffAssemblyGeneration";
const char _TypeLibFormat[]                     = "TypeLibFormat";
const char _TypeLibraryFile[]                   = "TypeLibraryFile";
const char _TypeLibraryName[]                   = "TypeLibraryName";
const char _TypeLibraryResourceID[]             = "TypeLibraryResourceID";
const char _UACExecutionLevel[]                 = "UACExecutionLevel";
const char _UACUIAccess[]                       = "UACUIAccess";
const char _UndefineAllPreprocessorDefinitions[]= "UndefineAllPreprocessorDefinitions";
const char _UndefinePreprocessorDefinitions[]   = "UndefinePreprocessorDefinitions";
const char _UniqueIdentifier[]                  = "UniqueIdentifier";
const char _UseFullPaths[]                      = "UseFullPaths";
const char _UseOfATL[]                          = "UseOfATL";
const char _UseOfMfc[]                          = "UseOfMfc";
const char _UsePrecompiledHeader[]              = "UsePrecompiledHeader";
const char _UseUnicodeForAssemblerListing[]     = "UseUnicodeForAssemblerListing";
const char _ValidateAllParameters[]             = "ValidateAllParameters";
const char _VCCLCompilerTool[]                  = "VCCLCompilerTool";
const char _VCLibrarianTool[]                   = "VCLibrarianTool";
const char _VCLinkerTool[]                      = "VCLinkerTool";
const char _VCCustomBuildTool[]                 = "VCCustomBuildTool";
const char _VCResourceCompilerTool[]            = "VCResourceCompilerTool";
const char _VCMIDLTool[]                        = "VCMIDLTool";
const char _Verbose[]                           = "Verbose";
const char _Version[]                           = "Version";
const char _WarnAsError[]                       = "WarnAsError";
const char _WarningLevel[]                      = "WarningLevel";
const char _WholeProgramOptimization[]          = "WholeProgramOptimization";
const char _XMLDocumentationFileName[]          = "XMLDocumentationFileName";


// XmlOutput stream functions ------------------------------
inline XmlOutput::xml_output attrTagT(const char *name, const triState v)
{
    if(v == unset)
        return noxml();
    return tagValue(name, (v == _True ? "true" : "false"));
}

inline XmlOutput::xml_output attrTagL(const char *name, qint64 v)
{
    return tagValue(name, QString::number(v));
}

/*ifNot version*/
inline XmlOutput::xml_output attrTagL(const char *name, qint64 v, qint64 ifn)
{
    if (v == ifn)
        return noxml();
    return tagValue(name, QString::number(v));
}

inline XmlOutput::xml_output attrTagS(const char *name, const QString &v)
{
    if(v.isEmpty())
        return noxml();
    return tagValue(name, v);
}


inline XmlOutput::xml_output attrTagX(const char *name, const QStringList &v, const char *s = ",")
{
    if(v.isEmpty())
        return noxml();
    QStringList temp = v;
    temp.append(QString("%(%1)").arg(name));
    return tagValue(name, temp.join(s));
}

inline XmlOutput::xml_output valueTagX(const QStringList &v, const char *s = " ")
{
    if(v.isEmpty())
        return noxml();
    return valueTag(v.join(s));
}

inline XmlOutput::xml_output valueTagDefX(const QStringList &v, const QString &tagName, const char *s = " ")
{
    if(v.isEmpty())
        return noxml();
    QStringList temp = v;
    temp.append(QString("%(%1)").arg(tagName));
    return valueTag(temp.join(s));
}

inline XmlOutput::xml_output valueTagT( const triState v)
{
    if(v == unset)
        return noxml();
    return valueTag(v == _True ? "true" : "false");
}


static QString vcxCommandSeparator()
{
    // MSBuild puts the contents of the custom commands into a batch file and calls it.
    // As we want every sub-command to be error-checked (as is done by makefile-based
    // backends), we insert the checks ourselves, using the undocumented jump target.
    static QString cmdSep =
            QLatin1String("&#x000D;&#x000A;if errorlevel 1 goto VCEnd&#x000D;&#x000A;");
    return cmdSep;
}

// VCXCLCompilerTool -------------------------------------------------
VCXCLCompilerTool::VCXCLCompilerTool()
    :   BrowseInformation(_False),
        BufferSecurityCheck(_False),
        CreateHotpatchableImage(unset),
        DisableLanguageExtensions(unset),
        EnableFiberSafeOptimizations(unset),
        EnablePREfast(unset),
        ExpandAttributedSource(unset),
        FloatingPointExceptions(unset),
        ForceConformanceInForLoopScope(unset),
        FunctionLevelLinking(unset),
        GenerateXMLDocumentationFiles(unset),
        IgnoreStandardIncludePath(unset),
        IntrinsicFunctions(unset),
        MinimalRebuild(unset),
        MultiProcessorCompilation(unset),
        OmitDefaultLibName(unset),
        OmitFramePointers(unset),
        Optimization("Disabled"),
        OpenMPSupport(unset),
        PreprocessKeepComments(unset),
        PreprocessSuppressLineNumbers(unset),
        RuntimeTypeInfo(unset),
        ShowIncludes(unset),
        SmallerTypeCheck(unset),
        StringPooling(unset),
        SuppressStartupBanner(unset),
        TreatWarningAsError(unset),
        TreatWChar_tAsBuiltInType(unset),
        UndefineAllPreprocessorDefinitions(unset),
        UseFullPaths(unset),
        UseUnicodeForAssemblerListing(unset),
        WholeProgramOptimization(unset)
{
}

XmlOutput &operator<<(XmlOutput &xml, const VCXCLCompilerTool &tool)
{
    return xml
        << tag(_CLCompile)
            << attrTagX(_AdditionalIncludeDirectories, tool.AdditionalIncludeDirectories, ";")
            << attrTagX(_AdditionalOptions, tool.AdditionalOptions, " ")
            << attrTagX(_AdditionalUsingDirectories, tool.AdditionalUsingDirectories, ";")
            << attrTagS(_AlwaysAppend, tool.AlwaysAppend)
            << attrTagS(_AssemblerListingLocation, tool.AssemblerListingLocation)
            << attrTagS(_AssemblerOutput, tool.AssemblerOutput)
            << attrTagS(_BasicRuntimeChecks, tool.BasicRuntimeChecks)
            << attrTagT(_BrowseInformation, tool.BrowseInformation)
            << attrTagS(_BrowseInformationFile, tool.BrowseInformationFile)
            << attrTagT(_BufferSecurityCheck, tool.BufferSecurityCheck)
            << attrTagS(_CallingConvention, tool.CallingConvention)
            << attrTagS(_CompileAs, tool.CompileAs)
            << attrTagS(_CompileAsManaged, tool.CompileAsManaged)
            << attrTagT(_CreateHotpatchableImage, tool.CreateHotpatchableImage)
            << attrTagS(_DebugInformationFormat, tool.DebugInformationFormat)
            << attrTagT(_DisableLanguageExtensions, tool.DisableLanguageExtensions)
            << attrTagX(_DisableSpecificWarnings, tool.DisableSpecificWarnings, ";")
            << attrTagS(_EnableEnhancedInstructionSet, tool.EnableEnhancedInstructionSet)
            << attrTagT(_EnableFiberSafeOptimizations, tool.EnableFiberSafeOptimizations)
            << attrTagT(_EnablePREfast, tool.EnablePREfast)
            << attrTagS(_ErrorReporting, tool.ErrorReporting)
            << attrTagS(_ExceptionHandling, tool.ExceptionHandling)
            << attrTagT(_ExpandAttributedSource, tool.ExpandAttributedSource)
            << attrTagS(_FavorSizeOrSpeed, tool.FavorSizeOrSpeed)
            << attrTagT(_FloatingPointExceptions, tool.FloatingPointExceptions)
            << attrTagS(_FloatingPointModel, tool.FloatingPointModel)
            << attrTagT(_ForceConformanceInForLoopScope, tool.ForceConformanceInForLoopScope)
            << attrTagX(_ForcedIncludeFiles, tool.ForcedIncludeFiles, ";")
            << attrTagX(_ForcedUsingFiles, tool.ForcedUsingFiles, ";")
            << attrTagT(_FunctionLevelLinking, tool.FunctionLevelLinking)
            << attrTagT(_GenerateXMLDocumentationFiles, tool.GenerateXMLDocumentationFiles)
            << attrTagT(_IgnoreStandardIncludePath, tool.IgnoreStandardIncludePath)
            << attrTagS(_InlineFunctionExpansion, tool.InlineFunctionExpansion)
            << attrTagT(_IntrinsicFunctions, tool.IntrinsicFunctions)
            << attrTagT(_MinimalRebuild, tool.MinimalRebuild)
            << attrTagT(_MultiProcessorCompilation, tool.MultiProcessorCompilation)
            << attrTagS(_ObjectFileName, tool.ObjectFileName)
            << attrTagX(_ObjectFiles, tool.ObjectFiles, ";")
            << attrTagT(_OmitDefaultLibName, tool.OmitDefaultLibName)
            << attrTagT(_OmitFramePointers, tool.OmitFramePointers)
            << attrTagT(_OpenMPSupport, tool.OpenMPSupport)
            << attrTagS(_Optimization, tool.Optimization)
            << attrTagS(_PrecompiledHeader, tool.PrecompiledHeader)
            << attrTagS(_PrecompiledHeaderFile, tool.PrecompiledHeaderFile)
            << attrTagS(_PrecompiledHeaderOutputFile, tool.PrecompiledHeaderOutputFile)
            << attrTagT(_PreprocessKeepComments, tool.PreprocessKeepComments)
            << attrTagX(_PreprocessorDefinitions, tool.PreprocessorDefinitions, ";")
            << attrTagS(_PreprocessOutputPath, tool.PreprocessOutputPath)
            << attrTagT(_PreprocessSuppressLineNumbers, tool.PreprocessSuppressLineNumbers)
            << attrTagT(_PreprocessToFile, tool.PreprocessToFile)
            << attrTagS(_ProgramDataBaseFileName, tool.ProgramDataBaseFileName)
            << attrTagS(_ProcessorNumber, tool.ProcessorNumber)
            << attrTagS(_RuntimeLibrary, tool.RuntimeLibrary)
            << attrTagT(_RuntimeTypeInfo, tool.RuntimeTypeInfo)
            << attrTagT(_ShowIncludes, tool.ShowIncludes)
            << attrTagT(_SmallerTypeCheck, tool.SmallerTypeCheck)
            << attrTagT(_StringPooling, tool.StringPooling)
            << attrTagS(_StructMemberAlignment, tool.StructMemberAlignment)
            << attrTagT(_SuppressStartupBanner, tool.SuppressStartupBanner)
            << attrTagS(_TreatSpecificWarningsAsErrors, tool.TreatSpecificWarningsAsErrors)
            << attrTagT(_TreatWarningAsError, tool.TreatWarningAsError)
            << attrTagT(_TreatWChar_tAsBuiltInType, tool.TreatWChar_tAsBuiltInType)
            << attrTagT(_UndefineAllPreprocessorDefinitions, tool.UndefineAllPreprocessorDefinitions)
            << attrTagX(_UndefinePreprocessorDefinitions, tool.UndefinePreprocessorDefinitions, ";")
            << attrTagT(_UseFullPaths, tool.UseFullPaths)
            << attrTagT(_UseUnicodeForAssemblerListing, tool.UseUnicodeForAssemblerListing)
            << attrTagS(_WarningLevel, tool.WarningLevel)
            << attrTagT(_WholeProgramOptimization, tool.WholeProgramOptimization)
            << attrTagS(_XMLDocumentationFileName, tool.XMLDocumentationFileName)
        << closetag(_CLCompile);
}

bool VCXCLCompilerTool::parseOption(const char* option)
{
    // skip index 0 ('/' or '-')
    char first  = option[1];
    char second = option[2];
    char third  = option[3];
    char fourth = option[4];
    bool found = true;

    switch (first) {
    case '?':
        qWarning("Generator: Option '/?' : MSVC.NET projects do not support outputting help info");
        found = false;
        break;
    case '@':
        qWarning("Generator: Option '/@': MSVC.NET projects do not support the use of a response file");
        found = false;
        break;
    case 'l':
        qWarning("Generator: Option '/link': qmake generator does not support passing link options through the compiler tool");
        found = false;
        break;
    case 'A':
        if(second != 'I') {
            found = false;
            break;
        }
        AdditionalUsingDirectories += option+3;
        break;
    case 'C':
        PreprocessKeepComments = _True;
        break;
    case 'D':
        PreprocessorDefinitions += option+2;
        break;
    case 'E':
        if(second == 'H') {
            QString opt(option);
            if (opt.endsWith("EHa"))
                ExceptionHandling = "Async";
            else if (opt.endsWith("EHsc"))
                ExceptionHandling = "Sync";
            else if (opt.endsWith("EHs"))
                ExceptionHandling = "SyncCThrow";
            else {
                ExceptionHandling = "false";
            }
            break;
        }else if(second == 'P') {
            PreprocessSuppressLineNumbers = _True;
        }
        found = false;
        break;
    case 'F':
        if(second <= '9' && second >= '0') {
            AdditionalOptions += option;
            break;
        } else {
            switch (second) {
            case 'A':
                if(third == 'c') {
                    AssemblerOutput = "AssemblyAndMachineCode";
                    if(fourth == 's')
                        AssemblerOutput = "All";
                } else if(third == 's') {
                    AssemblerOutput = "AssemblyAndSourceCode";
                } else if(third == 'u') {
                    UseUnicodeForAssemblerListing = _True;
                } else {
                    AssemblerOutput = "AssemblyCode";
                }
                break;
            case 'a':
                AssemblerListingLocation = option+3;
                break;
            case 'C':
                UseFullPaths = _True;
                break;
            case 'd':
                ProgramDataBaseFileName = option+3;
                break;
            case 'I':
                ForcedIncludeFiles += option+3;
                break;
            case 'i':
                PreprocessOutputPath += option+3;
                break;
            case 'm':
                AdditionalOptions += option;
                break;
            case 'R':
                BrowseInformation = _True;
                BrowseInformationFile = option+3;
                break;
            case 'r':
                BrowseInformation = _True;
                BrowseInformationFile = option+3;
                break;
            case 'U':
                ForcedUsingFiles += option+3;
                break;
            case 'o':
                ObjectFileName = option+3;
                break;
            case 'p':
                PrecompiledHeaderOutputFile = option+3;
                break;
            case 'x':
                ExpandAttributedSource = _True;
                break;
            default:
                found = false;
                break;
            }
        }
        break;
    case 'G':
        switch (second) {
        case 'F':
            StringPooling = _True;
            break;
        case 'L':
            WholeProgramOptimization = _True;
            if(third == '-')
                WholeProgramOptimization = _False;
            break;
        case 'R':
            RuntimeTypeInfo = _True;
            if(third == '-')
                RuntimeTypeInfo = _False;
            break;
        case 'S':
            BufferSecurityCheck = _True;
            if(third == '-')
                BufferSecurityCheck = _False;
            break;
        case 'T':
            EnableFiberSafeOptimizations = _True;
            break;
        case 'd':
            CallingConvention = "Cdecl";
            break;
        case 'm':
            MinimalRebuild = _True;
            if(third == '-')
                MinimalRebuild = _False;
            break;
        case 'r':
            CallingConvention = "FastCall";
            break;
        case 'y':
            FunctionLevelLinking = _True;
            break;
        case 'z':
            CallingConvention = "StdCall";
            break;
        default:
            found = false;
            break;
        }
        break;
    case 'H':
        AdditionalOptions += option;
        break;
    case 'I':
        AdditionalIncludeDirectories += option+2;
        break;
    case 'L':
        if(second == 'D') {
            AdditionalOptions += option;
            break;
        }
        found = false;
        break;
    case 'M':
        if(second == 'D') {
            RuntimeLibrary = "MultiThreadedDLL";
            if(third == 'd')
                RuntimeLibrary = "MultiThreadedDebugDLL";
            break;
        } else if(second == 'P') {
            MultiProcessorCompilation = _True;
            ProcessorNumber = option+3;
            break;
        } else if(second == 'T') {
            RuntimeLibrary = "MultiThreaded";
            if(third == 'd')
                RuntimeLibrary = "MultiThreadedDebug";
            break;
        }
        found = false;
        break;
    case 'O':
        switch (second) {
        case '1':
            Optimization = "MinSpace";
            break;
        case '2':
            Optimization = "MaxSpeed";
            break;
        case 'b':
            if(third == '0')
                InlineFunctionExpansion = "Disabled";
            else if(third == '1')
                InlineFunctionExpansion = "OnlyExplicitInline";
            else if(third == '2')
                InlineFunctionExpansion = "AnySuitable";
            else
                found = false;
            break;
        case 'd':
            Optimization = "Disabled";
            break;
        case 'i':
            IntrinsicFunctions = _True;
            break;
        case 'p':
            if(third == 'e')
                OpenMPSupport = _True;
            else
                found = false;
            break;
        case 's':
            FavorSizeOrSpeed = "Size";
            break;
        case 't':
            FavorSizeOrSpeed = "Speed";
            break;
        case 'x':
            Optimization = "Full";
            break;
        case 'y':
            OmitFramePointers = _True;
            if(third == '-')
                OmitFramePointers = _False;
            break;
        default:
            found = false;
            break;
        }
        break;
    case 'P':
        PreprocessToFile = _True;
        break;
    case 'Q':
        if(second == 'I') {
            AdditionalOptions += option;
            break;
        }
        found = false;
        break;
    case 'R':
        if(second == 'T' && third == 'C') {
            if(fourth == '1')
                BasicRuntimeChecks = "EnableFastChecks";
            else if(fourth == 'c')
                SmallerTypeCheck = _True;
            else if(fourth == 's')
                BasicRuntimeChecks = "StackFrameRuntimeCheck";
            else if(fourth == 'u')
                BasicRuntimeChecks = "UninitializedLocalUsageCheck";
            else
                found = false; break;
        }
        break;
    case 'T':
        if(second == 'C') {
            CompileAs = "CompileAsC";
        } else if(second == 'P') {
            CompileAs = "CompileAsCpp";
        } else {
            qWarning("Generator: Options '/Tp<filename>' and '/Tc<filename>' are not supported by qmake");
            found = false; break;
        }
        break;
    case 'U':
        UndefinePreprocessorDefinitions += option+2;
        break;
    case 'V':
        AdditionalOptions += option;
        break;
    case 'W':
        switch (second) {
        case 'a':
            WarningLevel = "EnableAllWarnings";
            break;
        case '4':
            WarningLevel = "Level4";
            break;
        case '3':
            WarningLevel = "Level3";
            break;
        case '2':
            WarningLevel = "Level2";
            break;
        case '1':
            WarningLevel = "Level1";
            break;
        case '0':
            WarningLevel = "TurnOffAllWarnings";
            break;
        case 'L':
            AdditionalOptions += option;
            break;
        case 'X':
            TreatWarningAsError = _True;
            break;
        case 'p':
            if(third == '6' && fourth == '4') {
                // Deprecated for VS2010 but can be used under Additional Options.
                AdditionalOptions += option;
                break;
            }
            // Fallthrough
        default:
            found = false; break;
        }
        break;
    case 'X':
        IgnoreStandardIncludePath = _True;
        break;
    case 'Y':
        switch (second) {
        case '\0':
        case '-':
            AdditionalOptions += option;
            break;
        case 'c':
            PrecompiledHeader = "Create";
            PrecompiledHeaderFile = option+3;
            break;
        case 'd':
        case 'l':
            AdditionalOptions += option;
            break;
        case 'u':
            PrecompiledHeader = "Use";
            PrecompiledHeaderFile = option+3;
            break;
        default:
            found = false; break;
        }
        break;
    case 'Z':
        switch (second) {
        case '7':
            DebugInformationFormat = "OldStyle";
            break;
        case 'I':
            DebugInformationFormat = "EditAndContinue";
            break;
        case 'i':
            DebugInformationFormat = "ProgramDatabase";
            break;
        case 'l':
            OmitDefaultLibName = _True;
            break;
        case 'a':
            DisableLanguageExtensions = _True;
            break;
        case 'e':
            DisableLanguageExtensions = _False;
            break;
        case 'c':
            if(third == ':') {
                const char *c = option + 4;
                // Go to the end of the option
                while ( *c != '\0' && *c != ' ' && *c != '-')
                    ++c;
                if(fourth == 'f')
                    ForceConformanceInForLoopScope = ((*c) == '-' ? _False : _True);
                else if(fourth == 'w')
                    TreatWChar_tAsBuiltInType = ((*c) == '-' ? _False : _True);
                else
                    found = false;
            } else {
                found = false; break;
            }
            break;
        case 'g':
        case 'm':
        case 's':
            AdditionalOptions += option;
            break;
        case 'p':
            switch (third)
            {
            case '\0':
            case '1':
                StructMemberAlignment = "1Byte";
                if(fourth == '6')
                    StructMemberAlignment = "16Bytes";
                break;
            case '2':
                StructMemberAlignment = "2Bytes";
                break;
            case '4':
                StructMemberAlignment = "4Bytes";
                break;
            case '8':
                StructMemberAlignment = "8Bytes";
                break;
            default:
                found = false; break;
            }
            break;
        default:
            found = false; break;
        }
        break;
    case 'a':
        if (second == 'r' && third == 'c' && fourth == 'h') {
            if (option[5] == ':') {
                const char *o = option;
                if (o[6] == 'S' && o[7] == 'S' && o[8] == 'E') {
                    EnableEnhancedInstructionSet = o[9] == '2' ? "StreamingSIMDExtensions2" : "StreamingSIMDExtensions";
                    break;
                }
            }
        } else if (second == 'n' && third == 'a' && fourth == 'l') {
            EnablePREfast = _True;
            break;
        }
        found = false;
        break;
    case 'b':   // see http://msdn.microsoft.com/en-us/library/fwkeyyhe%28VS.100%29.aspx
        if (second == 'i' && third == 'g' && fourth == 'o') {
            const char *o = option;
            if (o[5] == 'b' && o[6] == 'j') {
                AdditionalOptions += option;
                break;
            }
        }
        found = false;
        break;
    case 'c':
        if(second == 'l') {
            if(*(option+5) == 'p') {
                CompileAsManaged = "Pure";
            } else if(*(option+5) == 's') {
                CompileAsManaged = "Safe";
            } else if(*(option+5) == 'o') {
                CompileAsManaged = "OldSyntax";
            } else {
                CompileAsManaged = "true";
            }
        } else {
            found = false;
            break;
        }
        break;
    case 'd':
        if(second != 'o' && third == 'c') {
            GenerateXMLDocumentationFiles = _True;
            XMLDocumentationFileName += option+4;
            break;
        }
        found = false;
        break;
    case 'e':
        if (second == 'r' && third == 'r' && fourth == 'o') {
            if (option[12] == ':') {
                if ( option[13] == 'n') {
                    ErrorReporting = "None";
                } else if (option[13] == 'p') {
                    ErrorReporting = "Prompt";
                } else if (option[13] == 'q') {
                    ErrorReporting = "Queue";
                } else if (option[13] == 's') {
                    ErrorReporting = "Send";
                } else {
                    found = false;
                }
                break;
            }
        }
        found = false;
        break;
    case 'f':
        if(second == 'p' && third == ':') {
            // Go to the end of the option
            const char *c = option + 4;
            while (*c != '\0' && *c != ' ' && *c != '-')
                ++c;
            switch (fourth) {
            case 'e':
                FloatingPointExceptions = ((*c) == '-' ? _False : _True);
                break;
            case 'f':
                FloatingPointModel = "Fast";
                break;
            case 'p':
                FloatingPointModel = "Precise";
                break;
            case 's':
                FloatingPointModel = "Strict";
                break;
            default:
                found = false;
                break;
            }
        }
        break;
    case 'h':
        if(second == 'o' && third == 't' && fourth == 'p') {
            CreateHotpatchableImage = _True;
            break;
        }
        qWarning("Generator: Option '/help': MSVC.NET projects do not support outputting help info");
        found = false;
        break;
    case 'n':
        if(second == 'o' && third == 'l' && fourth == 'o') {
            SuppressStartupBanner = _True;
            break;
        }
        found = false;
        break;
    case 'o':
        if (second == 'p' && third == 'e' && fourth == 'n') {
            OpenMPSupport = _True;
            break;
        }
        found = false;
        break;
    case 's':
        if(second == 'h' && third == 'o' && fourth == 'w') {
            ShowIncludes = _True;
            break;
        }
        found = false;
        break;
    case 'u':
        UndefineAllPreprocessorDefinitions = _True;
        break;
    case 'v':
        if(second == 'd' || second == 'm') {
            AdditionalOptions += option;
            break;
        }
        found = false;
        break;
    case 'w':
        switch (second) {
        case 'd':
            DisableSpecificWarnings += option+3;
            break;
        case 'e':
            TreatSpecificWarningsAsErrors = option+3;
            break;
        default:
            AdditionalOptions += option;
        }
        break;
    default:
        AdditionalOptions += option;
        break;
    }
    if(!found) {
        warn_msg(WarnLogic, "Could not parse Compiler option: %s, added as AdditionalOption", option);
        AdditionalOptions += option;
    }
    return true;
}

// VCLinkerTool -----------------------------------------------------
VCXLinkerTool::VCXLinkerTool()
    :        AllowIsolation(unset),
        AssemblyDebug(unset),
        DataExecutionPrevention(unset),
        DelaySign(unset),
        EnableCOMDATFolding(unset),
        EnableUAC(unset),
        FixedBaseAddress(unset),
        GenerateDebugInformation(unset),
        GenerateManifest(unset),
        GenerateMapFile(unset),
        HeapCommitSize(-1),
        HeapReserveSize(-1),
        IgnoreAllDefaultLibraries(unset),
        IgnoreEmbeddedIDL(unset),
        IgnoreImportLibrary(_True),
        ImageHasSafeExceptionHandlers(unset),
        LargeAddressAware(unset),
        LinkDLL(unset),
        LinkIncremental(unset),
        LinkStatus(unset),
        MapExports(unset),
        NoEntryPoint(unset),
        OptimizeReferences(unset),
        PreventDllBinding(unset),
        RandomizedBaseAddress(unset),
        RegisterOutput(unset),
        SectionAlignment(-1),
        SetChecksum(unset),
        //StackCommitSize(-1),
        //StackReserveSize(-1),
        SupportNobindOfDelayLoadedDLL(unset),
        SupportUnloadOfDelayLoadedDLL(unset),
        SuppressStartupBanner(unset),
        SwapRunFromCD(unset),
        SwapRunFromNet(unset),
        TerminalServerAware(unset),
        TreatLinkerWarningAsErrors(unset),
        TurnOffAssemblyGeneration(unset),
        TypeLibraryResourceID(0),
        UACUIAccess(unset)
{
}

XmlOutput &operator<<(XmlOutput &xml, const VCXLinkerTool &tool)
{
    return xml
        << tag(_Link)
            << attrTagX(_AdditionalDependencies, tool.AdditionalDependencies, ";")
            << attrTagX(_AdditionalLibraryDirectories, tool.AdditionalLibraryDirectories, ";")
            << attrTagX(_AdditionalManifestDependencies, tool.AdditionalManifestDependencies, ";")
            << attrTagX(_AdditionalOptions, tool.AdditionalOptions, " ")
            << attrTagX(_AddModuleNamesToAssembly, tool.AddModuleNamesToAssembly, ";")
            << attrTagT(_AllowIsolation, tool.AllowIsolation)
            << attrTagT(_AssemblyDebug, tool.AssemblyDebug)
            << attrTagX(_AssemblyLinkResource, tool.AssemblyLinkResource, ";")
            << attrTagS(_BaseAddress, tool.BaseAddress)
            << attrTagS(_CLRImageType, tool.CLRImageType)
            << attrTagS(_CLRSupportLastError, tool.CLRSupportLastError)
            << attrTagS(_CLRThreadAttribute, tool.CLRThreadAttribute)
            << attrTagS(_CLRUnmanagedCodeCheck, tool.CLRUnmanagedCodeCheck)
            << attrTagS(_CreateHotPatchableImage, tool.CreateHotPatchableImage)
            << attrTagT(_DataExecutionPrevention, tool.DataExecutionPrevention)
            << attrTagX(_DelayLoadDLLs, tool.DelayLoadDLLs, ";")
            << attrTagT(_DelaySign, tool.DelaySign)
            << attrTagS(_Driver, tool.Driver)
            << attrTagX(_EmbedManagedResourceFile, tool.EmbedManagedResourceFile, ";")
            << attrTagT(_EnableCOMDATFolding, tool.EnableCOMDATFolding)
            << attrTagT(_EnableUAC, tool.EnableUAC)
            << attrTagS(_EntryPointSymbol, tool.EntryPointSymbol)
            << attrTagT(_FixedBaseAddress, tool.FixedBaseAddress)
            << attrTagS(_ForceFileOutput, tool.ForceFileOutput)
            << attrTagX(_ForceSymbolReferences, tool.ForceSymbolReferences, ";")
            << attrTagS(_FunctionOrder, tool.FunctionOrder)
            << attrTagT(_GenerateDebugInformation, tool.GenerateDebugInformation)
            << attrTagT(_GenerateManifest, tool.GenerateManifest)
            << attrTagT(_GenerateMapFile, tool.GenerateMapFile)
            << attrTagL(_HeapCommitSize, tool.HeapCommitSize, /*ifNot*/ -1)
            << attrTagL(_HeapReserveSize, tool.HeapReserveSize, /*ifNot*/ -1)
            << attrTagT(_IgnoreAllDefaultLibraries, tool.IgnoreAllDefaultLibraries)
            << attrTagT(_IgnoreEmbeddedIDL, tool.IgnoreEmbeddedIDL)
            << attrTagT(_IgnoreImportLibrary, tool.IgnoreImportLibrary)
            << attrTagX(_IgnoreSpecificDefaultLibraries, tool.IgnoreSpecificDefaultLibraries, ";")
            << attrTagT(_ImageHasSafeExceptionHandlers, tool.ImageHasSafeExceptionHandlers)
            << attrTagS(_ImportLibrary, tool.ImportLibrary)
            << attrTagS(_KeyContainer, tool.KeyContainer)
            << attrTagS(_KeyFile, tool.KeyFile)
            << attrTagT(_LargeAddressAware, tool.LargeAddressAware)
            << attrTagT(_LinkDLL, tool.LinkDLL)
            << attrTagS(_LinkErrorReporting, tool.LinkErrorReporting)
            << attrTagT(_LinkIncremental, tool.LinkIncremental)
            << attrTagT(_LinkStatus, tool.LinkStatus)
            << attrTagS(_LinkTimeCodeGeneration, tool.LinkTimeCodeGeneration)
            << attrTagS(_ManifestFile, tool.ManifestFile)
            << attrTagT(_MapExports, tool.MapExports)
            << attrTagS(_MapFileName, tool.MapFileName)
            << attrTagS(_MergedIDLBaseFileName, tool.MergedIDLBaseFileName)
            << attrTagS(_MergeSections, tool.MergeSections)
            << attrTagS(_MidlCommandFile, tool.MidlCommandFile)
            << attrTagS(_ModuleDefinitionFile, tool.ModuleDefinitionFile)
            << attrTagS(_MSDOSStubFileName, tool.MSDOSStubFileName)
            << attrTagT(_NoEntryPoint, tool.NoEntryPoint)
            << attrTagT(_OptimizeReferences, tool.OptimizeReferences)
            << attrTagS(_OutputFile, tool.OutputFile)
            << attrTagT(_PreventDllBinding, tool.PreventDllBinding)
            << attrTagS(_Profile, tool.Profile)
            << attrTagS(_ProfileGuidedDatabase, tool.ProfileGuidedDatabase)
            << attrTagS(_ProgramDatabaseFile, tool.ProgramDatabaseFile)
            << attrTagT(_RandomizedBaseAddress, tool.RandomizedBaseAddress)
            << attrTagT(_RegisterOutput, tool.RegisterOutput)
            << attrTagL(_SectionAlignment, tool.SectionAlignment, /*ifNot*/ -1)
            << attrTagT(_SetChecksum, tool.SetChecksum)
            << attrTagS(_ShowProgress, tool.ShowProgress)
            << attrTagS(_SpecifySectionAttributes, tool.SpecifySectionAttributes)
            << attrTagS(_StackCommitSize, tool.StackCommitSize)
            << attrTagS(_StackReserveSize, tool.StackReserveSize)
            << attrTagS(_StripPrivateSymbols, tool.StripPrivateSymbols)
            << attrTagS(_SubSystem, tool.SubSystem)
            << attrTagT(_SupportNobindOfDelayLoadedDLL, tool.SupportNobindOfDelayLoadedDLL)
            << attrTagT(_SupportUnloadOfDelayLoadedDLL, tool.SupportUnloadOfDelayLoadedDLL)
            << attrTagT(_SuppressStartupBanner, tool.SuppressStartupBanner)
            << attrTagT(_SwapRunFromCD, tool.SwapRunFromCD)
            << attrTagT(_SwapRunFromNet, tool.SwapRunFromNet)
            << attrTagS(_TargetMachine, tool.TargetMachine)
            << attrTagT(_TerminalServerAware, tool.TerminalServerAware)
            << attrTagT(_TreatLinkerWarningAsErrors, tool.TreatLinkerWarningAsErrors)
            << attrTagT(_TurnOffAssemblyGeneration, tool.TurnOffAssemblyGeneration)
            << attrTagS(_TypeLibraryFile, tool.TypeLibraryFile)
            << attrTagL(_TypeLibraryResourceID, tool.TypeLibraryResourceID, /*ifNot*/ 0)
            << attrTagS(_UACExecutionLevel, tool.UACExecutionLevel)
            << attrTagT(_UACUIAccess, tool.UACUIAccess)
            << attrTagS(_Version, tool.Version)
        << closetag(_Link);
}

// Hashing routine to do fast option lookups ----
// Slightly rewritten to stop on ':' ',' and '\0'
// Original routine in qtranslator.cpp ----------
static uint elfHash(const char* name)
{
    const uchar *k;
    uint h = 0;
    uint g;

    if(name) {
        k = (const uchar *) name;
        while((*k) &&
                (*k)!= ':' &&
                (*k)!=',' &&
                (*k)!=' ') {
            h = (h << 4) + *k++;
            if((g = (h & 0xf0000000)) != 0)
                h ^= g >> 24;
            h &= ~g;
        }
    }
    if(!h)
        h = 1;
    return h;
}

//#define USE_DISPLAY_HASH
#ifdef USE_DISPLAY_HASH
static void displayHash(const char* str)
{
    printf("case 0x%07x: // %s\n    break;\n", elfHash(str), str);
}
#endif

bool VCXLinkerTool::parseOption(const char* option)
{
#ifdef USE_DISPLAY_HASH
    // Main options
    displayHash("/ALIGN"); displayHash("/ALLOWBIND"); displayHash("/ASSEMBLYMODULE");
    displayHash("/ASSEMBLYRESOURCE"); displayHash("/BASE"); displayHash("/DEBUG");
    displayHash("/DEF"); displayHash("/DEFAULTLIB"); displayHash("/DELAY");
    displayHash("/DELAYLOAD"); displayHash("/DLL"); displayHash("/DRIVER");
    displayHash("/ENTRY"); displayHash("/EXETYPE"); displayHash("/EXPORT");
    displayHash("/FIXED"); displayHash("/FORCE"); displayHash("/HEAP");
    displayHash("/IDLOUT"); displayHash("/IGNORE"); displayHash("/IGNOREIDL"); displayHash("/IMPLIB");
    displayHash("/INCLUDE"); displayHash("/INCREMENTAL"); displayHash("/LARGEADDRESSAWARE");
    displayHash("/LIBPATH"); displayHash("/LTCG"); displayHash("/MACHINE");
    displayHash("/MAP"); displayHash("/MAPINFO"); displayHash("/MERGE");
    displayHash("/MIDL"); displayHash("/NOASSEMBLY"); displayHash("/NODEFAULTLIB");
    displayHash("/NOENTRY"); displayHash("/NOLOGO"); displayHash("/OPT");
    displayHash("/ORDER"); displayHash("/OUT"); displayHash("/PDB");
    displayHash("/PDBSTRIPPED"); displayHash("/RELEASE"); displayHash("/SECTION");
    displayHash("/STACK"); displayHash("/STUB"); displayHash("/SUBSYSTEM");
    displayHash("/SWAPRUN"); displayHash("/TLBID"); displayHash("/TLBOUT");
    displayHash("/TSAWARE"); displayHash("/VERBOSE"); displayHash("/VERSION");
    displayHash("/VXD"); displayHash("/WS "); displayHash("/libpath");

#endif
#ifdef USE_DISPLAY_HASH
    // Sub options
    displayHash("UNLOAD"); displayHash("NOBIND"); displayHash("no"); displayHash("NOSTATUS"); displayHash("STATUS");
    displayHash("AM33"); displayHash("ARM"); displayHash("CEE"); displayHash("EBC"); displayHash("IA64"); displayHash("X86"); displayHash("X64"); displayHash("M32R");
    displayHash("MIPS"); displayHash("MIPS16"); displayHash("MIPSFPU"); displayHash("MIPSFPU16"); displayHash("MIPSR41XX"); displayHash("PPC");
    displayHash("SH3"); displayHash("SH3DSP"); displayHash("SH4"); displayHash("SH5"); displayHash("THUMB"); displayHash("TRICORE"); displayHash("EXPORTS");
    displayHash("LINES"); displayHash("REF"); displayHash("NOREF"); displayHash("ICF"); displayHash("WIN98"); displayHash("NOWIN98");
    displayHash("CONSOLE"); displayHash("EFI_APPLICATION"); displayHash("EFI_BOOT_SERVICE_DRIVER"); displayHash("EFI_ROM"); displayHash("EFI_RUNTIME_DRIVER"); displayHash("NATIVE");
    displayHash("POSIX"); displayHash("WINDOWS"); displayHash("WINDOWSCE"); displayHash("NET"); displayHash("CD"); displayHash("NO");
#endif
    bool found = true;
    switch (elfHash(option)) {
    case 0x6b21972: // /DEFAULTLIB:library
    case 0xaca9d75: // /EXETYPE[:DYNAMIC | :DEV386]
    case 0x3ad5444: // /EXPORT:entryname[,@ordinal[,NONAME]][,DATA]
    case 0x3dc3455: // /IGNORE:number,number,number,number  ### NOTE: This one is undocumented, but it is even used by Microsoft.
                    //                                      In recent versions of the Microsoft linker they have disabled this undocumented feature.
    case 0x0034bc4: // /VXD
        AdditionalOptions += option;
        break;
    case 0x3360dbe: // /ALIGN[:number]
        SectionAlignment = QString(option+7).toLongLong();
        break;
    case 0x1485c34: // /ALLOWBIND[:NO]
        if(*(option+10) == ':' && (*(option+11) == 'n' || *(option+11) == 'N'))
            PreventDllBinding = _False;
        else
            PreventDllBinding = _True;
        break;
    case 0x312011e: // /ALLOWISOLATION[:NO]
        if(*(option+15) == ':' && (*(option+16) == 'n' || *(option+16) == 'N'))
            AllowIsolation = _False;
        else
            AllowIsolation = _True;
        break;
    case 0x679c075: // /ASSEMBLYMODULE:filename
        AddModuleNamesToAssembly += option+15;
        break;
    case 0x75f35f7: // /ASSEMBLYDEBUG[:DISABLE]
        if(*(option+14) == ':' && (*(option+15) == 'D'))
            AssemblyDebug = _False;
        else
            AssemblyDebug = _True;
        break;
    case 0x43294a5: // /ASSEMBLYLINKRESOURCE:filename
            AssemblyLinkResource += option+22;
        break;
    case 0x062d065: // /ASSEMBLYRESOURCE:filename
        EmbedManagedResourceFile += option+18;
        break;
    case 0x0336675: // /BASE:{address | @filename,key}
        // Do we need to do a manual lookup when '@filename,key'?
        // Seems BaseAddress only can contain the location...
        // We don't use it in Qt, so keep it simple for now
        BaseAddress = option+6;
        break;
    case 0x63bf065: // /CLRIMAGETYPE:{IJW|PURE|SAFE}
        if(*(option+14) == 'I')
            CLRImageType = "ForceIJWImage";
        else if(*(option+14) == 'P')
            CLRImageType = "ForcePureILImage";
        else if(*(option+14) == 'S')
            CLRImageType = "ForceSafeILImage";
        break;
    case 0x5f2a6a2: // /CLRSUPPORTLASTERROR{:NO | SYSTEMDLL}
        if(*(option+20) == ':') {
            if(*(option+21) == 'N') {
                CLRSupportLastError = "Disabled";
            } else if(*(option+21) == 'S') {
                CLRSupportLastError = "SystemDlls";
            }
        } else {
            CLRSupportLastError = "Enabled";
        }
        break;
    case 0xc7984f5: // /CLRTHREADATTRIBUTE:{STA|MTA|NONE}
        if(*(option+20) == 'N')
            CLRThreadAttribute = "DefaultThreadingAttribute";
        else if(*(option+20) == 'M')
            CLRThreadAttribute = "MTAThreadingAttribute";
        else if(*(option+20) == 'S')
            CLRThreadAttribute = "STAThreadingAttribute";
        break;
    case 0xa8c637b: // /CLRUNMANAGEDCODECHECK[:NO]
        if(*(option+23) == 'N')
            CLRUnmanagedCodeCheck = _False;
        else
            CLRUnmanagedCodeCheck = _True;
        break;
    case 0x3389797: // /DEBUG
        GenerateDebugInformation = _True;
        break;
    case 0x0033896: // /DEF:filename
        ModuleDefinitionFile = option+5;
        break;
    case 0x338a069: // /DELAY:{UNLOAD | NOBIND}
        if(*(option+7) == 'U')
            SupportNobindOfDelayLoadedDLL = _True;
        else if(*(option+7) == 'N')
            SupportUnloadOfDelayLoadedDLL = _True;
        break;
    case 0x06f4bf4: // /DELAYLOAD:dllname
        DelayLoadDLLs += option+11;
        break;
    case 0x06d451e: // /DELAYSIGN[:NO]
        if(*(option+10) == ':' && (*(option+11) == 'n' || *(option+11) == 'N'))
            DelaySign = _False;
        else
            DelaySign = _True;
        break;
    case 0x003390c: // /DLL
        LinkDLL = _True;
        break;
    case 0x396ea92: // /DRIVER[:UPONLY | :WDM]
        if((*(option+7) == ':') && (*(option+8) == 'U'))
            Driver = "UpOnly";
        else if((*(option+7) == ':') && (*(option+8) == 'W'))
            Driver = "WDM";
        else
            Driver = "Driver";
        break;
    case 0x2ee8415: // /DYNAMICBASE[:NO]
        if(*(option+12) == ':' && (*(option+13) == 'n' || *(option+13) == 'N'))
            RandomizedBaseAddress = _False;
        else
            RandomizedBaseAddress = _True;
        break;
    case 0x33a3979: // /ENTRY:function
        EntryPointSymbol = option+7;
        break;
    case 0x4504334: // /ERRORREPORT:[ NONE | PROMPT | QUEUE | SEND ]
        if(*(option+12) == ':' ) {
            if(*(option+13) == 'N')
                LinkErrorReporting = "NoErrorReport";
            else if(*(option+13) == 'P')
                LinkErrorReporting = "PromptImmediately";
            else if(*(option+13) == 'Q')
                LinkErrorReporting = "QueueForNextLogin";
            else if(*(option+13) == 'S')
                LinkErrorReporting = "SendErrorReport";
        }
        break;
    case 0x33aec94: // /FIXED[:NO]
        if(*(option+6) == ':' && (*(option+7) == 'n' || *(option+7) == 'N'))
            FixedBaseAddress = _False;
        else
            FixedBaseAddress = _True;
        break;
    case 0x33b4675: // /FORCE:[MULTIPLE|UNRESOLVED]
        if(*(option+6) == ':' && *(option+7) == 'M' )
            ForceFileOutput = "MultiplyDefinedSymbolOnly";
        else if(*(option+6) == ':' && *(option+7) == 'U' )
            ForceFileOutput = "UndefinedSymbolOnly";
        else
            ForceFileOutput = "Enabled";
        break;
    case 0x96d4e4e: // /FUNCTIONPADMIN[:space]
        if(*(option+15) == ':') {
            if(*(option+16) == '5')
                CreateHotPatchableImage = "X86Image";
            else if(*(option+16) == '6')
                CreateHotPatchableImage = "X64Image";
            else if((*(option+16) == '1') && (*(option+17) == '6'))
                CreateHotPatchableImage = "ItaniumImage";
        } else {
            CreateHotPatchableImage = "Enabled";
        }
        break;
    case 0x033c960: // /HEAP:reserve[,commit]
        {
            QStringList both = QString(option+6).split(",");
            HeapReserveSize = both[0].toLongLong();
            if(both.count() == 2)
                HeapCommitSize = both[1].toLongLong();
        }
        break;
    case 0x3d91494: // /IDLOUT:[path\]filename
        MergedIDLBaseFileName = option+8;
        break;
    case 0x345a04c: // /IGNOREIDL
        IgnoreEmbeddedIDL = _True;
        break;
    case 0x3e250e2: // /IMPLIB:filename
        ImportLibrary = option+8;
        break;
    case 0xe281ab5: // /INCLUDE:symbol
        ForceSymbolReferences += option+9;
        break;
    case 0xb28103c: // /INCREMENTAL[:no]
        if(*(option+12) == ':' &&
             (*(option+13) == 'n' || *(option+13) == 'N'))
            LinkIncremental = _False;
        else
            LinkIncremental = _True;
        break;
    case 0x07f1ab2: // /KEYCONTAINER:name
        KeyContainer = option+14;
        break;
    case 0xfadaf35: // /KEYFILE:filename
        KeyFile = option+9;
        break;
    case 0x26e4675: // /LARGEADDRESSAWARE[:no]
        if(*(option+18) == ':' &&
             *(option+19) == 'n')
            LargeAddressAware = _False;
        else
            LargeAddressAware = _True;
        break;
    case 0x2f96bc8: // /libpath:dir
    case 0x0d745c8: // /LIBPATH:dir
        AdditionalLibraryDirectories += option+9;
        break;
    case 0x0341877: // /LTCG[:NOSTATUS|:STATUS]
        config->WholeProgramOptimization = _True;
        LinkTimeCodeGeneration = "UseLinkTimeCodeGeneration";
        if(*(option+5) == ':') {
            const char* str = option+6;
            if (*str == 'S')
                LinkStatus = _True;
            else if (*str == 'N')
                LinkStatus = _False;
#ifndef Q_OS_WIN
            else if (strncasecmp(str, "pginstrument", 12))
                LinkTimeCodeGeneration = "PGInstrument";
            else if (strncasecmp(str, "pgoptimize", 10))
                LinkTimeCodeGeneration = "PGOptimization";
            else if (strncasecmp(str, "pgupdate", 8 ))
                LinkTimeCodeGeneration = "PGUpdate";
#else

            else if (_stricmp(str, "pginstrument"))
                LinkTimeCodeGeneration = "PGInstrument";
            else if (_stricmp(str, "pgoptimize"))
                LinkTimeCodeGeneration = "PGOptimization";
            else if (_stricmp(str, "pgupdate"))
                LinkTimeCodeGeneration = "PGUpdate";
#endif
        }
        break;
	case 0x379ED25:
    case 0x157cf65: // /MACHINE:{AM33|ARM|CEE|IA64|X86|M32R|MIPS|MIPS16|MIPSFPU|MIPSFPU16|MIPSR41XX|PPC|SH3|SH4|SH5|THUMB|TRICORE}
        switch (elfHash(option+9)) {
        case 0x0005bb6: // X86
            TargetMachine = "MachineX86";
            break;
        case 0x0005b94: // X64
            TargetMachine = "MachineX64";
            break;
        case 0x000466d: // ARM
            TargetMachine = "MachineARM";
            break;
        case 0x0004963: // EBC
            TargetMachine = "MachineEBC";
            break;
        case 0x004d494: // IA64
            TargetMachine = "MachineIA64";
            break;
        case 0x0051e53: // MIPS
            TargetMachine = "MachineMIPS";
            break;
        case 0x51e5646: // MIPS16
            TargetMachine = "MachineMIPS16";
            break;
        case 0x1e57b05: // MIPSFPU
            TargetMachine = "MachineMIPSFPU";
            break;
        case 0x57b09a6: // MIPSFPU16
            TargetMachine = "MachineMIPSFPU16";
            break;
        case 0x00057b4: // SH4
            TargetMachine = "MachineSH4";
            break;
        case 0x058da12: // THUMB
            TargetMachine = "MachineTHUMB";
            break;
        // put the others in AdditionalOptions...
        case 0x0046063: // AM33
        case 0x0004795: // CEE
        case 0x0050672: // M32R
        case 0x5852738: // MIPSR41XX
        case 0x0005543: // PPC
        case 0x00057b3: // SH3
        case 0x57b7980: // SH3DSP
        case 0x00057b5: // SH5
        case 0x96d8435: // TRICORE
        default:
            AdditionalOptions += option;
            break;
        }
        break;
    case 0x62d9e94: // /MANIFEST[:NO]
        if ((*(option+9) == ':' && (*(option+10) == 'N' || *(option+10) == 'n')))
            GenerateManifest = _False;
        else
            GenerateManifest = _True;
        break;
    case 0x8b64559: // /MANIFESTDEPENDENCY:manifest_dependency
        AdditionalManifestDependencies += option+20;
        break;
    case 0xe9e8195: // /MANIFESTFILE:filename
        ManifestFile = option+14;
        break;
    case 0x9e9fb83: // /MANIFESTUAC http://msdn.microsoft.com/en-us/library/bb384691%28VS.100%29.aspx
        if ((*(option+12) == ':' && (*(option+13) == 'N' || *(option+13) == 'n')))
            EnableUAC = _False;
        else if((*(option+12) == ':' && (*(option+13) == 'l' || *(option+14) == 'e'))) { // level
            if(*(option+20) == 'a')
                UACExecutionLevel = "AsInvoker";
            else if(*(option+20) == 'h')
                UACExecutionLevel = "HighestAvailable";
            else if(*(option+20) == 'r')
                UACExecutionLevel = "RequireAdministrator";
        } else if((*(option+12) == ':' && (*(option+13) == 'u' || *(option+14) == 'i'))) { // uiAccess
            if(*(option+22) == 't')
                UACUIAccess = _True;
            else
                UACUIAccess = _False;
        } else if((*(option+12) == ':' && (*(option+13) == 'f' || *(option+14) == 'r'))) { // fragment
            AdditionalOptions += option;
        }else
            EnableUAC = _True;
        break;
    case 0x0034160: // /MAP[:filename]
        GenerateMapFile = _True;
        if (option[4] == ':')
            MapFileName = option+5;
        break;
    case 0x164e1ef: // /MAPINFO:{EXPORTS}
        if(*(option+9) == 'E')
            MapExports = _True;
        break;
    case 0x341a6b5: // /MERGE:from=to
        MergeSections = option+7;
        break;
    case 0x0341d8c: // /MIDL:@file
        MidlCommandFile = option+7;
        break;
    case 0x84e2679: // /NOASSEMBLY
        TurnOffAssemblyGeneration = _True;
        break;
    case 0x2b21942: // /NODEFAULTLIB[:library]
        if(*(option+13) == '\0')
            IgnoreAllDefaultLibraries = _True;
        else
            IgnoreSpecificDefaultLibraries += option+14;
        break;
    case 0x33a3a39: // /NOENTRY
        NoEntryPoint = _True;
        break;
    case 0x434138f: // /NOLOGO
        SuppressStartupBanner = _True;
        break;
    case 0xc841054: // /NXCOMPAT[:NO]
        if ((*(option+9) == ':' && (*(option+10) == 'N' || *(option+10) == 'n')))
            DataExecutionPrevention = _False;
        else
            DataExecutionPrevention = _True;
        break;
    case 0x0034454: // /OPT:{REF | NOREF | ICF[=iterations] | NOICF | WIN98 | NOWIN98}
        {
            char third = *(option+7);
            switch (third) {
            case 'F': // REF
                if(*(option+5) == 'R') {
                    OptimizeReferences = _True;
                } else { // ICF[=iterations]
                    EnableCOMDATFolding = _True;
                    // [=iterations] case is not documented
                }
                break;
            case 'R': // NOREF
                OptimizeReferences = _False;
                break;
            case 'I': // NOICF
                EnableCOMDATFolding = _False;
                break;
            default:
                found = false;
            }
        }
        break;
    case 0x34468a2: // /ORDER:@filename
        FunctionOrder = option+8;
        break;
    case 0x00344a4: // /OUT:filename
        OutputFile = option+5;
        break;
    case 0x0034482: // /PDB:filename
        ProgramDatabaseFile = option+5;
        break;
    case 0xa2ad314: // /PDBSTRIPPED:pdb_file_name
        StripPrivateSymbols = option+13;
        break;
    case 0x00344b4: // /PGD:filename
        ProfileGuidedDatabase = option+5;
        break;
    case 0x573af45: // /PROFILE
        Profile = _True;
        break;
    case 0x6a09535: // /RELEASE
        SetChecksum = _True;
        break;
    case 0x75AA4D8: // /SAFESEH:{NO}
        {
            if(*(option+8) == ':' && *(option+9) == 'N')
                ImageHasSafeExceptionHandlers = _False;
            else
                ImageHasSafeExceptionHandlers = _True;
        }
        break;
    case 0x7988f7e: // /SECTION:name,[E][R][W][S][D][K][L][P][X][,ALIGN=#]
        SpecifySectionAttributes = option+9;
        break;
    case 0x348857b: // /STACK:reserve[,commit]
        {
            QStringList both = QString(option+7).split(",");
            StackReserveSize = both[0].toLongLong();
            if(both.count() == 2)
                StackCommitSize = both[1].toLongLong();
        }
        break;
    case 0x0348992: // /STUB:filename
        MSDOSStubFileName = option+6;
        break;
    case 0x9B3C00D:
    case 0x78dc00d: // /SUBSYSTEM:{CONSOLE|EFI_APPLICATION|EFI_BOOT_SERVICE_DRIVER|EFI_ROM|EFI_RUNTIME_DRIVER|NATIVE|POSIX|WINDOWS|WINDOWSCE}[,major[.minor]]
        {
            // Split up in subsystem, and version number
            QStringList both = QString(option+11).split(",");
            switch (elfHash(both[0].toLatin1())) {
            case 0x8438445: // CONSOLE
                SubSystem = "Console";
                break;
            case 0xbe29493: // WINDOWS
                SubSystem = "Windows";
                break;
            case 0x5268ea5: // NATIVE
                SubSystem = "Native";
                break;
            case 0x240949e: // EFI_APPLICATION
                SubSystem = "EFI Application";
                break;
            case 0xe617652: // EFI_BOOT_SERVICE_DRIVER
                SubSystem = "EFI Boot Service Driver";
                break;
            case 0x9af477d: // EFI_ROM
                SubSystem = "EFI ROM";
                break;
            case 0xd34df42: // EFI_RUNTIME_DRIVER
                SubSystem = "EFI Runtime";
                break;
            case 0x2949c95: // WINDOWSCE
                SubSystem = "WindowsCE";
                break;
            case 0x05547e8: // POSIX
                SubSystem = "POSIX";
                break;
            // The following are undocumented, so add them to AdditionalOptions
            case 0x4B69795: // windowsce
                AdditionalOptions += option;
                break;
            default:
                found = false;
            }
        }
        break;
    case 0x8b654de: // /SWAPRUN:{NET | CD}
        if(*(option+9) == 'N')
            SwapRunFromNet = _True;
        else if(*(option+9) == 'C')
            SwapRunFromCD = _True;
        else
            found = false;
        break;
    case 0x34906d4: // /TLBID:id
        TypeLibraryResourceID = QString(option+7).toLongLong();
        break;
    case 0x4907494: // /TLBOUT:[path\]filename
        TypeLibraryFile = option+8;
        break;
    case 0x976b525: // /TSAWARE[:NO]
        if(*(option+8) == ':')
            TerminalServerAware = _False;
        else
            TerminalServerAware = _True;
        break;
    case 0xaa67735: // /VERBOSE[:ICF |:LIB |:REF |:SAFESEH]
        if(*(option+9) == ':') {
            if (*(option+10) == 'I') {
                ShowProgress = "LinkVerboseICF";
            } else if ( *(option+10) == 'L') {
                ShowProgress = "LinkVerboseLib";
            } else if ( *(option+10) == 'R') {
                ShowProgress = "LinkVerboseREF";
            } else if ( *(option+10) == 'S') {
                ShowProgress = "LinkVerboseSAFESEH";
            } else if ( *(option+10) == 'C') {
                ShowProgress = "LinkVerboseCLR";
            }
        } else {
            ShowProgress = "LinkVerbose";
        }
        break;
    case 0xaa77f7e: // /VERSION:major[.minor]
        Version = option+9;
        break;
    case 0x0034c50: // /WS[:NO]
        if(*(option+3) == ':')
            TreatLinkerWarningAsErrors = _False;
        else
            TreatLinkerWarningAsErrors = _True;
        break;
    default:
        AdditionalOptions += option;
        break;
    }
    if(!found) {
        warn_msg(WarnLogic, "Could not parse Linker options: %s, added as AdditionalOption", option);
        AdditionalOptions += option;
    }
    return found;
}

// VCMIDLTool -------------------------------------------------------
VCXMIDLTool::VCXMIDLTool()
    :   ApplicationConfigurationMode(unset),
        ErrorCheckAllocations(unset),
        ErrorCheckBounds(unset),
        ErrorCheckEnumRange(unset),
        ErrorCheckRefPointers(unset),
        ErrorCheckStubData(unset),
        GenerateStublessProxies(unset),
        GenerateTypeLibrary(unset),
        IgnoreStandardIncludePath(unset),
        LocaleID(-1),
        MkTypLibCompatible(unset),
        SuppressCompilerWarnings(unset),
        SuppressStartupBanner(unset),
        ValidateAllParameters(unset),
        WarnAsError(unset)
{
}

XmlOutput &operator<<(XmlOutput &xml, const VCXMIDLTool &tool)
{
    return xml
        << tag(_Midl)
            << attrTagX(_AdditionalIncludeDirectories, tool.AdditionalIncludeDirectories, ";")
            << attrTagX(_AdditionalOptions, tool.AdditionalOptions, " ")
            << attrTagT(_ApplicationConfigurationMode, tool.ApplicationConfigurationMode)
            << attrTagS(_ClientStubFile, tool.ClientStubFile)
            << attrTagS(_CPreprocessOptions, tool.CPreprocessOptions)
            << attrTagS(_DefaultCharType, tool.DefaultCharType)
            << attrTagS(_DLLDataFileName, tool.DLLDataFileName)
            << attrTagS(_EnableErrorChecks, tool.EnableErrorChecks)
            << attrTagT(_ErrorCheckAllocations, tool.ErrorCheckAllocations)
            << attrTagT(_ErrorCheckBounds, tool.ErrorCheckBounds)
            << attrTagT(_ErrorCheckEnumRange, tool.ErrorCheckEnumRange)
            << attrTagT(_ErrorCheckRefPointers, tool.ErrorCheckRefPointers)
            << attrTagT(_ErrorCheckStubData, tool.ErrorCheckStubData)
            << attrTagS(_GenerateClientFiles, tool.GenerateClientFiles)
            << attrTagS(_GenerateServerFiles, tool.GenerateServerFiles)
            << attrTagT(_GenerateStublessProxies, tool.GenerateStublessProxies)
            << attrTagT(_GenerateTypeLibrary, tool.GenerateTypeLibrary)
            << attrTagS(_HeaderFileName, tool.HeaderFileName)
            << attrTagT(_IgnoreStandardIncludePath, tool.IgnoreStandardIncludePath)
            << attrTagS(_InterfaceIdentifierFileName, tool.InterfaceIdentifierFileName)
            << attrTagL(_LocaleID, tool.LocaleID, /*ifNot*/ -1)
            << attrTagT(_MkTypLibCompatible, tool.MkTypLibCompatible)
            << attrTagS(_OutputDirectory, tool.OutputDirectory)
            << attrTagX(_PreprocessorDefinitions, tool.PreprocessorDefinitions, ";")
            << attrTagS(_ProxyFileName, tool.ProxyFileName)
            << attrTagS(_RedirectOutputAndErrors, tool.RedirectOutputAndErrors)
            << attrTagS(_ServerStubFile, tool.ServerStubFile)
            << attrTagS(_StructMemberAlignment, tool.StructMemberAlignment)
            << attrTagT(_SuppressCompilerWarnings, tool.SuppressCompilerWarnings)
            << attrTagT(_SuppressStartupBanner, tool.SuppressStartupBanner)
            << attrTagS(_TargetEnvironment, tool.TargetEnvironment)
            << attrTagS(_TypeLibFormat, tool.TypeLibFormat)
            << attrTagS(_TypeLibraryName, tool.TypeLibraryName)
            << attrTagX(_UndefinePreprocessorDefinitions, tool.UndefinePreprocessorDefinitions, ";")
            << attrTagT(_ValidateAllParameters, tool.ValidateAllParameters)
            << attrTagT(_WarnAsError, tool.WarnAsError)
            << attrTagS(_WarningLevel, tool.WarningLevel)
        << closetag(_Midl);
}

bool VCXMIDLTool::parseOption(const char* option)
{
#ifdef USE_DISPLAY_HASH
    displayHash("/D name[=def]"); displayHash("/I directory-list"); displayHash("/Oi");
    displayHash("/Oic"); displayHash("/Oicf"); displayHash("/Oif"); displayHash("/Os");
    displayHash("/U name"); displayHash("/WX"); displayHash("/W{0|1|2|3|4}");
    displayHash("/Zp {N}"); displayHash("/Zs"); displayHash("/acf filename");
    displayHash("/align {N}"); displayHash("/app_config"); displayHash("/c_ext");
    displayHash("/char ascii7"); displayHash("/char signed"); displayHash("/char unsigned");
    displayHash("/client none"); displayHash("/client stub"); displayHash("/confirm");
    displayHash("/cpp_cmd cmd_line"); displayHash("/cpp_opt options");
    displayHash("/cstub filename"); displayHash("/dlldata filename"); displayHash("/env win32");
    displayHash("/env win64"); displayHash("/error all"); displayHash("/error allocation");
    displayHash("/error bounds_check"); displayHash("/error enum"); displayHash("/error none");
    displayHash("/error ref"); displayHash("/error stub_data"); displayHash("/h filename");
    displayHash("/header filename"); displayHash("/iid filename"); displayHash("/lcid");
    displayHash("/mktyplib203"); displayHash("/ms_ext"); displayHash("/ms_union");
    displayHash("/msc_ver <nnnn>"); displayHash("/newtlb"); displayHash("/no_cpp");
    displayHash("/no_def_idir"); displayHash("/no_default_epv"); displayHash("/no_format_opt");
    displayHash("/no_warn"); displayHash("/nocpp"); displayHash("/nologo"); displayHash("/notlb");
    displayHash("/o filename"); displayHash("/oldnames"); displayHash("/oldtlb");
    displayHash("/osf"); displayHash("/out directory"); displayHash("/pack {N}");
    displayHash("/prefix all"); displayHash("/prefix client"); displayHash("/prefix server");
    displayHash("/prefix switch"); displayHash("/protocol all"); displayHash("/protocol dce");
    displayHash("/protocol ndr64"); displayHash("/proxy filename"); displayHash("/robust");
    displayHash("/rpcss"); displayHash("/savePP"); displayHash("/server none");
    displayHash("/server stub"); displayHash("/sstub filename"); displayHash("/syntax_check");
    displayHash("/target {system}"); displayHash("/tlb filename"); displayHash("/use_epv");
    displayHash("/win32"); displayHash("/win64");
#endif
    bool found = true;
    int offset = 0;
    switch(elfHash(option)) {
    case 0x0000334: // /D name[=def]
        PreprocessorDefinitions += option+3;
        break;
    case 0x0000339: // /I directory-list
        AdditionalIncludeDirectories += option+3;
        break;
    case 0x0345f96: // /Oicf
    case 0x00345f6: // /Oif
        GenerateStublessProxies = _True;
        break;
    case 0x0000345: // /U name
        UndefinePreprocessorDefinitions += option+3;
        break;
    case 0x00034c8: // /WX
        WarnAsError = _True;
        break;
    case 0x3582fde: // /align {N}
        offset = 3;  // Fallthrough
    case 0x0003510: // /Zp {N}
        switch (*(option+offset+4)) {
        case '1':
            StructMemberAlignment = (*(option+offset+5) == '\0') ? "1" : "16";
            break;
        case '2':
            StructMemberAlignment = "2";
            break;
        case '4':
            StructMemberAlignment = "4";
            break;
        case '8':
            StructMemberAlignment = "8";
            break;
        default:
            found = false;
        }
        break;
    case 0x5b1cb97: // /app_config
        ApplicationConfigurationMode = _True;
        break;
    case 0x0359e82: // /char {ascii7|signed|unsigned}
        switch(*(option+6)) {
        case 'a':
            DefaultCharType = "Ascii";
            break;
        case 's':
            DefaultCharType = "Signed";
            break;
        case 'u':
            DefaultCharType = "Unsigned";
            break;
        default:
            found = false;
        }
        break;
    case 0x5a2fc64: // /client {none|stub}
        if(*(option+8) == 's')
            GenerateClientFiles = "Stub";
        else
            GenerateClientFiles = "None";
        break;
    case 0xa766524: // /cpp_opt options
        CPreprocessOptions += option+9;
        break;
    case 0x35aabb2: // /cstub filename
        ClientStubFile = option+7;
        break;
    case 0xb32abf1: // /dlldata filename
        DLLDataFileName = option + 9;
        break;
    case 0x0035c56: // /env {win32|ia64|x64}
        if(*(option+7) == 'w' && *(option+10) == '3')
            TargetEnvironment = "Win32";
        else if(*(option+7) == 'i')
            TargetEnvironment = "Itanium";
        else if(*(option+7) == 'x')
            TargetEnvironment = "X64";
        else
            AdditionalOptions += option;
        break;
    case 0x35c9962: // /error {all|allocation|bounds_check|enum|none|ref|stub_data}
        EnableErrorChecks = midlEnableCustom;
        switch (*(option+7)) {
        case '\0':
            EnableErrorChecks = "EnableCustom";
            break;
        case 'a':
            if(*(option+10) == '\0')
                EnableErrorChecks = "All";
            else
                ErrorCheckAllocations = _True;
            break;
        case 'b':
            ErrorCheckBounds = _True;
            break;
        case 'e':
            ErrorCheckEnumRange = _True;
            break;
        case 'n':
            EnableErrorChecks = "None";
            break;
        case 'r':
            ErrorCheckRefPointers = _True;
            break;
        case 's':
            ErrorCheckStubData = _True;
            break;
        default:
            found = false;
        }
        break;
    case 0x5eb7af2: // /header filename
        offset = 5;
    case 0x0000358: // /h filename
        HeaderFileName = option + offset + 3;
        break;
    case 0x0035ff4: // /iid filename
        InterfaceIdentifierFileName = option+5;
        break;
    case 0x64b7933: // /mktyplib203
        MkTypLibCompatible = _True;
        break;
    case 0x64ceb12: // /newtlb
        TypeLibFormat = "NewFormat";
        break;
    case 0x8e0b0a2: // /no_def_idir
        IgnoreStandardIncludePath = _True;
        break;
    case 0x65635ef: // /nologo
        SuppressStartupBanner = _True;
        break;
    case 0x695e9f4: // /no_robust
        ValidateAllParameters = _False;
        break;
    case 0x3656b22: // /notlb
        GenerateTypeLibrary = _True;
        break;
    case 0x556dbee: // /no_warn
        SuppressCompilerWarnings = _True;
        break;
    case 0x000035f: // /o filename
        RedirectOutputAndErrors = option+3;
        break;
    case 0x662bb12: // /oldtlb
        TypeLibFormat = "OldFormat";
        break;
    case 0x00366c4: // /out directory
        OutputDirectory = option+5;
        break;
    case 0x36796f9: // /proxy filename
        ProxyFileName = option+7;
        break;
    case 0x6959c94: // /robust
        ValidateAllParameters = _True;
        break;
    case 0x69c9cf2: // /server {none|stub}
        if(*(option+8) == 's')
            GenerateServerFiles = "Stub";
        else
            GenerateServerFiles = "None";
        break;
    case 0x36aabb2: // /sstub filename
        ServerStubFile = option+7;
        break;
    case 0x0036b22: // /tlb filename
        TypeLibraryName = option+5;
        break;
    case 0x36e0162: // /win32
        TargetEnvironment = "Win32";
        break;
    case 0x36e0194: // /win64
        TargetEnvironment = "Itanium";
        break;
    case 0x0003459: // /Oi
    case 0x00345f3: // /Oic
    case 0x0003463: // /Os
    case 0x0003513: // /Zs
    case 0x0035796: // /acf filename
    case 0x3595cf4: // /c_ext
    case 0xa64d3dd: // /confirm
    case 0xa765b64: // /cpp_cmd cmd_line
    case 0x03629f4: // /lcid
    case 0x6495cc4: // /ms_ext
    case 0x96c7a1e: // /ms_union
    case 0x4996fa2: // /msc_ver <nnnn>
    case 0x6555a40: // /no_cpp
    case 0xf64d6a6: // /no_default_epv
    case 0x6dd9384: // /no_format_opt
    case 0x3655a70: // /nocpp
    case 0x2b455a3: // /oldnames
    case 0x0036696: // /osf
    case 0x036679b: // /pack {N}
    case 0x678bd38: // /prefix {all|client|server|switch}
    case 0x96b702c: // /protocol {all|dce|ndr64}
    case 0x3696aa3: // /rpcss
    case 0x698ca60: // /savePP
    case 0xce9b12b: // /syntax_check
    case 0xc9b5f16: // /use_epv
        AdditionalOptions += option;
        break;
    default:
        // /W{0|1|2|3|4} case
        if(*(option+1) == 'W') {
            switch (*(option+2)) {
            case '0':
                WarningLevel = "0";
                break;
            case '1':
                WarningLevel = "1";
                break;
            case '2':
                WarningLevel = "2";
                break;
            case '3':
                WarningLevel = "3";
                break;
            case '4':
                WarningLevel = "4";
                break;
            default:
                found = false;
            }
        }
        break;
    }
    if(!found)
        warn_msg(WarnLogic, "Could not parse MIDL option: %s", option);
    return true;
}

// VCLibrarianTool --------------------------------------------------
VCXLibrarianTool::VCXLibrarianTool()
    :        IgnoreAllDefaultLibraries(unset),
        LinkTimeCodeGeneration(unset),
        SuppressStartupBanner(_True),
        TreatLibWarningAsErrors(unset),
        Verbose(unset)
{
}

XmlOutput &operator<<(XmlOutput &xml, const VCXLibrarianTool &tool)
{
    return xml
        << tag(_Link)
            << attrTagX(_AdditionalDependencies, tool.AdditionalDependencies, ";")
            << attrTagX(_AdditionalLibraryDirectories, tool.AdditionalLibraryDirectories, ";")
            << attrTagX(_AdditionalOptions, tool.AdditionalOptions, " ")
            << attrTagS(_DisplayLibrary, tool.DisplayLibrary)
            << attrTagS(_ErrorReporting, tool.ErrorReporting)
            << attrTagX(_ExportNamedFunctions, tool.ExportNamedFunctions, ";")
            << attrTagX(_ForceSymbolReferences, tool.ForceSymbolReferences, ";")
            << attrTagT(_IgnoreAllDefaultLibraries, tool.IgnoreAllDefaultLibraries)
            << attrTagX(_IgnoreSpecificDefaultLibraries, tool.IgnoreSpecificDefaultLibraries, ";")
            << attrTagT(_LinkTimeCodeGeneration, tool.LinkTimeCodeGeneration)
            << attrTagS(_ModuleDefinitionFile, tool.ModuleDefinitionFile)
            << attrTagS(_Name, tool.Name)
            << attrTagS(_OutputFile, tool.OutputFile)
            << attrTagX(_RemoveObjects, tool.RemoveObjects, ";")
            << attrTagS(_SubSystem, tool.SubSystem)
            << attrTagT(_SuppressStartupBanner, tool.SuppressStartupBanner)
            << attrTagS(_TargetMachine, tool.TargetMachine)
            << attrTagT(_TreatLibWarningAsErrors, tool.TreatLibWarningAsErrors)
            << attrTagT(_Verbose, tool.Verbose)
        << closetag(_Link);
}

// VCCustomBuildTool ------------------------------------------------
VCXCustomBuildTool::VCXCustomBuildTool()
{
    ToolName = "VCXCustomBuildTool";
}

XmlOutput &operator<<(XmlOutput &xml, const VCXCustomBuildTool &tool)
{
    if ( !tool.AdditionalDependencies.isEmpty() )
    {
        xml << tag("AdditionalInputs")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.ConfigName))
            << valueTagDefX(tool.AdditionalDependencies, "AdditionalInputs", ";");
    }

    if( !tool.CommandLine.isEmpty() )
    {
        xml << tag("Command")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.ConfigName))
            << valueTag(tool.CommandLine.join(vcxCommandSeparator()));
    }

    if ( !tool.Description.isEmpty() )
    {
        xml << tag("Message")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.ConfigName))
            << valueTag(tool.Description);
    }

    if ( !tool.Outputs.isEmpty() )
    {
        xml << tag("Outputs")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.ConfigName))
            << valueTagDefX(tool.Outputs, "Outputs", ";");
    }

    return xml;
}

// VCResourceCompilerTool -------------------------------------------
VCXResourceCompilerTool::VCXResourceCompilerTool()
    :   IgnoreStandardIncludePath(unset),
        NullTerminateStrings(unset),
        ShowProgress(unset),
        SuppressStartupBanner(unset)
{
    PreprocessorDefinitions = QStringList("NDEBUG");
}

XmlOutput &operator<<(XmlOutput &xml, const VCXResourceCompilerTool &tool)
{
    return xml
        << tag(_ResourceCompile)
            << attrTagX(_AdditionalIncludeDirectories, tool.AdditionalIncludeDirectories, ";")
            << attrTagS(_AdditionalOptions, tool.AdditionalOptions)
            << attrTagS(_Culture, tool.Culture)
            << attrTagT(_IgnoreStandardIncludePath, tool.IgnoreStandardIncludePath)
            << attrTagT(_NullTerminateStrings, tool.NullTerminateStrings)
            << attrTagX(_PreprocessorDefinitions, tool.PreprocessorDefinitions, ";")
            << attrTagS(_ResourceOutputFileName, tool.ResourceOutputFileName)
            << attrTagT(_ShowProgress, tool.ShowProgress)
            << attrTagT(_SuppressStartupBanner, tool.SuppressStartupBanner)
            << attrTagS(_TrackerLogDirectory, tool.TrackerLogDirectory)
            << attrTagS(_UndefinePreprocessorDefinitions, tool.UndefinePreprocessorDefinitions)
        << closetag(_ResourceCompile);
}

// VCXDeploymentTool --------------------------------------------
VCXDeploymentTool::VCXDeploymentTool()
{
    DeploymentTag = "DeploymentTool";
    RemoteDirectory = "";
}

// http://msdn.microsoft.com/en-us/library/sa69he4t.aspx
XmlOutput &operator<<(XmlOutput &xml, const VCXDeploymentTool &tool)
{
    if (tool.AdditionalFiles.isEmpty())
        return xml;
    return xml
        << tag(tool.DeploymentTag)
        << closetag(tool.DeploymentTag);
}

// VCEventTool -------------------------------------------------
XmlOutput &operator<<(XmlOutput &xml, const VCXEventTool &tool)
{
    return xml
        << tag(tool.EventName)
        << attrTagS(_Command, tool.CommandLine.join(vcxCommandSeparator()))
        << attrTagS(_Message, tool.Description)
        << closetag(tool.EventName);
}

// VCXPostBuildEventTool ---------------------------------------------
VCXPostBuildEventTool::VCXPostBuildEventTool()
{
    EventName = "PostBuildEvent";
}

// VCXPreBuildEventTool ----------------------------------------------
VCXPreBuildEventTool::VCXPreBuildEventTool()
{
    EventName = "PreBuildEvent";
}

// VCXPreLinkEventTool -----------------------------------------------
VCXPreLinkEventTool::VCXPreLinkEventTool()
{
    EventName = "PreLinkEvent";
}

// VCConfiguration --------------------------------------------------

VCXConfiguration::VCXConfiguration()
    :        ATLMinimizesCRunTimeLibraryUsage(unset),
        BuildBrowserInformation(unset),
        RegisterOutput(unset),
        WholeProgramOptimization(unset)
{
    compiler.config = this;
    linker.config = this;
    idl.config = this;
}

XmlOutput &operator<<(XmlOutput &xml, const VCXConfiguration &tool)
{
        xml << tag("PropertyGroup")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Name))
            << attrTag("Label", "Configuration")
            << attrTagS(_OutputDirectory, tool.OutputDirectory)
            << attrTagT(_ATLMinimizesCRunTimeLibraryUsage, tool.ATLMinimizesCRunTimeLibraryUsage)
            << attrTagT(_BuildBrowserInformation, tool.BuildBrowserInformation)
            << attrTagS(_CharacterSet, tool.CharacterSet)
            << attrTagS(_ConfigurationType, tool.ConfigurationType)
            << attrTagS(_DeleteExtensionsOnClean, tool.DeleteExtensionsOnClean)
            << attrTagS(_ImportLibrary, tool.ImportLibrary)
            << attrTagS(_IntermediateDirectory, tool.IntermediateDirectory)
            << attrTagS(_PrimaryOutput, tool.PrimaryOutput)
            << attrTagS(_ProgramDatabase, tool.ProgramDatabase)
            << attrTagT(_RegisterOutput, tool.RegisterOutput)
            << attrTagS(_UseOfATL, tool.UseOfATL)
            << attrTagS(_UseOfMfc, tool.UseOfMfc)
            << attrTagT(_WholeProgramOptimization, tool.WholeProgramOptimization)
        << closetag();
    return xml;
}
// VCXFilter ---------------------------------------------------------
VCXFilter::VCXFilter()
    :   ParseFiles(unset),
        Config(0)
{
    useCustomBuildTool = false;
    useCompilerTool = false;
}

void VCXFilter::addFile(const QString& filename)
{
    Files += VCXFilterFile(filename);
}

void VCXFilter::addFile(const VCXFilterFile& fileInfo)
{
    Files += VCXFilterFile(fileInfo);
}

void VCXFilter::addFiles(const QStringList& fileList)
{
    for (int i = 0; i < fileList.count(); ++i)
        addFile(fileList.at(i));
}

void VCXFilter::modifyPCHstage(QString str)
{
    bool autogenSourceFile = Project->autogenPrecompCPP;
    bool pchThroughSourceFile = !Project->precompCPP.isEmpty();
    bool isCFile = false;
    for (QStringList::Iterator it = Option::c_ext.begin(); it != Option::c_ext.end(); ++it) {
        if (str.endsWith(*it)) {
            isCFile = true;
            break;
        }
    }
    bool isHFile = str.endsWith(".h") && (str == Project->precompH);
    bool isCPPFile = pchThroughSourceFile && (str == Project->precompCPP);

    if(!isCFile && !isHFile && !isCPPFile)
        return;

    if(isHFile && pchThroughSourceFile) {
        if (autogenSourceFile) {
            useCustomBuildTool = true;
            QString toFile(Project->precompCPP);
            CustomBuildTool.Description = "Generating precompiled header source file '" + toFile + "' ...";
            CustomBuildTool.Outputs += toFile;

            QStringList lines;
            CustomBuildTool.CommandLine +=
                "echo /*-------------------------------------------------------------------- >" + toFile;
            lines << "* Precompiled header source file used by Visual Studio.NET to generate";
            lines << "* the .pch file.";
            lines << "*";
            lines << "* Due to issues with the dependencies checker within the IDE, it";
            lines << "* sometimes fails to recompile the PCH file, if we force the IDE to";
            lines << "* create the PCH file directly from the header file.";
            lines << "*";
            lines << "* This file is auto-generated by qmake since no PRECOMPILED_SOURCE was";
            lines << "* specified, and is used as the common stdafx.cpp. The file is only";
            lines << "* generated when creating .vcxproj project files, and is not used for";
            lines << "* command line compilations by nmake.";
            lines << "*";
            lines << "* WARNING: All changes made in this file will be lost.";
            lines << "--------------------------------------------------------------------*/";
            lines << "#include \"" + Project->precompHFilename + "\"";
            foreach(QString line, lines)
                CustomBuildTool.CommandLine += "echo " + line + ">>" + toFile;
        }
        return;
    }

    useCompilerTool = true;
    // Setup PCH options
    CompilerTool.PrecompiledHeader     = (isCFile ? "NotUsing" : "Create" );
    CompilerTool.PrecompiledHeaderFile = (isCPPFile ? QString("$(INHERIT)") : QString("$(NOINHERIT)"));
    CompilerTool.ForcedIncludeFiles       = QStringList("$(NOINHERIT)");
}

bool VCXFilter::addExtraCompiler(const VCXFilterFile &info)
{
    const QStringList &extraCompilers = Project->extraCompilerSources.value(info.file);
    if (extraCompilers.isEmpty())
        return false;

    QString inFile = info.file;

    // is the extracompiler rule on a file with a built in compiler?
    const QStringList &objectMappedFile = Project->extraCompilerOutputs[inFile];
    bool hasBuiltIn = false;
    if (!objectMappedFile.isEmpty()) {
        hasBuiltIn = Project->hasBuiltinCompiler(objectMappedFile.at(0));
//        qDebug("*** Extra compiler file has object mapped file '%s' => '%s'", qPrintable(inFile), qPrintable(objectMappedFile.join(" ")));
    }

    CustomBuildTool.AdditionalDependencies.clear();
    CustomBuildTool.CommandLine.clear();
    CustomBuildTool.Description.clear();
    CustomBuildTool.Outputs.clear();
    CustomBuildTool.ToolPath.clear();
    CustomBuildTool.ToolName = QLatin1String(_VCCustomBuildTool);

    for (int x = 0; x < extraCompilers.count(); ++x) {
        const QString &extraCompilerName = extraCompilers.at(x);

        if (!Project->verifyExtraCompiler(extraCompilerName, inFile) && !hasBuiltIn)
            continue;

        // All information about the extra compiler
        QString tmp_out = Project->project->first(extraCompilerName + ".output");
        QString tmp_cmd = Project->project->variables()[extraCompilerName + ".commands"].join(" ");
        QString tmp_cmd_name = Project->project->variables()[extraCompilerName + ".name"].join(" ");
        QStringList tmp_dep = Project->project->variables()[extraCompilerName + ".depends"];
        QString tmp_dep_cmd = Project->project->variables()[extraCompilerName + ".depend_command"].join(" ");
        QStringList vars = Project->project->variables()[extraCompilerName + ".variables"];
        QStringList configs = Project->project->variables()[extraCompilerName + ".CONFIG"];
        bool combined = configs.indexOf("combine") != -1;

        QString cmd, cmd_name, out;
        QStringList deps, inputs;
        // Variabel replacement of output name
        out = Option::fixPathToTargetOS(
                    Project->replaceExtraCompilerVariables(tmp_out, inFile, QString()),
                    false);

        // If file has built-in compiler, we've swapped the input and output of
        // the command, as we in Visual Studio cannot have a Custom Buildstep on
        // a file which uses a built-in compiler. We would in this case only get
        // the result from the extra compiler. If 'hasBuiltIn' is true, we know
        // that we're actually on the _output_file_ of the result, and we
        // therefore swap inFile and out below, since the extra-compiler still
        // must see it as the original way. If the result also has a built-in
        // compiler, too bad..
        if (hasBuiltIn) {
            out = inFile;
            inFile = objectMappedFile.at(0);
        }

        // Dependency for the output
        if(!tmp_dep.isEmpty())
            deps = tmp_dep;
        if(!tmp_dep_cmd.isEmpty()) {
            // Execute dependency command, and add every line as a dep
            char buff[256];
            QString dep_cmd = Project->replaceExtraCompilerVariables(tmp_dep_cmd, Option::fixPathToLocalOS(inFile, true, false), out);
            if(Project->canExecute(dep_cmd)) {
                if(FILE *proc = QT_POPEN(dep_cmd.toLatin1().constData(), "r")) {
                    QString indeps;
                    while(!feof(proc)) {
                        int read_in = (int)fread(buff, 1, 255, proc);
                        if(!read_in)
                            break;
                        indeps += QByteArray(buff, read_in);
                    }
                    QT_PCLOSE(proc);
                    if(!indeps.isEmpty()) {
                        QStringList extradeps = indeps.split(QLatin1Char('\n'));
                        for (int i = 0; i < extradeps.count(); ++i) {
                            QString dd = extradeps.at(i).simplified();
                            if (!dd.isEmpty())
                                deps += Project->fileFixify(dd);
                        }
                    }
                }
            }
        }
        for (int i = 0; i < deps.count(); ++i)
            deps[i] = Option::fixPathToTargetOS(
                        Project->replaceExtraCompilerVariables(deps.at(i), inFile, out),
                        false).trimmed();
        // Command for file
        if (combined) {
            // Add dependencies for each file
            QStringList tmp_in = Project->project->variables()[extraCompilerName + ".input"];
            for (int a = 0; a < tmp_in.count(); ++a) {
                const QStringList &files = Project->project->variables()[tmp_in.at(a)];
                for (int b = 0; b < files.count(); ++b) {
                    deps += Project->findDependencies(files.at(b));
                    inputs += Option::fixPathToTargetOS(files.at(b), false);
                }
            }
            deps += inputs; // input files themselves too..

            // Replace variables for command w/all input files
            // ### join gives path issues with directories containing spaces!
            cmd = Project->replaceExtraCompilerVariables(tmp_cmd,
                                                         inputs.join(" "),
                                                         out);
        } else {
            deps += inFile; // input file itself too..
            cmd = Project->replaceExtraCompilerVariables(tmp_cmd,
                                                         inFile,
                                                         out);
        }
        // Name for command
    if(!tmp_cmd_name.isEmpty()) {
        cmd_name = Project->replaceExtraCompilerVariables(tmp_cmd_name, inFile, out);
	} else {
        int space = cmd.indexOf(' ');
        if(space != -1)
        cmd_name = cmd.left(space);
        else
        cmd_name = cmd;
        if((cmd_name[0] == '\'' || cmd_name[0] == '"') &&
        cmd_name[0] == cmd_name[cmd_name.length()-1])
        cmd_name = cmd_name.mid(1,cmd_name.length()-2);
	}

        // Fixify paths
        for (int i = 0; i < deps.count(); ++i)
            deps[i] = Option::fixPathToTargetOS(deps[i], false);


        // Output in info.additionalFile -----------
        if (!CustomBuildTool.Description.isEmpty())
            CustomBuildTool.Description += ", ";
        CustomBuildTool.Description += cmd_name;
        CustomBuildTool.CommandLine += VCToolBase::fixCommandLine(cmd.trimmed());
        int space = cmd.indexOf(' ');
        QFileInfo finf(cmd.left(space));
        if (CustomBuildTool.ToolPath.isEmpty())
            CustomBuildTool.ToolPath += Option::fixPathToTargetOS(finf.path());
        CustomBuildTool.Outputs += out;

        deps += CustomBuildTool.AdditionalDependencies;
        deps += cmd.left(cmd.indexOf(' '));
        // Make sure that all deps are only once
        QMap<QString, bool> uniqDeps;
        for (int c = 0; c < deps.count(); ++c) {
            QString aDep = deps.at(c).trimmed();
            if (!aDep.isEmpty())
                uniqDeps[aDep] = false;
        }
        CustomBuildTool.AdditionalDependencies = uniqDeps.keys();
    }

    // Ensure that none of the output files are also dependencies. Or else, the custom buildstep
    // will be rebuild every time, even if nothing has changed.
    foreach(QString output, CustomBuildTool.Outputs) {
        CustomBuildTool.AdditionalDependencies.removeAll(output);
    }

    useCustomBuildTool = !CustomBuildTool.CommandLine.isEmpty();
    return useCustomBuildTool;
}

bool VCXFilter::outputFileConfig(XmlOutput &xml, XmlOutput &xmlFilter, const QString &filename, const QString &filtername, bool fileAllreadyAdded)
{
    bool fileAdded = false;

    // Clearing each filter tool
    useCustomBuildTool = false;
    useCompilerTool = false;
    CustomBuildTool = VCXCustomBuildTool();
    CompilerTool = VCXCLCompilerTool();

    // Unset some default options
    CompilerTool.BufferSecurityCheck = unset;
    CompilerTool.DebugInformationFormat = "";
    CompilerTool.ExceptionHandling = "";
    //CompilerTool.Optimization = optimizeDefault;
    CompilerTool.ProgramDataBaseFileName.clear();
    CompilerTool.RuntimeLibrary = "";
    //CompilerTool.WarningLevel = warningLevelUnknown;
    CompilerTool.config = Config;

    bool inBuild = false;
    VCXFilterFile info;
    for (int i = 0; i < Files.count(); ++i) {
        if (Files.at(i).file == filename) {
            info = Files.at(i);
            inBuild = true;
        }
    }
    inBuild &= !info.excludeFromBuild;

    if (inBuild) {
        addExtraCompiler(info);
        if(Project->usePCH)
            modifyPCHstage(info.file);
    } else {
        // Excluded files uses an empty compiler stage
        if(info.excludeFromBuild)
            useCompilerTool = true;
    }

    // Actual XML output ----------------------------------
    if(useCustomBuildTool || useCompilerTool || !inBuild) {

        if (useCustomBuildTool)
        {
            CustomBuildTool.ConfigName = (*Config).Name;

            if ( !fileAllreadyAdded ) {

                fileAdded = true;

                xmlFilter << tag("CustomBuild")
                    << attrTag("Include",Option::fixPathToLocalOS(filename))
                    << attrTagS("Filter", filtername);

                xml << tag("CustomBuild")
                    << attrTag("Include",Option::fixPathToLocalOS(filename));

                if ( filtername == "Form Files" || filtername == "Generated Files" || filtername == "Resource Files" )
                    xml << attrTagS("FileType", "Document");
            }

            xml << CustomBuildTool;
        }

        if ( !fileAdded && !fileAllreadyAdded )
        {
            fileAdded = true;

            if (filtername == "Source Files") {

                xmlFilter << tag("ClCompile")
                          << attrTag("Include",Option::fixPathToLocalOS(filename))
                          << attrTagS("Filter", filtername);

                xml << tag("ClCompile")
                    << attrTag("Include",Option::fixPathToLocalOS(filename));

            } else if(filtername == "Header Files") {

                xmlFilter << tag("ClInclude")
                    << attrTag("Include",Option::fixPathToLocalOS(filename))
                    << attrTagS("Filter", filtername);

                xml << tag("ClInclude")
                    << attrTag("Include",Option::fixPathToLocalOS(filename));
            } else if(filtername == "Generated Files" || filtername == "Form Files") {

                if (filename.endsWith(".h")) {

                    xmlFilter << tag("ClInclude")
                              << attrTag("Include",Option::fixPathToLocalOS(filename))
                              << attrTagS("Filter", filtername);

                    xml << tag("ClInclude")
                        << attrTag("Include",Option::fixPathToLocalOS(filename));
                } else if(filename.endsWith(".cpp")) {

                    xmlFilter << tag("ClCompile")
                              << attrTag("Include",Option::fixPathToLocalOS(filename))
                              << attrTagS("Filter", filtername);

                    xml << tag("ClCompile")
                        << attrTag("Include",Option::fixPathToLocalOS(filename));
                } else if(filename.endsWith(".res")) {

                    xmlFilter << tag("CustomBuild")
                              << attrTag("Include",Option::fixPathToLocalOS(filename))
                              << attrTagS("Filter", filtername);

                    xml << tag("CustomBuild")
                        << attrTag("Include",Option::fixPathToLocalOS(filename));
                } else {

                    xmlFilter << tag("CustomBuild")
                              << attrTag("Include",Option::fixPathToLocalOS(filename))
                              << attrTagS("Filter", filtername);

                    xml << tag("CustomBuild")
                        << attrTag("Include",Option::fixPathToLocalOS(filename));
                }
            } else if(filtername == "Root Files") {

                if (filename.endsWith(".rc")) {

                    xmlFilter << tag("ResourceCompile")
                              << attrTag("Include",Option::fixPathToLocalOS(filename));

                    xml << tag("ResourceCompile")
                        << attrTag("Include",Option::fixPathToLocalOS(filename));
                }
            }
        }

        if(!inBuild) {

            xml << tag("ExcludedFromBuild")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg((*Config).Name))
                << valueTag("true");
        }

        if (useCompilerTool) {

            if ( !CompilerTool.ForcedIncludeFiles.isEmpty() ) {
                xml << tag("ForcedIncludeFiles")
                    << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg((*Config).Name))
                    << valueTagX(CompilerTool.ForcedIncludeFiles);
            }

            if ( !CompilerTool.PrecompiledHeaderFile.isEmpty() ) {

                xml << tag("PrecompiledHeaderFile")
                    << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg((*Config).Name))
                    << valueTag(CompilerTool.PrecompiledHeaderFile);
            }

            if ( !CompilerTool.PrecompiledHeader.isEmpty() ) {

                xml << tag("PrecompiledHeader")
                    << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg((*Config).Name))
                    << valueTag(CompilerTool.PrecompiledHeader);
            }
        }
    }

    return fileAdded;
}


// VCXProjectSingleConfig --------------------------------------------
VCXFilter nullVCXFilter;
VCXFilter& VCXProjectSingleConfig::filterForExtraCompiler(const QString &compilerName)
{
    for (int i = 0; i < ExtraCompilersFiles.count(); ++i)
        if (ExtraCompilersFiles.at(i).Name == compilerName)
            return ExtraCompilersFiles[i];
    return nullVCXFilter;
}


XmlOutput &operator<<(XmlOutput &xml, const VCXProjectSingleConfig &tool)
{
    xml.setIndentString("  ");

    xml << decl("1.0", "utf-8")
        << tag("Project")
        << attrTag("DefaultTargets","Build")
        << attrTag("ToolsVersion", "4.0")
        << attrTag("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003")
        << tag("ItemGroup")
        << attrTag("Label", "ProjectConfigurations");

    xml << tag("ProjectConfiguration")
        << attrTag("Include" , tool.Configuration.Name)
        << tagValue("Configuration", tool.Configuration.ConfigurationName)
        << tagValue("Platform", tool.PlatformName)
        << closetag();

    xml << closetag()
        << tag("PropertyGroup")
        << attrTag("Label", "Globals")
        << tagValue("ProjectGuid", tool.ProjectGUID)
        << tagValue("RootNamespace", tool.Name)
        << tagValue("Keyword", tool.Keyword)
        << closetag();

    // config part.
    xml << import("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");

    xml << tool.Configuration;

    xml << import("Project", "$(VCTargetsPath)\\Microsoft.Cpp.props");

    // Extension settings
    xml << tag("ImportGroup")
        << attrTag("Label", "ExtensionSettings")
        << closetag();

    // PropertySheets
    xml << tag("ImportGroup")
        << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
        << attrTag("Label", "PropertySheets");

    xml << tag("Import")
        << attrTag("Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props")
        << attrTag("Condition", "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')")
        << closetag()
        << closetag();


    // UserMacros
    xml << tag("PropertyGroup")
        << attrTag("Label", "UserMacros")
        << closetag();

    xml << tag("PropertyGroup");

    if ( !tool.Configuration.OutputDirectory.isEmpty() ) {
        xml<< tag("OutDir")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
            << valueTag(tool.Configuration.OutputDirectory);
    }
    if ( !tool.Configuration.IntermediateDirectory.isEmpty() ) {
        xml<< tag("IntDir")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
            << valueTag(tool.Configuration.IntermediateDirectory);
    }
    if ( !tool.Configuration.TargetName.isEmpty() ) {
        xml<< tag("TargetName")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
            << valueTag(tool.Configuration.TargetName);
    }

    if ( tool.Configuration.linker.IgnoreImportLibrary != unset) {
        xml<< tag("IgnoreImportLibrary")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
            << valueTagT(tool.Configuration.linker.IgnoreImportLibrary);
    }

    if ( tool.Configuration.linker.LinkIncremental != unset) {
        xml<< tag("LinkIncremental")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
            << valueTagT(tool.Configuration.linker.LinkIncremental);
    }

    if ( tool.Configuration.preBuild.UseInBuild != unset )
    {
        xml<< tag("PreBuildEventUseInBuild")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
            << valueTagT(tool.Configuration.preBuild.UseInBuild);
    }

    if ( tool.Configuration.preLink.UseInBuild != unset )
    {
        xml<< tag("PreLinkEventUseInBuild")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
            << valueTagT(tool.Configuration.preLink.UseInBuild);
    }

    if ( tool.Configuration.postBuild.UseInBuild != unset )
    {
        xml<< tag("PostBuildEventUseInBuild")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name))
            << valueTagT(tool.Configuration.postBuild.UseInBuild);
    }
    xml << closetag();

    xml << tag("ItemDefinitionGroup")
        << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.Configuration.Name));

    // ClCompile
    xml << tool.Configuration.compiler;

    // Link
    xml << tool.Configuration.linker;

    // Midl
    xml << tool.Configuration.idl;

    // ResourceCompiler
    xml << tool.Configuration.resource;

    // Post build event
    if ( tool.Configuration.postBuild.UseInBuild != unset )
        xml << tool.Configuration.postBuild;
    
    // Pre build event
    if ( tool.Configuration.preBuild.UseInBuild != unset )
        xml << tool.Configuration.preBuild;

    // Pre link event
    if ( tool.Configuration.preLink.UseInBuild != unset )
        xml << tool.Configuration.preLink;

    xml << closetag();

    QFile filterFile;
    filterFile.setFileName(Option::output.fileName().append(".filters"));
    filterFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    QTextStream ts(&filterFile);
    XmlOutput xmlFilter(ts, XmlOutput::NoConversion);

    xmlFilter.setIndentString("  ");

    xmlFilter << decl("1.0", "utf-8")
              << tag("Project")
              << attrTag("ToolsVersion", "4.0")
              << attrTag("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

    xmlFilter << tag("ItemGroup");

    VCXProject tempProj;
    tempProj.SingleProjects += tool;

    tempProj.addFilters(xmlFilter, "Form Files");
    tempProj.addFilters(xmlFilter, "Generated Files");
    tempProj.addFilters(xmlFilter, "Header Files");
    tempProj.addFilters(xmlFilter, "LexYacc Files");
    tempProj.addFilters(xmlFilter, "Resource Files");
    tempProj.addFilters(xmlFilter, "Source Files");
    tempProj.addFilters(xmlFilter, "Translation Files");
    xmlFilter << closetag();

    tempProj.outputFilter(xml, xmlFilter, "Source Files");
    tempProj.outputFilter(xml, xmlFilter, "Header Files");
    tempProj.outputFilter(xml, xmlFilter, "Generated Files");
    tempProj.outputFilter(xml, xmlFilter, "LexYacc Files");
    tempProj.outputFilter(xml, xmlFilter, "Translation Files");
    tempProj.outputFilter(xml, xmlFilter, "Form Files");
    tempProj.outputFilter(xml, xmlFilter, "Resource Files");

    for (int x = 0; x < tempProj.ExtraCompilers.count(); ++x) {
        tempProj.outputFilter(xml, xmlFilter, tempProj.ExtraCompilers.at(x));
    }

    xml << import("Project", "$(VCTargetsPath)\\Microsoft.Cpp.targets");

    xml << tag("ImportGroup")
        << attrTag("Label", "ExtensionTargets")
        << closetag();

    return xml;
}


// Tree file generation ---------------------------------------------
void XTreeNode::generateXML(XmlOutput &xml, XmlOutput &xmlFilter, const QString &tagName, VCXProject &tool, const QString &filter) {

    if (children.size()) {
        // Filter
        QString tempFilterName;
        ChildrenMap::ConstIterator it, end = children.constEnd();
        if (!tagName.isEmpty()) {
            tempFilterName.append(filter);
            tempFilterName.append("\\");
            tempFilterName.append(tagName);
            xmlFilter << tag(_ItemGroup);
            xmlFilter << tag("Filter")
                      << attrTag("Include", tempFilterName)
                      << closetag();
            xmlFilter << closetag();
        }
        // First round, do nested filters
        for (it = children.constBegin(); it != end; ++it)
            if ((*it)->children.size())
            {
                if ( !tempFilterName.isEmpty() )
                    (*it)->generateXML(xml, xmlFilter, it.key(), tool, tempFilterName);
                else
                    (*it)->generateXML(xml, xmlFilter, it.key(), tool, filter);
            }
        // Second round, do leafs
        for (it = children.constBegin(); it != end; ++it)
            if (!(*it)->children.size())
            {
                if ( !tempFilterName.isEmpty() )
                    (*it)->generateXML(xml, xmlFilter, it.key(), tool, tempFilterName);
                else
                    (*it)->generateXML(xml, xmlFilter, it.key(), tool, filter);
            }
    } else {
        // Leaf
        xml << tag(_ItemGroup);
        xmlFilter << tag(_ItemGroup);        
        tool.outputFileConfigs(xml, xmlFilter, info, filter);
        xmlFilter << closetag();
        xml << closetag();
    }
}


// Flat file generation ---------------------------------------------
void XFlatNode::generateXML(XmlOutput &xml, XmlOutput &xmlFilter, const QString &/*tagName*/, VCXProject &tool, const QString &filter) {
    if (children.size()) {
        ChildrenMapFlat::ConstIterator it = children.constBegin();
        ChildrenMapFlat::ConstIterator end = children.constEnd();
        xml << tag(_ItemGroup);
        xmlFilter << tag(_ItemGroup);
        for (; it != end; ++it) {
            tool.outputFileConfigs(xml, xmlFilter, (*it), filter);
        }
        xml << closetag();
        xmlFilter << closetag();
    }
}


// VCXProject --------------------------------------------------------
// Output all configurations (by filtername) for a file (by info)
// A filters config output is in VCXFilter.outputFileConfig()
void VCXProject::outputFileConfigs(XmlOutput &xml,
                                  XmlOutput &xmlFilter,
                                  const VCXFilterFile &info,
                                  const QString &filtername)
{
    // We need to check if the file has any custom build step.
    // If there is one then it has to be included with "CustomBuild Include"
    bool fileAdded = false;

    for (int i = 0; i < SingleProjects.count(); ++i) {
        VCXFilter filter;
        if (filtername == "Root Files") {
            filter = SingleProjects.at(i).RootFiles;
        } else if (filtername == "Source Files") {
            filter = SingleProjects.at(i).SourceFiles;
        } else if (filtername == "Header Files") {
            filter = SingleProjects.at(i).HeaderFiles;
        } else if (filtername == "Generated Files") {
            filter = SingleProjects.at(i).GeneratedFiles;
        } else if (filtername == "LexYacc Files") {
            filter = SingleProjects.at(i).LexYaccFiles;
        } else if (filtername == "Translation Files") {
            filter = SingleProjects.at(i).TranslationFiles;
        } else if (filtername == "Form Files") {
            filter = SingleProjects.at(i).FormFiles;
        } else if (filtername == "Resource Files") {
            filter = SingleProjects.at(i).ResourceFiles;
        } else {
            // ExtraCompilers
            filter = SingleProjects[i].filterForExtraCompiler(filtername);
        }

        if (filter.Config) // only if the filter is not empty
            if (filter.outputFileConfig(xml, xmlFilter, info.file, filtername, fileAdded)) // only add it once.
                fileAdded = true;
    }

    if ( !fileAdded )
    {
        if (filtername == "Source Files") {

            xmlFilter << tag("ClCompile")
                      << attrTag("Include",Option::fixPathToLocalOS(info.file))
                      << attrTagS("Filter", filtername);

            xml << tag("ClCompile")
                << attrTag("Include",Option::fixPathToLocalOS(info.file));

        } else if(filtername == "Header Files") {

            xmlFilter << tag("ClInclude")
                << attrTag("Include",Option::fixPathToLocalOS(info.file))
                << attrTagS("Filter", filtername);

            xml << tag("ClInclude")
                << attrTag("Include",Option::fixPathToLocalOS(info.file));
        } else if(filtername == "Generated Files" || filtername == "Form Files") {

            if (info.file.endsWith(".h")) {

                xmlFilter << tag("ClInclude")
                          << attrTag("Include",Option::fixPathToLocalOS(info.file))
                          << attrTagS("Filter", filtername);

                xml << tag("ClInclude")
                    << attrTag("Include",Option::fixPathToLocalOS(info.file));
            } else if(info.file.endsWith(".cpp")) {

                xmlFilter << tag("ClCompile")
                          << attrTag("Include",Option::fixPathToLocalOS(info.file))
                          << attrTagS("Filter", filtername);

                xml << tag("ClCompile")
                    << attrTag("Include",Option::fixPathToLocalOS(info.file));
            } else if(info.file.endsWith(".res")) {

                    xmlFilter << tag("CustomBuild")
                              << attrTag("Include",Option::fixPathToLocalOS(info.file))
                              << attrTagS("Filter", filtername);

                    xml << tag("CustomBuild")
                        << attrTag("Include",Option::fixPathToLocalOS(info.file));
            } else {

                xmlFilter << tag("CustomBuild")
                          << attrTag("Include",Option::fixPathToLocalOS(info.file))
                          << attrTagS("Filter", filtername);

                xml << tag("CustomBuild")
                    << attrTag("Include",Option::fixPathToLocalOS(info.file));
            }

        } else if(filtername == "Root Files") {

            if (info.file.endsWith(".rc")) {

                xmlFilter << tag("ResourceCompile")
                          << attrTag("Include",Option::fixPathToLocalOS(info.file));

                xml << tag("ResourceCompile")
                    << attrTag("Include",Option::fixPathToLocalOS(info.file));
            }
        } else {

            xmlFilter << tag("None")
                      << attrTag("Include",Option::fixPathToLocalOS(info.file))
                      << attrTagS("Filter", filtername);

                xml << tag("None")
                    << attrTag("Include",Option::fixPathToLocalOS(info.file));
        }
    }

    xml << closetag();
    xmlFilter << closetag();
}

// outputs a given filter for all existing configurations of a project
void VCXProject::outputFilter(XmlOutput &xml,
                              XmlOutput &xmlFilter,
                              const QString &filtername)
{
    XNode *root;
    if (SingleProjects.at(0).flat_files)
        root = new XFlatNode;
    else
        root = new XTreeNode;

    QString name, extfilter;
    triState parse;

    for (int i = 0; i < SingleProjects.count(); ++i) {
        VCXFilter filter;
        if (filtername == "Root Files") {
            filter = SingleProjects.at(i).RootFiles;
        } else if (filtername == "Source Files") {
            filter = SingleProjects.at(i).SourceFiles;
        } else if (filtername == "Header Files") {
            filter = SingleProjects.at(i).HeaderFiles;
        } else if (filtername == "Generated Files") {
            filter = SingleProjects.at(i).GeneratedFiles;
        } else if (filtername == "LexYacc Files") {
            filter = SingleProjects.at(i).LexYaccFiles;
        } else if (filtername == "Translation Files") {
            filter = SingleProjects.at(i).TranslationFiles;
        } else if (filtername == "Form Files") {
            filter = SingleProjects.at(i).FormFiles;
        } else if (filtername == "Resource Files") {
            filter = SingleProjects.at(i).ResourceFiles;
        } else {
            // ExtraCompilers
            filter = SingleProjects[i].filterForExtraCompiler(filtername);
        }

        // Merge all files in this filter to root tree
        for (int x = 0; x < filter.Files.count(); ++x)
            root->addElement(filter.Files.at(x));

        // Save filter setting from first filter. Next filters
        // may differ but we cannot handle that. (ex. extfilter)
        if (name.isEmpty()) {
            name = filter.Name;
            extfilter = filter.Filter;
            parse = filter.ParseFiles;
        }
    }

    if (!root->hasElements())
        return;

    root->generateXML(xml, xmlFilter, "", *this, filtername); // output root tree
}


void VCXProject::addFilters(XmlOutput &xmlFilter,
                            const QString &filtername)
{
    bool added = false;

    for (int i = 0; i < SingleProjects.count(); ++i) {
        VCXFilter filter;
        if (filtername == "Root Files") {
            filter = SingleProjects.at(i).RootFiles;
        } else if (filtername == "Source Files") {
            filter = SingleProjects.at(i).SourceFiles;
        } else if (filtername == "Header Files") {
            filter = SingleProjects.at(i).HeaderFiles;
        } else if (filtername == "Generated Files") {
            filter = SingleProjects.at(i).GeneratedFiles;
        } else if (filtername == "LexYacc Files") {
            filter = SingleProjects.at(i).LexYaccFiles;
        } else if (filtername == "Translation Files") {
            filter = SingleProjects.at(i).TranslationFiles;
        } else if (filtername == "Form Files") {
            filter = SingleProjects.at(i).FormFiles;
        } else if (filtername == "Resource Files") {
            filter = SingleProjects.at(i).ResourceFiles;
        } else {
            // ExtraCompilers
            filter = SingleProjects[i].filterForExtraCompiler(filtername);
        }

        if(!filter.Files.isEmpty() && !added) {
            added = true;
            xmlFilter << tag("Filter")
                      << attrTag("Include", filtername)
                      << attrTagS("UniqueIdentifier", filter.Guid)
                      << attrTagS("Extensions", filter.Filter)
                      << attrTagT("ParseFiles", filter.ParseFiles)
                      << closetag();
        }
    }
}


XmlOutput &operator<<(XmlOutput &xml, VCXProject &tool)
{

    if (tool.SingleProjects.count() == 0) {
        warn_msg(WarnLogic, "Generator: .NET: no single project in merge project, no output");
        return xml;
    }

    xml.setIndentString("  ");

    xml << decl("1.0", "utf-8")
        << tag("Project")
        << attrTag("DefaultTargets","Build")
        << attrTag("ToolsVersion", "4.0")
        << attrTag("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003")
        << tag("ItemGroup")
        << attrTag("Label", "ProjectConfigurations");

    for (int i = 0; i < tool.SingleProjects.count(); ++i) {
        xml << tag("ProjectConfiguration")
            << attrTag("Include" , tool.SingleProjects.at(i).Configuration.Name)
            << tagValue("Configuration", tool.SingleProjects.at(i).Configuration.ConfigurationName)
            << tagValue("Platform", tool.SingleProjects.at(i).PlatformName)
            << closetag();
    }

    xml << closetag()
        << tag("PropertyGroup")
        << attrTag("Label", "Globals")
        << tagValue("ProjectGuid", tool.ProjectGUID)
        << tagValue("RootNamespace", tool.Name)
        << tagValue("Keyword", tool.Keyword)
        << closetag();

    // config part.
    xml << import("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
    for (int i = 0; i < tool.SingleProjects.count(); ++i)
        xml << tool.SingleProjects.at(i).Configuration;
    xml << import("Project", "$(VCTargetsPath)\\Microsoft.Cpp.props");

    // Extension settings
    xml << tag("ImportGroup")
        << attrTag("Label", "ExtensionSettings")
        << closetag();

    // PropertySheets
    for (int i = 0; i < tool.SingleProjects.count(); ++i) {
        xml << tag("ImportGroup")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
            << attrTag("Label", "PropertySheets");

        xml << tag("Import")
            << attrTag("Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props")
            << attrTag("Condition", "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')")
            << closetag()
            << closetag();
    }

    // UserMacros
    xml << tag("PropertyGroup")
        << attrTag("Label", "UserMacros")
        << closetag();

    xml << tag("PropertyGroup");
    for (int i = 0; i < tool.SingleProjects.count(); ++i) {

        if ( !tool.SingleProjects.at(i).Configuration.OutputDirectory.isEmpty() ) {
            xml << tag("OutDir")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
                << valueTag(tool.SingleProjects.at(i).Configuration.OutputDirectory);
        }
        if ( !tool.SingleProjects.at(i).Configuration.IntermediateDirectory.isEmpty() ) {
            xml << tag("IntDir")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
                << valueTag(tool.SingleProjects.at(i).Configuration.IntermediateDirectory);
        }
        if ( !tool.SingleProjects.at(i).Configuration.TargetName.isEmpty() ) {
            xml << tag("TargetName")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
                << valueTag(tool.SingleProjects.at(i).Configuration.TargetName);
        }

        if ( tool.SingleProjects.at(i).Configuration.linker.IgnoreImportLibrary != unset) {
            xml << tag("IgnoreImportLibrary")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
                << valueTagT(tool.SingleProjects.at(i).Configuration.linker.IgnoreImportLibrary);
        }

        if ( tool.SingleProjects.at(i).Configuration.linker.LinkIncremental != unset) {
            xml << tag("LinkIncremental")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
                << valueTagT(tool.SingleProjects.at(i).Configuration.linker.LinkIncremental);
        }

        if ( tool.SingleProjects.at(i).Configuration.preBuild.UseInBuild != unset )
        {
            xml << tag("PreBuildEventUseInBuild")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
                << valueTagT(tool.SingleProjects.at(i).Configuration.preBuild.UseInBuild);
        }

        if ( tool.SingleProjects.at(i).Configuration.preLink.UseInBuild != unset )
        {
            xml << tag("PreLinkEventUseInBuild")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
                << valueTagT(tool.SingleProjects.at(i).Configuration.preLink.UseInBuild);
        }

        if ( tool.SingleProjects.at(i).Configuration.postBuild.UseInBuild != unset )
        {
            xml << tag("PostBuildEventUseInBuild")
                << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name))
                << valueTagT(tool.SingleProjects.at(i).Configuration.postBuild.UseInBuild);
        }
    }
    xml << closetag();

    for (int i = 0; i < tool.SingleProjects.count(); ++i) {
        xml << tag("ItemDefinitionGroup")
            << attrTag("Condition", QString("'$(Configuration)|$(Platform)'=='%1'").arg(tool.SingleProjects.at(i).Configuration.Name));

        // ClCompile
        xml << tool.SingleProjects.at(i).Configuration.compiler;

        // Link
        xml << tool.SingleProjects.at(i).Configuration.linker;

        // Midl
        xml << tool.SingleProjects.at(i).Configuration.idl;

        // ResourceCompiler
        xml << tool.SingleProjects.at(i).Configuration.resource;

        // Post build event
        if ( tool.SingleProjects.at(i).Configuration.postBuild.UseInBuild != unset )
            xml << tool.SingleProjects.at(i).Configuration.postBuild;
        
        // Pre build event
        if ( tool.SingleProjects.at(i).Configuration.preBuild.UseInBuild != unset )
            xml << tool.SingleProjects.at(i).Configuration.preBuild;

        // Pre link event
        if ( tool.SingleProjects.at(i).Configuration.preLink.UseInBuild != unset )
            xml << tool.SingleProjects.at(i).Configuration.preLink;

        xml << closetag();
    }

    // The file filters are added in a separate file for MSBUILD.
    QFile filterFile;
    filterFile.setFileName(Option::output.fileName().append(".filters"));
    filterFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    QTextStream ts(&filterFile);
    XmlOutput xmlFilter(ts, XmlOutput::NoConversion);

    xmlFilter.setIndentString("  ");

    xmlFilter << decl("1.0", "utf-8")
              << tag("Project")
              << attrTag("ToolsVersion", "4.0")
              << attrTag("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

    xmlFilter << tag("ItemGroup");

    tool.addFilters(xmlFilter, "Form Files");
    tool.addFilters(xmlFilter, "Generated Files");
    tool.addFilters(xmlFilter, "Header Files");
    tool.addFilters(xmlFilter, "LexYacc Files");
    tool.addFilters(xmlFilter, "Resource Files");
    tool.addFilters(xmlFilter, "Source Files");
    tool.addFilters(xmlFilter, "Translation Files");
    xmlFilter << closetag();

    tool.outputFilter(xml, xmlFilter, "Source Files");
    tool.outputFilter(xml, xmlFilter, "Header Files");
    tool.outputFilter(xml, xmlFilter, "Generated Files");
    tool.outputFilter(xml, xmlFilter, "LexYacc Files");
    tool.outputFilter(xml, xmlFilter, "Translation Files");
    tool.outputFilter(xml, xmlFilter, "Form Files");
    tool.outputFilter(xml, xmlFilter, "Resource Files");
    for (int x = 0; x < tool.ExtraCompilers.count(); ++x) {
        tool.outputFilter(xml, xmlFilter, tool.ExtraCompilers.at(x));
    }
    tool.outputFilter(xml, xmlFilter, "Root Files");

    xml << import("Project", "$(VCTargetsPath)\\Microsoft.Cpp.targets");

    xml << tag("ImportGroup")
        << attrTag("Label", "ExtensionTargets")
        << closetag();

    return xml;
}

QT_END_NAMESPACE
