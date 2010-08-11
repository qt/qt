
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
  ditaxmlgenerator.cpp
*/

#include "codemarker.h"
#include "codeparser.h"
#include "ditaxmlgenerator.h"
#include "node.h"
#include "separator.h"
#include "tree.h"
#include <ctype.h>
#include <qdebug.h>
#include <qlist.h>
#include <qiterator.h>
#include <qtextcodec.h>
#include <QUuid>

QT_BEGIN_NAMESPACE

#define COMMAND_VERSION                         Doc::alias("version")
int DitaXmlGenerator::id = 0;

#define cxxapi_d_xref                      	Doc::alias("cxxapi-d-xref")
#define cxxclass                           	Doc::alias("cxxclass")
#define cxxdefine                          	Doc::alias("cxxdefine")
#define cxxenumeration                     	Doc::alias("cxxenumeration")
#define cxxfile                            	Doc::alias("cxxfile")
#define cxxfunction                        	Doc::alias("cxxfunction")
#define cxxstruct                          	Doc::alias("cxxstruct")
#define cxxtypedef                         	Doc::alias("cxxtypedef")
#define cxxunion                           	Doc::alias("cxxunion")
#define cxxvariable                        	Doc::alias("cxxvariable")

#define CXXAPIMAP                          	Doc::alias("cxxAPIMap")
#define CXXCLASSREF                        	Doc::alias("cxxClassRef")
#define CXXDEFINEREF                       	Doc::alias("cxxDefineRef")
#define CXXENUMERATIONREF                  	Doc::alias("cxxEnumerationRef")
#define CXXFILEREF                         	Doc::alias("cxxFileRef")
#define CXXFUNCTIONREF                     	Doc::alias("cxxFunctionRef")
#define CXXSTRUCTREF                       	Doc::alias("cxxStructRef")
#define CXXTYPDEFREF                       	Doc::alias("cxxTypedefRef")
#define CXXUNIONREF                        	Doc::alias("cxxUnionRef")
#define CXXVARIABLEREF                     	Doc::alias("cxxVariableRef")

#define CXXCLASS                           	Doc::alias("cxxClass")
#define CXXCLASSABSTRACT                   	Doc::alias("cxxClassAbstract")
#define CXXCLASSACCESSSPECIFIER            	Doc::alias("cxxClassAccessSpecifier")
#define CXXCLASSAPIITEMLOCATION            	Doc::alias("cxxClassAPIItemLocation")
#define CXXCLASSBASECLASS                  	Doc::alias("cxxClassBaseClass")
#define CXXCLASSBASECLASSSTRUCT            	Doc::alias("cxxClassBaseStruct")
#define CXXCLASSBASEUNION                  	Doc::alias("cxxClassBaseUnion")
#define CXXCLASSDECLARATIONFILE            	Doc::alias("cxxClassDeclarationFile")
#define CXXCLASSDECLARATIONFILELINE        	Doc::alias("cxxClassDeclarationFileLine")
#define CXXCLASSDEFINITION                 	Doc::alias("cxxClassDefinition")
#define CXXCLASSDEFINITIONFILE             	Doc::alias("cxxClassDefinitionFile")
#define CXXCLASSDEFINITIONFILEEND          	Doc::alias("cxxClassDefinitionFileLineEnd")
#define CXXCLASSDEFINITIONFILESTART        	Doc::alias("cxxClassDefinitionFileLineStart")
#define CXXCLASSDERIVATION                 	Doc::alias("cxxClassDerivation")
#define CXXCLASSDERIVATIONACCESSSPECIFIER  	Doc::alias("cxxClassDerivationAccessSpecifier")
#define CXXCLASSDERIVATIONS                	Doc::alias("cxxClassDerivations")
#define CXXCLASSDERIVATIONVIRTUAL          	Doc::alias("cxxClassDerivationVirtual")
#define CXXCLASSDETAIL                     	Doc::alias("cxxClassDetail")
#define CXXCLASSENUMERATIONINHERITED       	Doc::alias("cxxClassEnumerationInherited")
#define CXXCLASSENUMERATORINHERITED        	Doc::alias("cxxClassEnumeratorInherited")
#define CXXCLASSFUNCTIONINHERITED          	Doc::alias("cxxClassFunctionInherited")
#define CXXCLASSINHERITS                   	Doc::alias("cxxClassInherits")
#define CXXCLASSINHERITSDETAIL             	Doc::alias("cxxClassInheritsDetail")
#define CXXCLASSNESTED                     	Doc::alias("cxxClassNested")
#define CXXCLASSNESTEDCLASS                	Doc::alias("cxxClassNestedClass")
#define CXXCLASSNESTEDDETAIL               	Doc::alias("cxxClassNestedDetail")
#define CXXCLASSNESTEDSTRUCT               	Doc::alias("cxxClassNestedStruct")
#define CXXCLASSNESTEDUNION                	Doc::alias("cxxClassNestedUnion")
#define CXXCLASSTEMPLATEPARAMETER          	Doc::alias("cxxClassTemplateParameter")
#define CXXCLASSTEMPLATEPARAMETERS         	Doc::alias("cxxClassTemplateParameters")
#define CXXCLASSTEMPLATEPARAMETERTYPE      	Doc::alias("cxxClassTemplateParameterType")
#define CXXCLASSVARIABLEINHERITED          	Doc::alias("cxxClassVariableInherited")

#define CXXDEFINE                          	Doc::alias("cxxDefine")
#define CXXDEFINEACCESSSPECIFIER           	Doc::alias("cxxDefineAccessSpecifier")
#define CXXDEFINEAPIITEMLOCATION           	Doc::alias("cxxDefineAPIItemLocation")
#define CXXDEFINEDECLARATIONFILE           	Doc::alias("cxxDefineDeclarationFile")
#define CXXDEFINEDECLARATIONFILELINE       	Doc::alias("cxxDefineDeclarationFileLine")
#define CXXDEFINEDEFINITION                	Doc::alias("cxxDefineDefinition")
#define CXXDEFINEDETAIL                    	Doc::alias("cxxDefineDetail")
#define CXXDEFINENAMELOOKUP                	Doc::alias("cxxDefineNameLookup")
#define CXXDEFINEPARAMETER                 	Doc::alias("cxxDefineParameter")
#define CXXDEFINEPARAMETERDECLARATIONNAME  	Doc::alias("cxxDefineParameterDeclarationName")
#define CXXDEFINEPARAMETERS                	Doc::alias("cxxDefineParameters")
#define CXXDEFINEPROTOTYPE                 	Doc::alias("cxxDefinePrototype")
#define CXXDEFINEREIMPLEMENTED             	Doc::alias("cxxDefineReimplemented")

#define CXXENUMERATION                     	Doc::alias("cxxEnumeration")
#define CXXENUMERATIONACCESSSPECIFIER      	Doc::alias("cxxEnumerationAccessSpecifier")
#define CXXENUMERATIONAPIITEMLOCATION      	Doc::alias("cxxEnumerationAPIItemLocation")
#define CXXENUMERATIONDECLARATIONFILE      	Doc::alias("cxxEnumerationDeclarationFile")
#define CXXENUMERATIONDECLARATIONFILELINE  	Doc::alias("cxxEnumerationDeclarationFileLine")
#define CXXENUMERATIONDEFINITION                Doc::alias("cxxEnumerationDefinition")
#define CXXENUMERATIONDEFINITIONFILE            Doc::alias("cxxEnumerationDefinitionFile")
#define CXXENUMERATIONDEFINITIONFILELINEEND     Doc::alias("cxxEnumerationDefinitionFileLineEnd")
#define CXXENUMERATIONDEFINITIONFILELINESTART   Doc::alias("cxxEnumerationDefinitionFileLineStart")
#define CXXENUMERATIONDETAIL                    Doc::alias("cxxEnumerationDetail")
#define CXXENUMERATIONNAMELOOKUP                Doc::alias("cxxEnumerationNameLookup")
#define CXXENUMERATIONPROTOTYPE                 Doc::alias("cxxEnumerationPrototype")
#define CXXENUMERATIONREIMPLEMENTED             Doc::alias("cxxEnumerationReimplemented")
#define CXXENUMERATIONSCOPEDNAME                Doc::alias("cxxEnumerationScopedName")
#define CXXENUMERATOR                           Doc::alias("cxxEnumerator")
#define CXXENUMERATORAPIITEMLOCATION            Doc::alias("cxxEnumeratorAPIItemLocation")
#define CXXENUMERATORDECLARATIONFILE            Doc::alias("cxxEnumeratorDeclarationFile")
#define CXXENUMERATORDECLARATIONFILELINE        Doc::alias("cxxEnumeratorDeclarationFileLine")
#define CXXENUMERATORINITIALISER                Doc::alias("cxxEnumeratorInitialiser")
#define CXXENUMERATORNAMELOOKUP                 Doc::alias("cxxEnumeratorNameLookup")
#define CXXENUMERATORPROTOTYPE                  Doc::alias("cxxEnumeratorPrototype")
#define CXXENUMERATORS                          Doc::alias("cxxEnumerators")
#define CXXENUMERATORSCOPEDNAME                 Doc::alias("cxxEnumeratorScopedName")

#define CXXFILE_INFO_TYPES                      Doc::alias("cxxFile-info-types")
#define CXXFILE_TYPES_DEFAULT                   Doc::alias("cxxFile-types-default")
#define CXXFILE                                 Doc::alias("cxxFile")
#define CXXFILEAPIITMELOCATION                  Doc::alias("cxxFileAPIItemLocation")
#define CXXFILEDECLARATIONFILE                  Doc::alias("cxxFileDeclarationFile")

#define CXXFUNCTION                             Doc::alias("cxxFunction")
#define CXXFUNCTIONACCESSSPECIFIER              Doc::alias("cxxFunctionAccessSpecifier")
#define CXXFUNCTIONAPIITEMLOCATION              Doc::alias("cxxFunctionAPIItemLocation")
#define CXXFUNCTIONCONST                        Doc::alias("cxxFunctionConst")
#define CXXFUNCTIONCONSTRUCTOR                  Doc::alias("cxxFunctionConstructor")
#define CXXFUNCTIONDECLARATIONFILE              Doc::alias("cxxFunctionDeclarationFile")
#define CXXFUNCTIONDECLARATIONFILELINE          Doc::alias("cxxFunctionDeclarationFileLine")
#define CXXFUNCTIONDECLAREDTYPE                 Doc::alias("cxxFunctionDeclaredType")
#define CXXFUNCTIONDEFINITION                   Doc::alias("cxxFunctionDefinition")
#define CXXFUNCTIONDEFINITIONFILE               Doc::alias("cxxFunctionDefinitionFile")
#define CXXFUNCTIONDEFINITIONFILELINEEND        Doc::alias("cxxFunctionDefinitionFileLineEnd")
#define CXXFUNCTIONDEFINITIONFILELINESTART      Doc::alias("cxxFunctionDefinitionFileLineStart")
#define CXXFUNCTIONDESTRUCTOR                   Doc::alias("cxxFunctionDestructor")
#define CXXFUNCTIONDETAIL                       Doc::alias("cxxFunctionDetail")
#define CXXFUNCTIONEXPLICIT                     Doc::alias("cxxFunctionExplicit")
#define CXXFUNCTIONINLINE                       Doc::alias("cxxFunctionInline")
#define CXXFUNCTIONNAMELOOKUP                   Doc::alias("cxxFunctionNameLookup")
#define CXXFUNCTIONPARAMETER                    Doc::alias("cxxFunctionParameter")
#define CXXFUNCTIONPARAMETERDECLARATIONNAME     Doc::alias("cxxFunctionParameterDeclarationName")
#define CXXFUNCTIONPARAMETERDECLAREDTYPE        Doc::alias("cxxFunctionParameterDeclaredType")
#define CXXFUNCTIONPARAMETERDEFAULTVALUE        Doc::alias("cxxFunctionParameterDefaultValue")
#define CXXFUNCTIONPARAMETERDEFINITIONNAME      Doc::alias("cxxFunctionParameterDefinitionName")
#define CXXFUNCTIONPARAMETERS                   Doc::alias("cxxFunctionParameters")
#define CXXFUNCTIONPROTOTYPE                    Doc::alias("cxxFunctionPrototype")
#define CXXFUNCTIONPUREVIRTUAL                  Doc::alias("cxxFunctionPureVirtual")
#define CXXFUNCTIONREIMPLEMENTED                Doc::alias("cxxFunctionReimplemented")
#define CXXFUNCTIONRETURNTYPE                   Doc::alias("cxxFunctionReturnType")
#define CXXFUNCTIONSCOPEDNAME                   Doc::alias("cxxFunctionScopedName")
#define CXXFUNCTIONSTORAGECLASSSPECIFIEREXTERN  Doc::alias("cxxFunctionStorageClassSpecifierExtern")
#define CXXFUNCTIONSTORAGECLASSSPECIFIERMUTABLE Doc::alias("cxxFunctionStorageClassSpecifierMutable")
#define CXXFUNCTIONSTORAGECLASSSPECIFIERSTATIC  Doc::alias("cxxFunctionStorageClassSpecifierStatic")
#define CXXFUNCTIONTEMPLATEPARAMETER            Doc::alias("cxxFunctionTemplateParameter")
#define CXXFUNCTIONTEMPLATEPARAMETERS           Doc::alias("cxxFunctionTemplateParameters")
#define CXXFUNCTIONTEMPLATEPARAMETERTYPE        Doc::alias("cxxFunctionTemplateParameterType")
#define CXXFUNCTIONVIRTUAL                      Doc::alias("cxxFunctionVirtual")
#define CXXFUNCTIONVOLATILE                     Doc::alias("cxxFunctionVolatile")

#define CXXSTRUCT                               Doc::alias("cxxStruct")
#define CXXSTRUCTABSTRACT                       Doc::alias("cxxStructAbstract")
#define CXXSTRUCTACCESSSPECIFIER                Doc::alias("cxxStructAccessSpecifier")
#define CXXSTRUCTAPIITEMLOCATION                Doc::alias("cxxStructAPIItemLocation")
#define CXXSTRUCTBASECLASS                      Doc::alias("cxxStructBaseClass")
#define CXXSTRUCTBASESTRUCT                     Doc::alias("cxxStructBaseStruct")
#define CXXSTRUCTBASEUNION                      Doc::alias("cxxStructBaseUnion")
#define CXXSTRUCTDECLARATIONFILE                Doc::alias("cxxStructDeclarationFile")
#define CXXSTRUCTDECLARATIONFILELINE            Doc::alias("cxxStructDeclarationFileLine")
#define CXXSTRUCTDEFINITION                     Doc::alias("cxxStructDefinition")
#define CXXSTRUCTDEFINITIONFILE                 Doc::alias("cxxStructDefinitionFile")
#define CXXSTRUCTDEFINITIONFILELINEEND          Doc::alias("cxxStructDefinitionFileLineEnd")
#define CXXSTRUCTDEFINITIONFILELINESTART        Doc::alias("cxxStructDefinitionFileLineStart")
#define CXXSTRUCTDERIVATION                     Doc::alias("cxxStructDerivation")
#define CXXSTRUCTDERIVATIONACCESSSPECIFIER      Doc::alias("cxxStructDerivationAccessSpecifier")
#define CXXSTRUCTDERIVATIONS                    Doc::alias("cxxStructDerivations")
#define CXXSTRUCTDERIVATIONVIRTUAL              Doc::alias("cxxStructDerivationVirtual")
#define CXXSTRUCTDETAIL                         Doc::alias("cxxStructDetail")
#define CXXSTRUCTENUMERATIONINHERITED           Doc::alias("cxxStructEnumerationInherited")
#define CXXSTRUCTENUMERATORINHERITED            Doc::alias("cxxStructEnumeratorInherited")
#define CXXSTRUCTFUNCTIONINHERITED              Doc::alias("cxxStructFunctionInherited")
#define CXXSTRUCTINHERITS                       Doc::alias("cxxStructInherits")
#define CXXSTRUCTINHERITSDETAIL                 Doc::alias("cxxStructInheritsDetail")
#define CXXSTRUCTNESTED                         Doc::alias("cxxStructNested")
#define CXXSTRUCTNESTEDCLASS                    Doc::alias("cxxStructNestedClass")
#define CXXSTRUCTNESTEDDETAIL                   Doc::alias("cxxStructNestedDetail")
#define CXXSTRUCTNESTEDSTRUCT                   Doc::alias("cxxStructNestedStruct")
#define CXXSTRUCTNESTEDUNION                    Doc::alias("cxxStructNestedUnion")
#define CXXSTRUCTTEMPLATEPARAMETER              Doc::alias("cxxStructTemplateParameter")
#define CXXSTRUCTTEMPLATEPARAMETERS             Doc::alias("cxxStructTemplateParameters")
#define CXXSTRUCTTEMPLATEPARAMETERTYPE          Doc::alias("cxxStructTemplateParameterType")
#define CXXSTRUCTVARIABLEINHERITED              Doc::alias("cxxStructVariableInherited")

#define CXXTYPEDEF                              Doc::alias("cxxTypedef")
#define CXXTYPEDEFACCESSSPECIFIER               Doc::alias("cxxTypedefAccessSpecifier")
#define CXXTYPEDEFAPIITEMLOCATION               Doc::alias("cxxTypedefAPIItemLocation")
#define CXXTYPEDEFDECLARATIONFILE               Doc::alias("cxxTypedefDeclarationFile")
#define CXXTYPEDEFDECLARATIONFILELINE           Doc::alias("cxxTypedefDeclarationFileLine")
#define CXXTYPEDEFDECLAREDTYPE                  Doc::alias("cxxTypedefDeclaredType")
#define CXXTYPEDEFDEFINITION                    Doc::alias("cxxTypedefDefinition")
#define CXXTYPEDEFDETAIL                        Doc::alias("cxxTypedefDetail")
#define CXXTYPEDEFNAMELOOKUP                    Doc::alias("cxxTypedefNameLookup")
#define CXXTYPEDEFPROTOTYPE                     Doc::alias("cxxTypedefPrototype")
#define CXXTYPEDEFREIMPLEMENTED                 Doc::alias("cxxTypedefReimplemented")
#define CXXTYPEDEFSCOPEDNAME                    Doc::alias("cxxTypedefScopedName")

#define CXXUNION                                Doc::alias("cxxUnion")
#define CXXUNIONABSTRACT                        Doc::alias("cxxUnionAbstract")
#define CXXUNIONACCESSSPECIFIER                 Doc::alias("cxxUnionAccessSpecifier")
#define CXXUNIONAPIITEMLOCATION                 Doc::alias("cxxUnionAPIItemLocation")
#define CXXUNIONDECLARATIONFILE                 Doc::alias("cxxUnionDeclarationFile")
#define CXXUNIONDECLARATIONFILELINE             Doc::alias("cxxUnionDeclarationFileLine")
#define CXXUNIONDEFINITION                      Doc::alias("cxxUnionDefinition")
#define CXXUNIONDEFINITIONFILE                  Doc::alias("cxxUnionDefinitionFile")
#define CXXUNIONDEFINITIONFILELINEEND           Doc::alias("cxxUnionDefinitionFileLineEnd")
#define CXXUNIONDEFINITIONFILELINESTART         Doc::alias("cxxUnionDefinitionFileLineStart")
#define CXXUNIONDETAIL                          Doc::alias("cxxUnionDetail")
#define CXXUNIONNESTED                          Doc::alias("cxxUnionNested")
#define CXXUNIONNESTEDCLASS                     Doc::alias("cxxUnionNestedClass")
#define CXXUNIONNESTEDDETAIL                    Doc::alias("cxxUnionNestedDetail")
#define CXXUNIONNESTEDSTRUCT                    Doc::alias("cxxUnionNestedStruct")
#define CXXUNIONNESTEDUNION                     Doc::alias("cxxUnionNestedUnion")
#define CXXUNIONTEMPLATEPARAMETER               Doc::alias("cxxUnionTemplateParameter")
#define CXXUNIONTEMPLATEPARAMETERS              Doc::alias("cxxUnionTemplateParameters")
#define CXXUNIONTEMPLATEPARAMETERTYPE           Doc::alias("cxxUnionTemplateParameterType")

#define CXXVARIABLE                             Doc::alias("cxxVariable")
#define CXXVARIABLEACCESSSPECIFIER              Doc::alias("cxxVariableAccessSpecifier")
#define CXXVARIABLEAPIITEMLOCATION              Doc::alias("cxxVariableAPIItemLocation")
#define CXXVARIABLECONST                        Doc::alias("cxxVariableConst")
#define CXXVARIABLEDECLARATIONFILE              Doc::alias("cxxVariableDeclarationFile")
#define CXXVARIABLEDECLARATIONFILELINE          Doc::alias("cxxVariableDeclarationFileLine")
#define CXXVARIABLEDECLAREDTYPE                 Doc::alias("cxxVariableDeclaredType")
#define CXXVARIABLEDEFINITION                   Doc::alias("cxxVariableDefinition")
#define CXXVARIABLEDETAIL                       Doc::alias("cxxVariableDetail")
#define CXXVARIABLENAMELOOKUP                   Doc::alias("cxxVariableNameLookup")
#define CXXVARIABLEPROTOTYPE                    Doc::alias("cxxVariablePrototype")
#define CXXVARIABLEREIMPLEMENTED                Doc::alias("cxxVariableReimplemented")
#define CXXVARIABLESCOPEDNAME                   Doc::alias("cxxVariableScopedName")
#define CXXVARIABLESTORAGECLASSSPECIFIEREXTERN  Doc::alias("cxxVariableStorageClassSpecifierExtern")
#define CXXVARIABLESTORAGECLASSSPECIFIERMUTABLE Doc::alias("cxxVariableStorageClassSpecifierMutable")
#define CXXVARIABLESTORAGECLASSSPECIFIERSTATIC  Doc::alias("cxxVariableStorageClassSpecifierStatic")
#define CXXVARIABLEVOLATILE                     Doc::alias("cxxVariableVolatile")

#define APIREF		                        Doc::alias("apiRef")
#define APINAME		                        Doc::alias("apiName")
#define APIDETAIL		                Doc::alias("apiDetail")
#define APISYNTAX		                Doc::alias("apiSyntax")
#define APISYNTAXTEXT				Doc::alias("apiSyntaxText")
#define APISYNTAXITEM				Doc::alias("apiSyntaxItem")
#define APIDEF		        		Doc::alias("apiDef")
#define APIQUALIFIER				Doc::alias("apiQualifier")
#define APIRELATION				Doc::alias("apiRelation")
#define APITYPE		        		Doc::alias("apiType")
#define APIARRAY		        	Doc::alias("apiArray")
#define APIDATA		        		Doc::alias("apiData")
#define APIDEFNOTE		        	Doc::alias("apiDefNote")
#define APIDEFITEM		        	Doc::alias("apiDefItem")
#define APIITEMNAME				Doc::alias("apiItemName")
#define APIDESC		        		Doc::alias("apiDesc")
#define APIIMPL		        		Doc::alias("apiImpl")

#define APIPACKAGE		        	Doc::alias("apiPackage")

#define APICLASSIFIER				Doc::alias("apiClassifier")
#define APICLASSIFIERDETAIL			Doc::alias("apiClassifierDetail")
#define APICLASSIFIERDEF			Doc::alias("apiClassifierDef")
#define APICLASSIFIERMEMBER			Doc::alias("apiClassifierMember")
#define APIOTHERCLASSIFIER			Doc::alias("apiOtherClassifier")
#define APIBASECLASSIFIER			Doc::alias("apiBaseClassifier")

#define APIOPERATION				Doc::alias("apiOperation")
#define APIOPERATIONDETAIL			Doc::alias("apiOperationDetail")
#define APIOPERATIONDEF				Doc::alias("apiOperationDef")
#define APIRETURN		        	Doc::alias("apiReturn")
#define APIPARAM		        	Doc::alias("apiParam")
#define APIEVENT		        	Doc::alias("apiEvent")
#define APIOPERATIONDEFITEM			Doc::alias("apiOperationDefItem")
#define APIOPERATIONCLASSIFIER			Doc::alias("apiOperationClassifier")
#define APICONSTRUCTORDEF			Doc::alias("apiConstructorDef")

#define APIVALUE		        	Doc::alias("apiValue")
#define APIVALUEDETAIL				Doc::alias("apiValueDetail")
#define APIVALUEDEF				Doc::alias("apiValueDef")
#define APIVALUEMEMBER				Doc::alias("apiValueMember")
#define APIVALUECLASSIFIER			Doc::alias("apiValueClassifier")

#define APIclassifier				Doc::alias("apiclassifier")
#define APIoperation				Doc::alias("apioperation")
#define APIpackage		        	Doc::alias("apipackage")
#define APIvalue		        	Doc::alias("apivalue")

#define APIMAP		        		Doc::alias("apiMap")
#define APIITEMREF		        	Doc::alias("apiItemRef")

#define SHORTDESC                               Doc::alias("shortdesc")

QString DitaXmlGenerator::sinceTitles[] =
    {
        "    New Namespaces",
        "    New Classes",
        "    New Member Functions",
        "    New Functions in Namespaces",
        "    New Global Functions",
        "    New Macros",
        "    New Enum Types",
        "    New Typedefs",
        "    New Properties",
        "    New Variables",
        "    New QML Elements",
        "    New Qml Properties",
        "    New Qml Signals",
        "    New Qml Methods",
        ""
    };

static bool showBrokenLinks = false;

static void addLink(const QString &linkTarget,
                    const QStringRef &nestedStuff,
                    QString *res)
{
    if (!linkTarget.isEmpty()) {
        *res += "<xref href=\"";
        *res += linkTarget;
        *res += "\">";
        *res += nestedStuff;
        *res += "</xref>";
    }
    else {
        *res += nestedStuff;
    }
}


DitaXmlGenerator::DitaXmlGenerator()
    : inLink(false),
      inContents(false),
      inSectionHeading(false),
      inTableHeader(false),
      numTableRows(0),
      threeColumnEnumValueTable(true),
      offlineDocs(true),
      funcLeftParen("\\S(\\()"),
      myTree(0),
      slow(false),
      obsoleteLinks(false),
      noLinks(0)
{
}

DitaXmlGenerator::~DitaXmlGenerator()
{
    // nothing yet.
}

void DitaXmlGenerator::initializeGenerator(const Config &config)
{
    static const struct {
        const char *key;
        const char *tag;
    } defaults[] = {
        { ATOM_FORMATTING_BOLD, "b" },
        { ATOM_FORMATTING_INDEX, "<!--" },
        { ATOM_FORMATTING_ITALIC, "i" },
        { ATOM_FORMATTING_PARAMETER, "i" },
        { ATOM_FORMATTING_SUBSCRIPT, "sub" },
        { ATOM_FORMATTING_SUPERSCRIPT, "sup" },
        { ATOM_FORMATTING_TELETYPE, "tt", },
        { ATOM_FORMATTING_UNDERLINE, "u", },
        { 0, 0 }
    };

    Generator::initializeGenerator(config);
    obsoleteLinks = config.getBool(QLatin1String(CONFIG_OBSOLETELINKS));
    setImageFileExtensions(QStringList() << "png" << "jpg" << "jpeg" << "gif");
    int i = 0;
    while (defaults[i].key) {
        formattingLeftMap().insert(defaults[i].key, defaults[i].tag);
        i++;
    }

    style = config.getString(DitaXmlGenerator::format() +
                             Config::dot +
                             DITAXMLGENERATOR_STYLE);
    postHeader = config.getString(DitaXmlGenerator::format() +
                                  Config::dot +
                                  DITAXMLGENERATOR_POSTHEADER);
    postPostHeader = config.getString(DitaXmlGenerator::format() +
                                      Config::dot +
                                      DITAXMLGENERATOR_POSTPOSTHEADER);
    footer = config.getString(DitaXmlGenerator::format() +
                              Config::dot +
                              DITAXMLGENERATOR_FOOTER);
    address = config.getString(DitaXmlGenerator::format() +
                               Config::dot +
                               DITAXMLGENERATOR_ADDRESS);
    pleaseGenerateMacRef = config.getBool(DitaXmlGenerator::format() +
                                          Config::dot +
                                          DITAXMLGENERATOR_GENERATEMACREFS);

    project = config.getString(CONFIG_PROJECT);
    projectDescription = config.getString(CONFIG_DESCRIPTION);
    if (projectDescription.isEmpty() && !project.isEmpty())
        projectDescription = project + " Reference Documentation";

    projectUrl = config.getString(CONFIG_URL);

    outputEncoding = config.getString(CONFIG_OUTPUTENCODING);
    if (outputEncoding.isEmpty())
        outputEncoding = QLatin1String("ISO-8859-1");
    outputCodec = QTextCodec::codecForName(outputEncoding.toLocal8Bit());

    naturalLanguage = config.getString(CONFIG_NATURALLANGUAGE);
    if (naturalLanguage.isEmpty())
        naturalLanguage = QLatin1String("en");

    QSet<QString> editionNames = config.subVars(CONFIG_EDITION);
    QSet<QString>::ConstIterator edition = editionNames.begin();
    while (edition != editionNames.end()) {
        QString editionName = *edition;
        QStringList editionModules = config.getStringList(CONFIG_EDITION +
                                                          Config::dot +
                                                          editionName +
                                                          Config::dot +
                                                          "modules");
        QStringList editionGroups = config.getStringList(CONFIG_EDITION +
                                                         Config::dot +
                                                         editionName +
                                                         Config::dot +
                                                         "groups");

        if (!editionModules.isEmpty())
            editionModuleMap[editionName] = editionModules;
        if (!editionGroups.isEmpty())
            editionGroupMap[editionName] = editionGroups;

        ++edition;
    }

    slow = config.getBool(CONFIG_SLOW);

    stylesheets = config.getStringList(DitaXmlGenerator::format() +
                                       Config::dot +
                                       DITAXMLGENERATOR_STYLESHEETS);
    customHeadElements = config.getStringList(DitaXmlGenerator::format() +
                                              Config::dot +
                                              DITAXMLGENERATOR_CUSTOMHEADELEMENTS);
    codeIndent = config.getInt(CONFIG_CODEINDENT);

}

void DitaXmlGenerator::terminateGenerator()
{
    Generator::terminateGenerator();
}

QString DitaXmlGenerator::format()
{
    return "DITAXML";
}

/*!
  Calls lookupGuid() to get a GUID for \a text, then writes
  it to the XML stream as an "id" attribute, and returns it.
 */
QString DitaXmlGenerator::writeGuidAttribute(QString text)
{
    QString guid = lookupGuid(text);
    writer.writeAttribute("id",guid);
    return guid;
}

/*!
  Looks up \a text in the GUID map. If it finds \a text,
  it returns the associated GUID. Otherwise it inserts
  \a text into the map with a new GUID, and it returns
  the new GUID.
 */
QString DitaXmlGenerator::lookupGuid(QString text)
{
    QMap<QString, QString>::const_iterator i = name2guidMap.find(text);
    if (i != name2guidMap.end())
        return i.value();
    QString guid = QUuid::createUuid().toString();
    name2guidMap.insert(text,guid);
    return guid;
}

/*!
  This is where the DITA XML files are written.
  \note The file generation is done in the base class,
  PageGenerator::generateTree().
 */
void DitaXmlGenerator::generateTree(const Tree *tree, CodeMarker *marker)
{
    myTree = tree;
    nonCompatClasses.clear();
    mainClasses.clear();
    compatClasses.clear();
    obsoleteClasses.clear();
    moduleClassMap.clear();
    moduleNamespaceMap.clear();
    funcIndex.clear();
    legaleseTexts.clear();
    serviceClasses.clear();
    findAllClasses(tree->root());
    findAllFunctions(tree->root());
    findAllLegaleseTexts(tree->root());
    findAllNamespaces(tree->root());
    findAllSince(tree->root());

    PageGenerator::generateTree(tree, marker);
}

void DitaXmlGenerator::startText(const Node* /* relative */,
                                 CodeMarker* /* marker */)
{
    inLink = false;
    inContents = false;
    inSectionHeading = false;
    inTableHeader = false;
    numTableRows = 0;
    threeColumnEnumValueTable = true;
    link.clear();
    sectionNumber.clear();
}

/*!
  Generate html from an instance of Atom.
 */
int DitaXmlGenerator::generateAtom(const Atom *atom,
                                   const Node *relative,
                                   CodeMarker *marker)
{
    int skipAhead = 0;
    QString hx;
    static bool in_para = false;
    QString guid;
    
    switch (atom->type()) {
    case Atom::AbstractLeft:
        break;
    case Atom::AbstractRight:
        break;
    case Atom::AutoLink:
        if ((noLinks > 0) && !inLink && !inContents && !inSectionHeading) {
            const Node *node = 0;
            QString link = getLink(atom, relative, marker, &node);
            if (!link.isEmpty()) {
                beginLink(link, node, relative, marker);
                generateLink(atom, relative, marker);
                endLink();
            }
            else {
                writer.writeCharacters(protectEnc(atom->string()));
            }
        }
        else {
            writer.writeCharacters(protectEnc(atom->string()));
        }
        break;
    case Atom::BaseName:
        break;
    case Atom::BriefLeft:
        if (relative->type() == Node::Fake) {
            skipAhead = skipAtoms(atom, Atom::BriefRight);
            break;
        }
        writer.writeStartElement(SHORTDESC);
        if (relative->type() == Node::Property ||
            relative->type() == Node::Variable) {
            QString str;
            atom = atom->next();
            while (atom != 0 && atom->type() != Atom::BriefRight) {
                if (atom->type() == Atom::String ||
                    atom->type() == Atom::AutoLink)
                    str += atom->string();
                skipAhead++;
                atom = atom->next();
            }
            str[0] = str[0].toLower();
            if (str.right(1) == ".")
                str.truncate(str.length() - 1);
            writer.writeCharacters("This ");
            if (relative->type() == Node::Property)
                writer.writeCharacters("property");
            else
                writer.writeCharacters("variable");
            QStringList words = str.split(" ");
            if (!(words.first() == "contains" || words.first() == "specifies"
                || words.first() == "describes" || words.first() == "defines"
                || words.first() == "holds" || words.first() == "determines"))
                writer.writeCharacters(" holds ");
            else
                writer.writeCharacters(" ");
            writer.writeCharacters(str + ".");
        }
        break;
    case Atom::BriefRight:
        if (relative->type() != Node::Fake) {
            writer.writeEndElement(); // </shortdesc>
        }
        break;
    case Atom::C:
        writer.writeStartElement(formattingLeftMap()[ATOM_FORMATTING_TELETYPE]);
        if (inLink) {
            writer.writeCharacters(protectEnc(plainCode(atom->string())));
        }
        else {
            writer.writeCharacters(highlightedCode(atom->string(), marker, relative));
        }
        writer.writeEndElement(); // sse writeStartElement() above
        break;
    case Atom::Code:
        writer.writeStartElement("pre");
        writer.writeAttribute("outputclass","highlightedCode");
        writer.writeCharacters(trimmedTrailing(highlightedCode(indent(codeIndent,atom->string()),
                                                               marker,
                                                               relative)));
        writer.writeEndElement(); // </pre>
	break;
#ifdef QDOC_QML
    case Atom::Qml:
        writer.writeStartElement("pre");
        writer.writeAttribute("outputclass","highlightedCode");
        writer.writeCharacters(trimmedTrailing(highlightedCode(indent(codeIndent,atom->string()),
                                                               marker,
                                                               relative)));
        writer.writeEndElement(); // pre
	break;
#endif
    case Atom::CodeNew:
        writer.writeStartElement("p");
        writer.writeCharacters("you can rewrite it as");
        writer.writeEndElement(); // </p>
        writer.writeStartElement("pre");
        writer.writeAttribute("outputclass","highlightedCode");
        writer.writeCharacters(trimmedTrailing(highlightedCode(indent(codeIndent,atom->string()),
                                                               marker,
                                                               relative)));
        writer.writeEndElement(); // </pre>
        break;
    case Atom::CodeOld:
        writer.writeStartElement("p");
        writer.writeCharacters("For example, if you have code like");
        writer.writeEndElement(); // </p>
        // fallthrough
    case Atom::CodeBad:
        writer.writeStartElement("pre");
        writer.writeAttribute("outputclass","highlightedCode");
        writer.writeCharacters(trimmedTrailing(protectEnc(plainCode(indent(codeIndent,atom->string())))));
        writer.writeEndElement(); // </pre>
	break;
    case Atom::FootnoteLeft:
        // ### For now
        if (in_para) {
            writer.writeEndElement(); // </p>
            in_para = false;
        }
        writer.writeCharacters("<!-- ");
        break;
    case Atom::FootnoteRight:
        // ### For now
        writer.writeCharacters("-->");
        break;
    case Atom::FormatElse:
    case Atom::FormatEndif:
    case Atom::FormatIf:
        break;
    case Atom::FormattingLeft:
        writer.writeStartElement(formattingLeftMap()[atom->string()]);
        if (atom->string() == ATOM_FORMATTING_PARAMETER) {
            if (atom->next() != 0 && atom->next()->type() == Atom::String) {
                QRegExp subscriptRegExp("([a-z]+)_([0-9n])");
                if (subscriptRegExp.exactMatch(atom->next()->string())) {
                    writer.writeCharacters(subscriptRegExp.cap(1));
                    writer.writeStartElement("sub");
                    writer.writeCharacters(subscriptRegExp.cap(2));
                    writer.writeEndElement(); // </sub>
                    skipAhead = 1;
                }
            }
        }
        break;
    case Atom::FormattingRight:
        if (atom->string() == ATOM_FORMATTING_LINK) {
            endLink();
        }
        else {
            writer.writeEndElement(); // ?
        }
        break;
    case Atom::AnnotatedList:
        {
            QList<Node*> values = myTree->groups().values(atom->string());
            NodeMap nodeMap;
            for (int i = 0; i < values.size(); ++i) {
                const Node* n = values.at(i);
                if ((n->status() != Node::Internal) && (n->access() != Node::Private)) {
                    nodeMap.insert(n->nameForLists(),n);
                }
            }
            generateAnnotatedList(relative, marker, nodeMap);
        }
        break;
    case Atom::GeneratedList:
        if (atom->string() == "annotatedclasses") {
            generateAnnotatedList(relative, marker, nonCompatClasses);
        }
        else if (atom->string() == "classes") {
            generateCompactList(relative, marker, nonCompatClasses, true);
        }
        else if (atom->string().contains("classesbymodule")) {
            QString arg = atom->string().trimmed();
            QString moduleName = atom->string().mid(atom->string().indexOf(
                "classesbymodule") + 15).trimmed();
            if (moduleClassMap.contains(moduleName))
                generateAnnotatedList(relative, marker, moduleClassMap[moduleName]);
        }
        else if (atom->string().contains("classesbyedition")) {

            QString arg = atom->string().trimmed();
            QString editionName = atom->string().mid(atom->string().indexOf(
                "classesbyedition") + 16).trimmed();

            if (editionModuleMap.contains(editionName)) {

                // Add all classes in the modules listed for that edition.
                NodeMap editionClasses;
                foreach (const QString &moduleName, editionModuleMap[editionName]) {
                    if (moduleClassMap.contains(moduleName))
                        editionClasses.unite(moduleClassMap[moduleName]);
                }

                // Add additional groups and remove groups of classes that
                // should be excluded from the edition.

                QMultiMap <QString, Node *> groups = myTree->groups();
                foreach (const QString &groupName, editionGroupMap[editionName]) {
                    QList<Node *> groupClasses;
                    if (groupName.startsWith("-")) {
                        groupClasses = groups.values(groupName.mid(1));
                        foreach (const Node *node, groupClasses)
                            editionClasses.remove(node->name());
                    }
                    else {
                        groupClasses = groups.values(groupName);
                        foreach (const Node *node, groupClasses)
                            editionClasses.insert(node->name(), node);
                    }
                }
                generateAnnotatedList(relative, marker, editionClasses);
            }
        }
        else if (atom->string() == "classhierarchy") {
            generateClassHierarchy(relative, marker, nonCompatClasses);
        }
        else if (atom->string() == "compatclasses") {
            generateCompactList(relative, marker, compatClasses, false);
        }
        else if (atom->string() == "obsoleteclasses") {
            generateCompactList(relative, marker, obsoleteClasses, false);
        }
        else if (atom->string() == "functionindex") {
            generateFunctionIndex(relative, marker);
        }
        else if (atom->string() == "legalese") {
            generateLegaleseList(relative, marker);
        }
        else if (atom->string() == "mainclasses") {
            generateCompactList(relative, marker, mainClasses, true);
        }
        else if (atom->string() == "services") {
            generateCompactList(relative, marker, serviceClasses, false);
        }
        else if (atom->string() == "overviews") {
            generateOverviewList(relative, marker);
        }
        else if (atom->string() == "namespaces") {
            generateAnnotatedList(relative, marker, namespaceIndex);
        }
        else if (atom->string() == "related") {
            const FakeNode *fake = static_cast<const FakeNode *>(relative);
            if (fake && !fake->groupMembers().isEmpty()) {
                NodeMap groupMembersMap;
                foreach (const Node *node, fake->groupMembers()) {
                    if (node->type() == Node::Fake)
                        groupMembersMap[fullName(node, relative, marker)] = node;
                }
                generateAnnotatedList(fake, marker, groupMembersMap);
            }
        }
        else if (atom->string() == "relatedinline") {
            const FakeNode *fake = static_cast<const FakeNode *>(relative);
            if (fake && !fake->groupMembers().isEmpty()) {
                // Reverse the list into the original scan order.
                // Should be sorted.  But on what?  It may not be a
                // regular class or page definition.
                QList<const Node *> list;
                foreach (const Node *node, fake->groupMembers())
                    list.prepend(node);
                foreach (const Node *node, list)
                    generateBody(node, marker);
            }
        }
        break;
    case Atom::SinceList:
        {
            NewSinceMaps::const_iterator nsmap;
            nsmap = newSinceMaps.find(atom->string());
            NewClassMaps::const_iterator ncmap;
            ncmap = newClassMaps.find(atom->string());
            NewClassMaps::const_iterator nqcmap;
            nqcmap = newQmlClassMaps.find(atom->string());
            if ((nsmap != newSinceMaps.constEnd()) && !nsmap.value().isEmpty()) {
                QList<Section> sections;
                QList<Section>::ConstIterator s;
                for (int i=0; i<LastSinceType; ++i)
                    sections.append(Section(sinceTitle(i),QString(),QString(),QString()));

                NodeMultiMap::const_iterator n = nsmap.value().constBegin();
                while (n != nsmap.value().constEnd()) {
                    const Node* node = n.value();
                    switch (node->type()) {
                      case Node::Fake:
                          if (node->subType() == Node::QmlClass) {
                              sections[QmlClass].appendMember((Node*)node);
                          }
                          break;
                      case Node::Namespace:
                          sections[Namespace].appendMember((Node*)node);
                          break;
                      case Node::Class: 
                          sections[Class].appendMember((Node*)node);
                          break;
                      case Node::Enum: 
                          sections[Enum].appendMember((Node*)node);
                          break;
                      case Node::Typedef: 
                          sections[Typedef].appendMember((Node*)node);
                          break;
                      case Node::Function: {
                          const FunctionNode* fn = static_cast<const FunctionNode*>(node);
                          if (fn->isMacro())
                              sections[Macro].appendMember((Node*)node);
                          else {
                              Node* p = fn->parent();
                              if (p) {
                                  if (p->type() == Node::Class)
                                      sections[MemberFunction].appendMember((Node*)node);
                                  else if (p->type() == Node::Namespace) {
                                      if (p->name().isEmpty())
                                          sections[GlobalFunction].appendMember((Node*)node);
                                      else
                                          sections[NamespaceFunction].appendMember((Node*)node);
                                  }
                                  else
                                      sections[GlobalFunction].appendMember((Node*)node);
                              }
                              else
                                  sections[GlobalFunction].appendMember((Node*)node);
                          }
                          break;
                      }
                      case Node::Property:
                          sections[Property].appendMember((Node*)node);
                          break;
                      case Node::Variable: 
                          sections[Variable].appendMember((Node*)node);
                          break;
                      case Node::QmlProperty:
                          sections[QmlProperty].appendMember((Node*)node);
                          break;
                      case Node::QmlSignal:
                          sections[QmlSignal].appendMember((Node*)node);
                          break;
                      case Node::QmlMethod:
                          sections[QmlMethod].appendMember((Node*)node);
                          break;
                      default:
                          break;
                    }
                    ++n;
                }

                /*
                  First generate the table of contents.
                 */
                writer.writeStartElement("ul");
                s = sections.constBegin();
                while (s != sections.constEnd()) {
                    if (!(*s).members.isEmpty()) {

                        writer.writeStartElement("li");
                        writer.writeStartElement("xref");
                        writer.writeAttribute("href",QString("#" + Doc::canonicalTitle((*s).name)));
                        writer.writeCharacters((*s).name);
                        writer.writeEndElement(); // </xref>
                        writer.writeEndElement(); // </li>
                    }
                    ++s;
                }
                writer.writeEndElement(); // </ul>

                int idx = 0;
                s = sections.constBegin();
                while (s != sections.constEnd()) {
                    if (!(*s).members.isEmpty()) {
                        writer.writeStartElement("p");
                        writeGuidAttribute(Doc::canonicalTitle((*s).name));
                        writer.writeAttribute("outputclass","h3");
                        writer.writeCharacters(protectEnc((*s).name));
                        writer.writeEndElement(); // </p>
                        if (idx == Class)
                            generateCompactList(0, marker, ncmap.value(), false, QString("Q"));
                        else if (idx == QmlClass)
                            generateCompactList(0, marker, nqcmap.value(), false, QString("Q"));
                        else if (idx == MemberFunction) {
                            ParentMaps parentmaps;
                            ParentMaps::iterator pmap;
                            NodeList::const_iterator i = s->members.constBegin();
                            while (i != s->members.constEnd()) {
                                Node* p = (*i)->parent();
                                pmap = parentmaps.find(p);
                                if (pmap == parentmaps.end())
                                    pmap = parentmaps.insert(p,NodeMultiMap());
                                pmap->insert((*i)->name(),(*i));
                                ++i;
                            }
                            pmap = parentmaps.begin();
                            while (pmap != parentmaps.end()) {
                                NodeList nlist = pmap->values();
                                writer.writeStartElement("p");
                                writer.writeCharacters("Class ");
                                writer.writeStartElement("xref");
                                writer.writeAttribute("href",linkForNode(pmap.key(), 0));
                                QStringList pieces = fullName(pmap.key(), 0, marker).split("::");
                                writer.writeCharacters(protectEnc(pieces.last()));
                                writer.writeEndElement(); // </xref>
                                writer.writeCharacters(":");
                                writer.writeEndElement(); // </p>

                                generateSection(nlist, 0, marker, CodeMarker::Summary);
                                writer.writeEmptyElement("br");
                                ++pmap;
                            }
                        }
                        else
                            generateSection(s->members, 0, marker, CodeMarker::Summary);
                     }
                    ++idx;
                    ++s;
                }
            }
        }
        break;
    case Atom::Image:
    case Atom::InlineImage:
        {
            QString fileName = imageFileName(relative, atom->string());
            QString text;
            if (atom->next() != 0)
                text = atom->next()->string();
            if (atom->type() == Atom::Image) {
                writer.writeStartElement("p");
                writer.writeAttribute("outputclass","centerAlign");
            }
            if (fileName.isEmpty()) {
                writer.writeStartElement("font");
                writer.writeAttribute("color","red");
                writer.writeCharacters("[Missing image: ");
                writer.writeCharacters(protectEnc(atom->string()));
                writer.writeEndElement(); // </font>
            }
            else {
                writer.writeStartElement("img");
                writer.writeAttribute("src",protectEnc(fileName));
                if (!text.isEmpty())
                    writer.writeAttribute("alt",protectEnc(text));
                writer.writeEndElement(); // </img>
            }
            if (atom->type() == Atom::Image)
                writer.writeEndElement(); // </p>
        }
        break;
    case Atom::ImageText:
        // nothing
        break;
    case Atom::LegaleseLeft:
        writer.writeStartElement("p");
        writer.writeAttribute("outputclass","legalese");
        break;
    case Atom::LegaleseRight:
        writer.writeEndElement(); // </p>
        break;
    case Atom::LineBreak:
        writer.writeEmptyElement("br");
        break;
    case Atom::Link:
        {
            const Node *node = 0;
            QString myLink = getLink(atom, relative, marker, &node);
            if (myLink.isEmpty()) {
                relative->doc().location().warning(tr("Cannot link to '%1' in %2")
                        .arg(atom->string())
                        .arg(marker->plainFullName(relative)));
            }
            beginLink(myLink, node, relative, marker);
            skipAhead = 1;
        }
        break;
    case Atom::LinkNode:
        {
            const Node *node = CodeMarker::nodeForString(atom->string());
            beginLink(linkForNode(node, relative), node, relative, marker);
            skipAhead = 1;
        }
        break;
    case Atom::ListLeft:
        if (in_para) {
            writer.writeEndElement(); // </p>
            in_para = false;
        }
        if (atom->string() == ATOM_LIST_BULLET) {
            writer.writeStartElement("ul");
        }
        else if (atom->string() == ATOM_LIST_TAG) {
            writer.writeStartElement("dl");
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            threeColumnEnumValueTable = isThreeColumnEnumValueTable(atom);
            if (threeColumnEnumValueTable) {
                writer.writeStartElement("table");
                writer.writeAttribute("outputclass","valuelist");
                writer.writeStartElement("tr");
                if (++numTableRows % 2 == 1)
                    writer.writeAttribute("outputclass","odd");
                else
                    writer.writeAttribute("outputclass","even");
                writer.writeStartElement("th");
                writer.writeCharacters("Constant");
                writer.writeEndElement(); // </th>
                writer.writeStartElement("th");
                writer.writeCharacters("Value");
                writer.writeEndElement(); // </th>
                writer.writeStartElement("th");
                writer.writeCharacters("Description");
                writer.writeEndElement(); // </th>
                writer.writeEndElement(); // </tr>
            }
            else {
                writer.writeStartElement("table");
                writer.writeAttribute("outputclass","valuelist");
                writer.writeStartElement("tr");
                writer.writeStartElement("th");
                writer.writeCharacters("Constant");
                writer.writeEndElement(); // </th>
                writer.writeStartElement("th");
                writer.writeCharacters("Value");
                writer.writeEndElement(); // </th>
                writer.writeEndElement(); // </tr>
            }
        }
        else {
            writer.writeStartElement("ol");
            if (atom->string() == ATOM_LIST_UPPERALPHA)
                writer.writeAttribute("type","A");
            else if (atom->string() == ATOM_LIST_LOWERALPHA)
                writer.writeAttribute("type","a");
            else if (atom->string() == ATOM_LIST_UPPERROMAN)
                writer.writeAttribute("type","I");
            else if (atom->string() == ATOM_LIST_LOWERROMAN)
                writer.writeAttribute("type","i");
            else // (atom->string() == ATOM_LIST_NUMERIC)
                writer.writeAttribute("type","1");
            if (atom->next() != 0 && atom->next()->string().toInt() != 1)
                writer.writeAttribute("start",atom->next()->string());
        }
        break;
    case Atom::ListItemNumber:
        // nothing
        break;
    case Atom::ListTagLeft:
        if (atom->string() == ATOM_LIST_TAG) {
            writer.writeStartElement("dt");
        }
        else { // (atom->string() == ATOM_LIST_VALUE)
            writer.writeStartElement("tr");
            writer.writeStartElement("td");
            writer.writeAttribute("outputclass","topAlign");
            writer.writeStartElement("tt");
            writer.writeCharacters(protectEnc(plainCode(marker->markedUpEnumValue(atom->next()->string(),
                                                                                  relative))));
            writer.writeEndElement(); // </tt>
            writer.writeEndElement(); // </td>
            writer.writeStartElement("td");
            writer.writeAttribute("outputclass","topAlign");

            QString itemValue;
            if (relative->type() == Node::Enum) {
                const EnumNode *enume = static_cast<const EnumNode *>(relative);
                itemValue = enume->itemValue(atom->next()->string());
            }

            if (itemValue.isEmpty())
                writer.writeCharacters("?");
            else {
                writer.writeStartElement("tt");
                writer.writeCharacters(protectEnc(itemValue));
                writer.writeEndElement(); // </tt>
            }
            skipAhead = 1;
        }
        break;
    case Atom::ListTagRight:
        if (atom->string() == ATOM_LIST_TAG)
            writer.writeEndElement(); // </dt>
        break;
    case Atom::ListItemLeft:
        if (atom->string() == ATOM_LIST_TAG) {
            writer.writeStartElement("dd");
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            if (threeColumnEnumValueTable) {
                writer.writeEndElement(); // </td>
                writer.writeStartElement("td");
                writer.writeAttribute("outputclass","topAlign");
                if (matchAhead(atom, Atom::ListItemRight))
                    writer.writeCharacters("&nbsp;");
            }
        }
        else {
            writer.writeStartElement("li");
        }
        if (matchAhead(atom, Atom::ParaLeft))
            skipAhead = 1;
        break;
    case Atom::ListItemRight:
        if (atom->string() == ATOM_LIST_TAG) {
            writer.writeEndElement(); // </dd>
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            writer.writeEndElement(); // </td>
            writer.writeEndElement(); // </tr>
        }
        else {
            writer.writeEndElement(); // </li>
        }
        break;
    case Atom::ListRight:
        if (atom->string() == ATOM_LIST_BULLET) {
            writer.writeEndElement(); // </ul>
        }
        else if (atom->string() == ATOM_LIST_TAG) {
            writer.writeEndElement(); // </dl>
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            writer.writeEndElement(); // </table>
        }
        else {
            writer.writeEndElement(); // </ol>
        }
        break;
    case Atom::Nop:
        // nothing
        break;
    case Atom::ParaLeft:
        writer.writeStartElement("p");
        in_para = true;
        break;
    case Atom::ParaRight:
        endLink();
        if (in_para) {
            writer.writeEndElement(); // </p?
            in_para = false;
        }
        break;
    case Atom::QuotationLeft:
        writer.writeStartElement("blockquote");
        break;
    case Atom::QuotationRight:
        writer.writeEndElement(); // </blockquote>
        break;
    case Atom::RawString:
        writer.writeCharacters(atom->string());
        break;
    case Atom::SectionLeft:
        writer.writeStartElement("p");
        writeGuidAttribute(Doc::canonicalTitle(Text::sectionHeading(atom).toString()));
        writer.writeAttribute("outputclass","target");
        writer.writeCharacters(protectEnc(Text::sectionHeading(atom).toString()));
        writer.writeEndElement(); // </p>
        break;
    case Atom::SectionRight:
        // nothing
        break;
    case Atom::SectionHeadingLeft:
        writer.writeStartElement("p");
        hx = "h" + QString::number(atom->string().toInt() + hOffset(relative));
        writer.writeAttribute("outputclass",hx);
        inSectionHeading = true;
        break;
    case Atom::SectionHeadingRight:
        writer.writeEndElement(); // </p> (see case Atom::SectionHeadingLeft)
        inSectionHeading = false;
        break;
    case Atom::SidebarLeft:
        // nothing
        break;
    case Atom::SidebarRight:
        // nothing
        break;
    case Atom::String:
        if (inLink && !inContents && !inSectionHeading) {
            generateLink(atom, relative, marker);
        }
        else {
            writer.writeCharacters(protectEnc(atom->string()));
        }
        break;
    case Atom::TableLeft:
        if (in_para) {
            writer.writeEndElement(); // </p>
            in_para = false;
        }
        writer.writeStartElement("table");
        writer.writeAttribute("outputclass","generic");
        numTableRows = 0;
        break;
    case Atom::TableRight:
        writer.writeEndElement(); // </table>
        break;
    case Atom::TableHeaderLeft:
        writer.writeStartElement("thead");
        writer.writeStartElement("tr");
        writer.writeAttribute("outputclass","qt-style topAlign");
        inTableHeader = true;
        break;
    case Atom::TableHeaderRight:
        writer.writeEndElement(); // </tr>
        if (matchAhead(atom, Atom::TableHeaderLeft)) {
            skipAhead = 1;
            writer.writeStartElement("tr");
            writer.writeAttribute("outputclass","qt-style topAlign");
        }
        else { 
            writer.writeEndElement(); // </thead>
            inTableHeader = false;
        }
        break;
    case Atom::TableRowLeft:
        writer.writeStartElement("tr");
        if (++numTableRows % 2 == 1)
            writer.writeAttribute("outputclass","odd topAlign");
        else
            writer.writeAttribute("outputclass","even topAlign");
        break;
    case Atom::TableRowRight:
        writer.writeEndElement(); // </tr>\n";
        break;
    case Atom::TableItemLeft:
        {
            if (inTableHeader)
                writer.writeStartElement("th");
            else
                writer.writeStartElement("td");

            QStringList spans = atom->string().split(",");
            if (spans.size() == 2) {
#if zzz        
                
                if (spans.at(0) != "1")
                    out() << " colspan=\"" << spans.at(0) << "\"";
                if (spans.at(1) != "1")
                    out() << " rowspan=\"" << spans.at(1) << "\"";
#endif                
                if (!inTableHeader)
                    writer.writeStartElement("p"); 
            }
            if (matchAhead(atom, Atom::ParaLeft))
                skipAhead = 1;
        }
        break;
    case Atom::TableItemRight:
        if (inTableHeader)
            writer.writeEndElement(); // </th>
        else {
            writer.writeEndElement(); // </p>
            writer.writeEndElement(); // </td>
        }
        if (matchAhead(atom, Atom::ParaLeft))
            skipAhead = 1;
        break;
    case Atom::TableOfContents:
        {
            int numColumns = 1;
            const Node *node = relative;

            Doc::SectioningUnit sectioningUnit = Doc::Section4;
            QStringList params = atom->string().split(",");
            QString columnText = params.at(0);
            QStringList pieces = columnText.split(" ", QString::SkipEmptyParts);
            if (pieces.size() >= 2) {
                columnText = pieces.at(0);
                pieces.pop_front();
                QString path = pieces.join(" ").trimmed();
                node = findNodeForTarget(path, relative, marker, atom);
            }

            if (params.size() == 2) {
                numColumns = qMax(columnText.toInt(), numColumns);
                sectioningUnit = (Doc::SectioningUnit)params.at(1).toInt();
            }

            if (node)
                generateTableOfContents(node,
                                        marker,
                                        sectioningUnit,
                                        numColumns,
                                        relative);
        }
        break;
    case Atom::Target:
        writer.writeStartElement("p");
        writeGuidAttribute(Doc::canonicalTitle(atom->string()));
        writer.writeAttribute("outputclass","target");
        writer.writeCharacters(protectEnc(atom->string()));
        writer.writeEndElement(); // </p>
        break;
    case Atom::UnhandledFormat:
        writer.writeStartElement("b");
        writer.writeAttribute("outputclass","redFont");
        writer.writeCharacters("&lt;Missing DITAXML&gt");
        writer.writeEndElement(); // </b>
        break;
    case Atom::UnknownCommand:
        writer.writeStartElement("b");
        writer.writeAttribute("outputclass","redFont code");
        writer.writeCharacters(protectEnc(atom->string()));
        writer.writeEndElement(); // </b>
        break;
#ifdef QDOC_QML
    case Atom::QmlText:
    case Atom::EndQmlText:
        // don't do anything with these. They are just tags.
        break;
#endif
    default:
        //        unknownAtom(atom);
        break;
    }
    return skipAhead;
}

/*!
  Generate a reference page for a C++ class.
 */
void
DitaXmlGenerator::generateClassLikeNode(const InnerNode* inner, CodeMarker* marker)
{
    QList<Section> sections;
    QList<Section>::ConstIterator s;

    const ClassNode* cn = 0;
    const NamespaceNode* namespasse = 0;

    QString title;
    QString rawTitle;
    QString fullTitle;
    if (inner->type() == Node::Namespace) {
        namespasse = const_cast<NamespaceNode*>(static_cast<const NamespaceNode*>(inner));
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Namespace";
    }
    else if (inner->type() == Node::Class) {
        cn = const_cast<ClassNode*>(static_cast<const ClassNode*>(inner));
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Class Reference";

        generateHeader(inner);

        writer.writeStartElement(CXXCLASS);
        writer.writeAttribute("id",cn->guid());
        writer.writeStartElement(APINAME);
        writer.writeCharacters(fullTitle);
        writer.writeEndElement(); // </apiName>

        generateBrief(inner, marker);
    
        writer.writeStartElement(CXXCLASSDETAIL);
        writer.writeStartElement(CXXCLASSDEFINITION);
        writer.writeStartElement(CXXCLASSACCESSSPECIFIER);
        writer.writeAttribute("value",inner->accessString());
        writer.writeEndElement(); // <cxxClassAccessSpecifier>
        if (cn->isAbstract()) {
            writer.writeStartElement(CXXCLASSABSTRACT);
            writer.writeAttribute("name","abstract");
            writer.writeAttribute("value","abstract");
            writer.writeEndElement(); // </cxxClassAbstract>
        }
        writeDerivations(cn, marker);
        writeLocation(cn);
        writer.writeEndElement(); // <cxxClassDefinition>
        writer.writeStartElement(APIDESC);

        if (!inner->doc().isEmpty()) {
            writer.writeStartElement("p");
            writer.writeAttribute("outputclass","h2");
            writer.writeCharacters("Detailed Description");
            writer.writeEndElement(); // </p>
            generateBody(inner, marker);
            //        generateAlsoList(inner, marker);
        }
    
        writer.writeEndElement(); // </apiDesc>
        writer.writeEndElement(); // </cxxClassDetail>

        sections = marker->sections(inner, CodeMarker::Detailed, CodeMarker::Okay);
        s = sections.begin();
        while (s != sections.end()) {
            if ((*s).name == "Member Function Documentation") {
                writeFunctions((*s),cn,marker);
            }
            else if ((*s).name == "Member Type Documentation") {
                writeEnumerations((*s),cn,marker);
                writeTypedefs((*s),cn,marker);
            }
            else if ((*s).name == "Member Variable Documentation") {
                writeDataMembers((*s),cn,marker);
            }
            else if ((*s).name == "Property Documentation") {
                writeProperties((*s),cn,marker);
            }
            else if ((*s).name == "Macro Documentation") {
                writeMacros((*s),cn,marker);
            }
            ++s;
        }
        writer.writeEndElement(); // </cxxClass>
    }

#ifdef WRITE_HTML    
    Text subtitleText;
    if (rawTitle != fullTitle)
        subtitleText << "(" << Atom(Atom::AutoLink, fullTitle) << ")"
                     << Atom(Atom::LineBreak);

    QString shortVersion;
    shortVersion = project + " " + shortVersion + ": ";
    shortVersion = myTree->version();
    if (shortVersion.count(QChar('.')) == 2)
        shortVersion.truncate(shortVersion.lastIndexOf(QChar('.')));
    if (!shortVersion.isEmpty()) {
        if (project == "QSA")
            shortVersion = "QSA " + shortVersion + ": ";
        else
            shortVersion = "Qt " + shortVersion + ": ";
    }

    out() << "  <title>" << shortVersion << protectEnc(title) << "</title>\n";

#if 0    
    out() << QString(postHeader).replace("\\" + COMMAND_VERSION, myTree->version());
    generateBreadCrumbs(title,node,marker);
    out() << QString(postPostHeader).replace("\\" + COMMAND_VERSION, myTree->version());
#endif    
    
    sections = marker->sections(inner, CodeMarker::Summary, CodeMarker::Okay);
    generateTableOfContents(inner,marker,&sections);
    generateTitle(title, subtitleText, SmallSubTitle, inner, marker);

#ifdef QDOC_QML
    if (cn && !cn->qmlElement().isEmpty()) {
        generateInstantiatedBy(cn,marker);
    }
#endif
    
    generateBrief(inner, marker);
    generateIncludes(inner, marker);
    generateStatus(inner, marker);
    if (cn) {
        generateInherits(cn, marker);
        generateInheritedBy(cn, marker);
    }
    generateThreadSafeness(inner, marker);
    generateSince(inner, marker);

    out() << "<ul>\n";

    QString membersLink = generateListOfAllMemberFile(inner, marker);
    if (!membersLink.isEmpty())
        out() << "<li><xref href=\"" << membersLink << "\">"
              << "List of all members, including inherited members</xref></li>\n";

    QString obsoleteLink = generateLowStatusMemberFile(inner,
                                                       marker,
                                                       CodeMarker::Obsolete);
    if (!obsoleteLink.isEmpty())
        out() << "<li><xref href=\"" << obsoleteLink << "\">"
              << "Obsolete members</xref></li>\n";

    QString compatLink = generateLowStatusMemberFile(inner,
                                                     marker,
                                                     CodeMarker::Compat);
    if (!compatLink.isEmpty())
        out() << "<li><xref href=\"" << compatLink << "\">"
              << "Qt 3 support members</xref></li>\n";

    out() << "</ul>\n";

    bool needOtherSection = false;

    /*
      sections is built above for the call to generateTableOfContents().
     */
    s = sections.begin();
    while (s != sections.end()) {
        if (s->members.isEmpty() && s->reimpMembers.isEmpty()) {
            if (!s->inherited.isEmpty())
                needOtherSection = true;
        }
        else {
            if (!s->members.isEmpty()) {
                out() << "<hr />\n";
                out() << "<a name=\""
                      << registerRef((*s).name.toLower())
                      << "\"></a>\n";
                out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";
                generateSection(s->members, inner, marker, CodeMarker::Summary);
            }
            if (!s->reimpMembers.isEmpty()) {
                QString name = QString("Reimplemented ") + (*s).name;
                out() << "<hr />\n";
                out() << "<a name=\""
                      << registerRef(name.toLower())
                      << "\"></a>\n";
                out() << "<h2>" << protectEnc(name) << "</h2>\n";
                generateSection(s->reimpMembers, inner, marker, CodeMarker::Summary);
            }

            if (!s->inherited.isEmpty()) {
                out() << "<ul>\n";
                generateSectionInheritedList(*s, inner, marker, true);
                out() << "</ul>\n";
            }
        }
        ++s;
    }

    if (needOtherSection) {
        out() << "<h3>Additional Inherited Members</h3>\n"
                 "<ul>\n";

        s = sections.begin();
        while (s != sections.end()) {
            if (s->members.isEmpty() && !s->inherited.isEmpty())
                generateSectionInheritedList(*s, inner, marker);
            ++s;
        }
        out() << "</ul>\n";
    }

    out() << "<a name=\"" << registerRef("details") << "\"></a>\n";

    if (!inner->doc().isEmpty()) {
        out() << "<hr />\n"
              << "<div class=\"descr\"/>\n" // QTBUG-9504
              << "<h2>" << "Detailed Description" << "</h2>\n";
        generateBody(inner, marker);
        out() << "</div>\n"; // QTBUG-9504
        generateAlsoList(inner, marker);
    }

    sections = marker->sections(inner, CodeMarker::Detailed, CodeMarker::Okay);
    s = sections.begin();
    while (s != sections.end()) {
        out() << "<hr />\n";
        if (!(*s).divClass.isEmpty())
            out() << "<div class=\"" << (*s).divClass << "\"/>\n"; // QTBUG-9504
        out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";

        NodeList::ConstIterator m = (*s).members.begin();
        while (m != (*s).members.end()) {
            if ((*m)->access() != Node::Private) { // ### check necessary?
                if ((*m)->type() != Node::Class)
                    generateDetailedMember(*m, inner, marker);
                else {
                    out() << "<h3> class ";
                    generateFullName(*m, inner, marker);
                    out() << "</h3>";
                    generateBrief(*m, marker, inner);
                }

                QStringList names;
                names << (*m)->name();
                if ((*m)->type() == Node::Function) {
                    const FunctionNode *func = reinterpret_cast<const FunctionNode *>(*m);
                    if (func->metaness() == FunctionNode::Ctor ||
                        func->metaness() == FunctionNode::Dtor ||
                        func->overloadNumber() != 1)
                        names.clear();
                }
                else if ((*m)->type() == Node::Property) {
                    const PropertyNode *prop = reinterpret_cast<const PropertyNode *>(*m);
                    if (!prop->getters().isEmpty() &&
                        !names.contains(prop->getters().first()->name()))
                        names << prop->getters().first()->name();
                    if (!prop->setters().isEmpty())
                        names << prop->setters().first()->name();
                    if (!prop->resetters().isEmpty())
                        names << prop->resetters().first()->name();
                }
                else if ((*m)->type() == Node::Enum) {
                    const EnumNode *enume = reinterpret_cast<const EnumNode*>(*m);
                    if (enume->flagsType())
                        names << enume->flagsType()->name();

                    foreach (const QString &enumName,
                             enume->doc().enumItemNames().toSet() -
                             enume->doc().omitEnumItemNames().toSet())
                        names << plainCode(marker->markedUpEnumValue(enumName,
                                                                     enume));
                }
            }
            ++m;
        }
        if (!(*s).divClass.isEmpty())
            out() << "</div>\n"; // QTBUG-9504
        ++s;
    }
#endif
}

/*!
  Generate the html page for a qdoc file that doesn't map
  to an underlying c++ file.
 */
void DitaXmlGenerator::generateFakeNode(const FakeNode *fake, CodeMarker *marker)
{
    return; // zzz
    
    SubTitleSize subTitleSize = LargeSubTitle;
    QList<Section> sections;
    QList<Section>::const_iterator s;
    QString fullTitle = fake->fullTitle();
    QString htmlTitle = fullTitle;

    if (fake->subType() == Node::File && !fake->subTitle().isEmpty()) {
        subTitleSize = SmallSubTitle;
        htmlTitle += " (" + fake->subTitle() + ")";
    }
    else if (fake->subType() == Node::QmlBasicType) {
        fullTitle = "QML Basic Type: " + fullTitle;
        htmlTitle = fullTitle;
    }

    generateHeader(fake);
        
    /*
      Generate the TOC for the new doc format.
      Don't generate a TOC for the home page.
    */
    if (fake->name() != QString("index.html"))
        generateTableOfContents(fake,marker,0);

    generateTitle(fullTitle,
                  Text() << fake->subTitle(),
                  subTitleSize,
                  fake,
                  marker);

    if (fake->subType() == Node::Module) {
        // Generate brief text and status for modules.
        generateBrief(fake, marker);
        generateStatus(fake, marker);

        if (moduleNamespaceMap.contains(fake->name())) {
            out() << "<a name=\"" << registerRef("namespaces") << "\"></a>\n";
            out() << "<h2>Namespaces</h2>\n";
            generateAnnotatedList(fake, marker, moduleNamespaceMap[fake->name()]);
        }
        if (moduleClassMap.contains(fake->name())) {
            out() << "<a name=\"" << registerRef("classes") << "\"></a>\n";
            out() << "<h2>Classes</h2>\n";
            generateAnnotatedList(fake, marker, moduleClassMap[fake->name()]);
        }
    }
    else if (fake->subType() == Node::HeaderFile) {
        // Generate brief text and status for modules.
        generateBrief(fake, marker);
        generateStatus(fake, marker);

        out() << "<ul>\n";

        QString membersLink = generateListOfAllMemberFile(fake, marker);
        if (!membersLink.isEmpty())
            out() << "<li><xref href=\"" << membersLink << "\">"
                  << "List of all members, including inherited members</xref></li>\n";

        QString obsoleteLink = generateLowStatusMemberFile(fake,
                                                           marker,
                                                           CodeMarker::Obsolete);
        if (!obsoleteLink.isEmpty())
            out() << "<li><xref href=\"" << obsoleteLink << "\">"
                  << "Obsolete members</xref></li>\n";

        QString compatLink = generateLowStatusMemberFile(fake,
                                                         marker,
                                                         CodeMarker::Compat);
        if (!compatLink.isEmpty())
            out() << "<li><xref href=\"" << compatLink << "\">"
                  << "Qt 3 support members</xref></li>\n";

        out() << "</ul>\n";
    }
#ifdef QDOC_QML
    else if (fake->subType() == Node::QmlClass) {
        const QmlClassNode* qml_cn = static_cast<const QmlClassNode*>(fake);
        const ClassNode* cn = qml_cn->classNode();
        generateQmlInherits(qml_cn, marker);
        generateQmlInstantiates(qml_cn, marker);
        generateBrief(qml_cn, marker);
        generateQmlInheritedBy(qml_cn, marker);
        sections = marker->qmlSections(qml_cn,CodeMarker::Summary,0);
        s = sections.begin();
        while (s != sections.end()) {
            out() << "<a name=\"" << registerRef((*s).name) << "\"></a>\n";
            out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";
            generateQmlSummary(*s,fake,marker);
            ++s;
        }

        out() << "<a name=\"" << registerRef("details") << "\"></a>\n";
        out() << "<h2>" << "Detailed Description" << "</h2>\n";
        generateBody(fake, marker);
        if (cn)
            generateQmlText(cn->doc().body(), cn, marker, fake->name());
        generateAlsoList(fake, marker);
        out() << "<hr />\n";

        sections = marker->qmlSections(qml_cn,CodeMarker::Detailed,0);
        s = sections.begin();
        while (s != sections.end()) {
            out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";
            NodeList::ConstIterator m = (*s).members.begin();
            while (m != (*s).members.end()) {
                generateDetailedQmlMember(*m, fake, marker);
                out() << "<br/>\n";
                ++m;
            }
            ++s;
        }
        return;
    }
#endif
    
    sections = marker->sections(fake, CodeMarker::Summary, CodeMarker::Okay);
    s = sections.begin();
    while (s != sections.end()) {
        out() << "<a name=\"" << registerRef((*s).name) << "\"></a>\n";
        out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";
        generateSectionList(*s, fake, marker, CodeMarker::Summary);
        ++s;
    }

    Text brief = fake->doc().briefText();
    if (fake->subType() == Node::Module && !brief.isEmpty()) {
        out() << "<a name=\"" << registerRef("details") << "\"></a>\n";
        out() << "<div class=\"descr\"/>\n"; // QTBUG-9504
        out() << "<h2>" << "Detailed Description" << "</h2>\n";
    }
    else
        out() << "<div class=\"descr\"/>\n"; // QTBUG-9504

    generateBody(fake, marker);
    out() << "</div>\n"; // QTBUG-9504
    generateAlsoList(fake, marker);

    if (!fake->groupMembers().isEmpty()) {
        NodeMap groupMembersMap;
        foreach (const Node *node, fake->groupMembers()) {
            if (node->type() == Node::Class || node->type() == Node::Namespace)
                groupMembersMap[node->name()] = node;
        }
        generateAnnotatedList(fake, marker, groupMembersMap);
    }

    sections = marker->sections(fake, CodeMarker::Detailed, CodeMarker::Okay);
    s = sections.begin();
    while (s != sections.end()) {
        out() << "<hr />\n";
        out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";

        NodeList::ConstIterator m = (*s).members.begin();
        while (m != (*s).members.end()) {
            generateDetailedMember(*m, fake, marker);
            ++m;
        }
        ++s;
    }
}

/*!
  Returns "xml" for this subclass of Generator.
 */
QString DitaXmlGenerator::fileExtension(const Node * /* node */) const
{
    return "xml";
}

/*!
  Output breadcrumb list in the html file.
 */
void DitaXmlGenerator::generateBreadCrumbs(const QString& title,
                                           const Node *node,
                                           CodeMarker *marker)
{
    Text breadcrumb;
    if (node->type() == Node::Class) {
        const ClassNode* cn = static_cast<const ClassNode*>(node);
        QString name =  node->moduleName();
        out() << "              <li><xref href=\"modules.html\">All Modules</xref></li>";
        if (!name.isEmpty()) {
            out() << "              <li>";
            breadcrumb << Atom(Atom::AutoLink,name);
            generateText(breadcrumb, node, marker);
            out() << "</li>\n";
        }
        breadcrumb.clear();
        if (!cn->name().isEmpty()) {
            out() << "              <li>";
            breadcrumb << Atom(Atom::AutoLink,cn->name());
            generateText(breadcrumb, 0, marker);
            out() << "</li>\n";
        }
    }
    else if (node->type() == Node::Fake) {
        const FakeNode* fn = static_cast<const FakeNode*>(node);
        if (node->subType() == Node::Module) {
            out() << "              <li><xref href=\"modules.html\">All Modules</xref></li>";
            QString name =  node->name();
            if (!name.isEmpty()) {
                out() << "              <li>";
                breadcrumb << Atom(Atom::AutoLink,name);
                generateText(breadcrumb, 0, marker);
                out() << "</li>\n";
            }
        }
        else if (node->subType() == Node::Group) {
            if (fn->name() == QString("modules"))
                out() << "              <li><xref href=\"modules.html\">All Modules</xref></li>";
            else {
                out() << "              <li><xref href=\"" << fn->name() << "\">" << title
                      << "</xref></li>";
            }
        }
        else if (node->subType() == Node::Page) {
            if (fn->name() == QString("examples.html")) {
                out() << "              <li><xref href=\"all-examples.html\">Examples</xref></li>";
            }
            else if (fn->name().startsWith("examples-")) {
                out() << "              <li><xref href=\"all-examples.html\">Examples</xref></li>";
                out() << "              <li><xref href=\"" << fn->name() << "\">" << title
                      << "</xref></li>";
            }
            else if (fn->name() == QString("namespaces.html")) {
                out() << "              <li><xref href=\"namespaces.html\">All Namespaces</xref></li>";
            }
            else {
                out() << "              <li><xref href=\"" << fn->name() << "\">" << title
                      << "</xref></li>";
            }
        }
        else if (node->subType() == Node::QmlClass) {
            out() << "              <li><xref href=\"qdeclarativeelements.html\">QML Elements</xref></li>";
            out() << "              <li><xref href=\"" << fn->name() << "\">" << title
                  << "</xref></li>";
        }
        else if (node->subType() == Node::Example) {
            out() << "              <li><xref href=\"all-examples.html\">Examples</xref></li>";
            QStringList sl = fn->name().split('/');
            QString name = "examples-" + sl.at(0) + ".html";
            QString t = CodeParser::titleFromName(name);
            out() << "              <li><xref href=\"" << name << "\">"
                  << t << "</xref></li>";
            out() << "              <li><xref href=\"" << sl.at(0)
                  << "-" << sl.at(sl.size()-1) << ".html\">"
                  << title << "</xref></li>";
        }
    }
    else if (node->type() == Node::Namespace) {
        const NamespaceNode* nsn = static_cast<const NamespaceNode*>(node);
        out() << "              <li><xref href=\"namespaces.html\">All Namespaces</xref></li>";
        out() << "              <li><xref href=\"" << fileName(nsn) << "\">" << title
              << "</xref></li>";
    }
}

void DitaXmlGenerator::generateHeader(const Node* node)
{
    writer.setDevice(out().device());
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(4);
    writer.writeStartDocument();
    
    if (!node)
        return;

    QString docType;
    QString dtd;
    QString version;
    if (node->type() == Node::Class) {
        docType = "cxxClass";
        dtd = "dtd/cxxClass.dtd";
        version = "0.6.0";
    }

    QString doctype = "<!DOCTYPE " + docType +
        " PUBLIC \"-//NOKIA//DTD DITA C++ API Class Reference Type v" +
        version + "//EN\" \"" + dtd + "\">";
    writer.writeDTD(doctype);
    writer.writeComment(node->doc().location().fileName());
}

void DitaXmlGenerator::generateTitle(const QString& title,
                                     const Text &subTitle,
                                     SubTitleSize subTitleSize,
                                     const Node *relative,
                                     CodeMarker *marker)
{
    if (!title.isEmpty())
        out() << "<h1 class=\"title\">" << protectEnc(title) << "</h1>\n";
    if (!subTitle.isEmpty()) {
 			out() << "<span";
       if (subTitleSize == SmallSubTitle)
            out() << " class=\"small-subtitle\">";
        else
            out() << " class=\"subtitle\">";
        generateText(subTitle, relative, marker);
        out() << "</span>\n";
    }
}

/*!
  Outputs the brief command as a <shortdesc" element.
 */
void DitaXmlGenerator::generateBrief(const Node* node, CodeMarker* marker)
{
    Text brief = node->doc().briefText();
    if (!brief.isEmpty()) {
        ++noLinks;
        writer.writeStartElement(SHORTDESC);
        generateText(brief, node, marker);
        writer.writeEndElement(); // shortdesc
        --noLinks;
    }
}

void DitaXmlGenerator::generateIncludes(const InnerNode *inner, CodeMarker *marker)
{
    if (!inner->includes().isEmpty()) {
        out() << "<pre class=\"highlightedCode\">"
              << trimmedTrailing(highlightedCode(indent(codeIndent,
                                                        marker->markedUpIncludes(inner->includes())),
                                                 marker,inner))
              << "</pre>";
    }
}

/*!
  Generates a table of contents begining at \a node.
 */
void DitaXmlGenerator::generateTableOfContents(const Node *node,
                                            CodeMarker *marker,
                                            Doc::SectioningUnit sectioningUnit,
                                            int numColumns,
                                            const Node *relative)

{
    return;
    if (!node->doc().hasTableOfContents())
        return;
    QList<Atom *> toc = node->doc().tableOfContents();
    if (toc.isEmpty())
        return;

    QString nodeName = "";
    if (node != relative)
        nodeName = node->name();

    QStringList sectionNumber;
    int columnSize = 0;

    QString tdTag;
    if (numColumns > 1) {
        tdTag = "<td>"; /* width=\"" + QString::number((100 + numColumns - 1) / numColumns) + "%\">";*/
        out() << "<table class=\"toc\">\n<tr class=\"topAlign\">"
              << tdTag << "\n";
    }

    // disable nested links in table of contents
    inContents = true;
    inLink = true;

    for (int i = 0; i < toc.size(); ++i) {
        Atom *atom = toc.at(i);

        int nextLevel = atom->string().toInt();
        if (nextLevel > (int)sectioningUnit)
            continue;

        if (sectionNumber.size() < nextLevel) {
            do {
                out() << "<ul>";
                sectionNumber.append("1");
            } while (sectionNumber.size() < nextLevel);
        }
        else {
            while (sectionNumber.size() > nextLevel) {
                out() << "</ul>\n";
                sectionNumber.removeLast();
            }
            sectionNumber.last() = QString::number(sectionNumber.last().toInt() + 1);
        }
        int numAtoms;
        Text headingText = Text::sectionHeading(atom);

        if (sectionNumber.size() == 1 && columnSize > toc.size() / numColumns) {
            out() << "</ul></td>" << tdTag << "<ul>\n";
            columnSize = 0;
        }
        out() << "<li>";
        out() << "<xref href=\""
              << nodeName
              << "#"
              << Doc::canonicalTitle(headingText.toString())
              << "\">";
        generateAtomList(headingText.firstAtom(), node, marker, true, numAtoms);
        out() << "</xref></li>\n";

        ++columnSize;
    }
    while (!sectionNumber.isEmpty()) {
        out() << "</ul>\n";
        sectionNumber.removeLast();
    }

    if (numColumns > 1)
        out() << "</td></tr></table>\n";

    inContents = false;
    inLink = false;
}

/*!
  Revised for the new doc format.
  Generates a table of contents begining at \a node.
 */
void DitaXmlGenerator::generateTableOfContents(const Node *node,
                                            CodeMarker *marker,
                                            QList<Section>* sections)
{
    QList<Atom*> toc;
    if (node->doc().hasTableOfContents()) 
        toc = node->doc().tableOfContents();
    if (toc.isEmpty() && !sections && (node->subType() != Node::Module))
        return;

    QStringList sectionNumber;
    int detailsBase = 0;

    // disable nested links in table of contents
    inContents = true;
    inLink = true;

    out() << "<div class=\"toc\">\n";
    out() << "<h3>Contents</h3>\n";
    sectionNumber.append("1");
    out() << "<ul>\n";

    if (node->subType() == Node::Module) {
        if (moduleNamespaceMap.contains(node->name())) {
            out() << "<li class=\"level"
                  << sectionNumber.size()
                  << "\"><xref href=\"#"
                  << registerRef("namespaces")
                  << "\">Namespaces</xref></li>\n";
        }
        if (moduleClassMap.contains(node->name())) {
            out() << "<li class=\"level"
                  << sectionNumber.size()
                  << "\"><xref href=\"#"
                  << registerRef("classes")
                  << "\">Classes</xref></li>\n";
        }
        out() << "<li class=\"level"
              << sectionNumber.size()
              << "\"><xref href=\"#"
              << registerRef("details")
              << "\">Detailed Description</xref></li>\n";
        for (int i = 0; i < toc.size(); ++i) {
            if (toc.at(i)->string().toInt() == 1) {
                detailsBase = 1;
                break;
            }
        }
    }
    else if (sections && (node->type() == Node::Class)) {
        QList<Section>::ConstIterator s = sections->begin();
        while (s != sections->end()) {
            if (!s->members.isEmpty() || !s->reimpMembers.isEmpty()) {
                out() << "<li class=\"level"
                      << sectionNumber.size()
                      << "\"><xref href=\"#"
                      << registerRef((*s).pluralMember)
                      << "\">" << (*s).name
                      << "</xref></li>\n";
            }
            ++s;
        }
        out() << "<li class=\"level"
              << sectionNumber.size()
              << "\"><xref href=\"#"
              << registerRef("details")
              << "\">Detailed Description</xref></li>\n";
        for (int i = 0; i < toc.size(); ++i) {
            if (toc.at(i)->string().toInt() == 1) {
                detailsBase = 1;
                break;
            }
        }
    }

    for (int i = 0; i < toc.size(); ++i) {
        Atom *atom = toc.at(i);
        int nextLevel = atom->string().toInt() + detailsBase;
        if (sectionNumber.size() < nextLevel) {
            do {
                sectionNumber.append("1");
            } while (sectionNumber.size() < nextLevel);
        }
        else {
            while (sectionNumber.size() > nextLevel) {
                sectionNumber.removeLast();
            }
            sectionNumber.last() = QString::number(sectionNumber.last().toInt() + 1);
        }
        int numAtoms;
        Text headingText = Text::sectionHeading(atom);
        QString s = headingText.toString();
        out() << "<li class=\"level"
              << sectionNumber.size()
              << "\">";
        out() << "<xref href=\""
              << "#"
              << Doc::canonicalTitle(s)
              << "\">";
        generateAtomList(headingText.firstAtom(), node, marker, true, numAtoms);
        out() << "</xref></li>\n";
    }
    while (!sectionNumber.isEmpty()) {
        sectionNumber.removeLast();
    }
    out() << "</ul>\n";
    out() << "</div>\n";
    inContents = false;
    inLink = false;
}

QString DitaXmlGenerator::generateListOfAllMemberFile(const InnerNode *inner,
                                                   CodeMarker *marker)
{
    QList<Section> sections;
    QList<Section>::ConstIterator s;

    sections = marker->sections(inner,
                                CodeMarker::SeparateList,
                                CodeMarker::Okay);
    if (sections.isEmpty())
        return QString();

    QString fileName = fileBase(inner) + "-members." + fileExtension(inner);
    beginSubPage(inner->location(), fileName);
    QString title = "List of All Members for " + inner->name();
    generateHeader(inner);
    generateTitle(title, Text(), SmallSubTitle, inner, marker);
    out() << "<p>This is the complete list of members for ";
    generateFullName(inner, 0, marker);
    out() << ", including inherited members.</p>\n";

    Section section = sections.first();
    generateSectionList(section, 0, marker, CodeMarker::SeparateList);

    endSubPage();
    return fileName;
}

QString DitaXmlGenerator::generateLowStatusMemberFile(const InnerNode *inner,
                                                   CodeMarker *marker,
                                                   CodeMarker::Status status)
{
    QList<Section> sections = marker->sections(inner,
                                               CodeMarker::Summary,
                                               status);
    QMutableListIterator<Section> j(sections);
    while (j.hasNext()) {
        if (j.next().members.size() == 0)
            j.remove();
    }
    if (sections.isEmpty())
        return QString();

    int i;

    QString title;
    QString fileName;

    if (status == CodeMarker::Compat) {
        title = "Qt 3 Support Members for " + inner->name();
        fileName = fileBase(inner) + "-qt3." + fileExtension(inner);
    }
    else {
        title = "Obsolete Members for " + inner->name();
        fileName = fileBase(inner) + "-obsolete." + fileExtension(inner);
    }

    beginSubPage(inner->location(), fileName);
    generateHeader(inner);
    generateTitle(title, Text(), SmallSubTitle, inner, marker);

    if (status == CodeMarker::Compat) {
        out() << "<p><b>The following class members are part of the "
                 "<xref href=\"qt3support.html\">Qt 3 support layer</xref>.</b> "
                 "They are provided to help you port old code to Qt 4. We advise against "
                 "using them in new code.</p>\n";
    }
    else {
        out() << "<p><b>The following class members are obsolete.</b> "
              << "They are provided to keep old source code working. "
              << "We strongly advise against using them in new code.</p>\n";
    }

    out() << "<p><ul><li><xref href=\""
          << linkForNode(inner, 0) << "\">"
          << protectEnc(inner->name())
          << " class reference</xref></li></ul></p>\n";

    for (i = 0; i < sections.size(); ++i) {
        out() << "<h2>" << protectEnc(sections.at(i).name) << "</h2>\n";
        generateSectionList(sections.at(i), inner, marker, CodeMarker::Summary);
    }

    sections = marker->sections(inner, CodeMarker::Detailed, status);
    for (i = 0; i < sections.size(); ++i) {
        out() << "<hr />\n";
        out() << "<h2>" << protectEnc(sections.at(i).name) << "</h2>\n";

        NodeList::ConstIterator m = sections.at(i).members.begin();
        while (m != sections.at(i).members.end()) {
            if ((*m)->access() != Node::Private)
                generateDetailedMember(*m, inner, marker);
            ++m;
        }
    }

    endSubPage();
    return fileName;
}

void DitaXmlGenerator::generateClassHierarchy(const Node *relative,
                                           CodeMarker *marker,
                                           const QMap<QString,const Node*> &classMap)
{
    if (classMap.isEmpty())
        return;

    NodeMap topLevel;
    NodeMap::ConstIterator c = classMap.begin();
    while (c != classMap.end()) {
        const ClassNode *classe = static_cast<const ClassNode *>(*c);
        if (classe->baseClasses().isEmpty())
            topLevel.insert(classe->name(), classe);
        ++c;
    }

    QStack<NodeMap > stack;
    stack.push(topLevel);

    out() << "<ul>\n";
    while (!stack.isEmpty()) {
        if (stack.top().isEmpty()) {
            stack.pop();
            out() << "</ul>\n";
        }
        else {
            const ClassNode *child =
                static_cast<const ClassNode *>(*stack.top().begin());
            out() << "<li>";
            generateFullName(child, relative, marker);
            out() << "</li>\n";
            stack.top().erase(stack.top().begin());

            NodeMap newTop;
            foreach (const RelatedClass &d, child->derivedClasses()) {
                if (d.access != Node::Private)
                    newTop.insert(d.node->name(), d.node);
            }
            if (!newTop.isEmpty()) {
                stack.push(newTop);
                out() << "<ul>\n";
            }
        }
    }
}

void DitaXmlGenerator::generateAnnotatedList(const Node *relative,
                                          CodeMarker *marker,
                                          const NodeMap &nodeMap)
{
    out() << "<table class=\"annotated\">\n";

    int row = 0;
    foreach (const QString &name, nodeMap.keys()) {
        const Node *node = nodeMap[name];

        if (node->status() == Node::Obsolete)
            continue;

        if (++row % 2 == 1)
            out() << "<tr class=\"odd topAlign\">";
        else
            out() << "<tr class=\"even topAlign\">";
        out() << "<td><p>";
        generateFullName(node, relative, marker);
        out() << "</p></td>";

        if (!(node->type() == Node::Fake)) {
            Text brief = node->doc().trimmedBriefText(name);
            if (!brief.isEmpty()) {
                out() << "<td><p>";
                generateText(brief, node, marker);
                out() << "</p></td>";
            }
        }
        else {
            out() << "<td><p>";
            out() << protectEnc(node->doc().briefText().toString());
            out() << "</p></td>";
        }
        out() << "</tr>\n";
    }
    out() << "</table>\n";
}

/*!
  This function finds the common prefix of the names of all
  the classes in \a classMap and then generates a compact
  list of the class names alphabetized on the part of the
  name not including the common prefix. You can tell the
  function to use \a comonPrefix as the common prefix, but
  normally you let it figure it out itself by looking at
  the name of the first and last classes in \a classMap.
 */
void DitaXmlGenerator::generateCompactList(const Node *relative,
                                        CodeMarker *marker,
                                        const NodeMap &classMap,
                                        bool includeAlphabet,
                                        QString commonPrefix)
{
    const int NumParagraphs = 37; // '0' to '9', 'A' to 'Z', '_'

    if (classMap.isEmpty())
        return;

    /*
      If commonPrefix is not empty, then the caller knows what
      the common prefix is and has passed it in, so just use that
      one.
     */
    int commonPrefixLen = commonPrefix.length();
    if (commonPrefixLen == 0) {
        QString first;
        QString last;
        
        /*
          The caller didn't pass in a common prefix, so get the common
          prefix by looking at the class names of the first and last
          classes in the class map. Discard any namespace names and
          just use the bare class names. For Qt, the prefix is "Q".

          Note that the algorithm used here to derive the common prefix
          from the first and last classes in alphabetical order (QAccel
          and QXtWidget in Qt 2.1), fails if either class name does not
          begin with Q.
        */

        NodeMap::const_iterator iter = classMap.begin();
        while (iter != classMap.end()) {
            if (!iter.key().contains("::")) {
                first = iter.key();
                break;
            }
            ++iter;
        }

        if (first.isEmpty())
            first = classMap.begin().key();

        iter = classMap.end();
        while (iter != classMap.begin()) {
            --iter;
            if (!iter.key().contains("::")) {
                last = iter.key();
                break;
            }
        }

        if (last.isEmpty())
            last = classMap.begin().key();

        if (classMap.size() > 1) {
            while (commonPrefixLen < first.length() + 1 &&
                   commonPrefixLen < last.length() + 1 &&
                   first[commonPrefixLen] == last[commonPrefixLen])
                ++commonPrefixLen;
        }

        commonPrefix = first.left(commonPrefixLen);
    }

    /*
      Divide the data into 37 paragraphs: 0, ..., 9, A, ..., Z,
      underscore (_). QAccel will fall in paragraph 10 (A) and
      QXtWidget in paragraph 33 (X). This is the only place where we
      assume that NumParagraphs is 37. Each paragraph is a NodeMap.
    */
    NodeMap paragraph[NumParagraphs+1];
    QString paragraphName[NumParagraphs+1];
    QSet<char> usedParagraphNames;

    NodeMap::ConstIterator c = classMap.begin();
    while (c != classMap.end()) {
        QStringList pieces = c.key().split("::");
        QString key;
        int idx = commonPrefixLen;
        if (!pieces.last().startsWith(commonPrefix))
            idx = 0;
        if (pieces.size() == 1)
            key = pieces.last().mid(idx).toLower();
        else
            key = pieces.last().toLower();

        int paragraphNr = NumParagraphs - 1;

        if (key[0].digitValue() != -1) {
            paragraphNr = key[0].digitValue();
        }
        else if (key[0] >= QLatin1Char('a') && key[0] <= QLatin1Char('z')) {
            paragraphNr = 10 + key[0].unicode() - 'a';
        }

        paragraphName[paragraphNr] = key[0].toUpper();
        usedParagraphNames.insert(key[0].toLower().cell());
        paragraph[paragraphNr].insert(key, c.value());
        ++c;
    }

    /*
      Each paragraph j has a size: paragraph[j].count(). In the
      discussion, we will assume paragraphs 0 to 5 will have sizes
      3, 1, 4, 1, 5, 9.

      We now want to compute the paragraph offset. Paragraphs 0 to 6
      start at offsets 0, 3, 4, 8, 9, 14, 23.
    */
    int paragraphOffset[NumParagraphs + 1];     // 37 + 1
    paragraphOffset[0] = 0;
    for (int i=0; i<NumParagraphs; i++)         // i = 0..36
        paragraphOffset[i+1] = paragraphOffset[i] + paragraph[i].count();

    int curParNr = 0;
    int curParOffset = 0;

    /*
      Output the alphabet as a row of links.
     */
    if (includeAlphabet) {
        out() << "<p  class=\"centerAlign functionIndex\"><b>";
        for (int i = 0; i < 26; i++) {
            QChar ch('a' + i);
            if (usedParagraphNames.contains(char('a' + i)))
                out() << QString("<xref href=\"#%1\">%2</xref>&nbsp;").arg(ch).arg(ch.toUpper());
        }
        out() << "</b></p>\n";
    }

    /*
      Output a <div> element to contain all the <dl> elements.
     */
    out() << "<div class=\"flowListDiv\">\n";

    for (int i=0; i<classMap.count()-1; i++) {
        while ((curParNr < NumParagraphs) &&
               (curParOffset == paragraph[curParNr].count())) {
            ++curParNr;
            curParOffset = 0;
        }

        /*
          Starting a new paragraph means starting a new <dl>.
        */
        if (curParOffset == 0) {
            if (i > 0)
                out() << "</dl>\n";
            if (++numTableRows % 2 == 1)
                out() << "<dl class=\"flowList odd\">";
            else
                out() << "<dl class=\"flowList even\">";
            out() << "<dt class=\"alphaChar\">";
            if (includeAlphabet) {
                QChar c = paragraphName[curParNr][0].toLower();
                out() << QString("<a name=\"%1\"></a>").arg(c);
            }
            out() << "<b>"
                  << paragraphName[curParNr]
                  << "</b>";
            out() << "</dt>\n";
        }

        /*
          Output a <dd> for the current offset in the current paragraph.
         */
        out() << "<dd>";
        if ((curParNr < NumParagraphs) &&
            !paragraphName[curParNr].isEmpty()) {
            NodeMap::Iterator it;
            it = paragraph[curParNr].begin();
            for (int i=0; i<curParOffset; i++)
                ++it;

            /*
              Previously, we used generateFullName() for this, but we
              require some special formatting.
            */
            out() << "<xref href=\"" << linkForNode(it.value(), relative) << "\">";
            
            QStringList pieces;
            if (it.value()->subType() == Node::QmlClass)
                pieces << it.value()->name();
            else
                pieces = fullName(it.value(), relative, marker).split("::");
            out() << protectEnc(pieces.last());
            out() << "</xref>";
            if (pieces.size() > 1) {
                out() << " (";
                generateFullName(it.value()->parent(), relative, marker);
                out() << ")";
            }
        }
        out() << "</dd>\n";
        curParOffset++;
    }
    out() << "</dl>\n";
    out() << "</div>\n";
}

void DitaXmlGenerator::generateFunctionIndex(const Node *relative,
                                          CodeMarker *marker)
{
    out() << "<p  class=\"centerAlign functionIndex\"><b>";
    for (int i = 0; i < 26; i++) {
        QChar ch('a' + i);
        out() << QString("<xref href=\"#%1\">%2</xref>&nbsp;").arg(ch).arg(ch.toUpper());
    }
    out() << "</b></p>\n";

    char nextLetter = 'a';
    char currentLetter;

#if 1
    out() << "<ul>\n";
#endif
    QMap<QString, NodeMap >::ConstIterator f = funcIndex.begin();
    while (f != funcIndex.end()) {
#if 1
        out() << "<li>";
#else
        out() << "<p>";
#endif
        out() << protectEnc(f.key()) << ":";

        currentLetter = f.key()[0].unicode();
        while (islower(currentLetter) && currentLetter >= nextLetter) {
            out() << QString("<a name=\"%1\"></a>").arg(nextLetter);
            nextLetter++;
        }

        NodeMap::ConstIterator s = (*f).begin();
        while (s != (*f).end()) {
            out() << " ";
            generateFullName((*s)->parent(), relative, marker, *s);
            ++s;
        }
#if 1
        out() << "</li>";
#else
        out() << "</p>";
#endif
        out() << "\n";
        ++f;
    }
#if 1
    out() << "</ul>\n";
#endif
}

void DitaXmlGenerator::generateLegaleseList(const Node *relative,
                                         CodeMarker *marker)
{
    QMap<Text, const Node *>::ConstIterator it = legaleseTexts.begin();
    while (it != legaleseTexts.end()) {
        Text text = it.key();
        out() << "<hr />\n";
        generateText(text, relative, marker);
        out() << "<ul>\n";
        do {
            out() << "<li>";
            generateFullName(it.value(), relative, marker);
            out() << "</li>\n";
            ++it;
        } while (it != legaleseTexts.end() && it.key() == text);
        out() << "</ul>\n";
    }
}

/*void DitaXmlGenerator::generateSynopsis(const Node *node,
                                     const Node *relative,
                                     CodeMarker *marker,
                                     CodeMarker::SynopsisStyle style)
{
    QString marked = marker->markedUpSynopsis(node, relative, style);
    QRegExp templateTag("(<[^@>]*>)");
    if (marked.indexOf(templateTag) != -1) {
        QString contents = protectEnc(marked.mid(templateTag.pos(1),
                                              templateTag.cap(1).length()));
        marked.replace(templateTag.pos(1), templateTag.cap(1).length(),
                        contents);
    }
    marked.replace(QRegExp("<@param>([a-z]+)_([1-9n])</@param>"),
                   "<i>\\1<sub>\\2</sub></i>");
    marked.replace("<@param>", "<i>");
    marked.replace("</@param>", "</i>");

    if (style == CodeMarker::Summary)
        marked.replace("@name>", "b>");

    if (style == CodeMarker::SeparateList) {
        QRegExp extraRegExp("<@extra>.*</@extra>");
        extraRegExp.setMinimal(true);
        marked.replace(extraRegExp, "");
    }
    else {
        marked.replace("<@extra>", "&nbsp;&nbsp;<tt>");
        marked.replace("</@extra>", "</tt>");
    }

    if (style != CodeMarker::Detailed) {
        marked.replace("<@type>", "");
        marked.replace("</@type>", "");
    }
    out() << highlightedCode(marked, marker, relative);
}*/

#ifdef QDOC_QML
void DitaXmlGenerator::generateQmlItem(const Node *node,
                                    const Node *relative,
                                    CodeMarker *marker,
                                    bool summary)
{ 
    QString marked = marker->markedUpQmlItem(node,summary);
    QRegExp templateTag("(<[^@>]*>)");
    if (marked.indexOf(templateTag) != -1) {
        QString contents = protectEnc(marked.mid(templateTag.pos(1),
                                              templateTag.cap(1).length()));
        marked.replace(templateTag.pos(1), templateTag.cap(1).length(),
                        contents);
    }
    marked.replace(QRegExp("<@param>([a-z]+)_([1-9n])</@param>"),
                   "<i>\\1<sub>\\2</sub></i>");
    marked.replace("<@param>", "<i>");
    marked.replace("</@param>", "</i>");

    if (summary)
        marked.replace("@name>", "b>");

    marked.replace("<@extra>", "<tt>");
    marked.replace("</@extra>", "</tt>");

    if (summary) {
        marked.replace("<@type>", "");
        marked.replace("</@type>", "");
    }
    out() << highlightedCode(marked, marker, relative);
}
#endif

void DitaXmlGenerator::generateOverviewList(const Node *relative, CodeMarker * /* marker */)
{
    QMap<const FakeNode *, QMap<QString, FakeNode *> > fakeNodeMap;
    QMap<QString, const FakeNode *> groupTitlesMap;
    QMap<QString, FakeNode *> uncategorizedNodeMap;
    QRegExp singleDigit("\\b([0-9])\\b");

    const NodeList children = myTree->root()->childNodes();
    foreach (Node *child, children) {
        if (child->type() == Node::Fake && child != relative) {
            FakeNode *fakeNode = static_cast<FakeNode *>(child);

            // Check whether the page is part of a group or is the group
            // definition page.
            QString group;
            bool isGroupPage = false;
            if (fakeNode->doc().metaCommandsUsed().contains("group")) {
                group = fakeNode->doc().metaCommandArgs("group")[0];
                isGroupPage = true;
            }

            // there are too many examples; they would clutter the list
            if (fakeNode->subType() == Node::Example)
                continue;

            // not interested either in individual (Qt Designer etc.) manual chapters
            if (fakeNode->links().contains(Node::ContentsLink))
                continue;

            // Discard external nodes.
            if (fakeNode->subType() == Node::ExternalPage)
                continue;

            QString sortKey = fakeNode->fullTitle().toLower();
            if (sortKey.startsWith("the "))
                sortKey.remove(0, 4);
            sortKey.replace(singleDigit, "0\\1");

            if (!group.isEmpty()) {
                if (isGroupPage) {
                    // If we encounter a group definition page, we add all
                    // the pages in that group to the list for that group.
                    foreach (Node *member, fakeNode->groupMembers()) {
                        if (member->type() != Node::Fake)
                            continue;
                        FakeNode *page = static_cast<FakeNode *>(member);
                        if (page) {
                            QString sortKey = page->fullTitle().toLower();
                            if (sortKey.startsWith("the "))
                                sortKey.remove(0, 4);
                            sortKey.replace(singleDigit, "0\\1");
                            fakeNodeMap[const_cast<const FakeNode *>(fakeNode)].insert(sortKey, page);
                            groupTitlesMap[fakeNode->fullTitle()] = const_cast<const FakeNode *>(fakeNode);
                        }
                    }
                }
                else if (!isGroupPage) {
                    // If we encounter a page that belongs to a group then
                    // we add that page to the list for that group.
                    const FakeNode *groupNode = static_cast<const FakeNode *>(myTree->root()->findNode(group, Node::Fake));
                    if (groupNode)
                        fakeNodeMap[groupNode].insert(sortKey, fakeNode);
                    //else
                    //    uncategorizedNodeMap.insert(sortKey, fakeNode);
                }// else
                //    uncategorizedNodeMap.insert(sortKey, fakeNode);
            }// else
            //    uncategorizedNodeMap.insert(sortKey, fakeNode);
        }
    }

    // We now list all the pages found that belong to groups.
    // If only certain pages were found for a group, but the definition page
    // for that group wasn't listed, the list of pages will be intentionally
    // incomplete. However, if the group definition page was listed, all the
    // pages in that group are listed for completeness.

    if (!fakeNodeMap.isEmpty()) {
        foreach (const QString &groupTitle, groupTitlesMap.keys()) {
            const FakeNode *groupNode = groupTitlesMap[groupTitle];
            out() << QString("<h3><xref href=\"%1\">%2</xref></h3>\n").arg(
                        linkForNode(groupNode, relative)).arg(
                        protectEnc(groupNode->fullTitle()));

            if (fakeNodeMap[groupNode].count() == 0)
                continue;

            out() << "<ul>\n";

            foreach (const FakeNode *fakeNode, fakeNodeMap[groupNode]) {
                QString title = fakeNode->fullTitle();
                if (title.startsWith("The "))
                    title.remove(0, 4);
                out() << "<li><xref href=\"" << linkForNode(fakeNode, relative) << "\">"
                      << protectEnc(title) << "</xref></li>\n";
            }
            out() << "</ul>\n";
        }
    }

    if (!uncategorizedNodeMap.isEmpty()) {
        out() << QString("<h3>Miscellaneous</h3>\n");
        out() << "<ul>\n";
        foreach (const FakeNode *fakeNode, uncategorizedNodeMap) {
            QString title = fakeNode->fullTitle();
            if (title.startsWith("The "))
                title.remove(0, 4);
            out() << "<li><xref href=\"" << linkForNode(fakeNode, relative) << "\">"
                  << protectEnc(title) << "</xref></li>\n";
        }
        out() << "</ul>\n";
    }
}

void DitaXmlGenerator::generateSection(const NodeList& nl,
                                    const Node *relative,
                                    CodeMarker *marker,
                                    CodeMarker::SynopsisStyle style)
{
    bool name_alignment = true;
    if (!nl.isEmpty()) {
        bool twoColumn = false;
        if (style == CodeMarker::SeparateList) {
            name_alignment = false;
            twoColumn = (nl.count() >= 16);
        }
        else if (nl.first()->type() == Node::Property) {
            twoColumn = (nl.count() >= 5);
            name_alignment = false;
        }
        if (name_alignment) {
            out() << "<table class=\"alignedsummary\">\n";
        }
        else {
            if (twoColumn)
                out() << "<table class=\"propsummary\">\n"
                      << "<tr><td  class=\"topAlign\">";
            out() << "<ul>\n";
        }

        int i = 0;
        NodeList::ConstIterator m = nl.begin();
        while (m != nl.end()) {
            if ((*m)->access() == Node::Private) {
                ++m;
                continue;
            }

            if (name_alignment) {
                out() << "<tr><td class=\"memItemLeft rightAlign topAlign\"> ";
            }
            else {
                if (twoColumn && i == (int) (nl.count() + 1) / 2)
                    out() << "</ul></td><td  class=\"topAlign\"><ul>\n";
                out() << "<li class=\"fn\">";
            }

            generateSynopsis(*m, relative, marker, style, name_alignment);
            if (name_alignment)
                out() << "</td></tr>\n";
            else
                out() << "</li>\n";
            i++;
            ++m;
        }
        if (name_alignment)
            out() << "</table>\n";
        else {
            out() << "</ul>\n";
            if (twoColumn)
                out() << "</td></tr>\n</table>\n";
        }
    }
}

void DitaXmlGenerator::generateSectionList(const Section& section,
                                        const Node *relative,
                                        CodeMarker *marker,
                                        CodeMarker::SynopsisStyle style)
{
    bool name_alignment = true;
    if (!section.members.isEmpty()) {
        bool twoColumn = false;
        if (style == CodeMarker::SeparateList) {
            name_alignment = false;
            twoColumn = (section.members.count() >= 16);
        }
        else if (section.members.first()->type() == Node::Property) {
            twoColumn = (section.members.count() >= 5);
            name_alignment = false;
        }
        if (name_alignment) {
            out() << "<table class=\"alignedsummary\">\n";
        }
        else {
            if (twoColumn)
                out() << "<table class=\"propsummary\">\n"
                      << "<tr><td  class=\"topAlign\">";
            out() << "<ul>\n";
        }

        int i = 0;
        NodeList::ConstIterator m = section.members.begin();
        while (m != section.members.end()) {
            if ((*m)->access() == Node::Private) {
                ++m;
                continue;
            }

            if (name_alignment) {
                out() << "<tr><td class=\"memItemLeft topAlign rightAlign\"> ";
            }
            else {
                if (twoColumn && i == (int) (section.members.count() + 1) / 2)
                    out() << "</ul></td><td class=\"topAlign\"><ul>\n";
                out() << "<li class=\"fn\">";
            }

            generateSynopsis(*m, relative, marker, style, name_alignment);
            if (name_alignment)
                out() << "</td></tr>\n";
            else
                out() << "</li>\n";
            i++;
            ++m;
        }
        if (name_alignment)
            out() << "</table>\n";
        else {
            out() << "</ul>\n";
            if (twoColumn)
                out() << "</td></tr>\n</table>\n";
        }
    }

    if (style == CodeMarker::Summary && !section.inherited.isEmpty()) {
        out() << "<ul>\n";
        generateSectionInheritedList(section, relative, marker, name_alignment);
        out() << "</ul>\n";
    }
}

void DitaXmlGenerator::generateSectionInheritedList(const Section& section,
                                                 const Node *relative,
                                                 CodeMarker *marker,
                                                 bool nameAlignment)
{
    QList<QPair<ClassNode *, int> >::ConstIterator p = section.inherited.begin();
    while (p != section.inherited.end()) {
        if (nameAlignment)
            out() << "<li class=\"fn\">";
        else
            out() << "<li class=\"fn\">";
        out() << (*p).second << " ";
        if ((*p).second == 1) {
            out() << section.singularMember;
        }
        else {
            out() << section.pluralMember;
        }
        out() << " inherited from <xref href=\"" << fileName((*p).first)
              << "#" << DitaXmlGenerator::cleanRef(section.name.toLower()) << "\">"
              << protectEnc(marker->plainFullName((*p).first, relative))
              << "</xref></li>\n";
        ++p;
    }
}

void DitaXmlGenerator::generateSynopsis(const Node *node,
                                     const Node *relative,
                                     CodeMarker *marker,
                                     CodeMarker::SynopsisStyle style,
                                     bool nameAlignment)
{
    QString marked = marker->markedUpSynopsis(node, relative, style);
    QRegExp templateTag("(<[^@>]*>)");
    if (marked.indexOf(templateTag) != -1) {
        QString contents = protectEnc(marked.mid(templateTag.pos(1),
                                              templateTag.cap(1).length()));
        marked.replace(templateTag.pos(1), templateTag.cap(1).length(),
                        contents);
    }
    marked.replace(QRegExp("<@param>([a-z]+)_([1-9n])</@param>"),
                   "<i>\\1<sub>\\2</sub></i>");
    marked.replace("<@param>", "<i>");
    marked.replace("</@param>", "</i>");

    if (style == CodeMarker::Summary) {
        marked.replace("<@name>", "");   // was "<b>"
        marked.replace("</@name>", "");  // was "</b>"
    }

    if (style == CodeMarker::SeparateList) {
        QRegExp extraRegExp("<@extra>.*</@extra>");
        extraRegExp.setMinimal(true);
        marked.replace(extraRegExp, "");
    } else {
        marked.replace("<@extra>", "<tt>");
        marked.replace("</@extra>", "</tt>");
    }

    if (style != CodeMarker::Detailed) {
        marked.replace("<@type>", "");
        marked.replace("</@type>", "");
    }
    out() << highlightedCode(marked, marker, relative, style, nameAlignment);
}

QString DitaXmlGenerator::highlightedCode(const QString& markedCode,
                                       CodeMarker *marker,
                                       const Node *relative,
                                       CodeMarker::SynopsisStyle ,
                                       bool nameAlignment)
{
    QString src = markedCode;
    QString html;
    QStringRef arg;
    QStringRef par1;

    const QChar charLangle = '<';
    const QChar charAt = '@';

    // replace all <@link> tags: "(<@link node=\"([^\"]+)\">).*(</@link>)"
    static const QString linkTag("link");
    bool done = false;
    for (int i = 0, n = src.size(); i < n;) {
        if (src.at(i) == charLangle && src.at(i + 1).unicode() == '@') {
            if (nameAlignment && !done) {// && (i != 0)) Why was this here?
                html += "</td><td class=\"memItemRight bottomAlign\">";
                done = true;
            }
            i += 2;
            if (parseArg(src, linkTag, &i, n, &arg, &par1)) {
                html += "<b>";
                QString link = linkForNode(
                    CodeMarker::nodeForString(par1.toString()), relative);
                addLink(link, arg, &html);
                html += "</b>";
            }
            else {
                html += charLangle;
                html += charAt;
            }
        }
        else {
            html += src.at(i++);
        }
    }


    if (slow) {
        // is this block ever used at all?
        // replace all <@func> tags: "(<@func target=\"([^\"]*)\">)(.*)(</@func>)"
        src = html;
        html = QString();
        static const QString funcTag("func");
        for (int i = 0, n = src.size(); i < n;) {
            if (src.at(i) == charLangle && src.at(i + 1) == charAt) {
                i += 2;
                if (parseArg(src, funcTag, &i, n, &arg, &par1)) {
                    QString link = linkForNode(
                            marker->resolveTarget(par1.toString(),
                                                  myTree,
                                                  relative),
                            relative);
                    addLink(link, arg, &html);
                    par1 = QStringRef();
                }
                else {
                    html += charLangle;
                    html += charAt;
                }
            }
            else {
                html += src.at(i++);
            }
        }
    }

    // replace all "(<@(type|headerfile|func)(?: +[^>]*)?>)(.*)(</@\\2>)" tags
    src = html;
    html = QString();
    static const QString typeTags[] = { "type", "headerfile", "func" };
    for (int i = 0, n = src.size(); i < n;) {
        if (src.at(i) == charLangle && src.at(i + 1) == charAt) {
            i += 2;
            bool handled = false;
            for (int k = 0; k != 3; ++k) {
                if (parseArg(src, typeTags[k], &i, n, &arg, &par1)) {
                    par1 = QStringRef();
                    QString link = linkForNode(
                            marker->resolveTarget(arg.toString(), myTree, relative),
                            relative);
                    addLink(link, arg, &html);
                    handled = true;
                    break;
                }
            }
            if (!handled) {
                html += charLangle;
                html += charAt;
            }
        }
        else {
            html += src.at(i++);
        }
    }

    // replace all
    // "<@comment>" -> "<span class=\"comment\">";
    // "<@preprocessor>" -> "<span class=\"preprocessor\">";
    // "<@string>" -> "<span class=\"string\">";
    // "<@char>" -> "<span class=\"char\">";
    // "</@(?:comment|preprocessor|string|char)>" -> "</span>"
    src = html;
    html = QString();
    static const QString spanTags[] = {
        "<@comment>",      "<span class=\"comment\">",
        "<@preprocessor>", "<span class=\"preprocessor\">",
        "<@string>",       "<span class=\"string\">",
        "<@char>",         "<span class=\"char\">",
        "</@comment>",     "</span>",
        "</@preprocessor>","</span>",
        "</@string>",      "</span>",
        "</@char>",        "</span>"
        // "<@char>",      "<font color=blue>",
        // "</@char>",     "</font>",
        // "<@func>",      "<font color=green>",
        // "</@func>",     "</font>",
        // "<@id>",        "<i>",
        // "</@id>",       "</i>",
        // "<@keyword>",   "<b>",
        // "</@keyword>",  "</b>",
        // "<@number>",    "<font color=yellow>",
        // "</@number>",   "</font>",
        // "<@op>",        "<b>",
        // "</@op>",       "</b>",
        // "<@param>",     "<i>",
        // "</@param>",    "</i>",
        // "<@string>",    "<font color=green>",
        // "</@string>",  "</font>",
    };
    for (int i = 0, n = src.size(); i < n;) {
        if (src.at(i) == charLangle) {
            bool handled = false;
            for (int k = 0; k != 8; ++k) {
                const QString & tag = spanTags[2 * k];
                if (tag == QStringRef(&src, i, tag.length())) {
                    html += spanTags[2 * k + 1];
                    i += tag.length();
                    handled = true;
                    break;
                }
            }
            if (!handled) {
                ++i;
                if (src.at(i) == charAt ||
                    (src.at(i) == QLatin1Char('/') && src.at(i + 1) == charAt)) {
                    // drop 'our' unknown tags (the ones still containing '@')
                    while (i < n && src.at(i) != QLatin1Char('>'))
                        ++i;
                    ++i;
                }
                else {
                    // retain all others
                    html += charLangle;
                }
            }
        }
        else {
            html += src.at(i);
            ++i;
        }
    }

    return html;
}

void DitaXmlGenerator::generateLink(const Atom* atom,
                                 const Node* /* relative */,
                                 CodeMarker* marker)
{
    static QRegExp camelCase("[A-Z][A-Z][a-z]|[a-z][A-Z0-9]|_");

    if (funcLeftParen.indexIn(atom->string()) != -1 && marker->recognizeLanguage("Cpp")) {
        // hack for C++: move () outside of link
        int k = funcLeftParen.pos(1);
        writer.writeCharacters(protectEnc(atom->string().left(k)));
        if (link.isEmpty()) {
            if (showBrokenLinks)
                writer.writeEndElement(); // </i>
        }
        else {
            writer.writeEndElement(); // </xref>
        }
        inLink = false;
        writer.writeCharacters(protectEnc(atom->string().mid(k)));
    } else if (marker->recognizeLanguage("Java")) {
	// hack for Java: remove () and use <tt> when appropriate
        bool func = atom->string().endsWith("()");
        bool tt = (func || atom->string().contains(camelCase));
        if (tt)
            writer.writeStartElement("tt");
        if (func) {
            writer.writeCharacters(protectEnc(atom->string().left(atom->string().length() - 2)));
        }
        else {
            writer.writeCharacters(protectEnc(atom->string()));
        }
        writer.writeEndElement(); // </tt>
    }
    else {
        writer.writeCharacters(protectEnc(atom->string()));
    }
}

QString DitaXmlGenerator::cleanRef(const QString& ref)
{
    QString clean;

    if (ref.isEmpty())
        return clean;

    clean.reserve(ref.size() + 20);
    const QChar c = ref[0];
    const uint u = c.unicode();

    if ((u >= 'a' && u <= 'z') ||
         (u >= 'A' && u <= 'Z') ||
         (u >= '0' && u <= '9')) {
        clean += c;
    } else if (u == '~') {
        clean += "dtor.";
    } else if (u == '_') {
        clean += "underscore.";
    } else {
        clean += "A";
    }

    for (int i = 1; i < (int) ref.length(); i++) {
        const QChar c = ref[i];
        const uint u = c.unicode();
        if ((u >= 'a' && u <= 'z') ||
             (u >= 'A' && u <= 'Z') ||
             (u >= '0' && u <= '9') || u == '-' ||
             u == '_' || u == ':' || u == '.') {
            clean += c;
        } else if (c.isSpace()) {
            clean += "-";
        } else if (u == '!') {
            clean += "-not";
        } else if (u == '&') {
            clean += "-and";
        } else if (u == '<') {
            clean += "-lt";
        } else if (u == '=') {
            clean += "-eq";
        } else if (u == '>') {
            clean += "-gt";
        } else if (u == '#') {
            clean += "#";
        } else {
            clean += "-";
            clean += QString::number((int)u, 16);
        }
    }
    return clean;
}

QString DitaXmlGenerator::registerRef(const QString& ref)
{
    QString clean = DitaXmlGenerator::cleanRef(ref);

    for (;;) {
        QString& prevRef = refMap[clean.toLower()];
        if (prevRef.isEmpty()) {
            prevRef = ref;
            break;
        } else if (prevRef == ref) {
            break;
        }
        clean += "x";
    }
    return clean;
}

QString DitaXmlGenerator::protectEnc(const QString &string)
{
    return protect(string, outputEncoding);
}

QString DitaXmlGenerator::protect(const QString &string, const QString &outputEncoding)
{
#define APPEND(x) \
    if (xml.isEmpty()) { \
        xml = string; \
        xml.truncate(i); \
    } \
    xml += (x);

    QString xml;
    int n = string.length();

    for (int i = 0; i < n; ++i) {
        QChar ch = string.at(i);

        if (ch == QLatin1Char('&')) {
            APPEND("&amp;");
        } else if (ch == QLatin1Char('<')) {
            APPEND("&lt;");
        } else if (ch == QLatin1Char('>')) {
            APPEND("&gt;");
        } else if (ch == QLatin1Char('"')) {
            APPEND("&quot;");
        } else if ((outputEncoding == "ISO-8859-1" && ch.unicode() > 0x007F)
                   || (ch == QLatin1Char('*') && i + 1 < n && string.at(i) == QLatin1Char('/'))
                   || (ch == QLatin1Char('.') && i > 2 && string.at(i - 2) == QLatin1Char('.'))) {
            // we escape '*/' and the last dot in 'e.g.' and 'i.e.' for the Javadoc generator
            APPEND("&#x");
            xml += QString::number(ch.unicode(), 16);
            xml += QLatin1Char(';');
        } else {
            if (!xml.isEmpty())
                xml += ch;
        }
    }

    if (!xml.isEmpty())
        return xml;
    return string;

#undef APPEND
}

QString DitaXmlGenerator::fileBase(const Node *node)
{
    QString result;

    result = PageGenerator::fileBase(node);

    if (!node->isInnerNode()) {
        switch (node->status()) {
        case Node::Compat:
            result += "-qt3";
            break;
        case Node::Obsolete:
            result += "-obsolete";
            break;
        default:
            ;
        }
    }
    return result;
}

#if 0
QString DitaXmlGenerator::fileBase(const Node *node,
                                const SectionIterator& section)
{
    QStringList::ConstIterator s = section.sectionNumber().end();
    QStringList::ConstIterator b = section.baseNameStack().end();

    QString suffix;
    QString base = fileBase(node);

    while (s != section.sectionNumber().begin()) {
        --s;
        --b;
        if (!(*b).isEmpty()) {
            base = *b;
            break;
        }
        suffix.prepend("-" + *s);
    }
    return base + suffix;
}
#endif

QString DitaXmlGenerator::fileName(const Node *node)
{
    if (node->type() == Node::Fake) {
        if (static_cast<const FakeNode *>(node)->subType() == Node::ExternalPage)
            return node->name();
        if (static_cast<const FakeNode *>(node)->subType() == Node::Image)
            return node->name();
    }
    return PageGenerator::fileName(node);
}

QString DitaXmlGenerator::refForNode(const Node *node)
{
    const FunctionNode *func;
    const TypedefNode *typedeffe;
    QString ref;

    switch (node->type()) {
    case Node::Namespace:
    case Node::Class:
    default:
        break;
    case Node::Enum:
        ref = node->name() + "-enum";
        break;
    case Node::Typedef:
        typedeffe = static_cast<const TypedefNode *>(node);
        if (typedeffe->associatedEnum()) {
            return refForNode(typedeffe->associatedEnum());
        }
        else {
            ref = node->name() + "-typedef";
        }
        break;
    case Node::Function:
        func = static_cast<const FunctionNode *>(node);
        if (func->associatedProperty()) {
            return refForNode(func->associatedProperty());
        }
        else {
            ref = func->name();
            if (func->overloadNumber() != 1)
                ref += "-" + QString::number(func->overloadNumber());
        }
        break;
#ifdef QDOC_QML        
    case Node::Fake:
        if (node->subType() != Node::QmlPropertyGroup)
            break;
    case Node::QmlProperty:
#endif        
    case Node::Property:
        ref = node->name() + "-prop";
        break;
#ifdef QDOC_QML
    case Node::QmlSignal:
        ref = node->name() + "-signal";
        break;
    case Node::QmlMethod:
        ref = node->name() + "-method";
        break;
#endif        
    case Node::Variable:
        ref = node->name() + "-var";
        break;
    case Node::Target:
        return protectEnc(node->name());
    }
    return registerRef(ref);
}

QString DitaXmlGenerator::linkForNode(const Node *node, const Node *relative)
{
    QString link;
    QString fn;
    QString ref;

    if (node == 0 || node == relative)
        return QString();
    if (!node->url().isEmpty())
        return node->url();
    if (fileBase(node).isEmpty())
        return QString();
    if (node->access() == Node::Private)
        return QString();

    fn = fileName(node);
    link += fn;

    if (!node->isInnerNode() || node->subType() == Node::QmlPropertyGroup) {
        ref = refForNode(node);
        if (relative && fn == fileName(relative) && ref == refForNode(relative))
            return QString();

        link += "#";
        link += ref;
    }
    return link;
}

QString DitaXmlGenerator::refForAtom(Atom *atom, const Node * /* node */)
{
    if (atom->type() == Atom::SectionLeft) {
        return Doc::canonicalTitle(Text::sectionHeading(atom).toString());
    }
    else if (atom->type() == Atom::Target) {
        return Doc::canonicalTitle(atom->string());
    }
    else {
        return QString();
    }
}

void DitaXmlGenerator::generateFullName(const Node *apparentNode,
                                     const Node *relative,
                                     CodeMarker *marker,
                                     const Node *actualNode)
{
    if (actualNode == 0)
        actualNode = apparentNode;
    out() << "<xref href=\"" << linkForNode(actualNode, relative);
    if (true || relative == 0 || relative->status() != actualNode->status()) {
        switch (actualNode->status()) {
        case Node::Obsolete:
            out() << "\" class=\"obsolete";
            break;
        case Node::Compat:
            out() << "\" class=\"compat";
            break;
        default:
            ;
        }
    }
    out() << "\">";
    out() << protectEnc(fullName(apparentNode, relative, marker));
    out() << "</xref>";
}

void DitaXmlGenerator::generateDetailedMember(const Node *node,
                                           const InnerNode *relative,
                                           CodeMarker *marker)
{
    const EnumNode *enume;

#ifdef GENERATE_MAC_REFS    
    generateMacRef(node, marker);
#endif    
    if (node->type() == Node::Enum
            && (enume = static_cast<const EnumNode *>(node))->flagsType()) {
#ifdef GENERATE_MAC_REFS    
        generateMacRef(enume->flagsType(), marker);
#endif        
        out() << "<h3 class=\"flags\">";
        out() << "<a name=\"" + refForNode(node) + "\"></a>";
        generateSynopsis(enume, relative, marker, CodeMarker::Detailed);
        out() << "<br/>";
        generateSynopsis(enume->flagsType(),
                         relative,
                         marker,
                         CodeMarker::Detailed);
        out() << "</h3>\n";
    }
    else {
        out() << "<h3 class=\"fn\">";
        out() << "<a name=\"" + refForNode(node) + "\"></a>";
        generateSynopsis(node, relative, marker, CodeMarker::Detailed);
        out() << "</h3>\n";
    }

    generateStatus(node, marker);
    generateBody(node, marker);
    generateThreadSafeness(node, marker);
    generateSince(node, marker);

    if (node->type() == Node::Property) {
        const PropertyNode *property = static_cast<const PropertyNode *>(node);
        Section section;

        section.members += property->getters();
        section.members += property->setters();
        section.members += property->resetters();

        if (!section.members.isEmpty()) {
            out() << "<p><b>Access functions:</b></p>\n";
            generateSectionList(section, node, marker, CodeMarker::Accessors);
        }

        Section notifiers;
        notifiers.members += property->notifiers();
        
        if (!notifiers.members.isEmpty()) {
            out() << "<p><b>Notifier signal:</b></p>\n";
            //out() << "<p>This signal is emitted when the property value is changed.</p>\n";
            generateSectionList(notifiers, node, marker, CodeMarker::Accessors);
        }
    }
    else if (node->type() == Node::Enum) {
        const EnumNode *enume = static_cast<const EnumNode *>(node);
        if (enume->flagsType()) {
            out() << "<p>The " << protectEnc(enume->flagsType()->name())
                  << " type is a typedef for "
                  << "<xref href=\"qflags.html\">QFlags</xref>&lt;"
                  << protectEnc(enume->name())
                  << "&gt;. It stores an OR combination of "
                  << protectEnc(enume->name())
                  << " values.</p>\n";
        }
    }
    generateAlsoList(node, marker);
}

void DitaXmlGenerator::findAllClasses(const InnerNode *node)
{
    NodeList::const_iterator c = node->childNodes().constBegin();
    while (c != node->childNodes().constEnd()) {
        if ((*c)->access() != Node::Private && (*c)->url().isEmpty()) {
            if ((*c)->type() == Node::Class && !(*c)->doc().isEmpty()) {
                QString className = (*c)->name();
                if ((*c)->parent() &&
                    (*c)->parent()->type() == Node::Namespace &&
                    !(*c)->parent()->name().isEmpty())
                    className = (*c)->parent()->name()+"::"+className;

                if (!(static_cast<const ClassNode *>(*c))->hideFromMainList()) {
                    if ((*c)->status() == Node::Compat) {
                        compatClasses.insert(className, *c);
                    }
                    else if ((*c)->status() == Node::Obsolete) {
                        obsoleteClasses.insert(className, *c);
                    }
                    else {
                        nonCompatClasses.insert(className, *c);
                        if ((*c)->status() == Node::Main)
                            mainClasses.insert(className, *c);
                    }
                }

                QString moduleName = (*c)->moduleName();
                if (moduleName == "Qt3SupportLight") {
                    moduleClassMap[moduleName].insert((*c)->name(), *c);
                    moduleName = "Qt3Support";
                }
                if (!moduleName.isEmpty())
                    moduleClassMap[moduleName].insert((*c)->name(), *c);

                QString serviceName =
                    (static_cast<const ClassNode *>(*c))->serviceName();
                if (!serviceName.isEmpty())
                    serviceClasses.insert(serviceName, *c);
            }
            else if ((*c)->isInnerNode()) {
                findAllClasses(static_cast<InnerNode *>(*c));
            }
        }
        ++c;
    }
}

/*!
  For generating the "New Classes... in 4.6" section on the
  What's New in 4.6" page.
 */
void DitaXmlGenerator::findAllSince(const InnerNode *node)
{
    NodeList::const_iterator child = node->childNodes().constBegin();
    while (child != node->childNodes().constEnd()) {
        QString sinceVersion = (*child)->since();
        if (((*child)->access() != Node::Private) && !sinceVersion.isEmpty()) {
            NewSinceMaps::iterator nsmap = newSinceMaps.find(sinceVersion);
            if (nsmap == newSinceMaps.end())
                nsmap = newSinceMaps.insert(sinceVersion,NodeMultiMap());
            NewClassMaps::iterator ncmap = newClassMaps.find(sinceVersion);
            if (ncmap == newClassMaps.end())
                ncmap = newClassMaps.insert(sinceVersion,NodeMap());
            NewClassMaps::iterator nqcmap = newQmlClassMaps.find(sinceVersion);
            if (nqcmap == newQmlClassMaps.end())
                nqcmap = newQmlClassMaps.insert(sinceVersion,NodeMap());
 
            if ((*child)->type() == Node::Function) {
                FunctionNode *func = static_cast<FunctionNode *>(*child);
                if ((func->status() > Node::Obsolete) &&
                    (func->metaness() != FunctionNode::Ctor) &&
                    (func->metaness() != FunctionNode::Dtor)) {
                    nsmap.value().insert(func->name(),(*child));
                }
            }
            else if ((*child)->url().isEmpty()) {
                if ((*child)->type() == Node::Class && !(*child)->doc().isEmpty()) {
                    QString className = (*child)->name();
                    if ((*child)->parent() &&
                        (*child)->parent()->type() == Node::Namespace &&
                        !(*child)->parent()->name().isEmpty())
                        className = (*child)->parent()->name()+"::"+className;
                    nsmap.value().insert(className,(*child));
                    ncmap.value().insert(className,(*child));
                }
                else if ((*child)->subType() == Node::QmlClass) {
                    QString className = (*child)->name();
                    if ((*child)->parent() &&
                        (*child)->parent()->type() == Node::Namespace &&
                        !(*child)->parent()->name().isEmpty())
                        className = (*child)->parent()->name()+"::"+className;
                    nsmap.value().insert(className,(*child));
                    nqcmap.value().insert(className,(*child));
                }
            }
            else {
                QString name = (*child)->name();
                if ((*child)->parent() &&
                    (*child)->parent()->type() == Node::Namespace &&
                    !(*child)->parent()->name().isEmpty())
                    name = (*child)->parent()->name()+"::"+name;
                nsmap.value().insert(name,(*child));
            }
            if ((*child)->isInnerNode()) {
                findAllSince(static_cast<InnerNode *>(*child));
            }
        }
        ++child;
    }
}

#if 0
    const QRegExp versionSeparator("[\\-\\.]");
    const int minorIndex = version.indexOf(versionSeparator);
    const int patchIndex = version.indexOf(versionSeparator, minorIndex+1);
    version = version.left(patchIndex);
#endif

void DitaXmlGenerator::findAllFunctions(const InnerNode *node)
{
    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
        if ((*c)->access() != Node::Private) {
            if ((*c)->isInnerNode() && (*c)->url().isEmpty()) {
                findAllFunctions(static_cast<const InnerNode *>(*c));
            }
            else if ((*c)->type() == Node::Function) {
                const FunctionNode *func = static_cast<const FunctionNode *>(*c);
                if ((func->status() > Node::Obsolete) &&
                    (func->metaness() != FunctionNode::Ctor) &&
                    (func->metaness() != FunctionNode::Dtor)) {
                    funcIndex[(*c)->name()].insert(myTree->fullDocumentName((*c)->parent()), *c);
                }
            }
        }
        ++c;
    }
}

void DitaXmlGenerator::findAllLegaleseTexts(const InnerNode *node)
{
    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
        if ((*c)->access() != Node::Private) {
            if (!(*c)->doc().legaleseText().isEmpty())
                legaleseTexts.insertMulti((*c)->doc().legaleseText(), *c);
            if ((*c)->isInnerNode())
                findAllLegaleseTexts(static_cast<const InnerNode *>(*c));
        }
        ++c;
    }
}

void DitaXmlGenerator::findAllNamespaces(const InnerNode *node)
{
    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
        if ((*c)->access() != Node::Private) {
            if ((*c)->isInnerNode() && (*c)->url().isEmpty()) {
                findAllNamespaces(static_cast<const InnerNode *>(*c));
                if ((*c)->type() == Node::Namespace) {
                    const NamespaceNode *nspace = static_cast<const NamespaceNode *>(*c);
                    // Ensure that the namespace's name is not empty (the root
                    // namespace has no name).
                    if (!nspace->name().isEmpty()) {
                        namespaceIndex.insert(nspace->name(), *c);
                        QString moduleName = (*c)->moduleName();
                        if (moduleName == "Qt3SupportLight") {
                            moduleNamespaceMap[moduleName].insert((*c)->name(), *c);
                            moduleName = "Qt3Support";
                        }
                        if (!moduleName.isEmpty())
                            moduleNamespaceMap[moduleName].insert((*c)->name(), *c);
                    }
                }
            }
        }
        ++c;
    }
}

int DitaXmlGenerator::hOffset(const Node *node)
{
    switch (node->type()) {
    case Node::Namespace:
    case Node::Class:
        return 2;
    case Node::Fake:
        return 1;
#if 0        
        if (node->doc().briefText().isEmpty())
            return 1;
        else
            return 2;
#endif        
    case Node::Enum:
    case Node::Typedef:
    case Node::Function:
    case Node::Property:
    default:
        return 3;
    }
}

bool DitaXmlGenerator::isThreeColumnEnumValueTable(const Atom *atom)
{
    while (atom != 0 && !(atom->type() == Atom::ListRight && atom->string() == ATOM_LIST_VALUE)) {
        if (atom->type() == Atom::ListItemLeft && !matchAhead(atom, Atom::ListItemRight))
            return true;
        atom = atom->next();
    }
    return false;
}

const Node *DitaXmlGenerator::findNodeForTarget(const QString &target,
                                             const Node *relative,
                                             CodeMarker *marker,
                                             const Atom *atom)
{
    const Node *node = 0;

    if (target.isEmpty()) {
        node = relative;
    }
    else if (target.endsWith(".html")) {
        node = myTree->root()->findNode(target, Node::Fake);
    }
    else if (marker) {
        node = marker->resolveTarget(target, myTree, relative);
        if (!node)
            node = myTree->findFakeNodeByTitle(target);
        if (!node && atom) {
            node = myTree->findUnambiguousTarget(target,
                *const_cast<Atom**>(&atom));
        }
    }

    if (!node)
        relative->doc().location().warning(tr("Cannot link to '%1'").arg(target));

    return node;
}

const QPair<QString,QString> DitaXmlGenerator::anchorForNode(const Node *node)
{
    QPair<QString,QString> anchorPair;

    anchorPair.first = PageGenerator::fileName(node);
    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode*>(node);
        anchorPair.second = fakeNode->title();
    }

    return anchorPair;
}

QString DitaXmlGenerator::getLink(const Atom *atom,
                               const Node *relative,
                               CodeMarker *marker,
                               const Node** node)
{
    QString link;
    *node = 0;
    inObsoleteLink = false;

    if (atom->string().contains(":") &&
            (atom->string().startsWith("file:")
             || atom->string().startsWith("http:")
             || atom->string().startsWith("https:")
             || atom->string().startsWith("ftp:")
             || atom->string().startsWith("mailto:"))) {

        link = atom->string();
    }
    else {
        QStringList path;
        if (atom->string().contains('#')) {
            path = atom->string().split('#');
        }
        else {
            path.append(atom->string());
        }

        Atom *targetAtom = 0;

        QString first = path.first().trimmed();
        if (first.isEmpty()) {
            *node = relative;
        }
        else if (first.endsWith(".html")) {
            *node = myTree->root()->findNode(first, Node::Fake);
        }
        else {
            *node = marker->resolveTarget(first, myTree, relative);
            if (!*node) {
                *node = myTree->findFakeNodeByTitle(first);
            }
            if (!*node) {
                *node = myTree->findUnambiguousTarget(first, targetAtom);
            }
        }

        if (*node) {
            if (!(*node)->url().isEmpty())
                return (*node)->url();
            else
                path.removeFirst();
        }
        else {
            *node = relative;
        }

        if (*node) {
            if ((*node)->status() == Node::Obsolete) {
                if (relative) {
                    if (relative->parent() != *node) {
                        if (relative->status() != Node::Obsolete) {
                            bool porting = false;
                            if (relative->type() == Node::Fake) {
                                const FakeNode* fake = static_cast<const FakeNode*>(relative);
                                if (fake->title().startsWith("Porting"))
                                    porting = true;
                            }
                            QString name = marker->plainFullName(relative);
                            if (!porting && !name.startsWith("Q3")) {
                                if (obsoleteLinks) {
                                    relative->doc().location().warning(tr("Link to obsolete item '%1' in %2")
                                                                       .arg(atom->string())
                                                                       .arg(name));
                                }
                                inObsoleteLink = true;
                            }
                        }
                    }
                }
                else {
                    qDebug() << "Link to Obsolete entity"
                             << (*node)->name() << "no relative";
                }
            }
#if 0                    
            else if ((*node)->status() == Node::Deprecated) {
                qDebug() << "Link to Deprecated entity";
            }
            else if ((*node)->status() == Node::Internal) {
                qDebug() << "Link to Internal entity";
            }
#endif                
        }

        while (!path.isEmpty()) {
            targetAtom = myTree->findTarget(path.first(), *node);
            if (targetAtom == 0)
                break;
            path.removeFirst();
        }

        if (path.isEmpty()) {
            link = linkForNode(*node, relative);
            if (*node && (*node)->subType() == Node::Image)
                link = "images/used-in-examples/" + link;
            if (targetAtom)
                link += "#" + refForAtom(targetAtom, *node);
        }
    }
    return link;
}

void DitaXmlGenerator::generateIndex(const QString &fileBase,
                                  const QString &url,
                                  const QString &title)
{
    myTree->generateIndex(outputDir() + "/" + fileBase + ".index", url, title);
}

void DitaXmlGenerator::generateStatus(const Node *node, CodeMarker *marker)
{
    Text text;

    switch (node->status()) {
    case Node::Obsolete:
        if (node->isInnerNode())
	    Generator::generateStatus(node, marker);
        break;
    case Node::Compat:
        if (node->isInnerNode()) {
            text << Atom::ParaLeft
                 << Atom(Atom::FormattingLeft,ATOM_FORMATTING_BOLD)
                 << "This "
                 << typeString(node)
                 << " is part of the Qt 3 support library."
                 << Atom(Atom::FormattingRight, ATOM_FORMATTING_BOLD)
                 << " It is provided to keep old source code working. "
                 << "We strongly advise against "
                 << "using it in new code. See ";

            const FakeNode *fakeNode = myTree->findFakeNodeByTitle("Porting To Qt 4");
            Atom *targetAtom = 0;
            if (fakeNode && node->type() == Node::Class) {
                QString oldName(node->name());
                targetAtom = myTree->findTarget(oldName.replace("3", ""),
                                                fakeNode);
            }

            if (targetAtom) {
                text << Atom(Atom::Link, linkForNode(fakeNode, node) + "#" +
                                         refForAtom(targetAtom, fakeNode));
            }
            else
                text << Atom(Atom::Link, "Porting to Qt 4");

            text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
                 << Atom(Atom::String, "Porting to Qt 4")
                 << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK)
                 << " for more information."
                 << Atom::ParaRight;
        }
        generateText(text, node, marker);
        break;
    default:
        Generator::generateStatus(node, marker);
    }
}

#ifdef GENERATE_MAC_REFS    
/*
  No longer valid.
 */
void DitaXmlGenerator::generateMacRef(const Node *node, CodeMarker *marker)
{
    if (!pleaseGenerateMacRef || marker == 0)
        return;

    QStringList macRefs = marker->macRefsForNode(node);
    foreach (const QString &macRef, macRefs)
        out() << "<a name=\"" << "//apple_ref/" << macRef << "\"></a>\n";
}
#endif

void DitaXmlGenerator::beginLink(const QString &link,
                              const Node *node,
                              const Node *relative,
                              CodeMarker *marker)
{
    Q_UNUSED(marker)
    Q_UNUSED(relative)

    this->link = link;
    if (link.isEmpty()) {
        if (showBrokenLinks)
            writer.writeStartElement("i");
    }
    else if (node == 0 || (relative != 0 &&
                           node->status() == relative->status())) {
        writer.writeStartElement("xref");
        writer.writeAttribute("href",link);
    }
    else {
        switch (node->status()) {
        case Node::Obsolete:
            writer.writeStartElement("xref");
            writer.writeAttribute("href",link);
            writer.writeAttribute("outputclass","obsolete");
            break;
        case Node::Compat:
            writer.writeStartElement("xref");
            writer.writeAttribute("href",link);
            writer.writeAttribute("outputclass","compat");
            break;
        default:
            writer.writeStartElement("xref");
            writer.writeAttribute("href",link);
        }
    }
    inLink = true;
}

void DitaXmlGenerator::endLink()
{
    if (inLink) {
        if (link.isEmpty()) {
            if (showBrokenLinks)
                writer.writeEndElement(); // i
        }
        else {
            if (inObsoleteLink) {
                writer.writeStartElement("sup");
                writer.writeCharacters("(obsolete)");
                writer.writeEndElement(); // sup
            }
            writer.writeEndElement(); // xref
        }
    }
    inLink = false;
    inObsoleteLink = false;
}

#ifdef QDOC_QML

/*!
  Generates the summary for the \a section. Only used for
  sections of QML element documentation.

  Currently handles only the QML property group.
 */
void DitaXmlGenerator::generateQmlSummary(const Section& section,
                                       const Node *relative,
                                       CodeMarker *marker)
{
    if (!section.members.isEmpty()) {
        NodeList::ConstIterator m;
        int count = section.members.size();
        bool twoColumn = false;
        if (section.members.first()->type() == Node::QmlProperty) {
            twoColumn = (count >= 5);
        }
        if (twoColumn)
            out() << "<table class=\"qmlsummary\">\n";
			        if (++numTableRows % 2 == 1)
				out() << "<tr class=\"odd topAlign\">";
				else
				out() << "<tr class=\"even topAlign\">";
            //      << "<tr><td class=\"topAlign\">";
        out() << "<ul>\n";

        int row = 0;
        m = section.members.begin();
        while (m != section.members.end()) {
            if (twoColumn && row == (int) (count + 1) / 2)
                out() << "</ul></td><td class=\"topAlign\"><ul>\n";
            out() << "<li class=\"fn\">";
            generateQmlItem(*m,relative,marker,true);
            out() << "</li>\n";
            row++;
            ++m;
        }
        out() << "</ul>\n";
        if (twoColumn)
            out() << "</td></tr>\n</table>\n";
    }
}

/*!
  Outputs the html detailed documentation for a section
  on a QML element reference page.
 */
void DitaXmlGenerator::generateDetailedQmlMember(const Node *node,
                                              const InnerNode *relative,
                                              CodeMarker *marker)
{
    const QmlPropertyNode* qpn = 0;
#ifdef GENERATE_MAC_REFS    
    generateMacRef(node, marker);
#endif    
    out() << "<div class=\"qmlitem\">";
    if (node->subType() == Node::QmlPropertyGroup) {
        const QmlPropGroupNode* qpgn = static_cast<const QmlPropGroupNode*>(node);
        NodeList::ConstIterator p = qpgn->childNodes().begin();
        out() << "<div class=\"qmlproto\">";
        out() << "<table class=\"qmlname\">";

        while (p != qpgn->childNodes().end()) {
            if ((*p)->type() == Node::QmlProperty) {
                qpn = static_cast<const QmlPropertyNode*>(*p);
                
				if (++numTableRows % 2 == 1)
					out() << "<tr class=\"odd\">";
				else
					out() << "<tr class=\"even\">";
				
				out() << "<td><p>";
                //out() << "<tr><td>"; // old
                out() << "<a name=\"" + refForNode(qpn) + "\"></a>";
                if (!qpn->isWritable(myTree))
                    out() << "<span class=\"qmlreadonly\">read-only</span>";
                if (qpgn->isDefault())
                    out() << "<span class=\"qmldefault\">default</span>";
                generateQmlItem(qpn, relative, marker, false);
                out() << "</td></tr>";
            }
            ++p;
        }
        out() << "</table>";
        out() << "</div>";
    }
    else if (node->type() == Node::QmlSignal) {
        const FunctionNode* qsn = static_cast<const FunctionNode*>(node);
        out() << "<div class=\"qmlproto\">";
        out() << "<table class=\"qmlname\">";
        //out() << "<tr>";
		if (++numTableRows % 2 == 1)
			out() << "<tr class=\"odd\">";
		else
			out() << "<tr class=\"even\">";
        out() << "<td><p>";
        out() << "<a name=\"" + refForNode(qsn) + "\"></a>";
        generateSynopsis(qsn,relative,marker,CodeMarker::Detailed,false);
        //generateQmlItem(qsn,relative,marker,false);
        out() << "</p></td></tr>";
        out() << "</table>";
        out() << "</div>";
    }
    else if (node->type() == Node::QmlMethod) {
        const FunctionNode* qmn = static_cast<const FunctionNode*>(node);
        out() << "<div class=\"qmlproto\">";
        out() << "<table class=\"qmlname\">";
        //out() << "<tr>";
		if (++numTableRows % 2 == 1)
			out() << "<tr class=\"odd\">";
		else
			out() << "<tr class=\"even\">";
        out() << "<td><p>";
        out() << "<a name=\"" + refForNode(qmn) + "\"></a>";
        generateSynopsis(qmn,relative,marker,CodeMarker::Detailed,false);
        out() << "</p></td></tr>";
        out() << "</table>";
        out() << "</div>";
    }
    out() << "<div class=\"qmldoc\">";
    generateStatus(node, marker);
    generateBody(node, marker);
    generateThreadSafeness(node, marker);
    generateSince(node, marker);
    generateAlsoList(node, marker);
    out() << "</div>";
    out() << "</div>";
}

/*!
  Output the "Inherits" line for the QML element,
  if there should be one.
 */
void DitaXmlGenerator::generateQmlInherits(const QmlClassNode* cn,
                                        CodeMarker* marker)
{
    if (cn && !cn->links().empty()) {
        if (cn->links().contains(Node::InheritsLink)) {
            QPair<QString,QString> linkPair;
            linkPair = cn->links()[Node::InheritsLink];
            QStringList strList(linkPair.first);
            const Node* n = myTree->findNode(strList,Node::Fake);
            if (n && n->subType() == Node::QmlClass) {
                const QmlClassNode* qcn = static_cast<const QmlClassNode*>(n);
                out() << "<p class=\"centerAlign\">";
                Text text;
                text << "[Inherits ";
                text << Atom(Atom::LinkNode,CodeMarker::stringForNode(qcn));
                text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
                text << Atom(Atom::String, linkPair.second);
                text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
                text << "]";
                generateText(text, cn, marker);
                out() << "</p>";
            }
        }
    }
}

/*!
  Output the "Inherit by" list for the QML element,
  if it is inherited by any other elements.
 */
void DitaXmlGenerator::generateQmlInheritedBy(const QmlClassNode* cn,
                                           CodeMarker* marker)
{
    if (cn) {
        NodeList subs;
        QmlClassNode::subclasses(cn->name(),subs);
        if (!subs.isEmpty()) {
            Text text;
            text << Atom::ParaLeft << "Inherited by ";
            appendSortedQmlNames(text,cn,subs,marker);
            text << Atom::ParaRight;
            generateText(text, cn, marker);
        }
    }
}

/*!
  Output the "[Xxx instantiates the C++ class QmlGraphicsXxx]"
  line for the QML element, if there should be one.

  If there is no class node, or if the class node status
  is set to Node::Internal, do nothing. 
 */
void DitaXmlGenerator::generateQmlInstantiates(const QmlClassNode* qcn,
                                            CodeMarker* marker)
{
    const ClassNode* cn = qcn->classNode();
    if (cn && (cn->status() != Node::Internal)) {
        out() << "<p class=\"centerAlign\">";
        Text text;
        text << "[";
        text << Atom(Atom::LinkNode,CodeMarker::stringForNode(qcn));
        text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
        text << Atom(Atom::String, qcn->name());
        text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
        text << " instantiates the C++ class ";
        text << Atom(Atom::LinkNode,CodeMarker::stringForNode(cn));
        text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
        text << Atom(Atom::String, cn->name());
        text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
        text << "]";
        generateText(text, qcn, marker);
        out() << "</p>";
    }
}

/*!
  Output the "[QmlGraphicsXxx is instantiated by QML element Xxx]"
  line for the class, if there should be one.

  If there is no QML element, or if the class node status
  is set to Node::Internal, do nothing. 
 */
void DitaXmlGenerator::generateInstantiatedBy(const ClassNode* cn,
                                           CodeMarker* marker)
{
    if (cn &&  cn->status() != Node::Internal && !cn->qmlElement().isEmpty()) {
        const Node* n = myTree->root()->findNode(cn->qmlElement(),Node::Fake);
        if (n && n->subType() == Node::QmlClass) {
            out() << "<p class=\"centerAlign\">";
            Text text;
            text << "[";
            text << Atom(Atom::LinkNode,CodeMarker::stringForNode(cn));
            text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
            text << Atom(Atom::String, cn->name());
            text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
            text << " is instantiated by QML element ";
            text << Atom(Atom::LinkNode,CodeMarker::stringForNode(n));
            text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
            text << Atom(Atom::String, n->name());
            text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
            text << "]";
            generateText(text, cn, marker);
            out() << "</p>";
        }
    }
}

/*!
  Generate the <page> element for the given \a node using the \a writer.
  Return true if a <page> element was written; otherwise return false.
 */
bool DitaXmlGenerator::generatePageElement(QXmlStreamWriter& writer,
                                        const Node* node,
                                        CodeMarker* marker) const
{
    if (node->pageType() == Node::NoPageType)
        return false;
    if (node->name().isEmpty())
        return true;
    if (node->access() == Node::Private)
        return false;
    if (!node->isInnerNode())
        return false;

    QString title;
    QString rawTitle;
    QString fullTitle;
    const InnerNode* inner = static_cast<const InnerNode*>(node);
        
    writer.writeStartElement("page");
    QXmlStreamAttributes attributes;
    QString t;
    t.setNum(id++);
    switch (node->type()) {
    case Node::Fake:
        {
            const FakeNode* fake = static_cast<const FakeNode*>(node);
            title = fake->fullTitle();
            break;
        }
    case Node::Class:
        {
            title = node->name() + " Class Reference";
            break;
        }
    case Node::Namespace:
        {
            rawTitle = marker->plainName(inner);
            fullTitle = marker->plainFullName(inner);
            title = rawTitle + " Namespace Reference";
            break;
        }
    default:
        title = node->name();
        break;
    }
    writer.writeAttribute("id",t);
    writer.writeStartElement("pageWords");
    writer.writeCharacters(title);
    if (!inner->pageKeywords().isEmpty()) {
        const QStringList& w = inner->pageKeywords();
        for (int i = 0; i < w.size(); ++i) {
            writer.writeCharacters(" ");
            writer.writeCharacters(w.at(i).toLocal8Bit().constData());
        }
    }
    writer.writeEndElement();
    writer.writeStartElement("pageTitle");
    writer.writeCharacters(title);
    writer.writeEndElement();
    writer.writeStartElement("pageUrl");
    writer.writeCharacters(PageGenerator::fileName(node));
    writer.writeEndElement();
    writer.writeStartElement("pageType");
    switch (node->pageType()) {
    case Node::ApiPage:
        writer.writeCharacters("APIPage");
        break;
    case Node::ArticlePage:
        writer.writeCharacters("Article");
        break;
    case Node::ExamplePage:
        writer.writeCharacters("Example");
        break;
    default:
        break;
    }
    writer.writeEndElement();
    writer.writeEndElement();
    return true;
}

/*!
  Traverse the tree recursively and generate the <keyword>
  elements.
 */
void DitaXmlGenerator::generatePageElements(QXmlStreamWriter& writer, const Node* node, CodeMarker* marker) const
{
    if (generatePageElement(writer, node, marker)) {

        if (node->isInnerNode()) {
            const InnerNode *inner = static_cast<const InnerNode *>(node);

            // Recurse to write an element for this child node and all its children.
            foreach (const Node *child, inner->childNodes())
                generatePageElements(writer, child, marker);
        }
    }
}

/*!
  Outputs the file containing the index used for searching the html docs.
 */
void DitaXmlGenerator::generatePageIndex(const QString& fileName, CodeMarker* marker) const
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return ;

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("qtPageIndex");

    generatePageElements(writer, myTree->root(), marker);

    writer.writeEndElement(); // qtPageIndex
    writer.writeEndDocument();
    file.close();
}

#endif

/*!
  Return the full qualification of the node \a n, but without
  the name of \a n itself. e.g. A::B::C
 */
QString DitaXmlGenerator::fullQualification(const Node* n)
{
    QString fq;
    InnerNode* in = n->parent();
    while (in) {
        if ((in->type() == Node::Class) ||
            (in->type() == Node::Namespace)) {
            if (in->name().isEmpty())
                break;
            if (fq.isEmpty())
                fq = in->name();
            else
                fq = in->name() + "::" + fq;
        }
        else
            break;
        in = in->parent();
    }
    return fq;
}

void DitaXmlGenerator::writeDerivations(const ClassNode* cn, CodeMarker* marker)
{
    QList<RelatedClass>::ConstIterator r;
    int index;

    if (!cn->baseClasses().isEmpty()) {
        writer.writeStartElement(CXXCLASSDERIVATIONS);
        r = cn->baseClasses().begin();
        index = 0;
        while (r != cn->baseClasses().end()) {
            writer.writeStartElement(CXXCLASSDERIVATION);
            writer.writeStartElement(CXXCLASSDERIVATIONACCESSSPECIFIER);
            writer.writeAttribute("value",(*r).accessString());
            writer.writeEndElement(); // </cxxClassDerivationAccessSpecifier>
            writer.writeStartElement(CXXCLASSBASECLASS);
            writer.writeAttribute("href",(*r).node->ditaXmlHref());
            writer.writeCharacters(marker->plainFullName((*r).node));
            writer.writeEndElement(); // </cxxClassBaseClass>
            writer.writeEndElement(); // </cxxClassDerivation>
             ++r;
        }
        writer.writeEndElement(); // </cxxClassDerivations>
     }
}

void DitaXmlGenerator::writeLocation(const Node* n)
{
    QString s1, s2, s3;
    if (n->type() == Node::Class) {
        s1 = CXXCLASSAPIITEMLOCATION;
        s2 = CXXCLASSDECLARATIONFILE;
        s3 = CXXCLASSDECLARATIONFILELINE;
    }
    else if (n->type() == Node::Function) {
        s1 = CXXFUNCTIONAPIITEMLOCATION;
        s2 = CXXFUNCTIONDECLARATIONFILE;
        s3 = CXXFUNCTIONDECLARATIONFILELINE;
    }
    else if (n->type() == Node::Enum) {
        s1 = CXXENUMERATIONAPIITEMLOCATION;
        s2 = CXXENUMERATIONDECLARATIONFILE;
        s3 = CXXENUMERATIONDECLARATIONFILELINE;
    }
    else if (n->type() == Node::Typedef) {
        s1 = CXXTYPEDEFAPIITEMLOCATION;
        s2 = CXXTYPEDEFDECLARATIONFILE;
        s3 = CXXTYPEDEFDECLARATIONFILELINE;
    }
    else if ((n->type() == Node::Property) ||
             (n->type() == Node::Variable)) {
        s1 = CXXVARIABLEAPIITEMLOCATION;
        s2 = CXXVARIABLEDECLARATIONFILE;
        s3 = CXXVARIABLEDECLARATIONFILELINE;
    }
    writer.writeStartElement(s1);
    writer.writeStartElement(s2);
    writer.writeAttribute("name","filePath");
    writer.writeAttribute("value",n->location().filePath());
    writer.writeEndElement(); // </cxx<s2>DeclarationFile>
    writer.writeStartElement(s3);
    writer.writeAttribute("name","lineNumber");
    QString lineNr;
    writer.writeAttribute("value",lineNr.setNum(n->location().lineNo()));
    writer.writeEndElement(); // </cxx<s3>DeclarationFileLine>
    writer.writeEndElement(); // </cxx<s1>ApiItemLocation>
}

void DitaXmlGenerator::writeFunctions(const Section& s, 
                                      const ClassNode* cn, 
                                      CodeMarker* marker)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Function) {
            FunctionNode* fn = const_cast<FunctionNode*>(static_cast<const FunctionNode*>(*m));
            writer.writeStartElement(CXXFUNCTION);
            writer.writeAttribute("id",fn->guid());
            writer.writeStartElement(APINAME);
            writer.writeCharacters(fn->name());
            writer.writeEndElement(); // </apiName>
            generateBrief(fn,marker);
            writer.writeStartElement(CXXFUNCTIONDETAIL);
            writer.writeStartElement(CXXFUNCTIONDEFINITION);
            writer.writeStartElement(CXXFUNCTIONACCESSSPECIFIER);
            writer.writeAttribute("value",fn->accessString());
            writer.writeEndElement(); // <cxxFunctionAccessSpecifier>

            if (fn->isStatic()) {
                writer.writeStartElement(CXXFUNCTIONSTORAGECLASSSPECIFIERSTATIC);
                writer.writeAttribute("name","static");
                writer.writeAttribute("value","static");
                writer.writeEndElement(); // <cxxFunctionStorageClassSpecifierStatic>
            }
            
            if (fn->isConst()) {
                writer.writeStartElement(CXXFUNCTIONCONST);
                writer.writeAttribute("name","const");
                writer.writeAttribute("value","const");
                writer.writeEndElement(); // <cxxFunctionConst>
            }

            if (fn->virtualness() != FunctionNode::NonVirtual) {
                writer.writeStartElement(CXXFUNCTIONVIRTUAL);
                writer.writeAttribute("name","virtual");
                writer.writeAttribute("value","virtual");
                writer.writeEndElement(); // <cxxFunctionVirtual>
                if (fn->virtualness() == FunctionNode::PureVirtual) {
                    writer.writeStartElement(CXXFUNCTIONPUREVIRTUAL);
                    writer.writeAttribute("name","pure virtual");
                    writer.writeAttribute("value","pure virtual");
                    writer.writeEndElement(); // <cxxFunctionPureVirtual>
                }
            }
            
            if (fn->name() == cn->name()) {
                writer.writeStartElement(CXXFUNCTIONCONSTRUCTOR);
                writer.writeAttribute("name","constructor");
                writer.writeAttribute("value","constructor");
                writer.writeEndElement(); // <cxxFunctionConstructor>
            }
            else if (fn->name()[0] == QChar('~')) {
                writer.writeStartElement(CXXFUNCTIONDESTRUCTOR);
                writer.writeAttribute("name","destructor");
                writer.writeAttribute("value","destructor");
                writer.writeEndElement(); // <cxxFunctionDestructor>
            }
            else {
                writer.writeStartElement(CXXFUNCTIONDECLAREDTYPE);
                writer.writeCharacters(fn->returnType());
                writer.writeEndElement(); // <cxxFunctionDeclaredType>
            }
            QString fq = fullQualification(fn);
            if (!fq.isEmpty()) {
                writer.writeStartElement(CXXFUNCTIONSCOPEDNAME);
                writer.writeCharacters(fq);
                writer.writeEndElement(); // <cxxFunctionScopedName>
            }
            writer.writeStartElement(CXXFUNCTIONPROTOTYPE);
            writer.writeCharacters(fn->signature(true));
            writer.writeEndElement(); // <cxxFunctionPrototype>

            QString fnl = fn->signature(false);
            int idx = fnl.indexOf(' ');
            if (idx < 0)
                idx = 0;
            else
                ++idx;
            fnl = fn->parent()->name() + "::" + fnl.mid(idx);
            writer.writeStartElement(CXXFUNCTIONNAMELOOKUP);
            writer.writeCharacters(fnl);
            writer.writeEndElement(); // <cxxFunctionNameLookup>

            if (fn->isReimp() && fn->reimplementedFrom() != 0) {
                FunctionNode* rfn = (FunctionNode*)fn->reimplementedFrom();
                writer.writeStartElement(CXXFUNCTIONREIMPLEMENTED);
                writer.writeAttribute("href",rfn->ditaXmlHref());
                writer.writeCharacters(marker->plainFullName(rfn));
                writer.writeEndElement(); // </cxxFunctionReimplemented>
            }
            writeParameters(fn,marker);
            writeLocation(fn);
            writer.writeEndElement(); // <cxxFunctionDefinition>
            writer.writeStartElement(APIDESC);

            if (!fn->doc().isEmpty()) {
                generateBody(fn, marker);
                //        generateAlsoList(inner, marker);
            }

            writer.writeEndElement(); // </apiDesc>
            writer.writeEndElement(); // </cxxFunctionDetail>
            writer.writeEndElement(); // </cxxFunction>

            if (fn->metaness() == FunctionNode::Ctor ||
                fn->metaness() == FunctionNode::Dtor ||
                fn->overloadNumber() != 1) {
            }
        }
        ++m;
    }
}

void DitaXmlGenerator::writeParameters(const FunctionNode* fn, CodeMarker* marker)
{
    const QList<Parameter>& parameters = fn->parameters();
    if (!parameters.isEmpty()) {
        writer.writeStartElement(CXXFUNCTIONPARAMETERS);
        QList<Parameter>::ConstIterator p = parameters.begin();
        while (p != parameters.end()) {
            writer.writeStartElement(CXXFUNCTIONPARAMETER);
            writer.writeStartElement(CXXFUNCTIONPARAMETERDECLAREDTYPE);
            writer.writeCharacters((*p).leftType());
            if (!(*p).rightType().isEmpty())
                writer.writeCharacters((*p).rightType());
            writer.writeEndElement(); // <cxxFunctionParameterDeclaredType>
            writer.writeStartElement(CXXFUNCTIONPARAMETERDECLARATIONNAME);
            writer.writeCharacters((*p).name());
            writer.writeEndElement(); // <cxxFunctionParameterDeclarationName>
            if (!(*p).defaultValue().isEmpty()) {
                writer.writeStartElement(CXXFUNCTIONPARAMETERDEFAULTVALUE);
                writer.writeCharacters((*p).defaultValue());
                writer.writeEndElement(); // <cxxFunctionParameterDefaultValue>
            }
            writer.writeEndElement(); // <cxxFunctionParameter>
            ++p;
        }
        writer.writeEndElement(); // <cxxFunctionParameters>
    }
}

void DitaXmlGenerator::writeEnumerations(const Section& s, 
                                         const ClassNode* cn, 
                                         CodeMarker* marker)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Enum) {
            const EnumNode* en = static_cast<const EnumNode*>(*m);
            writer.writeStartElement(CXXENUMERATION);
            writer.writeAttribute("id",en->guid());
            writer.writeStartElement(APINAME);
            writer.writeCharacters(en->name());
            writer.writeEndElement(); // </apiName>
            generateBrief(en,marker);
            writer.writeStartElement(CXXENUMERATIONDETAIL);
            writer.writeStartElement(CXXENUMERATIONDEFINITION);
            writer.writeStartElement(CXXENUMERATIONACCESSSPECIFIER);
            writer.writeAttribute("value",en->accessString());
            writer.writeEndElement(); // <cxxEnumerationAccessSpecifier>

            QString fq = fullQualification(en);
            if (!fq.isEmpty()) {
                writer.writeStartElement(CXXENUMERATIONSCOPEDNAME);
                writer.writeCharacters(fq);
                writer.writeEndElement(); // <cxxEnumerationScopedName>
            }
            const QList<EnumItem>& items = en->items();
            if (!items.isEmpty()) {
                writer.writeStartElement(CXXENUMERATIONPROTOTYPE);
                writer.writeCharacters(en->name());
                writer.writeCharacters(" = { ");
                QList<EnumItem>::ConstIterator i = items.begin();
                while (i != items.end()) {
                    writer.writeCharacters((*i).name());
                    if (!(*i).value().isEmpty()) {
                        writer.writeCharacters(" = ");
                        writer.writeCharacters((*i).value());
                    }
                    ++i;
                    if (i != items.end())
                        writer.writeCharacters(", ");
                }
                writer.writeCharacters(" }");
                writer.writeEndElement(); // <cxxEnumerationPrototype>
            }

            writer.writeStartElement(CXXENUMERATIONNAMELOOKUP);
            writer.writeCharacters(en->parent()->name() + "::" + en->name());
            writer.writeEndElement(); // <cxxEnumerationNameLookup>

            if (!items.isEmpty()) {
                writer.writeStartElement(CXXENUMERATORS);
                QList<EnumItem>::ConstIterator i = items.begin();
                while (i != items.end()) {
                    writer.writeStartElement(CXXENUMERATOR);
                    writer.writeStartElement(APINAME);
                    writer.writeCharacters((*i).name());
                    writer.writeEndElement(); // </apiName>

                    QString fq = fullQualification(en->parent());
                    if (!fq.isEmpty()) {
                        writer.writeStartElement(CXXENUMERATORSCOPEDNAME);
                        writer.writeCharacters(fq + "::" + (*i).name());
                        writer.writeEndElement(); // <cxxEnumeratorScopedName>
                    }
                    writer.writeStartElement(CXXENUMERATORPROTOTYPE);
                    writer.writeCharacters((*i).name());
                    writer.writeEndElement(); // <cxxEnumeratorPrototype>
                    writer.writeStartElement(CXXENUMERATORNAMELOOKUP);
                    writer.writeCharacters(en->parent()->name() + "::" + (*i).name());
                    writer.writeEndElement(); // <cxxEnumeratorNameLookup>

                    if (!(*i).value().isEmpty()) {
                        writer.writeStartElement(CXXENUMERATORINITIALISER);
                        writer.writeAttribute("value", (*i).value());
                        writer.writeEndElement(); // <cxxEnumeratorInitialiser>
                    }
                    if (!(*i).text().isEmpty()) {
                        writer.writeStartElement(APIDESC);
                        generateText((*i).text(), en, marker);
                        writer.writeEndElement(); // </apiDesc>
                    }
                    writer.writeEndElement(); // <cxxEnumerator>
                    ++i;
                }
                writer.writeEndElement(); // <cxxEnumerators>
            }
            
            writeLocation(en);
            writer.writeEndElement(); // <cxxEnumerationDefinition>
            writer.writeStartElement(APIDESC);

            if (!en->doc().isEmpty()) {
                generateBody(en, marker);
            }

            writer.writeEndElement(); // </apiDesc>
            writer.writeEndElement(); // </cxxEnumerationDetail>
            writer.writeEndElement(); // </cxxEnumeration>
        }
        ++m;
    }
}

void DitaXmlGenerator::writeTypedefs(const Section& s, 
                                     const ClassNode* cn, 
                                     CodeMarker* marker)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Typedef) {
            const TypedefNode* tn = static_cast<const TypedefNode*>(*m);
            writer.writeStartElement(CXXTYPEDEF);
            writer.writeAttribute("id",tn->guid());
            writer.writeStartElement(APINAME);
            writer.writeCharacters(tn->name());
            writer.writeEndElement(); // </apiName>
            generateBrief(tn,marker);
            writer.writeStartElement(CXXTYPEDEFDETAIL);
            writer.writeStartElement(CXXTYPEDEFDEFINITION);
            writer.writeStartElement(CXXTYPEDEFACCESSSPECIFIER);
            writer.writeAttribute("value",tn->accessString());
            writer.writeEndElement(); // <cxxTypedefAccessSpecifier>

            QString fq = fullQualification(tn);
            if (!fq.isEmpty()) {
                writer.writeStartElement(CXXTYPEDEFSCOPEDNAME);
                writer.writeCharacters(fq);
                writer.writeEndElement(); // <cxxTypedefScopedName>
            }
            writer.writeStartElement(CXXTYPEDEFNAMELOOKUP);
            writer.writeCharacters(tn->parent()->name() + "::" + tn->name());
            writer.writeEndElement(); // <cxxTypedefNameLookup>
            
            writeLocation(tn);
            writer.writeEndElement(); // <cxxTypedefDefinition>
            writer.writeStartElement(APIDESC);

            if (!tn->doc().isEmpty()) {
                generateBody(tn, marker);
            }

            writer.writeEndElement(); // </apiDesc>
            writer.writeEndElement(); // </cxxTypedefDetail>
            writer.writeEndElement(); // </cxxTypedef>
        }
        ++m;
    }
}

void DitaXmlGenerator::writeProperties(const Section& s, 
                                       const ClassNode* cn, 
                                       CodeMarker* marker)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Property) {
            const PropertyNode* pn = static_cast<const PropertyNode*>(*m);
            writer.writeStartElement(CXXVARIABLE);
            writer.writeAttribute("id",pn->guid());
            writer.writeStartElement(APINAME);
            writer.writeCharacters(pn->name());
            writer.writeEndElement(); // </apiName>
            generateBrief(pn,marker);
            writer.writeStartElement(CXXVARIABLEDETAIL);
            writer.writeStartElement(CXXVARIABLEDEFINITION);
            writer.writeStartElement(CXXVARIABLEACCESSSPECIFIER);
            writer.writeAttribute("value",pn->accessString());
            writer.writeEndElement(); // <cxxVariableAccessSpecifier>

            if (!pn->qualifiedDataType().isEmpty()) {
                writer.writeStartElement(CXXVARIABLEDECLAREDTYPE);
                writer.writeCharacters(pn->qualifiedDataType());
                writer.writeEndElement(); // <cxxVariableDeclaredType>
            }
            QString fq = fullQualification(pn);
            if (!fq.isEmpty()) {
                writer.writeStartElement(CXXVARIABLESCOPEDNAME);
                writer.writeCharacters(fq);
                writer.writeEndElement(); // <cxxVariableScopedName>
            }
            
            writer.writeStartElement(CXXVARIABLEPROTOTYPE);
            writer.writeCharacters("Q_PROPERTY(");
            writer.writeCharacters(pn->qualifiedDataType());
            writer.writeCharacters(" ");
            writer.writeCharacters(pn->name());
            writePropParams("READ",pn->getters());
            writePropParams("WRITE",pn->setters());
            writePropParams("RESET",pn->resetters());
            writePropParams("NOTIFY",pn->notifiers());
            if (pn->isDesignable() != pn->designableDefault()) {
                writer.writeCharacters(" DESIGNABLE ");
                if (!pn->runtimeDesignabilityFunction().isEmpty())
                    writer.writeCharacters(pn->runtimeDesignabilityFunction());
                else
                    writer.writeCharacters(pn->isDesignable() ? "true" : "false");
            }
            if (pn->isScriptable() != pn->scriptableDefault()) {
                writer.writeCharacters(" SCRIPTABLE ");
                if (!pn->runtimeScriptabilityFunction().isEmpty())
                    writer.writeCharacters(pn->runtimeScriptabilityFunction());
                else
                    writer.writeCharacters(pn->isScriptable() ? "true" : "false");
            }
            if (pn->isWritable() != pn->writableDefault()) {
                writer.writeCharacters(" STORED ");
                writer.writeCharacters(pn->isStored() ? "true" : "false");
            }
            if (pn->isUser() != pn->userDefault()) {
                writer.writeCharacters(" USER ");
                writer.writeCharacters(pn->isUser() ? "true" : "false");
            }
            if (pn->isConstant())
                writer.writeCharacters(" CONSTANT");
            if (pn->isFinal())
                writer.writeCharacters(" FINAL");
            writer.writeCharacters(")");
            writer.writeEndElement(); // <cxxVariablePrototype>

            writer.writeStartElement(CXXVARIABLENAMELOOKUP);
            writer.writeCharacters(pn->parent()->name() + "::" + pn->name());
            writer.writeEndElement(); // <cxxVariableNameLookup>

            if (pn->overriddenFrom() != 0) {
                PropertyNode* opn = (PropertyNode*)pn->overriddenFrom();
                writer.writeStartElement(CXXVARIABLEREIMPLEMENTED);
                writer.writeAttribute("href",opn->ditaXmlHref());
                writer.writeCharacters(marker->plainFullName(opn));
                writer.writeEndElement(); // </cxxVariableReimplemented>
            }

            writeLocation(pn);
            writer.writeEndElement(); // <cxxVariableDefinition>
            writer.writeStartElement(APIDESC);

            if (!pn->doc().isEmpty()) {
                generateBody(pn, marker);
            }

            writer.writeEndElement(); // </apiDesc>
            writer.writeEndElement(); // </cxxVariableDetail>
            writer.writeEndElement(); // </cxxVariable>
        }
        ++m;
    }
}

void DitaXmlGenerator::writeDataMembers(const Section& s, 
                                        const ClassNode* cn, 
                                        CodeMarker* marker)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Variable) {
            const VariableNode* vn = static_cast<const VariableNode*>(*m);
            writer.writeStartElement(CXXVARIABLE);
            writer.writeAttribute("id",vn->guid());
            writer.writeStartElement(APINAME);
            writer.writeCharacters(vn->name());
            writer.writeEndElement(); // </apiName>
            generateBrief(vn,marker);
            writer.writeStartElement(CXXVARIABLEDETAIL);
            writer.writeStartElement(CXXVARIABLEDEFINITION);
            writer.writeStartElement(CXXVARIABLEACCESSSPECIFIER);
            writer.writeAttribute("value",vn->accessString());
            writer.writeEndElement(); // <cxxVariableAccessSpecifier>

            if (vn->isStatic()) {
                writer.writeStartElement(CXXVARIABLESTORAGECLASSSPECIFIERSTATIC);
                writer.writeAttribute("name","static");
                writer.writeAttribute("value","static");
                writer.writeEndElement(); // <cxxVariableStorageClassSpecifierStatic>
            }

            writer.writeStartElement(CXXVARIABLEDECLAREDTYPE);
            writer.writeCharacters(vn->leftType());
            if (!vn->rightType().isEmpty())
                writer.writeCharacters(vn->rightType());
            writer.writeEndElement(); // <cxxVariableDeclaredType>

            QString fq = fullQualification(vn);
            if (!fq.isEmpty()) {
                writer.writeStartElement(CXXVARIABLESCOPEDNAME);
                writer.writeCharacters(fq);
                writer.writeEndElement(); // <cxxVariableScopedName>
            }
            
            writer.writeStartElement(CXXVARIABLEPROTOTYPE);
            writer.writeCharacters(vn->leftType() + " ");
            //writer.writeCharacters(vn->parent()->name() + "::" + vn->name()); 
            writer.writeCharacters(vn->name()); 
            if (!vn->rightType().isEmpty())
                writer.writeCharacters(vn->rightType());
            writer.writeEndElement(); // <cxxVariablePrototype>

            writer.writeStartElement(CXXVARIABLENAMELOOKUP);
            writer.writeCharacters(vn->parent()->name() + "::" + vn->name());
            writer.writeEndElement(); // <cxxVariableNameLookup>

            writeLocation(vn);
            writer.writeEndElement(); // <cxxVariableDefinition>
            writer.writeStartElement(APIDESC);

            if (!vn->doc().isEmpty()) {
                generateBody(vn, marker);
            }

            writer.writeEndElement(); // </apiDesc>
            writer.writeEndElement(); // </cxxVariableDetail>
            writer.writeEndElement(); // </cxxVariable>
        }
        ++m;
    }
}

void DitaXmlGenerator::writeMacros(const Section& s, 
                                   const ClassNode* cn, 
                                   CodeMarker* marker)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Function) {
            const FunctionNode* fn = static_cast<const FunctionNode*>(*m);
            if (fn->isMacro()) {
                writer.writeStartElement(CXXDEFINE);
                writer.writeAttribute("id",fn->guid());
                writer.writeStartElement(APINAME);
                writer.writeCharacters(fn->name());
                writer.writeEndElement(); // </apiName>
                generateBrief(fn,marker);
                writer.writeStartElement(CXXDEFINEDETAIL);
                writer.writeStartElement(CXXDEFINEDEFINITION);
                writer.writeStartElement(CXXDEFINEACCESSSPECIFIER);
                writer.writeAttribute("value",fn->accessString());
                writer.writeEndElement(); // <cxxDefineAccessSpecifier>
            
                writer.writeStartElement(CXXDEFINEPROTOTYPE);
                writer.writeCharacters("#define ");
                writer.writeCharacters(fn->name());
                if (fn->metaness() == FunctionNode::MacroWithParams) {
                    QStringList params = fn->parameterNames();
                    if (!params.isEmpty()) {
                        writer.writeCharacters("(");
                        for (int i = 0; i < params.size(); ++i) {
                            if (params[i].isEmpty())
                                writer.writeCharacters("...");
                            else
                                writer.writeCharacters(params[i]);
                            if ((i+1) < params.size()) 
                                writer.writeCharacters(", ");
                        }
                        writer.writeCharacters(")");
                    }
                }
                writer.writeEndElement(); // <cxxDefinePrototype>

                writer.writeStartElement(CXXDEFINENAMELOOKUP);
                writer.writeCharacters(fn->name());
                writer.writeEndElement(); // <cxxDefineNameLookup>

                if (fn->reimplementedFrom() != 0) {
                    FunctionNode* rfn = (FunctionNode*)fn->reimplementedFrom();
                    writer.writeStartElement(CXXDEFINEREIMPLEMENTED);
                    writer.writeAttribute("href",rfn->ditaXmlHref());
                    writer.writeCharacters(marker->plainFullName(rfn));
                    writer.writeEndElement(); // </cxxDefineReimplemented>
                }

                if (fn->metaness() == FunctionNode::MacroWithParams) {
                    QStringList params = fn->parameterNames();
                    if (!params.isEmpty()) {
                        writer.writeStartElement(CXXDEFINEPARAMETERS);
                        for (int i = 0; i < params.size(); ++i) {
                            writer.writeStartElement(CXXDEFINEPARAMETER);
                            writer.writeStartElement(CXXDEFINEPARAMETERDECLARATIONNAME);
                            writer.writeCharacters(params[i]);
                            writer.writeEndElement(); // <cxxDefineParameterDeclarationName>
                            writer.writeEndElement(); // <cxxDefineParameter>
                        }
                        writer.writeEndElement(); // <cxxDefineParameters>
                    }
                }

                writeLocation(fn);
                writer.writeEndElement(); // <cxxDefineDefinition>
                writer.writeStartElement(APIDESC);

                if (!fn->doc().isEmpty()) {
                    generateBody(fn, marker);
                }

                writer.writeEndElement(); // </apiDesc>
                writer.writeEndElement(); // </cxxDefineDetail>
                writer.writeEndElement(); // </cxxDefine>
            }
        }
        ++m;
    }
}

void DitaXmlGenerator::writePropParams(const QString& tag, const NodeList& nlist)
{
    NodeList::const_iterator n = nlist.begin();
    while (n != nlist.end()) {
        writer.writeCharacters(" ");
        writer.writeCharacters(tag);
        writer.writeCharacters(" ");
        writer.writeCharacters((*n)->name());
        ++n;
    }
}

QT_END_NAMESPACE
