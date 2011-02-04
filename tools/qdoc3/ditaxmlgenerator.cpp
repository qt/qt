/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "quoter.h"
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
bool DitaXmlGenerator::inApiDesc = false;
bool DitaXmlGenerator::inSection = false;
bool DitaXmlGenerator::inDetailedDescription = false;
bool DitaXmlGenerator::inLegaleseText = false;

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

/*!
  Quick, dirty, and very ugly. Unescape \a text
  so QXmlStreamWriter::writeCharacters() can put
  the escapes back in again!
 */
void DitaXmlGenerator::writeCharacters(const QString& text)
{
    QString t = text;
    t = t.replace("&lt;","<");
    t = t.replace("&gt;",">");
    t = t.replace("&amp;","&");
    t = t.replace("&quot;","\"");
    xmlWriter().writeCharacters(t);
}

/*!
  Appends an <xref> element to the current XML stream
  with the \a href attribute and the \a text.
 */
void DitaXmlGenerator::addLink(const QString& href,
                               const QStringRef& text)
{
    if (!href.isEmpty()) {
        xmlWriter().writeStartElement("xref");
        xmlWriter().writeAttribute("href", href);
        writeCharacters(text.toString());
        xmlWriter().writeEndElement(); // </xref>
    }
    else {
        writeCharacters(text.toString());
    }
}

/*!
  The default constructor.
 */
DitaXmlGenerator::DitaXmlGenerator()
    : inLink(false),
      inContents(false),
      inSectionHeading(false),
      inTableHeader(false),
      inTableBody(false),
      numTableRows(0),
      threeColumnEnumValueTable(true),
      offlineDocs(true),
      funcLeftParen("\\S(\\()"),
      myTree(0),
      obsoleteLinks(false),
      noLinks(false),
      tableColumnCount(0)
{
    // nothing yet.
}

/*!
  The destructor has nothing to do.
 */
DitaXmlGenerator::~DitaXmlGenerator()
{
    GuidMaps::iterator i = guidMaps.begin();
    while (i != guidMaps.end()) {
        delete i.value();
        ++i;
    }
}

/*!
  A lot of internal structures are initialized.
 */
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

    stylesheets = config.getStringList(DitaXmlGenerator::format() +
                                       Config::dot +
                                       DITAXMLGENERATOR_STYLESHEETS);
    customHeadElements = config.getStringList(DitaXmlGenerator::format() +
                                              Config::dot +
                                              DITAXMLGENERATOR_CUSTOMHEADELEMENTS);
    codeIndent = config.getInt(CONFIG_CODEINDENT);

}

/*!
  All this does is call the same function in the base class.
 */
void DitaXmlGenerator::terminateGenerator()
{
    Generator::terminateGenerator();
}

/*!
  Returns "DITAXML".
 */
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
    QString guid = lookupGuid(outFileName(),text);
    xmlWriter().writeAttribute("id",guid);
    return guid;
}


/*!
  Write's the GUID for the \a node to the current XML stream
  as an "id" attribute. If the \a node doesn't yet have a GUID,
  one is generated.
 */
void DitaXmlGenerator::writeGuidAttribute(Node* node)
{
    xmlWriter().writeAttribute("id",node->guid());
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
    QString t = QUuid::createUuid().toString();
    QString guid = "id-" + t.mid(1,t.length()-2);
    name2guidMap.insert(text,guid);
    return guid;
}

/*!
  First, look up the GUID map for \a fileName. If there isn't
  a GUID map for \a fileName, create one and insert it into
  the map of GUID maps. Then look up \a text in that GUID map.
  If \a text is found, return the associated GUID. Otherwise,
  insert \a text into the GUID map with a new GUID, and return
  the new GUID.
 */
QString DitaXmlGenerator::lookupGuid(const QString& fileName, const QString& text)
{
    GuidMap* gm = lookupGuidMap(fileName);
    GuidMap::const_iterator i = gm->find(text);
    if (i != gm->end())
        return i.value();
    QString t = QUuid::createUuid().toString();
    QString guid = "id-" + t.mid(1,t.length()-2);
    gm->insert(text,guid);
    return guid;
}

/*!
  Looks up \a fileName in the map of GUID maps. If it finds
  \a fileName, it returns a pointer to the associated GUID
  map. Otherwise it creates a new GUID map and inserts it
  into the map of GUID maps with \a fileName as its key.
 */
GuidMap* DitaXmlGenerator::lookupGuidMap(const QString& fileName)
{
    GuidMaps::const_iterator i = guidMaps.find(fileName);
    if (i != guidMaps.end())
        return i.value();
    GuidMap* gm = new GuidMap;
    guidMaps.insert(fileName,gm);
    return gm;
}

/*!
  This is where the DITA XML files are written.
  \note The file generation is done in the base class,
  PageGenerator::generateTree().
 */
void DitaXmlGenerator::generateTree(const Tree *tree)
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
    qmlClasses.clear();
    findAllClasses(tree->root());
    findAllFunctions(tree->root());
    findAllLegaleseTexts(tree->root());
    findAllNamespaces(tree->root());
    findAllSince(tree->root());

    PageGenerator::generateTree(tree);
    writeDitaMap();
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

static int countTableColumns(const Atom* t)
{
    int result = 0;
    if (t->type() == Atom::TableHeaderLeft) {
        while (t->type() == Atom::TableHeaderLeft) {
            int count = 0;
            t = t->next();
            while (t->type() != Atom::TableHeaderRight) {
                if (t->type() == Atom::TableItemLeft)
                    ++count;
                t = t->next();
            }
            if (count > result)
                result = count;
            t = t->next();
        }
    }
    else if (t->type() == Atom::TableRowLeft) {
        while (t->type() != Atom::TableRowRight) {
            if (t->type() == Atom::TableItemLeft)
                ++result;
            t = t->next();
        }
    }
    return result;
}

/*!
  Generate html from an instance of Atom.
 */
int DitaXmlGenerator::generateAtom(const Atom *atom,
                                   const Node *relative,
                                   CodeMarker *marker)
{
    int skipAhead = 0;
    QString hx, str;
    static bool in_para = false;
    QString guid, hc;

    switch (atom->type()) {
    case Atom::AbstractLeft:
        break;
    case Atom::AbstractRight:
        break;
    case Atom::AutoLink:
        if (!noLinks && !inLink && !inContents && !inSectionHeading) {
            const Node* node = 0;
            QString link = getLink(atom, relative, marker, &node);
            if (!link.isEmpty()) {
                beginLink(link);
                generateLink(atom, relative, marker);
                endLink();
            }
            else {
                writeCharacters(protectEnc(atom->string()));
            }
        }
        else {
            writeCharacters(protectEnc(atom->string()));
        }
        break;
    case Atom::BaseName:
        break;
    case Atom::BriefLeft:
        //if (relative->type() == Node::Fake) {
        //skipAhead = skipAtoms(atom, Atom::BriefRight);
        //break;
        //}
        if (inApiDesc || inSection) {
            xmlWriter().writeStartElement("p");
            xmlWriter().writeAttribute("outputclass","brief");
        }
        else {
            noLinks = true;
            xmlWriter().writeStartElement("shortdesc");
        }
        if (relative->type() == Node::Property ||
            relative->type() == Node::Variable) {
            xmlWriter().writeCharacters("This ");
            if (relative->type() == Node::Property)
                xmlWriter().writeCharacters("property");
            else if (relative->type() == Node::Variable)
                xmlWriter().writeCharacters("variable");
            xmlWriter().writeCharacters(" holds ");
        }
        if (noLinks) {
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
            writeCharacters(str + ".");
        }
        break;
    case Atom::BriefRight:
        //        if (relative->type() != Node::Fake) 
        xmlWriter().writeEndElement(); // </shortdesc> or </p>
        noLinks = false;
        break;
    case Atom::C:
        xmlWriter().writeStartElement(formattingLeftMap()[ATOM_FORMATTING_TELETYPE]);
        if (inLink) {
            writeCharacters(protectEnc(plainCode(atom->string())));
        }
        else {
            writeText(atom->string(), marker, relative);
        }
        xmlWriter().writeEndElement(); // sse writeStartElement() above
        break;
    case Atom::Code:
        {
            xmlWriter().writeStartElement("codeblock");
            QString chars = trimmedTrailing(atom->string()); 
            writeText(chars, marker, relative);
            xmlWriter().writeEndElement(); // </codeblock>
        }
	break;
    case Atom::Qml:
        xmlWriter().writeStartElement("codeblock");
        writeText(trimmedTrailing(atom->string()), marker, relative);
        xmlWriter().writeEndElement(); // </codeblock>
	break;
    case Atom::CodeNew:
        xmlWriter().writeStartElement("p");
        xmlWriter().writeCharacters("you can rewrite it as");
        xmlWriter().writeEndElement(); // </p>
        xmlWriter().writeStartElement("codeblock");
        writeText(trimmedTrailing(atom->string()), marker, relative);
        xmlWriter().writeEndElement(); // </codeblock>
        break;
    case Atom::CodeOld:
        xmlWriter().writeStartElement("p");
        xmlWriter().writeCharacters("For example, if you have code like");
        xmlWriter().writeEndElement(); // </p>
        // fallthrough
    case Atom::CodeBad:
        xmlWriter().writeStartElement("codeblock");
        writeCharacters(trimmedTrailing(plainCode(atom->string())));
        xmlWriter().writeEndElement(); // </codeblock>
	break;
    case Atom::Div:
        xmlWriter().writeStartElement("bodydiv");
        if (!atom->string().isEmpty())
            xmlWriter().writeAttribute("outputclass", atom->string());
        break;
    case Atom::EndDiv:
        xmlWriter().writeEndElement(); // </bodydiv>
        break;
    case Atom::FootnoteLeft:
        // ### For now
        if (in_para) {
            xmlWriter().writeEndElement(); // </p>
            in_para = false;
        }
        xmlWriter().writeCharacters("<!-- ");
        break;
    case Atom::FootnoteRight:
        // ### For now
        xmlWriter().writeCharacters("-->");
        break;
    case Atom::FormatElse:
    case Atom::FormatEndif:
    case Atom::FormatIf:
        break;
    case Atom::FormattingLeft:
        xmlWriter().writeStartElement(formattingLeftMap()[atom->string()]);
        if (atom->string() == ATOM_FORMATTING_PARAMETER) {
            if (atom->next() != 0 && atom->next()->type() == Atom::String) {
                QRegExp subscriptRegExp("([a-z]+)_([0-9n])");
                if (subscriptRegExp.exactMatch(atom->next()->string())) {
                    xmlWriter().writeCharacters(subscriptRegExp.cap(1));
                    xmlWriter().writeStartElement("sub");
                    xmlWriter().writeCharacters(subscriptRegExp.cap(2));
                    xmlWriter().writeEndElement(); // </sub>
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
            xmlWriter().writeEndElement(); // ?
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
        else if (atom->string() == "qmlclasses") {
            generateCompactList(relative, marker, qmlClasses, true);
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
                xmlWriter().writeStartElement("ul");
                s = sections.constBegin();
                while (s != sections.constEnd()) {
                    if (!(*s).members.isEmpty()) {
                        QString li = outFileName() + "#" + Doc::canonicalTitle((*s).name);
                        writeXrefListItem(li, (*s).name);
                    }
                    ++s;
                }
                xmlWriter().writeEndElement(); // </ul>

                int idx = 0;
                s = sections.constBegin();
                while (s != sections.constEnd()) {
                    if (!(*s).members.isEmpty()) {
                        xmlWriter().writeStartElement("p");
                        writeGuidAttribute(Doc::canonicalTitle((*s).name));
                        xmlWriter().writeAttribute("outputclass","h3");
                        writeCharacters(protectEnc((*s).name));
                        xmlWriter().writeEndElement(); // </p>
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
                                xmlWriter().writeStartElement("p");
                                xmlWriter().writeCharacters("Class ");
                                xmlWriter().writeStartElement("xref");
                                xmlWriter().writeAttribute("href",linkForNode(pmap.key(), 0));
                                QStringList pieces = fullName(pmap.key(), 0, marker).split("::");
                                writeCharacters(protectEnc(pieces.last()));
                                xmlWriter().writeEndElement(); // </xref>
                                xmlWriter().writeCharacters(":");
                                xmlWriter().writeEndElement(); // </p>

                                generateSection(nlist, 0, marker, CodeMarker::Summary);
                                ++pmap;
                            }
                        }
                        else {
                            generateSection(s->members, 0, marker, CodeMarker::Summary);
                        }
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
            if (fileName.isEmpty()) {
                /*
                  Don't bother outputting an error message.
                  Just output the href as if the image is in
                  the images directory...
                 */
                fileName = QLatin1String("images/") + protectEnc(atom->string());
            }

            xmlWriter().writeStartElement("fig");
            xmlWriter().writeStartElement("image");
            xmlWriter().writeAttribute("href",protectEnc(fileName));
            if (atom->type() == Atom::InlineImage)
                xmlWriter().writeAttribute("placement","inline");
            else {
                xmlWriter().writeAttribute("placement","break");
                xmlWriter().writeAttribute("align","center");
            }
            if (!text.isEmpty()) {
                xmlWriter().writeStartElement("alt");
                writeCharacters(protectEnc(text));
                xmlWriter().writeEndElement(); // </alt>
            }
            xmlWriter().writeEndElement(); // </image>
            xmlWriter().writeEndElement(); // </fig>
        }
        break;
    case Atom::ImageText:
        // nothing
        break;
    case Atom::LegaleseLeft:
        inLegaleseText = true;
        break;
    case Atom::LegaleseRight:
        inLegaleseText = false;
        break;
    case Atom::LineBreak:
        //xmlWriter().writeEmptyElement("br");
        break;
    case Atom::Link:
        {
            const Node *node = 0;
            QString myLink = getLink(atom, relative, marker, &node);
            if (myLink.isEmpty()) {
                relative->doc().location().warning(tr("Can't link to '%1' in %2")
                        .arg(atom->string())
                        .arg(marker->plainFullName(relative)));
            }
            else if (!inSectionHeading) {
                beginLink(myLink);
            }
#if 0
            else {
                //xmlWriter().writeCharacters(atom->string());
                //qDebug() << "MYLINK:" << myLink << outFileName() << atom->string();
            }
#endif
            skipAhead = 1;
        }
        break;
    case Atom::GuidLink:
        {
#if 0            
            qDebug() << "GUID LINK:" << atom->string() << outFileName();
#endif            
            beginLink(atom->string());
            skipAhead = 1;
        }
        break;
    case Atom::LinkNode:
        {
            const Node* node = CodeMarker::nodeForString(atom->string());
            beginLink(linkForNode(node, relative));
            skipAhead = 1;
        }
        break;
    case Atom::ListLeft:
        if (in_para) {
            xmlWriter().writeEndElement(); // </p>
            in_para = false;
        }
        if (atom->string() == ATOM_LIST_BULLET) {
            xmlWriter().writeStartElement("ul");
        }
        else if (atom->string() == ATOM_LIST_TAG) {
            xmlWriter().writeStartElement("dl");
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            threeColumnEnumValueTable = isThreeColumnEnumValueTable(atom);
            if (threeColumnEnumValueTable) {
                xmlWriter().writeStartElement("simpletable");
                xmlWriter().writeAttribute("outputclass","valuelist");
                xmlWriter().writeStartElement("sthead");
                xmlWriter().writeStartElement("stentry");
                xmlWriter().writeCharacters("Constant");
                xmlWriter().writeEndElement(); // </stentry>
                xmlWriter().writeStartElement("stentry");
                xmlWriter().writeCharacters("Value");
                xmlWriter().writeEndElement(); // </stentry>
                xmlWriter().writeStartElement("stentry");
                xmlWriter().writeCharacters("Description");
                xmlWriter().writeEndElement(); // </stentry>
                xmlWriter().writeEndElement(); // </sthead>
            }
            else {
                xmlWriter().writeStartElement("simpletable");
                xmlWriter().writeAttribute("outputclass","valuelist");
                xmlWriter().writeStartElement("sthead");
                xmlWriter().writeStartElement("stentry");
                xmlWriter().writeCharacters("Constant");
                xmlWriter().writeEndElement(); // </stentry>
                xmlWriter().writeStartElement("stentry");
                xmlWriter().writeCharacters("Value");
                xmlWriter().writeEndElement(); // </stentry>
                xmlWriter().writeEndElement(); // </sthead>
            }
        }
        else {
            xmlWriter().writeStartElement("ol");
            if (atom->string() == ATOM_LIST_UPPERALPHA)
                xmlWriter().writeAttribute("outputclass","upperalpha");
            else if (atom->string() == ATOM_LIST_LOWERALPHA)
                xmlWriter().writeAttribute("outputclass","loweralpha");
            else if (atom->string() == ATOM_LIST_UPPERROMAN)
                xmlWriter().writeAttribute("outputclass","upperroman");
            else if (atom->string() == ATOM_LIST_LOWERROMAN)
                xmlWriter().writeAttribute("outputclass","lowerroman");
            else // (atom->string() == ATOM_LIST_NUMERIC)
                xmlWriter().writeAttribute("outputclass","numeric");
            if (atom->next() != 0 && atom->next()->string().toInt() != 1) {
                // I don't think this attribute is supported.
                xmlWriter().writeAttribute("start",atom->next()->string());
            }
        }
        break;
    case Atom::ListItemNumber:
        // nothing
        break;
    case Atom::ListTagLeft:
        if (atom->string() == ATOM_LIST_TAG) {
            xmlWriter().writeStartElement("dt");
        }
        else { // (atom->string() == ATOM_LIST_VALUE)
            xmlWriter().writeStartElement("strow");
            xmlWriter().writeStartElement("stentry");
            xmlWriter().writeStartElement("tt");
            writeCharacters(protectEnc(plainCode(marker->markedUpEnumValue(atom->next()->string(),
                                                                           relative))));
            xmlWriter().writeEndElement(); // </tt>
            xmlWriter().writeEndElement(); // </stentry>
            xmlWriter().writeStartElement("stentry");

            QString itemValue;
            if (relative->type() == Node::Enum) {
                const EnumNode *enume = static_cast<const EnumNode *>(relative);
                itemValue = enume->itemValue(atom->next()->string());
            }

            if (itemValue.isEmpty())
                xmlWriter().writeCharacters("?");
            else {
                xmlWriter().writeStartElement("tt");
                writeCharacters(protectEnc(itemValue));
                xmlWriter().writeEndElement(); // </tt>
            }
            skipAhead = 1;
        }
        break;
    case Atom::ListTagRight:
        if (atom->string() == ATOM_LIST_TAG)
            xmlWriter().writeEndElement(); // </dt>
        break;
    case Atom::ListItemLeft:
        if (atom->string() == ATOM_LIST_TAG) {
            xmlWriter().writeStartElement("dd");
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            if (threeColumnEnumValueTable) {
                xmlWriter().writeEndElement(); // </stentry>
                xmlWriter().writeStartElement("stentry");
            }
        }
        else {
            xmlWriter().writeStartElement("li");
        }
        if (matchAhead(atom, Atom::ParaLeft))
            skipAhead = 1;
        break;
    case Atom::ListItemRight:
        if (atom->string() == ATOM_LIST_TAG) {
            xmlWriter().writeEndElement(); // </dd>
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            xmlWriter().writeEndElement(); // </stentry>
            xmlWriter().writeEndElement(); // </strow>
        }
        else {
            xmlWriter().writeEndElement(); // </li>
        }
        break;
    case Atom::ListRight:
        if (atom->string() == ATOM_LIST_BULLET) {
            xmlWriter().writeEndElement(); // </ul>
        }
        else if (atom->string() == ATOM_LIST_TAG) {
            xmlWriter().writeEndElement(); // </dl>
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            xmlWriter().writeEndElement(); // </simpletable>
        }
        else {
            xmlWriter().writeEndElement(); // </ol>
        }
        break;
    case Atom::Nop:
        // nothing
        break;
    case Atom::ParaLeft:
        xmlWriter().writeStartElement("p");
        if (inLegaleseText)
            xmlWriter().writeAttribute("outputclass","legalese");
        in_para = true;
        break;
    case Atom::ParaRight:
        endLink();
        if (in_para) {
            xmlWriter().writeEndElement(); // </p>
            in_para = false;
        }
        break;
    case Atom::QuotationLeft:
        xmlWriter().writeStartElement("lq");
        break;
    case Atom::QuotationRight:
        xmlWriter().writeEndElement(); // </lq>
        break;
    case Atom::RawString:
        if (atom->string() == " ")
            break;
        if (atom->string().startsWith("&"))
            writeCharacters(atom->string());
        else if (atom->string() == "<sup>*</sup>") {
            xmlWriter().writeStartElement("sup");
            writeCharacters("*");
            xmlWriter().writeEndElement(); // </sup>
        }
        else {
            xmlWriter().writeStartElement("pre");
            xmlWriter().writeAttribute("outputclass","raw-html");
            writeCharacters(atom->string());
            xmlWriter().writeEndElement(); // </pre>
        }
        break;
    case Atom::SectionLeft:
        if (inSection || inApiDesc) {
            inApiDesc = false;
            xmlWriter().writeEndElement(); // </section> or </apiDesc>
        }
        inSection = true;
        xmlWriter().writeStartElement("section");
        writeGuidAttribute(Doc::canonicalTitle(Text::sectionHeading(atom).toString()));
        xmlWriter().writeAttribute("outputclass","details");
        break;
    case Atom::SectionRight:
        if (inSection) {
            inSection = false;
            xmlWriter().writeEndElement(); // </section>
        }
        break;
    case Atom::SectionHeadingLeft:
        xmlWriter().writeStartElement("title");
        hx = "h" + QString::number(atom->string().toInt() + hOffset(relative));
        xmlWriter().writeAttribute("outputclass",hx);
        inSectionHeading = true;
        break;
    case Atom::SectionHeadingRight:
        xmlWriter().writeEndElement(); // </title> (see case Atom::SectionHeadingLeft)
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
            writeCharacters(protectEnc(atom->string()));
        }
        break;
    case Atom::TableLeft:
        {
            if (in_para) {
                xmlWriter().writeEndElement(); // </p>
                in_para = false;
            }
            xmlWriter().writeStartElement("table");
            numTableRows = 0;
            if (tableColumnCount != 0) {
                qDebug() << "ERROR: Nested tables!";
                tableColumnCount = 0;
            }
            tableColumnCount = countTableColumns(atom->next());
            xmlWriter().writeStartElement("tgroup");
            xmlWriter().writeAttribute("cols",QString::number(tableColumnCount));
            inTableHeader = false;
            inTableBody = false;
        }
        break;
    case Atom::TableRight:
        xmlWriter().writeEndElement(); // </tbody>
        xmlWriter().writeEndElement(); // </tgroup>
        xmlWriter().writeEndElement(); // </table>
        inTableHeader = false;
        inTableBody = false;
        tableColumnCount = 0;
        break;
    case Atom::TableHeaderLeft:
        if (inTableBody) {
            xmlWriter().writeEndElement(); // </tbody>
            xmlWriter().writeEndElement(); // </tgroup>
            xmlWriter().writeEndElement(); // </table>
            inTableHeader = false;
            inTableBody = false;
            tableColumnCount = 0;
            xmlWriter().writeStartElement("table");
            numTableRows = 0;
            tableColumnCount = countTableColumns(atom);
            xmlWriter().writeStartElement("tgroup");
            xmlWriter().writeAttribute("cols",QString::number(tableColumnCount));
        }
        xmlWriter().writeStartElement("thead");
        xmlWriter().writeAttribute("valign","top");
        xmlWriter().writeStartElement("row");
        xmlWriter().writeAttribute("valign","top");
        inTableHeader = true;
        inTableBody = false;
        break;
    case Atom::TableHeaderRight:
        xmlWriter().writeEndElement(); // </row>
        if (matchAhead(atom, Atom::TableHeaderLeft)) {
            skipAhead = 1;
            xmlWriter().writeStartElement("row");
            xmlWriter().writeAttribute("valign","top");
        }
        else { 
            xmlWriter().writeEndElement(); // </thead>
            inTableHeader = false;
            inTableBody = true;
            xmlWriter().writeStartElement("tbody");
        }
        break;
    case Atom::TableRowLeft:
        if (!inTableHeader && !inTableBody) {
            inTableBody = true;
            xmlWriter().writeStartElement("tbody");
        }
        xmlWriter().writeStartElement("row");
        xmlWriter().writeAttribute("valign","top");
        break;
    case Atom::TableRowRight:
        xmlWriter().writeEndElement(); // </row>
        break;
    case Atom::TableItemLeft:
        {
            xmlWriter().writeStartElement("entry");
            QStringList spans = atom->string().split(",");
            if (spans.size() == 2) {
                if (inTableHeader ||
                    (spans[0].toInt() != 1) ||
                    (spans[1].toInt() != 1)) {
                    QString s = "span(" + spans[0] + "," + spans[1] + ")";
                    xmlWriter().writeAttribute("outputclass",s);
                }
            }
            if (matchAhead(atom, Atom::ParaLeft))
                skipAhead = 1;
        }
        break;
    case Atom::TableItemRight:
        if (inTableHeader)
            xmlWriter().writeEndElement(); // </entry>
        else {
            xmlWriter().writeEndElement(); // </entry>
        }
        if (matchAhead(atom, Atom::ParaLeft))
            skipAhead = 1;
        break;
    case Atom::TableOfContents:
        {
            int numColumns = 1;
            const Node* node = relative;

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
        if (in_para) {
            xmlWriter().writeEndElement(); // </p>
            in_para = false;
        }
        xmlWriter().writeStartElement("p");
        writeGuidAttribute(Doc::canonicalTitle(atom->string()));
        xmlWriter().writeAttribute("outputclass","target");
        //xmlWriter().writeCharacters(protectEnc(atom->string()));
        xmlWriter().writeEndElement(); // </p>
        break;
    case Atom::UnhandledFormat:
        xmlWriter().writeStartElement("b");
        xmlWriter().writeAttribute("outputclass","error");
        xmlWriter().writeCharacters("<Missing DITAXML>");
        xmlWriter().writeEndElement(); // </b>
        break;
    case Atom::UnknownCommand:
        xmlWriter().writeStartElement("b");
        xmlWriter().writeAttribute("outputclass","error unknown-command");
        writeCharacters(protectEnc(atom->string()));
        xmlWriter().writeEndElement(); // </b>
        break;
    case Atom::QmlText:
    case Atom::EndQmlText:
        // don't do anything with these. They are just tags.
        break;
    default:
        //        unknownAtom(atom);
        break;
    }
    return skipAhead;
}

/*!
  Generate a <cxxClass> element (and all the stuff inside it)
  for the C++ class represented by \a innerNode. \a marker is
  for marking up the code. I don't know what that means exactly.
 */
void
DitaXmlGenerator::generateClassLikeNode(const InnerNode* inner, CodeMarker* marker)
{
    QList<Section>::ConstIterator s;

    QString title;
    QString rawTitle;
    QString fullTitle;
    if (inner->type() == Node::Namespace) {
        const NamespaceNode* nsn = const_cast<NamespaceNode*>(static_cast<const NamespaceNode*>(inner));
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Namespace";

        /*
          Note: Because the C++ specialization we are using
          has no <cxxNamespace> element, we are using the
          <cxxClass> element with an outputclass attribute
          set to "namespace" .
         */
        generateHeader(inner, fullTitle);
        generateBrief(inner, marker); // <shortdesc>

        // not included: <prolog>
    
        xmlWriter().writeStartElement(CXXCLASSDETAIL);
        xmlWriter().writeStartElement(CXXCLASSDEFINITION);
        writeLocation(nsn);
        xmlWriter().writeEndElement(); // <cxxClassDefinition>

        xmlWriter().writeStartElement("apiDesc");
        xmlWriter().writeAttribute("spectitle",title);
        Text brief = nsn->doc().briefText(); // zzz
        if (!brief.isEmpty()) {
            xmlWriter().writeStartElement("p");
            generateText(brief, nsn, marker);
            xmlWriter().writeEndElement(); // </p>
        }
        generateIncludes(nsn, marker);
        generateStatus(nsn, marker);
        generateThreadSafeness(nsn, marker);
        generateSince(nsn, marker);
        xmlWriter().writeEndElement(); // </apiDesc>

        bool needOtherSection = false;
        QList<Section> summarySections;
        summarySections = marker->sections(inner, CodeMarker::Summary, CodeMarker::Okay);
        s = summarySections.begin();
        while (s != summarySections.end()) {
            if (s->members.isEmpty() && s->reimpMembers.isEmpty()) {
                if (!s->inherited.isEmpty())
                    needOtherSection = true;
            }
            else {
                QString attr;
                if (!s->members.isEmpty()) {
                    xmlWriter().writeStartElement("section");
                    attr = cleanRef((*s).name).toLower() + " redundant";
                    xmlWriter().writeAttribute("outputclass",attr);
                    xmlWriter().writeStartElement("title");
                    xmlWriter().writeAttribute("outputclass","h2");
                    writeCharacters(protectEnc((*s).name));
                    xmlWriter().writeEndElement(); // </title>
                    generateSection(s->members, inner, marker, CodeMarker::Summary);
                    generateSectionInheritedList(*s, inner, marker);
                    xmlWriter().writeEndElement(); // </section>
                }
                if (!s->reimpMembers.isEmpty()) {
                    QString name = QString("Reimplemented ") + (*s).name;
                    attr = cleanRef(name).toLower() + " redundant";
                    xmlWriter().writeStartElement("section");
                    xmlWriter().writeAttribute("outputclass",attr);
                    xmlWriter().writeStartElement("title");
                    xmlWriter().writeAttribute("outputclass","h2");
                    writeCharacters(protectEnc(name));
                    xmlWriter().writeEndElement(); // </title>
                    generateSection(s->reimpMembers, inner, marker, CodeMarker::Summary);
                    generateSectionInheritedList(*s, inner, marker);
                    xmlWriter().writeEndElement(); // </section>
                }
            }
            ++s;
        }
        if (needOtherSection) {
            xmlWriter().writeStartElement("section");
            xmlWriter().writeAttribute("outputclass","additional-inherited-members redundant");
            xmlWriter().writeStartElement("title");
            xmlWriter().writeAttribute("outputclass","h3");
            xmlWriter().writeCharacters("Additional Inherited Members");
            xmlWriter().writeEndElement(); // </title>
            s = summarySections.begin();
            while (s != summarySections.end()) {
                if (s->members.isEmpty())
                    generateSectionInheritedList(*s, inner, marker);
                ++s;
            }
            xmlWriter().writeEndElement(); // </section>
        }
        
        writeDetailedDescription(nsn, marker, false, QString("Detailed Description"));
        xmlWriter().writeEndElement(); // </cxxClassDetail>

        // not included: <related-links>
        // not included: <cxxClassNested>

        QList<Section> detailSections;
        detailSections = marker->sections(inner, CodeMarker::Detailed, CodeMarker::Okay);
        s = detailSections.begin();
        while (s != detailSections.end()) {
            if ((*s).name == "Classes") {
                writeNestedClasses((*s),nsn);
                break;
            }
            ++s;
        }

        s = detailSections.begin();
        while (s != detailSections.end()) {
            if ((*s).name == "Function Documentation") {
                writeFunctions((*s),nsn,marker);
            }
            else if ((*s).name == "Type Documentation") {
                writeEnumerations((*s),marker);
                writeTypedefs((*s),marker);
            }
            else if ((*s).name == "Namespaces") {
                qDebug() << "Nested namespaces" << outFileName();
            }
            else if ((*s).name == "Macro Documentation") {
                writeMacros((*s),marker);
            }
            ++s;
        }

        generateLowStatusMembers(inner,marker,CodeMarker::Obsolete);
        generateLowStatusMembers(inner,marker,CodeMarker::Compat);
        xmlWriter().writeEndElement(); // </cxxClass>
    }
    else if (inner->type() == Node::Class) {
        const ClassNode* cn = const_cast<ClassNode*>(static_cast<const ClassNode*>(inner));
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Class Reference";

        generateHeader(inner, fullTitle);
        generateBrief(inner, marker); // <shortdesc>

        // not included: <prolog>
    
        xmlWriter().writeStartElement(CXXCLASSDETAIL);
        xmlWriter().writeStartElement(CXXCLASSDEFINITION);
        xmlWriter().writeStartElement(CXXCLASSACCESSSPECIFIER);
        xmlWriter().writeAttribute("value",inner->accessString());
        xmlWriter().writeEndElement(); // <cxxClassAccessSpecifier>
        if (cn->isAbstract()) {
            xmlWriter().writeStartElement(CXXCLASSABSTRACT);
            xmlWriter().writeAttribute("name","abstract");
            xmlWriter().writeAttribute("value","abstract");
            xmlWriter().writeEndElement(); // </cxxClassAbstract>
        }
        writeDerivations(cn, marker); // <cxxClassDerivations>

        // not included: <cxxClassTemplateParameters>

        writeLocation(cn);
        xmlWriter().writeEndElement(); // <cxxClassDefinition>

        xmlWriter().writeStartElement("apiDesc");
        xmlWriter().writeAttribute("spectitle",title);
        Text brief = cn->doc().briefText(); // zzz
        if (!brief.isEmpty()) {
            xmlWriter().writeStartElement("p");
            generateText(brief, cn, marker);
            xmlWriter().writeEndElement(); // </p>
        }
        generateIncludes(cn, marker);
        generateStatus(cn, marker);
        generateInherits(cn, marker);
        generateInheritedBy(cn, marker);
        generateThreadSafeness(cn, marker);
        generateSince(cn, marker);
        xmlWriter().writeEndElement(); // </apiDesc>

        bool needOtherSection = false;
        QList<Section> summarySections;
        summarySections = marker->sections(inner, CodeMarker::Summary, CodeMarker::Okay);
        s = summarySections.begin();
        while (s != summarySections.end()) {
            if (s->members.isEmpty() && s->reimpMembers.isEmpty()) {
                if (!s->inherited.isEmpty())
                    needOtherSection = true;
            }
            else {
                QString attr;
                if (!s->members.isEmpty()) {
                    xmlWriter().writeStartElement("section");
                    attr = cleanRef((*s).name).toLower() + " redundant";
                    xmlWriter().writeAttribute("outputclass",attr);
                    xmlWriter().writeStartElement("title");
                    xmlWriter().writeAttribute("outputclass","h2");
                    writeCharacters(protectEnc((*s).name));
                    xmlWriter().writeEndElement(); // </title>
                    generateSection(s->members, inner, marker, CodeMarker::Summary);
                    generateSectionInheritedList(*s, inner, marker);
                    xmlWriter().writeEndElement(); // </section>
                }
                if (!s->reimpMembers.isEmpty()) {
                    QString name = QString("Reimplemented ") + (*s).name;
                    attr = cleanRef(name).toLower() + " redundant";
                    xmlWriter().writeStartElement("section");
                    xmlWriter().writeAttribute("outputclass",attr);
                    xmlWriter().writeStartElement("title");
                    xmlWriter().writeAttribute("outputclass","h2");
                    writeCharacters(protectEnc(name));
                    xmlWriter().writeEndElement(); // </title>
                    generateSection(s->reimpMembers, inner, marker, CodeMarker::Summary);
                    generateSectionInheritedList(*s, inner, marker);
                    xmlWriter().writeEndElement(); // </section>
                }
            }
            ++s;
        }
        if (needOtherSection) {
            xmlWriter().writeStartElement("section");
            xmlWriter().writeAttribute("outputclass","additional-inherited-members redundant");
            xmlWriter().writeStartElement("title");
            xmlWriter().writeAttribute("outputclass","h3");
            xmlWriter().writeCharacters("Additional Inherited Members");
            xmlWriter().writeEndElement(); // </title>
            s = summarySections.begin();
            while (s != summarySections.end()) {
                if (s->members.isEmpty())
                    generateSectionInheritedList(*s, inner, marker);
                ++s;
            }
            xmlWriter().writeEndElement(); // </section>
        }
        
        writeDetailedDescription(cn, marker, false, QString("Detailed Description"));

        // not included: <example> or <apiImpl>

        xmlWriter().writeEndElement(); // </cxxClassDetail>

        // not included: <related-links>
        // not included: <cxxClassNested>

        QList<Section> detailSections;
        detailSections = marker->sections(inner, CodeMarker::Detailed, CodeMarker::Okay);
        s = detailSections.begin();
        while (s != detailSections.end()) {
            if ((*s).name == "Member Function Documentation") {
                writeFunctions((*s),cn,marker);
            }
            else if ((*s).name == "Member Type Documentation") {
                writeEnumerations((*s),marker);
                writeTypedefs((*s),marker);
            }
            else if ((*s).name == "Member Variable Documentation") {
                writeDataMembers((*s),marker);
            }
            else if ((*s).name == "Property Documentation") {
                writeProperties((*s),marker);
            }
            else if ((*s).name == "Macro Documentation") {
                writeMacros((*s),marker);
            }
            ++s;
        }

        generateLowStatusMembers(inner,marker,CodeMarker::Obsolete);
        generateLowStatusMembers(inner,marker,CodeMarker::Compat);
        xmlWriter().writeEndElement(); // </cxxClass>
    }
    else if ((inner->type() == Node::Fake) && (inner->subType() == Node::HeaderFile)) {
        const FakeNode* fn = const_cast<FakeNode*>(static_cast<const FakeNode*>(inner));
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle;

        /*
          Note: Because the C++ specialization we are using
          has no <cxxHeaderFile> element, we are using the
          <cxxClass> element with an outputclass attribute
          set to "headerfile" .
         */
        generateHeader(inner, fullTitle);
        generateBrief(inner, marker); // <shortdesc>
        xmlWriter().writeStartElement(CXXCLASSDETAIL);
        xmlWriter().writeStartElement("apiDesc");
        xmlWriter().writeAttribute("spectitle",title);
        Text brief = fn->doc().briefText(); // zzz
        if (!brief.isEmpty()) {
            xmlWriter().writeStartElement("p");
            generateText(brief, fn, marker);
            xmlWriter().writeEndElement(); // </p>
        }
        generateIncludes(fn, marker);
        generateStatus(fn, marker);
        generateThreadSafeness(fn, marker);
        generateSince(fn, marker);
        xmlWriter().writeEndElement(); // </apiDesc>

        bool needOtherSection = false;
        QList<Section> summarySections;
        summarySections = marker->sections(inner, CodeMarker::Summary, CodeMarker::Okay);
        s = summarySections.begin();
        while (s != summarySections.end()) {
            if (s->members.isEmpty() && s->reimpMembers.isEmpty()) {
                if (!s->inherited.isEmpty())
                    needOtherSection = true;
            }
            else {
                QString attr;
                if (!s->members.isEmpty()) {
                    xmlWriter().writeStartElement("section");
                    attr = cleanRef((*s).name).toLower() + " redundant";
                    xmlWriter().writeAttribute("outputclass",attr);
                    xmlWriter().writeStartElement("title");
                    xmlWriter().writeAttribute("outputclass","h2");
                    writeCharacters(protectEnc((*s).name));
                    xmlWriter().writeEndElement(); // </title>
                    generateSection(s->members, inner, marker, CodeMarker::Summary);
                    generateSectionInheritedList(*s, inner, marker);
                    xmlWriter().writeEndElement(); // </section>
                }
                if (!s->reimpMembers.isEmpty()) {
                    QString name = QString("Reimplemented ") + (*s).name;
                    attr = cleanRef(name).toLower() + " redundant";
                    xmlWriter().writeStartElement("section");
                    xmlWriter().writeAttribute("outputclass",attr);
                    xmlWriter().writeStartElement("title");
                    xmlWriter().writeAttribute("outputclass","h2");
                    writeCharacters(protectEnc(name));
                    xmlWriter().writeEndElement(); // </title>
                    generateSection(s->reimpMembers, inner, marker, CodeMarker::Summary);
                    generateSectionInheritedList(*s, inner, marker);
                    xmlWriter().writeEndElement(); // </section>
                }
            }
            ++s;
        }
        if (needOtherSection) {
            xmlWriter().writeStartElement("section");
            xmlWriter().writeAttribute("outputclass","additional-inherited-members redundant");
            xmlWriter().writeStartElement("title");
            xmlWriter().writeAttribute("outputclass","h3");
            xmlWriter().writeCharacters("Additional Inherited Members");
            xmlWriter().writeEndElement(); // </title>
            s = summarySections.begin();
            while (s != summarySections.end()) {
                if (s->members.isEmpty())
                    generateSectionInheritedList(*s, inner, marker);
                ++s;
            }
            xmlWriter().writeEndElement(); // </section>
        }
        
        writeDetailedDescription(fn, marker, false, QString("Detailed Description"));
        xmlWriter().writeEndElement(); // </cxxClassDetail>

        // not included: <related-links>
        // not included: <cxxClassNested>

        QList<Section> detailSections;
        detailSections = marker->sections(inner, CodeMarker::Detailed, CodeMarker::Okay);
        s = detailSections.begin();
        while (s != detailSections.end()) {
            if ((*s).name == "Classes") {
                writeNestedClasses((*s),fn);
                break;
            }
            ++s;
        }

        s = detailSections.begin();
        while (s != detailSections.end()) {
            if ((*s).name == "Function Documentation") {
                writeFunctions((*s),fn,marker);
            }
            else if ((*s).name == "Type Documentation") {
                writeEnumerations((*s),marker);
                writeTypedefs((*s),marker);
            }
            else if ((*s).name == "Namespaces") {
                qDebug() << "Nested namespaces" << outFileName();
            }
            else if ((*s).name == "Macro Documentation") {
                writeMacros((*s),marker);
            }
            ++s;
        }
        generateLowStatusMembers(inner,marker,CodeMarker::Obsolete);
        generateLowStatusMembers(inner,marker,CodeMarker::Compat);
        xmlWriter().writeEndElement(); // </cxxClass>
    }
    else if ((inner->type() == Node::Fake) && (inner->subType() == Node::QmlClass)) {
        const QmlClassNode* qcn = const_cast<QmlClassNode*>(static_cast<const QmlClassNode*>(inner));
        const ClassNode* cn = qcn->classNode();
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Element Reference";
        //QString fullTitle = fake->fullTitle();
        //QString htmlTitle = fullTitle;

        generateHeader(inner, fullTitle);
        generateBrief(inner, marker); // <shortdesc>

        // not included: <prolog>
    
        xmlWriter().writeStartElement(CXXCLASSDETAIL);
        xmlWriter().writeStartElement("apiDesc");
        xmlWriter().writeAttribute("spectitle",title);
        Text brief = qcn->doc().briefText(); // zzz
        if (!brief.isEmpty()) {
            xmlWriter().writeStartElement("p");
            generateText(brief, qcn, marker);
            xmlWriter().writeEndElement(); // </p>
        }
        generateQmlInstantiates(qcn, marker);
        generateQmlInherits(qcn, marker);
        generateQmlInheritedBy(qcn, marker);
        generateSince(qcn, marker);
        xmlWriter().writeEndElement(); // </apiDesc>

        QList<Section> summarySections;
        summarySections = marker->qmlSections(qcn,CodeMarker::Summary,0);
        s = summarySections.begin();
        while (s != summarySections.end()) {
            QString attr;
            if (!s->members.isEmpty()) {
                xmlWriter().writeStartElement("section");
                attr = cleanRef((*s).name).toLower() + " redundant";
                xmlWriter().writeAttribute("outputclass",attr);
                xmlWriter().writeStartElement("title");
                xmlWriter().writeAttribute("outputclass","h2");
                writeCharacters(protectEnc((*s).name));
                xmlWriter().writeEndElement(); // </title>
                generateQmlSummary(*s,qcn,marker);
                //generateSection(s->members, inner, marker, CodeMarker::Summary);
                //generateSectionInheritedList(*s, inner, marker);
                xmlWriter().writeEndElement(); // </section>
            }
            ++s;
        }
        
        writeDetailedDescription(qcn, marker, false, QString("Detailed Description"));
        if (cn)
            generateQmlText(cn->doc().body(), cn, marker, qcn->name());

        QList<Section> detailSections;
        detailSections = marker->qmlSections(qcn,CodeMarker::Detailed,0);
        s = detailSections.begin();
        while (s != detailSections.end()) {
            if (!s->members.isEmpty()) {
                QString attr;
                inSection = true;
                xmlWriter().writeStartElement("section");
                attr = cleanRef((*s).name).toLower();
                xmlWriter().writeAttribute("outputclass",attr);
                xmlWriter().writeStartElement("title");
                xmlWriter().writeAttribute("outputclass","h2");
                writeCharacters(protectEnc((*s).name));
                xmlWriter().writeEndElement(); // </title>
                NodeList::ConstIterator m = (*s).members.begin();
                while (m != (*s).members.end()) {
                    generateDetailedQmlMember(*m, qcn, marker);
                    ++m;
                }
                xmlWriter().writeEndElement(); // </section>
                inSection = false;
            }
            ++s;
        }

        xmlWriter().writeEndElement(); // </cxxClassDetail>        
        xmlWriter().writeEndElement(); // </cxxClass>
    }
}


/*!
  Write a list item for a \a link with the given \a text.
 */
void DitaXmlGenerator::writeXrefListItem(const QString& link, const QString& text)
{
    xmlWriter().writeStartElement("li");
    xmlWriter().writeStartElement("xref");
    xmlWriter().writeAttribute("href",link);
    writeCharacters(text);
    xmlWriter().writeEndElement(); // </xref>
    xmlWriter().writeEndElement(); // </li>
}

/*!
  Generate the html page for a qdoc file that doesn't map
  to an underlying c++ file.
 */
void DitaXmlGenerator::generateFakeNode(const FakeNode* fake, CodeMarker* marker)
{
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

    generateHeader(fake, fullTitle);
    generateBrief(fake, marker); // <shortdesc>
    xmlWriter().writeStartElement("body");
    if (fake->subType() == Node::Module) {
        generateStatus(fake, marker);
        if (moduleNamespaceMap.contains(fake->name())) {
            xmlWriter().writeStartElement("section");
            xmlWriter().writeAttribute("outputclass","namespaces");
            xmlWriter().writeStartElement("title");
            xmlWriter().writeAttribute("outputclass","h2");
            xmlWriter().writeCharacters("Namespaces");
            xmlWriter().writeEndElement(); // </title>
            generateAnnotatedList(fake, marker, moduleNamespaceMap[fake->name()]);
            xmlWriter().writeEndElement(); // </section>
        }
        if (moduleClassMap.contains(fake->name())) {
            xmlWriter().writeStartElement("section");
            xmlWriter().writeAttribute("outputclass","classes");
            xmlWriter().writeStartElement("title");
            xmlWriter().writeAttribute("outputclass","h2");
            xmlWriter().writeCharacters("Classes");
            xmlWriter().writeEndElement(); // </title>
            generateAnnotatedList(fake, marker, moduleClassMap[fake->name()]);
            xmlWriter().writeEndElement(); // </section>
        }
    }

    if (fake->doc().isEmpty()) {
        if (fake->subType() == Node::File) {
            Text text;
            Quoter quoter;
            xmlWriter().writeStartElement("p");
            xmlWriter().writeAttribute("outputclass", "small-subtitle");
            text << fake->subTitle();
            generateText(text, fake, marker);
            xmlWriter().writeEndElement(); // </p>
            Doc::quoteFromFile(fake->doc().location(), quoter, fake->name());
            QString code = quoter.quoteTo(fake->location(), "", "");
            text.clear();
            text << Atom(Atom::Code, code);
            generateText(text, fake, marker);
        }
    }
    else {
        if (fake->subType() == Node::Module) {
            writeDetailedDescription(fake, marker, false, QString("Detailed Description"));
        }
        else
            writeDetailedDescription(fake, marker, false, QString());
        generateAlsoList(fake, marker);

        if (!fake->groupMembers().isEmpty()) {
            NodeMap groupMembersMap;
            foreach (const Node *node, fake->groupMembers()) {
                if (node->type() == Node::Class || node->type() == Node::Namespace)
                    groupMembersMap[node->name()] = node;
            }
            generateAnnotatedList(fake, marker, groupMembersMap);
        }
    }
    xmlWriter().writeEndElement(); // </body>
    writeRelatedLinks(fake, marker);
    xmlWriter().writeEndElement(); // </topic>
}

/*!
  This function writes a \e{<link>} element inside a
  \e{<related-links>} element.

  \sa writeRelatedLinks()
 */
void DitaXmlGenerator::writeLink(const Node* node,
                                 const QString& text,
                                 const QString& role)
{
    if (node) {
        QString link = fileName(node) + "#" + node->guid();
        xmlWriter().writeStartElement("link");
        xmlWriter().writeAttribute("href", link);
        xmlWriter().writeAttribute("role", role);
        xmlWriter().writeStartElement("linktext");
        writeCharacters(text);
        xmlWriter().writeEndElement(); // </linktext>
        xmlWriter().writeEndElement(); // </link>
    }
}

/*!
  This function writes a \e{<related-links>} element, which
  contains the \c{next}, \c{previous}, and \c{start}
  links for topic pages that have them. Note that the
  value of the \e role attribute is \c{parent} for the
  \c{start} link.
 */
void DitaXmlGenerator::writeRelatedLinks(const FakeNode* node, CodeMarker* marker)
{
    const Node* linkNode = 0;
    QPair<QString,QString> linkPair;
    if (node && !node->links().empty()) {
        xmlWriter().writeStartElement("related-links");
        if (node->links().contains(Node::PreviousLink)) {
            linkPair = node->links()[Node::PreviousLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            writeLink(linkNode, linkPair.second, "previous");
        }
        if (node->links().contains(Node::NextLink)) {
            linkPair = node->links()[Node::NextLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            writeLink(linkNode, linkPair.second, "next");
        }
        if (node->links().contains(Node::StartLink)) {
            linkPair = node->links()[Node::StartLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            writeLink(linkNode, linkPair.second, "parent");
        }
        xmlWriter().writeEndElement(); // </related-links>
    }
}

/*!
  Returns "xml" for this subclass of class Generator.
 */
QString DitaXmlGenerator::fileExtension(const Node * /* node */) const
{
    return "xml";
}

/*!
  Writes an XML file header to the current XML stream. This
  depends on which kind of DITA XML file is being generated,
  which is determined by the \a node type and subtype and the
  \a subpage flag. If the \subpage flag is true, a \c{<topic>}
  header is written, regardless of the type of \a node.
 */
void DitaXmlGenerator::generateHeader(const Node* node,
                                      const QString& name,
                                      bool subpage)
{
    if (!node)
        return;

    QString doctype;
    QString mainElement;
    QString nameElement;
    QString dtd;
    QString base;
    QString version;
    QString outputclass;

    if (node->type() == Node::Class) {
        mainElement = "cxxClass";
        nameElement = "apiName";
        dtd = "dtd/cxxClass.dtd";
        version = "0.6.0";
        doctype = "<!DOCTYPE " + mainElement +
            " PUBLIC \"-//NOKIA//DTD DITA C++ API Class Reference Type v" +
            version + "//EN\" \"" + dtd + "\">";
    }
    else if (node->type() == Node::Namespace) {
        mainElement = "cxxClass";
        nameElement = "apiName";
        dtd = "dtd/cxxClass.dtd";
        version = "0.6.0";
        doctype = "<!DOCTYPE " + mainElement +
            " PUBLIC \"-//NOKIA//DTD DITA C++ API Class Reference Type v" +
            version + "//EN\" \"" + dtd + "\">";
        outputclass = "namespace";
    }
    else if (node->type() == Node::Fake || subpage) {
        if (node->subType() == Node::HeaderFile) {
            mainElement = "cxxClass";
            nameElement = "apiName";
            dtd = "dtd/cxxClass.dtd";
            version = "0.6.0";
            doctype = "<!DOCTYPE " + mainElement +
                " PUBLIC \"-//NOKIA//DTD DITA C++ API Class Reference Type v" +
                version + "//EN\" \"" + dtd + "\">";
            outputclass = "headerfile";
        }
        else if (node->subType() == Node::QmlClass) {
            mainElement = "cxxClass";
            nameElement = "apiName";
            dtd = "dtd/cxxClass.dtd";
            version = "0.6.0";
            doctype = "<!DOCTYPE " + mainElement +
                " PUBLIC \"-//NOKIA//DTD DITA C++ API Class Reference Type v" +
                version + "//EN\" \"" + dtd + "\">";
            outputclass = "QML-class";
        }
        else {
            mainElement = "topic";
            nameElement = "title";
            dtd = "dtd/topic.dtd";
            doctype = "<!DOCTYPE " + mainElement +
                " PUBLIC \"-//OASIS//DTD DITA Topic//EN\" \"" + dtd + "\">";
            switch (node->subType()) {
            case Node::Page:
                outputclass = "page";
                break;
            case Node::Group:
                outputclass = "group";
                break;
            case Node::Example:
                outputclass = "example";
                break;
            case Node::File:
                outputclass = "file";
                break;
            case Node::Image:  // not used
                outputclass = "image";
                break;
            case Node::Module:
                outputclass = "module";
                break;
            case Node::ExternalPage: // not used
                outputclass = "externalpage";
                break;
            default:
                outputclass = "page";
            }
        }
    }

    xmlWriter().writeDTD(doctype);
    xmlWriter().writeComment(node->doc().location().fileName());
    xmlWriter().writeStartElement(mainElement);
    xmlWriter().writeAttribute("id",node->guid());
    if (!outputclass.isEmpty())
        xmlWriter().writeAttribute("outputclass",outputclass);
    xmlWriter().writeStartElement(nameElement); // <title> or <apiName>
    writeCharacters(name);
    xmlWriter().writeEndElement(); // </title> or </apiName> 
}

/*!
  Outputs the \e brief command as a <shortdesc> element.
 */
void DitaXmlGenerator::generateBrief(const Node* node, CodeMarker* marker)
{
    Text brief = node->doc().briefText(true); // zzz
    if (!brief.isEmpty()) {
        generateText(brief, node, marker);
    }
}

/*!
  Writes the \c {#include ...} required to include the class
  or namespace in a compilation.
 */
void DitaXmlGenerator::generateIncludes(const InnerNode* inner, CodeMarker* marker)
{
    if (!inner->includes().isEmpty()) {
        xmlWriter().writeStartElement("codeblock");
        writeText(marker->markedUpIncludes(inner->includes()), marker, inner);
        xmlWriter().writeEndElement(); // </codeblock>
    }
}

/*!
  zzz
  Generates a table of contents beginning at \a node.
  Currently just returns without writing anything.
 */
void DitaXmlGenerator::generateTableOfContents(const Node* node,
                                               CodeMarker* marker,
                                               Doc::SectioningUnit sectioningUnit,
                                               int numColumns,
                                               const Node* relative)

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
  zzz
  Revised for the new doc format.
  Generates a table of contents beginning at \a node.
 */
void DitaXmlGenerator::generateTableOfContents(const Node* node,
                                               CodeMarker* marker,
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

void DitaXmlGenerator::generateLowStatusMembers(const InnerNode* inner,
                                                CodeMarker* marker,
                                                CodeMarker::Status status)
{
    QString attribute;
    if (status == CodeMarker::Compat)
        attribute = "Qt3-support";
    else if (status == CodeMarker::Obsolete)
        attribute = "obsolete";
    else
        return;

    QList<Section> sections = marker->sections(inner, CodeMarker::Detailed, status);
    QMutableListIterator<Section> j(sections);
    while (j.hasNext()) {
        if (j.next().members.size() == 0)
            j.remove();
    }
    if (sections.isEmpty())
        return;

    QList<Section>::ConstIterator s = sections.begin();
    while (s != sections.end()) {
        if ((*s).name == "Member Function Documentation") {
            writeFunctions((*s),inner,marker,attribute);
        }
        else if ((*s).name == "Member Type Documentation") {
            writeEnumerations((*s),marker,attribute);
            writeTypedefs((*s),marker,attribute);
        }
        else if ((*s).name == "Member Variable Documentation") {
            writeDataMembers((*s),marker,attribute);
        }
        else if ((*s).name == "Property Documentation") {
            writeProperties((*s),marker,attribute);
        }
        else if ((*s).name == "Macro Documentation") {
            writeMacros((*s),marker,attribute);
        }
        ++s;
    }
}

/*!
  Write the XML for the class hierarchy to the current XML stream.
 */
void DitaXmlGenerator::generateClassHierarchy(const Node* relative,
                                              CodeMarker* marker,
                                              const QMap<QString,const Node*>& classMap)
{
    if (classMap.isEmpty())
        return;

    NodeMap topLevel;
    NodeMap::ConstIterator c = classMap.begin();
    while (c != classMap.end()) {
        const ClassNode* classe = static_cast<const ClassNode*>(*c);
        if (classe->baseClasses().isEmpty())
            topLevel.insert(classe->name(), classe);
        ++c;
    }

    QStack<NodeMap > stack;
    stack.push(topLevel);

    xmlWriter().writeStartElement("ul");
    while (!stack.isEmpty()) {
        if (stack.top().isEmpty()) {
            stack.pop();
            xmlWriter().writeEndElement(); // </ul>
            if (!stack.isEmpty())
                xmlWriter().writeEndElement(); // </li>
        }
        else {
            const ClassNode *child =
                static_cast<const ClassNode *>(*stack.top().begin());
            xmlWriter().writeStartElement("li");
            generateFullName(child, relative, marker);
            xmlWriter().writeEndElement(); // </li>
            stack.top().erase(stack.top().begin());

            NodeMap newTop;
            foreach (const RelatedClass &d, child->derivedClasses()) {
                if (d.access != Node::Private && !d.node->doc().isEmpty())
                    newTop.insert(d.node->name(), d.node);
            }
            if (!newTop.isEmpty()) {
                stack.push(newTop);
                xmlWriter().writeStartElement("li");
                xmlWriter().writeStartElement("ul");
            }
        }
    }
}

/*!
  Write XML for the contents of the \a nodeMap to the current
  XML stream. 
 */
void DitaXmlGenerator::generateAnnotatedList(const Node* relative,
                                             CodeMarker* marker,
                                             const NodeMap& nodeMap)
{
    if (nodeMap.isEmpty())
        return;
    xmlWriter().writeStartElement("table");
    xmlWriter().writeAttribute("outputclass","annotated");
    xmlWriter().writeStartElement("tgroup");
    xmlWriter().writeAttribute("cols","2");
    xmlWriter().writeStartElement("tbody");

    foreach (const QString& name, nodeMap.keys()) {
        const Node* node = nodeMap[name];

        if (node->status() == Node::Obsolete)
            continue;

        xmlWriter().writeStartElement("row");
        xmlWriter().writeStartElement("entry");
        xmlWriter().writeStartElement("p");
        generateFullName(node, relative, marker);
        xmlWriter().writeEndElement(); // </p>
        xmlWriter().writeEndElement(); // <entry>

        if (!(node->type() == Node::Fake)) {
            Text brief = node->doc().trimmedBriefText(name);
            if (!brief.isEmpty()) {
                xmlWriter().writeStartElement("entry");
                xmlWriter().writeStartElement("p");
                generateText(brief, node, marker);
                xmlWriter().writeEndElement(); // </p>
                xmlWriter().writeEndElement(); // <entry>
            }
        }
        else {
            xmlWriter().writeStartElement("entry");
            xmlWriter().writeStartElement("p");
            writeCharacters(protectEnc(node->doc().briefText().toString())); // zzz
            xmlWriter().writeEndElement(); // </p>
            xmlWriter().writeEndElement(); // <entry>
        }
        xmlWriter().writeEndElement(); // </row>
    }
    xmlWriter().writeEndElement(); // </tbody>
    xmlWriter().writeEndElement(); // </tgroup>
    xmlWriter().writeEndElement(); // </table>
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
void DitaXmlGenerator::generateCompactList(const Node* relative,
                                           CodeMarker* marker,
                                           const NodeMap& classMap,
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
    QMap<QChar,QString> cmap;

    /*
      Output the alphabet as a row of links.
     */
    if (includeAlphabet) {
        xmlWriter().writeStartElement("p");
        xmlWriter().writeAttribute("outputclass","alphabet");
        for (int i = 0; i < 26; i++) {
            QChar ch('a' + i);
            if (usedParagraphNames.contains(char('a' + i))) {
                xmlWriter().writeStartElement("xref");
                QString guid = lookupGuid(outFileName(),QString(ch));
                QString attr = outFileName() + QString("#%1").arg(guid);
                xmlWriter().writeAttribute("href", attr);
                xmlWriter().writeCharacters(QString(ch.toUpper()));
                xmlWriter().writeEndElement(); // </xref>
            }
        }
        xmlWriter().writeEndElement(); // </p>
    }

    /*
      Output a <p> element to contain all the <dl> elements.
     */
    xmlWriter().writeStartElement("p");
    xmlWriter().writeAttribute("outputclass","compactlist");

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
            if (i > 0) {
                xmlWriter().writeEndElement(); // </dlentry>
                xmlWriter().writeEndElement(); // </dl>
            }
            xmlWriter().writeStartElement("dl");
            xmlWriter().writeStartElement("dlentry");
            xmlWriter().writeStartElement("dt");
            if (includeAlphabet) {
                QChar c = paragraphName[curParNr][0].toLower();
                writeGuidAttribute(QString(c));
            }
            xmlWriter().writeAttribute("outputclass","sublist-header");
            xmlWriter().writeCharacters(paragraphName[curParNr]);
            xmlWriter().writeEndElement(); // </dt>
        }

        /*
          Output a <dd> for the current offset in the current paragraph.
         */
        xmlWriter().writeStartElement("dd");
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
            xmlWriter().writeStartElement("xref");
            xmlWriter().writeAttribute("href",linkForNode(it.value(), relative));
            
            QStringList pieces;
            if (it.value()->subType() == Node::QmlClass)
                pieces << it.value()->name();
            else
                pieces = fullName(it.value(), relative, marker).split("::");
            xmlWriter().writeCharacters(protectEnc(pieces.last()));
            xmlWriter().writeEndElement(); // </xref>
            if (pieces.size() > 1) {
                xmlWriter().writeCharacters(" (");
                generateFullName(it.value()->parent(),relative,marker);
                xmlWriter().writeCharacters(")");
            }
        }
        xmlWriter().writeEndElement(); // </dd>
        curParOffset++;
    }
    xmlWriter().writeEndElement(); // </dlentry>
    xmlWriter().writeEndElement(); // </dl>
    xmlWriter().writeEndElement(); // </p>
}

/*!
  Write XML for a function index to the current XML stream.
 */
void DitaXmlGenerator::generateFunctionIndex(const Node* relative,
                                             CodeMarker* marker)
{
    xmlWriter().writeStartElement("p");
    xmlWriter().writeAttribute("outputclass","alphabet");
    for (int i = 0; i < 26; i++) {
        QChar ch('a' + i);
        xmlWriter().writeStartElement("xref");
        QString guid = lookupGuid(outFileName(),QString(ch));
        QString attr = outFileName() + QString("#%1").arg(guid);
        xmlWriter().writeAttribute("href", attr);
        xmlWriter().writeCharacters(QString(ch.toUpper()));
        xmlWriter().writeEndElement(); // </xref>
        
    }
    xmlWriter().writeEndElement(); // </p>

    char nextLetter = 'a';
    char currentLetter;

    xmlWriter().writeStartElement("ul");
    QMap<QString, NodeMap >::ConstIterator f = funcIndex.begin();
    while (f != funcIndex.end()) {
        xmlWriter().writeStartElement("li");
        currentLetter = f.key()[0].unicode();
        while (islower(currentLetter) && currentLetter >= nextLetter) {
            xmlWriter().writeStartElement("p");
            writeGuidAttribute(QString(nextLetter));
            xmlWriter().writeAttribute("outputclass","target");
            xmlWriter().writeCharacters(QString(nextLetter));
            xmlWriter().writeEndElement(); // </p>
            nextLetter++;
        }
        xmlWriter().writeCharacters(protectEnc(f.key()));
        xmlWriter().writeCharacters(":");

        NodeMap::ConstIterator s = (*f).begin();
        while (s != (*f).end()) {
            generateFullName((*s)->parent(), relative, marker, *s);
            ++s;
        }
        xmlWriter().writeEndElement(); // </li>
        ++f;
    }
    xmlWriter().writeEndElement(); // </ul>
}

/*!
  Write the legalese texts as XML to the current XML stream.
 */
void DitaXmlGenerator::generateLegaleseList(const Node* relative,
                                            CodeMarker* marker)
{
    QMap<Text, const Node*>::ConstIterator it = legaleseTexts.begin();
    while (it != legaleseTexts.end()) {
        Text text = it.key();
        generateText(text, relative, marker);
        xmlWriter().writeStartElement("ul");
        do {
            xmlWriter().writeStartElement("li");
            generateFullName(it.value(), relative, marker);
            xmlWriter().writeEndElement(); // </li>
            ++it;
        } while (it != legaleseTexts.end() && it.key() == text);
        xmlWriter().writeEndElement(); //</ul>
    }
}

/*!
  Generate the text for the QML item described by \a node
  and write it to the current XML stream.
 */
void DitaXmlGenerator::generateQmlItem(const Node* node,
                                       const Node* relative,
                                       CodeMarker* marker,
                                       bool summary)
{ 
    QString marked = marker->markedUpQmlItem(node,summary);
    QRegExp tag("(<[^@>]*>)");
    if (marked.indexOf(tag) != -1) {
        QString tmp = protectEnc(marked.mid(tag.pos(1), tag.cap(1).length()));
        marked.replace(tag.pos(1), tag.cap(1).length(), tmp);
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
    writeText(marked, marker, relative);
}

/*!
  Writher the XML for the overview list to the current XML stream.
 */
void DitaXmlGenerator::generateOverviewList(const Node* relative, CodeMarker* /* marker */)
{
    QMap<const FakeNode*, QMap<QString, FakeNode*> > fakeNodeMap;
    QMap<QString, const FakeNode*> groupTitlesMap;
    QMap<QString, FakeNode*> uncategorizedNodeMap;
    QRegExp singleDigit("\\b([0-9])\\b");

    const NodeList children = myTree->root()->childNodes();
    foreach (Node* child, children) {
        if (child->type() == Node::Fake && child != relative) {
            FakeNode* fakeNode = static_cast<FakeNode*>(child);

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
                    foreach (Node* member, fakeNode->groupMembers()) {
                        if (member->type() != Node::Fake)
                            continue;
                        FakeNode* page = static_cast<FakeNode*>(member);
                        if (page) {
                            QString sortKey = page->fullTitle().toLower();
                            if (sortKey.startsWith("the "))
                                sortKey.remove(0, 4);
                            sortKey.replace(singleDigit, "0\\1");
                            fakeNodeMap[const_cast<const FakeNode*>(fakeNode)].insert(sortKey, page);
                            groupTitlesMap[fakeNode->fullTitle()] = const_cast<const FakeNode*>(fakeNode);
                        }
                    }
                }
                else if (!isGroupPage) {
                    // If we encounter a page that belongs to a group then
                    // we add that page to the list for that group.
                    const FakeNode* groupNode =
                        static_cast<const FakeNode*>(myTree->root()->findNode(group, Node::Fake));
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
        foreach (const QString& groupTitle, groupTitlesMap.keys()) {
            const FakeNode* groupNode = groupTitlesMap[groupTitle];
            xmlWriter().writeStartElement("p");
            xmlWriter().writeAttribute("outputclass","h3");
            xmlWriter().writeStartElement("xref");
            xmlWriter().writeAttribute("href",linkForNode(groupNode, relative));
            writeCharacters(protectEnc(groupNode->fullTitle()));
            xmlWriter().writeEndElement(); // </xref>
            xmlWriter().writeEndElement(); // </p>
            if (fakeNodeMap[groupNode].count() == 0)
                continue;

            xmlWriter().writeStartElement("ul");
            foreach (const FakeNode* fakeNode, fakeNodeMap[groupNode]) {
                QString title = fakeNode->fullTitle();
                if (title.startsWith("The "))
                    title.remove(0, 4);
                xmlWriter().writeStartElement("li");
                xmlWriter().writeStartElement("xref");
                xmlWriter().writeAttribute("href",linkForNode(fakeNode, relative));
                writeCharacters(protectEnc(title));
                xmlWriter().writeEndElement(); // </xref>
                xmlWriter().writeEndElement(); // </li>
            }
            xmlWriter().writeEndElement(); // </ul>
        }
    }

    if (!uncategorizedNodeMap.isEmpty()) {
        xmlWriter().writeStartElement("p");
        xmlWriter().writeAttribute("outputclass","h3");
        xmlWriter().writeCharacters("Miscellaneous");
        xmlWriter().writeEndElement(); // </p>
        xmlWriter().writeStartElement("ul");
        foreach (const FakeNode *fakeNode, uncategorizedNodeMap) {
            QString title = fakeNode->fullTitle();
            if (title.startsWith("The "))
                title.remove(0, 4);
            xmlWriter().writeStartElement("li");
            xmlWriter().writeStartElement("xref");
            xmlWriter().writeAttribute("href",linkForNode(fakeNode, relative));
            writeCharacters(protectEnc(title));
            xmlWriter().writeEndElement(); // </xref>
            xmlWriter().writeEndElement(); // </li>
        }
        xmlWriter().writeEndElement(); // </ul>
    }
}

/*!
  Write the XML for a standard section of a page, e.g.
  "Public Functions" or "Protected Slots." The section
  is written too the current XML stream as a table.
 */
void DitaXmlGenerator::generateSection(const NodeList& nl,
                                       const Node* relative,
                                       CodeMarker* marker,
                                       CodeMarker::SynopsisStyle style)
{
    if (!nl.isEmpty()) {
        xmlWriter().writeStartElement("ul");
        NodeList::ConstIterator m = nl.begin();
        while (m != nl.end()) {
            if ((*m)->access() != Node::Private) {
                xmlWriter().writeStartElement("li");
                QString marked = getMarkedUpSynopsis(*m, relative, marker, style);
                writeText(marked, marker, relative);
                xmlWriter().writeEndElement(); // </li>
            }
            ++m;
        }
        xmlWriter().writeEndElement(); // </ul>
    }
}

/*!
  Writes the "inherited from" list to the current XML stream.
 */
void DitaXmlGenerator::generateSectionInheritedList(const Section& section,
                                                    const Node* relative,
                                                    CodeMarker* marker)
{
    if (section.inherited.isEmpty())
        return;
    xmlWriter().writeStartElement("ul");
    QList<QPair<ClassNode*,int> >::ConstIterator p = section.inherited.begin();
    while (p != section.inherited.end()) {
        xmlWriter().writeStartElement("li");
        QString text;
        text.setNum((*p).second);
        text += " ";
        if ((*p).second == 1)
            text += section.singularMember;
        else
            text += section.pluralMember;
        text += " inherited from ";
        writeCharacters(text);
        xmlWriter().writeStartElement("xref");
        // zzz
        text = fileName((*p).first) + "#";
        text += DitaXmlGenerator::cleanRef(section.name.toLower());
        xmlWriter().writeAttribute("href",text);
        text = protectEnc(marker->plainFullName((*p).first, relative));
        writeCharacters(text);
        xmlWriter().writeEndElement(); // </xref>
        xmlWriter().writeEndElement(); // </li>
        ++p;
    }
    xmlWriter().writeEndElement(); // </ul>
}

/*!
  Get the synopsis from the \a node using the \a relative
  node if needed, and mark up the synopsis using \a marker.
  Use the style to decide which kind of sysnopsis to build,
  normally \c Summary or \c Detailed. Return the marked up
  string.
 */
QString DitaXmlGenerator::getMarkedUpSynopsis(const Node* node,
                                              const Node* relative,
                                              CodeMarker* marker,
                                              CodeMarker::SynopsisStyle style)
{
    QString marked = marker->markedUpSynopsis(node, relative, style);
    QRegExp tag("(<[^@>]*>)");
    if (marked.indexOf(tag) != -1) {
        QString tmp = protectEnc(marked.mid(tag.pos(1), tag.cap(1).length()));
        marked.replace(tag.pos(1), tag.cap(1).length(), tmp);
    }
    marked.replace(QRegExp("<@param>([a-z]+)_([1-9n])</@param>"),
                   "<i>\\1<sub>\\2</sub></i>");
#if 0    
    marked.replace("<@param>","<i>");
    marked.replace("</@param>","</i>");
#endif
    if (style == CodeMarker::Summary) {
        marked.replace("<@name>","");   // was "<b>"
        marked.replace("</@name>","");  // was "</b>"
    }

    if (style == CodeMarker::SeparateList) {
        QRegExp extraRegExp("<@extra>.*</@extra>");
        extraRegExp.setMinimal(true);
        marked.replace(extraRegExp,"");
    }
#if 0    
    else {
        marked.replace("<@extra>","<tt>");
        marked.replace("</@extra>","</tt>");
    }
#endif    

    if (style != CodeMarker::Detailed) {
        marked.replace("<@type>","");
        marked.replace("</@type>","");
    }
    return marked;
}

/*!
  Renamed from highlightedCode() in the html generator. Writes
  the \a markedCode to the current XML stream.
 */
void DitaXmlGenerator::writeText(const QString& markedCode,
                                 CodeMarker* marker,
                                 const Node* relative)
{
    QString src = markedCode;
    QString html;
    QStringRef arg;
    QStringRef par1;

    const QChar charLangle = '<';
    const QChar charAt = '@';

    /*
      First strip out all the extraneous markup. The table
      below contains the markup we want to keep. Everything
      else that begins with "<@" or "</@" is stripped out.
     */
    static const QString spanTags[] = {
        "<@link ",         "<@link ",
        "<@type>",         "<@type>",
        "<@headerfile>",   "<@headerfile>",
        "<@func>",         "<@func>",
        "<@param>",        "<@param>",
        "<@extra>",        "<@extra>",
        "</@link>",        "</@link>",
        "</@type>",        "</@type>",
        "</@headerfile>",  "</@headerfile>",
        "</@func>",        "</@func>",
        "</@param>",        "</@param>",
        "</@extra>",        "</@extra>"
    };
    for (int i = 0, n = src.size(); i < n;) {
        if (src.at(i) == charLangle) {
            bool handled = false;
            for (int k = 0; k != 12; ++k) {
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

    // replace all <@link> tags: "(<@link node=\"([^\"]+)\">).*(</@link>)"
    // replace all "(<@(type|headerfile|func)(?: +[^>]*)?>)(.*)(</@\\2>)" tags
    src = html;
    html = QString();
    static const QString markTags[] = {
        // 0       1         2           3       4        5
        "link", "type", "headerfile", "func", "param", "extra"
    };

    for (int i = 0, n = src.size(); i < n;) {
        if (src.at(i) == charLangle && src.at(i + 1) == charAt) {
            i += 2;
            bool handled = false;
            for (int k = 0; k != 6; ++k) {
                if (parseArg(src, markTags[k], &i, n, &arg, &par1)) {
                    const Node* n = 0;
                    if (k == 0) { // <@link>
                        if (!html.isEmpty()) {
                            writeCharacters(html);
                            html.clear();
                        }
                        n = CodeMarker::nodeForString(par1.toString());
                        QString link = linkForNode(n, relative);
                        addLink(link, arg);
                    }
                    else if (k == 4) { // <@param>
                        if (!html.isEmpty()) {
                            writeCharacters(html);
                            html.clear();
                        }
                        xmlWriter().writeStartElement("i");
                        writeCharacters(arg.toString());
                        xmlWriter().writeEndElement(); // </i>
                    }
                    else if (k == 5) { // <@extra>
                        if (!html.isEmpty()) {
                            writeCharacters(html);
                            html.clear();
                        }
                        xmlWriter().writeStartElement("tt");
                        writeCharacters(arg.toString());
                        xmlWriter().writeEndElement(); // </tt>
                    }
                    else {
                        if (!html.isEmpty()) {
                            writeCharacters(html);
                            html.clear();
                        }
                        par1 = QStringRef();
                        QString link;
                        n = marker->resolveTarget(arg.toString(), myTree, relative);
                        if (n && n->subType() == Node::QmlBasicType) {
                            if (relative && relative->subType() == Node::QmlClass) {
                                link = linkForNode(n,relative);
                                addLink(link, arg);
                            }
                            else {
                                link = arg.toString();
                            }
                        }
                        else {
                            // (zzz) Is this correct for all cases?
                            link = linkForNode(n,relative);
                            addLink(link, arg);
                        }
                    }
                    handled = true;
                    break;
                }
            }
        }
        else {
            html += src.at(i++);
        }
    }

    if (!html.isEmpty()) {
        writeCharacters(html);
    }
}

void DitaXmlGenerator::generateLink(const Atom* atom,
                                    const Node* /* relative */,
                                    CodeMarker* marker)
{
    static QRegExp camelCase("[A-Z][A-Z][a-z]|[a-z][A-Z0-9]|_");

    if (funcLeftParen.indexIn(atom->string()) != -1 && marker->recognizeLanguage("Cpp")) {
        // hack for C++: move () outside of link
        int k = funcLeftParen.pos(1);
        writeCharacters(protectEnc(atom->string().left(k)));
        if (link.isEmpty()) {
            if (showBrokenLinks)
                xmlWriter().writeEndElement(); // </i>
        }
        else
            xmlWriter().writeEndElement(); // </xref>
        inLink = false;
        writeCharacters(protectEnc(atom->string().mid(k)));
    }
    else if (marker->recognizeLanguage("Java")) {
	// hack for Java: remove () and use <tt> when appropriate
        bool func = atom->string().endsWith("()");
        bool tt = (func || atom->string().contains(camelCase));
        if (tt)
            xmlWriter().writeStartElement("tt");
        if (func)
            writeCharacters(protectEnc(atom->string().left(atom->string().length() - 2)));
        else
            writeCharacters(protectEnc(atom->string()));
        xmlWriter().writeEndElement(); // </tt>
    }
    else
        writeCharacters(protectEnc(atom->string()));
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
    }
    else if (u == '~') {
        clean += "dtor.";
    }
    else if (u == '_') {
        clean += "underscore.";
    }
    else {
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
        }
        else if (c.isSpace()) {
            clean += "-";
        }
        else if (u == '!') {
            clean += "-not";
        }
        else if (u == '&') {
            clean += "-and";
        }
        else if (u == '<') {
            clean += "-lt";
        }
        else if (u == '=') {
            clean += "-eq";
        }
        else if (u == '>') {
            clean += "-gt";
        }
        else if (u == '#') {
            clean += "#";
        }
        else {
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
        }
        else if (prevRef == ref)
            break;
#if 0        
        else
            qDebug() << "PREVREF:" << prevRef;
#endif        
        clean += "x";
    }
    return clean;
}

/*!
  Calls protect() with the \a string. Returns the result.
 */
QString DitaXmlGenerator::protectEnc(const QString& string)
{
    return protect(string, outputEncoding);
}

QString DitaXmlGenerator::protect(const QString& string, const QString& outputEncoding)
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
        }
        else if (ch == QLatin1Char('<')) {
            APPEND("&lt;");
        }
        else if (ch == QLatin1Char('>')) {
            APPEND("&gt;");
        }
        else if (ch == QLatin1Char('"')) {
            APPEND("&quot;");
        }
        else if ((outputEncoding == "ISO-8859-1" && ch.unicode() > 0x007F) ||
                 (ch == QLatin1Char('*') && i + 1 < n && string.at(i) == QLatin1Char('/')) ||
                 (ch == QLatin1Char('.') && i > 2 && string.at(i - 2) == QLatin1Char('.'))) {
            // we escape '*/' and the last dot in 'e.g.' and 'i.e.' for the Javadoc generator
            APPEND("&#x");
            xml += QString::number(ch.unicode(), 16);
            xml += QLatin1Char(';');
        }
        else {
            if (!xml.isEmpty())
                xml += ch;
        }
    }

    if (!xml.isEmpty())
        return xml;
    return string;

#undef APPEND
}

/*!
  Constructs a file name appropriate for the \a node
  and returns the file name.
 */
QString DitaXmlGenerator::fileBase(const Node* node) const
{
    QString result;
    result = PageGenerator::fileBase(node);
#if 0    
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
#endif    
    return result;
}

QString DitaXmlGenerator::refForNode(const Node* node)
{
    const FunctionNode* func;
    const TypedefNode* tdn;
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
        tdn = static_cast<const TypedefNode *>(node);
        if (tdn->associatedEnum()) {
            return refForNode(tdn->associatedEnum());
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
    case Node::Fake:
        if (node->subType() != Node::QmlPropertyGroup)
            break;
    case Node::QmlProperty:
    case Node::Property:
        ref = node->name() + "-prop";
        break;
    case Node::QmlSignal:
        ref = node->name() + "-signal";
        break;
    case Node::QmlMethod:
        ref = node->name() + "-method";
        break;
    case Node::Variable:
        ref = node->name() + "-var";
        break;
    case Node::Target:
        return protectEnc(node->name());
    }
    return registerRef(ref);
}

QString DitaXmlGenerator::guidForNode(const Node* node)
{
    switch (node->type()) {
    case Node::Namespace:
    case Node::Class:
    default:
        break;
    case Node::Enum:
        return node->guid();
    case Node::Typedef:
        {
            const TypedefNode* tdn = static_cast<const TypedefNode*>(node);
            if (tdn->associatedEnum()) 
                return guidForNode(tdn->associatedEnum());
        }
        return node->guid();
    case Node::Function:
        {
            const FunctionNode* fn = static_cast<const FunctionNode*>(node);
            if (fn->associatedProperty()) {
                return guidForNode(fn->associatedProperty());
            }
            else {
                QString ref = fn->name();
                if (fn->overloadNumber() != 1) {
                    ref += "-" + QString::number(fn->overloadNumber());
                    //qDebug() << "guidForNode() overloaded function:" << outFileName() << ref;
                }
            }
            return fn->guid();
        }
    case Node::Fake:
        if (node->subType() != Node::QmlPropertyGroup)
            break;
    case Node::QmlProperty:
    case Node::Property:
        return node->guid();
    case Node::QmlSignal:
        return node->guid();
    case Node::QmlMethod:
        return node->guid();
    case Node::Variable:
        return node->guid();
    case Node::Target:
        return node->guid();
    }
    return QString();
}

/*!
  Constructs a file name appropriate for the \a node and returns
  it. If the \a node is not a fake node, or if it is a fake node but
  it is neither an external page node nor an image node, call the
  PageGenerator::fileName() function.
 */
QString DitaXmlGenerator::fileName(const Node* node)
{
    if (node->type() == Node::Fake) {
        if (static_cast<const FakeNode*>(node)->subType() == Node::ExternalPage)
            return node->name();
        if (static_cast<const FakeNode*>(node)->subType() == Node::Image)
            return node->name();
    }
    return PageGenerator::fileName(node);
}

QString DitaXmlGenerator::linkForNode(const Node* node, const Node* relative)
{
    if (node == 0 || node == relative)
        return QString();
    if (!node->url().isEmpty())
        return node->url();
    if (fileBase(node).isEmpty())
        return QString();
    if (node->access() == Node::Private)
        return QString();

    QString fn = fileName(node);
    QString link = fn;

    if (!node->isInnerNode() || node->subType() == Node::QmlPropertyGroup) {
        QString guid = guidForNode(node);
        if (relative && fn == fileName(relative) && guid == guidForNode(relative)) {
            return QString();
        }
        link += "#";
        link += guid;
    }
    return link;
}

QString DitaXmlGenerator::refForAtom(Atom* atom, const Node* /* node */)
{
    if (atom->type() == Atom::SectionLeft)
        return Doc::canonicalTitle(Text::sectionHeading(atom).toString());
    if (atom->type() == Atom::Target)
        return Doc::canonicalTitle(atom->string());
    return QString();
}

void DitaXmlGenerator::generateFullName(const Node* apparentNode,
                                        const Node* relative,
                                        CodeMarker* marker,
                                        const Node* actualNode)
{
    if (actualNode == 0)
        actualNode = apparentNode;
    xmlWriter().writeStartElement("xref");
    QString href = linkForNode(actualNode, relative);
    xmlWriter().writeAttribute("href",href);
    writeCharacters(protectEnc(fullName(apparentNode, relative, marker)));
    xmlWriter().writeEndElement(); // </xref>
}

void DitaXmlGenerator::findAllClasses(const InnerNode* node)
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
            else if ((*c)->type() == Node::Fake &&
                     (*c)->subType() == Node::QmlClass &&
                     !(*c)->doc().isEmpty()) {
                QString qmlClassName = (*c)->name();
                qmlClasses.insert(qmlClassName,*c);
            }
            else if ((*c)->isInnerNode()) {
                findAllClasses(static_cast<InnerNode *>(*c));
            }
        }
        ++c;
    }
}

/*!
  For generating the "New Classes... in 4.x" section on the
  What's New in 4.x" page.
 */
void DitaXmlGenerator::findAllSince(const InnerNode* node)
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

void DitaXmlGenerator::findAllFunctions(const InnerNode* node)
{
    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
        if ((*c)->access() != Node::Private) {
            if ((*c)->isInnerNode() && (*c)->url().isEmpty()) {
                findAllFunctions(static_cast<const InnerNode*>(*c));
            }
            else if ((*c)->type() == Node::Function) {
                const FunctionNode* func = static_cast<const FunctionNode*>(*c);
                if ((func->status() > Node::Obsolete) &&
                    !func->isInternal() &&
                    (func->metaness() != FunctionNode::Ctor) &&
                    (func->metaness() != FunctionNode::Dtor)) {
                    funcIndex[(*c)->name()].insert(myTree->fullDocumentName((*c)->parent()), *c);
                }
            }
        }
        ++c;
    }
}

void DitaXmlGenerator::findAllLegaleseTexts(const InnerNode* node)
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

void DitaXmlGenerator::findAllNamespaces(const InnerNode* node)
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

int DitaXmlGenerator::hOffset(const Node* node)
{
    switch (node->type()) {
    case Node::Namespace:
    case Node::Class:
        return 2;
    case Node::Fake:
        return 1;
    case Node::Enum:
    case Node::Typedef:
    case Node::Function:
    case Node::Property:
    default:
        return 3;
    }
}

bool DitaXmlGenerator::isThreeColumnEnumValueTable(const Atom* atom)
{
    while (atom != 0 && !(atom->type() == Atom::ListRight && atom->string() == ATOM_LIST_VALUE)) {
        if (atom->type() == Atom::ListItemLeft && !matchAhead(atom, Atom::ListItemRight))
            return true;
        atom = atom->next();
    }
    return false;
}

const Node* DitaXmlGenerator::findNodeForTarget(const QString& target,
                                                const Node* relative,
                                                CodeMarker* marker,
                                                const Atom* atom)
{
    const Node* node = 0;

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

const QPair<QString,QString> DitaXmlGenerator::anchorForNode(const Node* node)
{
    QPair<QString,QString> anchorPair;
    anchorPair.first = PageGenerator::fileName(node);
    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode*>(node);
        anchorPair.second = fakeNode->title();
    }

    return anchorPair;
}

QString DitaXmlGenerator::getLink(const Atom* atom,
                                  const Node* relative,
                                  CodeMarker* marker,
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
        if (atom->string().contains('#'))
            path = atom->string().split('#');
        else
            path.append(atom->string());

        Atom* targetAtom = 0;
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
            if (!(*node)->url().isEmpty()) {
                return (*node)->url();
            }
            else {
                path.removeFirst();
            }
        }
        else {
            *node = relative;
        }

        if (*node && (*node)->status() == Node::Obsolete) {
            if (relative && (relative->parent() != *node) &&
                (relative->status() != Node::Obsolete)) {
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
            if (targetAtom) {
                if (link.isEmpty())
                    link = outFileName();
                QString guid = lookupGuid(link,refForAtom(targetAtom,*node));
                link += "#" + guid;
            }
#if 0
            else if (link.isEmpty() && *node) {
                link = outFileName() + "#" + (*node)->guid();
            }
#endif
            else if (!link.isEmpty() && *node && link.endsWith(".xml")) {
                link += "#" + (*node)->guid();
            }
        }
    }
    if (!link.isEmpty() && link[0] == '#') {
        link.prepend(outFileName());
        qDebug() << "LOCAL LINK:" << link;
    }
    return link;
}

void DitaXmlGenerator::generateIndex(const QString& fileBase,
                                     const QString& url,
                                     const QString& title)
{
    myTree->generateIndex(outputDir() + "/" + fileBase + ".index", url, title);
}

void DitaXmlGenerator::generateStatus(const Node* node, CodeMarker* marker)
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
                targetAtom = myTree->findTarget(oldName.replace("3",""),fakeNode);
            }

            if (targetAtom) {
                QString fn = fileName(fakeNode);
                QString guid = lookupGuid(fn,refForAtom(targetAtom,fakeNode));
                text << Atom(Atom::GuidLink, fn + "#" + guid);
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

void DitaXmlGenerator::beginLink(const QString& link)
{
    this->link = link;
    if (link.isEmpty())
        return;
    xmlWriter().writeStartElement("xref");
    xmlWriter().writeAttribute("href",link);
    inLink = true;
}

void DitaXmlGenerator::endLink()
{
    if (inLink) {
        if (link.isEmpty()) {
            if (showBrokenLinks)
                xmlWriter().writeEndElement(); // </i>
        }
        else {
            if (inObsoleteLink) {
                xmlWriter().writeStartElement("sup");
                xmlWriter().writeCharacters("(obsolete)");
                xmlWriter().writeEndElement(); // </sup>
            }
            xmlWriter().writeEndElement(); // </xref>
        }
    }
    inLink = false;
    inObsoleteLink = false;
}

/*!
  Generates the summary for the \a section. Only used for
  sections of QML element documentation.

  Currently handles only the QML property group.
 */
void DitaXmlGenerator::generateQmlSummary(const Section& section,
                                          const Node* relative,
                                          CodeMarker* marker)
{
    if (!section.members.isEmpty()) {
        xmlWriter().writeStartElement("ul");
        NodeList::ConstIterator m;
        m = section.members.begin();
        while (m != section.members.end()) {
            xmlWriter().writeStartElement("li");
            generateQmlItem(*m,relative,marker,true);
            xmlWriter().writeEndElement(); // </li>
            ++m;
        }
        xmlWriter().writeEndElement(); // </ul>
    }
}

/*!
  zzz
  Outputs the html detailed documentation for a section
  on a QML element reference page.
 */
void DitaXmlGenerator::generateDetailedQmlMember(const Node* node,
                                                 const InnerNode* relative,
                                                 CodeMarker* marker)
{
    QString marked;
    const QmlPropertyNode* qpn = 0;
    if (node->subType() == Node::QmlPropertyGroup) {
        const QmlPropGroupNode* qpgn = static_cast<const QmlPropGroupNode*>(node);
        NodeList::ConstIterator p = qpgn->childNodes().begin();
        xmlWriter().writeStartElement("ul");
        while (p != qpgn->childNodes().end()) {
            if ((*p)->type() == Node::QmlProperty) {
                qpn = static_cast<const QmlPropertyNode*>(*p);
                xmlWriter().writeStartElement("li");
                writeGuidAttribute((Node*)qpn);
                QString attr;
                if (!qpn->isWritable(myTree))
                    attr = "read-only";
                if (qpgn->isDefault()) {
                    if (!attr.isEmpty())
                        attr += " ";
                    attr += "default";
                }
                if (!attr.isEmpty())
                    xmlWriter().writeAttribute("outputclass",attr);
                generateQmlItem(qpn, relative, marker, false);
                xmlWriter().writeEndElement(); // </li>
            }
            ++p;
        }
        xmlWriter().writeEndElement(); // </ul>
    }
    else if (node->type() == Node::QmlSignal) {
        Node* n = const_cast<Node*>(node);
        xmlWriter().writeStartElement("ul");
        xmlWriter().writeStartElement("li");
        writeGuidAttribute(n);
        marked = getMarkedUpSynopsis(n, relative, marker, CodeMarker::Detailed);
        writeText(marked, marker, relative);
        xmlWriter().writeEndElement(); // </li>
        xmlWriter().writeEndElement(); // </ul>
    }
    else if (node->type() == Node::QmlMethod) {
        Node* n = const_cast<Node*>(node);
        xmlWriter().writeStartElement("ul");
        xmlWriter().writeStartElement("li");
        writeGuidAttribute(n);
        marked = getMarkedUpSynopsis(n, relative, marker, CodeMarker::Detailed);
        writeText(marked, marker, relative);
        xmlWriter().writeEndElement(); // </li>
        xmlWriter().writeEndElement(); // </ul>
    }
    generateStatus(node, marker);
    generateBody(node, marker);
    generateThreadSafeness(node, marker);
    generateSince(node, marker);
    generateAlsoList(node, marker);
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
                xmlWriter().writeStartElement("p");
                xmlWriter().writeAttribute("outputclass","inherits");
                Text text;
                text << "[Inherits ";
                text << Atom(Atom::LinkNode,CodeMarker::stringForNode(qcn));
                text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
                text << Atom(Atom::String, linkPair.second);
                text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
                text << "]";
                generateText(text, cn, marker);
                xmlWriter().writeEndElement(); // </p>
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
        xmlWriter().writeStartElement("p");
            xmlWriter().writeAttribute("outputclass","instantiates");
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
        xmlWriter().writeEndElement(); // </p>
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
            xmlWriter().writeStartElement("p");
            xmlWriter().writeAttribute("outputclass","instantiated-by");
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
            xmlWriter().writeEndElement(); // </p>
        }
    }
}

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

/*!
  Outputs the <cxxClassDerivations> element.
  \code
	<cxxClassDerivations>
		<cxxClassDerivation>
			...
		</cxxClassDerivation>
		...
	</cxxClassDerivations>
  \endcode

  The <cxxClassDerivation> element is:

  \code
	<cxxClassDerivation>
		<cxxClassDerivationAccessSpecifier value="public"/>
		<cxxClassBaseClass href="class_base">Base</cxxClassBaseClass>
	</cxxClassDerivation>
  \endcode
 */
void DitaXmlGenerator::writeDerivations(const ClassNode* cn, CodeMarker* marker)
{
    QList<RelatedClass>::ConstIterator r;
    int index;

    if (!cn->baseClasses().isEmpty()) {
        xmlWriter().writeStartElement(CXXCLASSDERIVATIONS);
        r = cn->baseClasses().begin();
        index = 0;
        while (r != cn->baseClasses().end()) {
            xmlWriter().writeStartElement(CXXCLASSDERIVATION);
            xmlWriter().writeStartElement(CXXCLASSDERIVATIONACCESSSPECIFIER);
            xmlWriter().writeAttribute("value",(*r).accessString());
            xmlWriter().writeEndElement(); // </cxxClassDerivationAccessSpecifier>

            // not included: <cxxClassDerivationVirtual>

            xmlWriter().writeStartElement(CXXCLASSBASECLASS);
            QString attr = fileName((*r).node) + "#" + (*r).node->guid();
            xmlWriter().writeAttribute("href",attr);
            writeCharacters(marker->plainFullName((*r).node));
            xmlWriter().writeEndElement(); // </cxxClassBaseClass>

            // not included: <ClassBaseStruct> or <cxxClassBaseUnion>

            xmlWriter().writeEndElement(); // </cxxClassDerivation>

            // not included: <cxxStructDerivation>

             ++r;
        }
        xmlWriter().writeEndElement(); // </cxxClassDerivations>
     }
}

/*!
  Writes a <cxxXXXAPIItemLocation> element, depending on the
  type of the node \a n, which can be a class, function, enum,
  typedef, or property.
 */
void DitaXmlGenerator::writeLocation(const Node* n)
{
    QString s1, s2, s3, s4, s5, s6;
    if (n->type() == Node::Class || n->type() == Node::Namespace) {
        s1 = CXXCLASSAPIITEMLOCATION;
        s2 = CXXCLASSDECLARATIONFILE;
        s3 = CXXCLASSDECLARATIONFILELINE;
    }
    else if (n->type() == Node::Function) {
        FunctionNode* fn = const_cast<FunctionNode*>(static_cast<const FunctionNode*>(n));
        if (fn->isMacro()) {
            s1 = CXXDEFINEAPIITEMLOCATION;
            s2 = CXXDEFINEDECLARATIONFILE;
            s3 = CXXDEFINEDECLARATIONFILELINE;
        }
        else {
            s1 = CXXFUNCTIONAPIITEMLOCATION;
            s2 = CXXFUNCTIONDECLARATIONFILE;
            s3 = CXXFUNCTIONDECLARATIONFILELINE;
        }
    }
    else if (n->type() == Node::Enum) {
        s1 = CXXENUMERATIONAPIITEMLOCATION;
        s2 = CXXENUMERATIONDECLARATIONFILE;
        s3 = CXXENUMERATIONDECLARATIONFILELINE;
        s4 = CXXENUMERATIONDEFINITIONFILE;
        s5 = CXXENUMERATIONDEFINITIONFILELINESTART;
        s6 = CXXENUMERATIONDEFINITIONFILELINEEND;
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
    xmlWriter().writeStartElement(s1);
    xmlWriter().writeStartElement(s2);
    xmlWriter().writeAttribute("name","filePath");
    xmlWriter().writeAttribute("value",n->location().filePath());
    xmlWriter().writeEndElement(); // </cxx<s2>DeclarationFile>
    xmlWriter().writeStartElement(s3);
    xmlWriter().writeAttribute("name","lineNumber");
    QString lineNr;
    xmlWriter().writeAttribute("value",lineNr.setNum(n->location().lineNo()));
    xmlWriter().writeEndElement(); // </cxx<s3>DeclarationFileLine>
    if (!s4.isEmpty()) { // zzz This stuff is temporary, I think.
        xmlWriter().writeStartElement(s4);
        xmlWriter().writeAttribute("name","filePath");
        xmlWriter().writeAttribute("value",n->location().filePath());
        xmlWriter().writeEndElement(); // </cxx<s4>DefinitionFile>
        xmlWriter().writeStartElement(s5);
        xmlWriter().writeAttribute("name","lineNumber");
        xmlWriter().writeAttribute("value",lineNr.setNum(n->location().lineNo()));
        xmlWriter().writeEndElement(); // </cxx<s5>DefinitionFileLineStart>
        xmlWriter().writeStartElement(s6);
        xmlWriter().writeAttribute("name","lineNumber");
        xmlWriter().writeAttribute("value",lineNr.setNum(n->location().lineNo()));
        xmlWriter().writeEndElement(); // </cxx<s6>DefinitionFileLineEnd>
    }

    // not included: <cxxXXXDefinitionFile>, <cxxXXXDefinitionFileLineStart>,
    //               and <cxxXXXDefinitionFileLineEnd>

    xmlWriter().writeEndElement(); // </cxx<s1>ApiItemLocation>
}

/*!
  Write the <cxxFunction> elements.
 */
void DitaXmlGenerator::writeFunctions(const Section& s, 
                                      const Node* n, 
                                      CodeMarker* marker,
                                      const QString& attribute)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Function) {
            FunctionNode* fn = const_cast<FunctionNode*>(static_cast<const FunctionNode*>(*m));
            xmlWriter().writeStartElement(CXXFUNCTION);
            xmlWriter().writeAttribute("id",fn->guid());
            if (!attribute.isEmpty())
                xmlWriter().writeAttribute("outputclass",attribute);
            xmlWriter().writeStartElement("apiName");
            if (fn->metaness() == FunctionNode::Signal)
                xmlWriter().writeAttribute("class","signal");
            else if (fn->metaness() == FunctionNode::Slot)
                xmlWriter().writeAttribute("class","slot");
            writeCharacters(fn->name());
            xmlWriter().writeEndElement(); // </apiName>
            generateBrief(fn,marker);

            // not included: <prolog>

            xmlWriter().writeStartElement(CXXFUNCTIONDETAIL);
            xmlWriter().writeStartElement(CXXFUNCTIONDEFINITION);
            xmlWriter().writeStartElement(CXXFUNCTIONACCESSSPECIFIER);
            xmlWriter().writeAttribute("value",fn->accessString());
            xmlWriter().writeEndElement(); // <cxxFunctionAccessSpecifier>

            // not included: <cxxFunctionStorageClassSpecifierExtern>

            if (fn->isStatic()) {
                xmlWriter().writeStartElement(CXXFUNCTIONSTORAGECLASSSPECIFIERSTATIC);
                xmlWriter().writeAttribute("name","static");
                xmlWriter().writeAttribute("value","static");
                xmlWriter().writeEndElement(); // <cxxFunctionStorageClassSpecifierStatic>
            }

            // not included: <cxxFunctionStorageClassSpecifierMutable>,

            if (fn->isConst()) {
                xmlWriter().writeStartElement(CXXFUNCTIONCONST);
                xmlWriter().writeAttribute("name","const");
                xmlWriter().writeAttribute("value","const");
                xmlWriter().writeEndElement(); // <cxxFunctionConst>
            }
            
            // not included: <cxxFunctionExplicit>
            //               <cxxFunctionInline

            if (fn->virtualness() != FunctionNode::NonVirtual) {
                xmlWriter().writeStartElement(CXXFUNCTIONVIRTUAL);
                xmlWriter().writeAttribute("name","virtual");
                xmlWriter().writeAttribute("value","virtual");
                xmlWriter().writeEndElement(); // <cxxFunctionVirtual>
                if (fn->virtualness() == FunctionNode::PureVirtual) {
                    xmlWriter().writeStartElement(CXXFUNCTIONPUREVIRTUAL);
                    xmlWriter().writeAttribute("name","pure virtual");
                    xmlWriter().writeAttribute("value","pure virtual");
                    xmlWriter().writeEndElement(); // <cxxFunctionPureVirtual>
                }
            }
            
            if (fn->name() == n->name()) {
                xmlWriter().writeStartElement(CXXFUNCTIONCONSTRUCTOR);
                xmlWriter().writeAttribute("name","constructor");
                xmlWriter().writeAttribute("value","constructor");
                xmlWriter().writeEndElement(); // <cxxFunctionConstructor>
            }
            else if (fn->name()[0] == QChar('~')) {
                xmlWriter().writeStartElement(CXXFUNCTIONDESTRUCTOR);
                xmlWriter().writeAttribute("name","destructor");
                xmlWriter().writeAttribute("value","destructor");
                xmlWriter().writeEndElement(); // <cxxFunctionDestructor>
            }
            else {
                xmlWriter().writeStartElement(CXXFUNCTIONDECLAREDTYPE);
                writeCharacters(fn->returnType());
                xmlWriter().writeEndElement(); // <cxxFunctionDeclaredType>
            }

            // not included: <cxxFunctionReturnType>

            QString fq = fullQualification(fn);
            if (!fq.isEmpty()) {
                xmlWriter().writeStartElement(CXXFUNCTIONSCOPEDNAME);
                writeCharacters(fq);
                xmlWriter().writeEndElement(); // <cxxFunctionScopedName>
            }
            xmlWriter().writeStartElement(CXXFUNCTIONPROTOTYPE);
            writeCharacters(fn->signature(true));
            xmlWriter().writeEndElement(); // <cxxFunctionPrototype>

            QString fnl = fn->signature(false);
            int idx = fnl.indexOf(' ');
            if (idx < 0)
                idx = 0;
            else
                ++idx;
            fnl = fn->parent()->name() + "::" + fnl.mid(idx);
            xmlWriter().writeStartElement(CXXFUNCTIONNAMELOOKUP);
            writeCharacters(fnl);
            xmlWriter().writeEndElement(); // <cxxFunctionNameLookup>

            if (!fn->isInternal() && fn->isReimp() && fn->reimplementedFrom() != 0) {
                FunctionNode* rfn = (FunctionNode*)fn->reimplementedFrom();
                if (rfn && !rfn->isInternal()) {
                    xmlWriter().writeStartElement(CXXFUNCTIONREIMPLEMENTED);
                    xmlWriter().writeAttribute("href",rfn->ditaXmlHref());
                    writeCharacters(marker->plainFullName(rfn));
                    xmlWriter().writeEndElement(); // </cxxFunctionReimplemented>
                }
            }
            writeParameters(fn);
            writeLocation(fn);
            xmlWriter().writeEndElement(); // <cxxFunctionDefinition>

            writeDetailedDescription(fn, marker, true, QString());
            // generateAlsoList(inner, marker);

            // not included: <example> or <apiImpl>

            xmlWriter().writeEndElement(); // </cxxFunctionDetail>
            xmlWriter().writeEndElement(); // </cxxFunction>

            if (fn->metaness() == FunctionNode::Ctor ||
                fn->metaness() == FunctionNode::Dtor ||
                fn->overloadNumber() != 1) {
            }
        }
        ++m;
    }
}

/*!
  This function writes the <cxxFunctionParameters> element.
 */
void DitaXmlGenerator::writeParameters(const FunctionNode* fn)
{
    const QList<Parameter>& parameters = fn->parameters();
    if (!parameters.isEmpty()) {
        xmlWriter().writeStartElement(CXXFUNCTIONPARAMETERS);
        QList<Parameter>::ConstIterator p = parameters.begin();
        while (p != parameters.end()) {
            xmlWriter().writeStartElement(CXXFUNCTIONPARAMETER);
            xmlWriter().writeStartElement(CXXFUNCTIONPARAMETERDECLAREDTYPE);
            writeCharacters((*p).leftType());
            if (!(*p).rightType().isEmpty())
                writeCharacters((*p).rightType());
            xmlWriter().writeEndElement(); // <cxxFunctionParameterDeclaredType>
            xmlWriter().writeStartElement(CXXFUNCTIONPARAMETERDECLARATIONNAME);
            writeCharacters((*p).name());
            xmlWriter().writeEndElement(); // <cxxFunctionParameterDeclarationName>

            // not included: <cxxFunctionParameterDefinitionName>

            if (!(*p).defaultValue().isEmpty()) {
                xmlWriter().writeStartElement(CXXFUNCTIONPARAMETERDEFAULTVALUE);
                writeCharacters((*p).defaultValue());
                xmlWriter().writeEndElement(); // <cxxFunctionParameterDefaultValue>
            }

            // not included: <apiDefNote>

            xmlWriter().writeEndElement(); // <cxxFunctionParameter>
            ++p;
        }
        xmlWriter().writeEndElement(); // <cxxFunctionParameters>
    }
}

/*!
  This function writes the enum types.
 */
void DitaXmlGenerator::writeEnumerations(const Section& s, 
                                         CodeMarker* marker,
                                         const QString& attribute)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Enum) {
            const EnumNode* en = static_cast<const EnumNode*>(*m);
            xmlWriter().writeStartElement(CXXENUMERATION);
            xmlWriter().writeAttribute("id",en->guid());
            if (!attribute.isEmpty())
                xmlWriter().writeAttribute("outputclass",attribute);
            xmlWriter().writeStartElement("apiName");
            writeCharacters(en->name());
            xmlWriter().writeEndElement(); // </apiName>
            generateBrief(en,marker);

            // not included <prolog>

            xmlWriter().writeStartElement(CXXENUMERATIONDETAIL);
            xmlWriter().writeStartElement(CXXENUMERATIONDEFINITION);
            xmlWriter().writeStartElement(CXXENUMERATIONACCESSSPECIFIER);
            xmlWriter().writeAttribute("value",en->accessString());
            xmlWriter().writeEndElement(); // <cxxEnumerationAccessSpecifier>

            QString fq = fullQualification(en);
            if (!fq.isEmpty()) {
                xmlWriter().writeStartElement(CXXENUMERATIONSCOPEDNAME);
                writeCharacters(fq);
                xmlWriter().writeEndElement(); // <cxxEnumerationScopedName>
            }
            const QList<EnumItem>& items = en->items();
            if (!items.isEmpty()) {
                xmlWriter().writeStartElement(CXXENUMERATIONPROTOTYPE);
                writeCharacters(en->name());
                xmlWriter().writeCharacters(" = { ");
                QList<EnumItem>::ConstIterator i = items.begin();
                while (i != items.end()) {
                    writeCharacters((*i).name());
                    if (!(*i).value().isEmpty()) {
                        xmlWriter().writeCharacters(" = ");
                        writeCharacters((*i).value());
                    }
                    ++i;
                    if (i != items.end())
                        xmlWriter().writeCharacters(", ");
                }
                xmlWriter().writeCharacters(" }");
                xmlWriter().writeEndElement(); // <cxxEnumerationPrototype>
            }

            xmlWriter().writeStartElement(CXXENUMERATIONNAMELOOKUP);
            writeCharacters(en->parent()->name() + "::" + en->name());
            xmlWriter().writeEndElement(); // <cxxEnumerationNameLookup>

            // not included: <cxxEnumerationReimplemented>

            if (!items.isEmpty()) {
                xmlWriter().writeStartElement(CXXENUMERATORS);
                QList<EnumItem>::ConstIterator i = items.begin();
                while (i != items.end()) {
                    xmlWriter().writeStartElement(CXXENUMERATOR);
                    xmlWriter().writeStartElement("apiName");
                    writeCharacters((*i).name());
                    xmlWriter().writeEndElement(); // </apiName>

                    QString fq = fullQualification(en->parent());
                    if (!fq.isEmpty()) {
                        xmlWriter().writeStartElement(CXXENUMERATORSCOPEDNAME);
                        writeCharacters(fq + "::" + (*i).name());
                        xmlWriter().writeEndElement(); // <cxxEnumeratorScopedName>
                    }
                    xmlWriter().writeStartElement(CXXENUMERATORPROTOTYPE);
                    writeCharacters((*i).name());
                    xmlWriter().writeEndElement(); // <cxxEnumeratorPrototype>
                    xmlWriter().writeStartElement(CXXENUMERATORNAMELOOKUP);
                    writeCharacters(en->parent()->name() + "::" + (*i).name());
                    xmlWriter().writeEndElement(); // <cxxEnumeratorNameLookup>

                    if (!(*i).value().isEmpty()) {
                        xmlWriter().writeStartElement(CXXENUMERATORINITIALISER);
                        xmlWriter().writeAttribute("value", (*i).value());
                        xmlWriter().writeEndElement(); // <cxxEnumeratorInitialiser>
                    }

                    // not included: <cxxEnumeratorAPIItemLocation>

                    if (!(*i).text().isEmpty()) {
                        xmlWriter().writeStartElement("apiDesc");
                        generateText((*i).text(), en, marker);
                        xmlWriter().writeEndElement(); // </apiDesc>
                    }
                    xmlWriter().writeEndElement(); // <cxxEnumerator>
                    ++i;
                }
                xmlWriter().writeEndElement(); // <cxxEnumerators>
            }
            
            writeLocation(en);
            xmlWriter().writeEndElement(); // <cxxEnumerationDefinition>

            writeDetailedDescription(en, marker, true, QString());

            // not included: <example> or <apiImpl>

            xmlWriter().writeEndElement(); // </cxxEnumerationDetail>

            // not included: <related-links>

            xmlWriter().writeEndElement(); // </cxxEnumeration>
        }
        ++m;
    }
}

/*!
  This function writes the output for the \typedef commands.
 */
void DitaXmlGenerator::writeTypedefs(const Section& s, 
                                     CodeMarker* marker,
                                     const QString& attribute)

{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Typedef) {
            const TypedefNode* tn = static_cast<const TypedefNode*>(*m);
            xmlWriter().writeStartElement(CXXTYPEDEF);
            xmlWriter().writeAttribute("id",tn->guid());
            if (!attribute.isEmpty())
                xmlWriter().writeAttribute("outputclass",attribute);
            xmlWriter().writeStartElement("apiName");
            writeCharacters(tn->name());
            xmlWriter().writeEndElement(); // </apiName>
            generateBrief(tn,marker);

            // not included: <prolog>

            xmlWriter().writeStartElement(CXXTYPEDEFDETAIL);
            xmlWriter().writeStartElement(CXXTYPEDEFDEFINITION);
            xmlWriter().writeStartElement(CXXTYPEDEFACCESSSPECIFIER);
            xmlWriter().writeAttribute("value",tn->accessString());
            xmlWriter().writeEndElement(); // <cxxTypedefAccessSpecifier>

            // not included: <cxxTypedefDeclaredType>

            QString fq = fullQualification(tn);
            if (!fq.isEmpty()) {
                xmlWriter().writeStartElement(CXXTYPEDEFSCOPEDNAME);
                writeCharacters(fq);
                xmlWriter().writeEndElement(); // <cxxTypedefScopedName>
            }

            // not included: <cxxTypedefPrototype>

            xmlWriter().writeStartElement(CXXTYPEDEFNAMELOOKUP);
            writeCharacters(tn->parent()->name() + "::" + tn->name());
            xmlWriter().writeEndElement(); // <cxxTypedefNameLookup>

            // not included: <cxxTypedefReimplemented>

            writeLocation(tn);
            xmlWriter().writeEndElement(); // <cxxTypedefDefinition>

            writeDetailedDescription(tn, marker, true, QString());

            // not included: <example> or <apiImpl>

            xmlWriter().writeEndElement(); // </cxxTypedefDetail>

            // not included: <related-links>

            xmlWriter().writeEndElement(); // </cxxTypedef>
        }
        ++m;
    }
}

/*!
  This function writes the output for the \property commands.
  This is the Q_PROPERTYs.
 */
void DitaXmlGenerator::writeProperties(const Section& s, 
                                       CodeMarker* marker,
                                       const QString& attribute)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Property) {
            const PropertyNode* pn = static_cast<const PropertyNode*>(*m);
            xmlWriter().writeStartElement(CXXVARIABLE);
            xmlWriter().writeAttribute("id",pn->guid());
            if (!attribute.isEmpty())
                xmlWriter().writeAttribute("outputclass",attribute);
            xmlWriter().writeStartElement("apiName");
            writeCharacters(pn->name());
            xmlWriter().writeEndElement(); // </apiName>
            generateBrief(pn,marker);

            // not included: <prolog>

            xmlWriter().writeStartElement(CXXVARIABLEDETAIL);
            xmlWriter().writeStartElement(CXXVARIABLEDEFINITION);
            xmlWriter().writeStartElement(CXXVARIABLEACCESSSPECIFIER);
            xmlWriter().writeAttribute("value",pn->accessString());
            xmlWriter().writeEndElement(); // <cxxVariableAccessSpecifier>

            // not included: <cxxVariableStorageClassSpecifierExtern>,
            //               <cxxVariableStorageClassSpecifierStatic>,
            //               <cxxVariableStorageClassSpecifierMutable>,
            //               <cxxVariableConst>, <cxxVariableVolatile>

            if (!pn->qualifiedDataType().isEmpty()) {
                xmlWriter().writeStartElement(CXXVARIABLEDECLAREDTYPE);
                writeCharacters(pn->qualifiedDataType());
                xmlWriter().writeEndElement(); // <cxxVariableDeclaredType>
            }
            QString fq = fullQualification(pn);
            if (!fq.isEmpty()) {
                xmlWriter().writeStartElement(CXXVARIABLESCOPEDNAME);
                writeCharacters(fq);
                xmlWriter().writeEndElement(); // <cxxVariableScopedName>
            }
            
            xmlWriter().writeStartElement(CXXVARIABLEPROTOTYPE);
            xmlWriter().writeCharacters("Q_PROPERTY(");
            writeCharacters(pn->qualifiedDataType());
            xmlWriter().writeCharacters(" ");
            writeCharacters(pn->name());
            writePropertyParameter("READ",pn->getters());
            writePropertyParameter("WRITE",pn->setters());
            writePropertyParameter("RESET",pn->resetters());
            writePropertyParameter("NOTIFY",pn->notifiers());
            if (pn->isDesignable() != pn->designableDefault()) {
                xmlWriter().writeCharacters(" DESIGNABLE ");
                if (!pn->runtimeDesignabilityFunction().isEmpty())
                    writeCharacters(pn->runtimeDesignabilityFunction());
                else
                    xmlWriter().writeCharacters(pn->isDesignable() ? "true" : "false");
            }
            if (pn->isScriptable() != pn->scriptableDefault()) {
                xmlWriter().writeCharacters(" SCRIPTABLE ");
                if (!pn->runtimeScriptabilityFunction().isEmpty())
                    writeCharacters(pn->runtimeScriptabilityFunction());
                else
                    xmlWriter().writeCharacters(pn->isScriptable() ? "true" : "false");
            }
            if (pn->isWritable() != pn->writableDefault()) {
                xmlWriter().writeCharacters(" STORED ");
                xmlWriter().writeCharacters(pn->isStored() ? "true" : "false");
            }
            if (pn->isUser() != pn->userDefault()) {
                xmlWriter().writeCharacters(" USER ");
                xmlWriter().writeCharacters(pn->isUser() ? "true" : "false");
            }
            if (pn->isConstant())
                xmlWriter().writeCharacters(" CONSTANT");
            if (pn->isFinal())
                xmlWriter().writeCharacters(" FINAL");
            xmlWriter().writeCharacters(")");
            xmlWriter().writeEndElement(); // <cxxVariablePrototype>

            xmlWriter().writeStartElement(CXXVARIABLENAMELOOKUP);
            writeCharacters(pn->parent()->name() + "::" + pn->name());
            xmlWriter().writeEndElement(); // <cxxVariableNameLookup>

            if (pn->overriddenFrom() != 0) {
                PropertyNode* opn = (PropertyNode*)pn->overriddenFrom();
                xmlWriter().writeStartElement(CXXVARIABLEREIMPLEMENTED);
                xmlWriter().writeAttribute("href",opn->ditaXmlHref());
                writeCharacters(marker->plainFullName(opn));
                xmlWriter().writeEndElement(); // </cxxVariableReimplemented>
            }

            writeLocation(pn);
            xmlWriter().writeEndElement(); // <cxxVariableDefinition>

            writeDetailedDescription(pn, marker, true, QString());
            
            // not included: <example> or <apiImpl>

            xmlWriter().writeEndElement(); // </cxxVariableDetail>

            // not included: <related-links>

            xmlWriter().writeEndElement(); // </cxxVariable>
        }
        ++m;
    }
}

/*!
  This function outputs the nodes resulting from \variable commands.
 */
void DitaXmlGenerator::writeDataMembers(const Section& s, 
                                        CodeMarker* marker,
                                        const QString& attribute)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Variable) {
            const VariableNode* vn = static_cast<const VariableNode*>(*m);
            xmlWriter().writeStartElement(CXXVARIABLE);
            xmlWriter().writeAttribute("id",vn->guid());
            if (!attribute.isEmpty())
                xmlWriter().writeAttribute("outputclass",attribute);
            xmlWriter().writeStartElement("apiName");
            writeCharacters(vn->name());
            xmlWriter().writeEndElement(); // </apiName>
            generateBrief(vn,marker);

            // not included: <prolog>

            xmlWriter().writeStartElement(CXXVARIABLEDETAIL);
            xmlWriter().writeStartElement(CXXVARIABLEDEFINITION);
            xmlWriter().writeStartElement(CXXVARIABLEACCESSSPECIFIER);
            xmlWriter().writeAttribute("value",vn->accessString());
            xmlWriter().writeEndElement(); // <cxxVariableAccessSpecifier>

            // not included: <cxxVAriableStorageClassSpecifierExtern>

            if (vn->isStatic()) {
                xmlWriter().writeStartElement(CXXVARIABLESTORAGECLASSSPECIFIERSTATIC);
                xmlWriter().writeAttribute("name","static");
                xmlWriter().writeAttribute("value","static");
                xmlWriter().writeEndElement(); // <cxxVariableStorageClassSpecifierStatic>
            }

            // not included: <cxxVAriableStorageClassSpecifierMutable>,
            //               <cxxVariableConst>, <cxxVariableVolatile>

            xmlWriter().writeStartElement(CXXVARIABLEDECLAREDTYPE);
            writeCharacters(vn->leftType());
            if (!vn->rightType().isEmpty())
                writeCharacters(vn->rightType());
            xmlWriter().writeEndElement(); // <cxxVariableDeclaredType>

            QString fq = fullQualification(vn);
            if (!fq.isEmpty()) {
                xmlWriter().writeStartElement(CXXVARIABLESCOPEDNAME);
                writeCharacters(fq);
                xmlWriter().writeEndElement(); // <cxxVariableScopedName>
            }
            
            xmlWriter().writeStartElement(CXXVARIABLEPROTOTYPE);
            writeCharacters(vn->leftType() + " ");
            //writeCharacters(vn->parent()->name() + "::" + vn->name()); 
            writeCharacters(vn->name()); 
            if (!vn->rightType().isEmpty())
                writeCharacters(vn->rightType());
            xmlWriter().writeEndElement(); // <cxxVariablePrototype>

            xmlWriter().writeStartElement(CXXVARIABLENAMELOOKUP);
            writeCharacters(vn->parent()->name() + "::" + vn->name());
            xmlWriter().writeEndElement(); // <cxxVariableNameLookup>

            // not included: <cxxVariableReimplemented>

            writeLocation(vn);
            xmlWriter().writeEndElement(); // <cxxVariableDefinition>

            writeDetailedDescription(vn, marker, true, QString());

            // not included: <example> or <apiImpl>

            xmlWriter().writeEndElement(); // </cxxVariableDetail>

            // not included: <related-links>

            xmlWriter().writeEndElement(); // </cxxVariable>
        }
        ++m;
    }
}

/*!
  This function writes a \macro as a <cxxDefine>.
 */
void DitaXmlGenerator::writeMacros(const Section& s, 
                                   CodeMarker* marker,
                                   const QString& attribute)
{
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Function) {
            const FunctionNode* fn = static_cast<const FunctionNode*>(*m);
            if (fn->isMacro()) {
                xmlWriter().writeStartElement(CXXDEFINE);
                xmlWriter().writeAttribute("id",fn->guid());
                if (!attribute.isEmpty())
                    xmlWriter().writeAttribute("outputclass",attribute);
                xmlWriter().writeStartElement("apiName");
                writeCharacters(fn->name());
                xmlWriter().writeEndElement(); // </apiName>
                generateBrief(fn,marker);

                // not included: <prolog>

                xmlWriter().writeStartElement(CXXDEFINEDETAIL);
                xmlWriter().writeStartElement(CXXDEFINEDEFINITION);
                xmlWriter().writeStartElement(CXXDEFINEACCESSSPECIFIER);
                xmlWriter().writeAttribute("value",fn->accessString());
                xmlWriter().writeEndElement(); // <cxxDefineAccessSpecifier>
            
                xmlWriter().writeStartElement(CXXDEFINEPROTOTYPE);
                xmlWriter().writeCharacters("#define ");
                writeCharacters(fn->name());
                if (fn->metaness() == FunctionNode::MacroWithParams) {
                    QStringList params = fn->parameterNames();
                    if (!params.isEmpty()) {
                        xmlWriter().writeCharacters("(");
                        for (int i = 0; i < params.size(); ++i) {
                            if (params[i].isEmpty())
                                xmlWriter().writeCharacters("...");
                            else
                                writeCharacters(params[i]);
                            if ((i+1) < params.size()) 
                                xmlWriter().writeCharacters(", ");
                        }
                        xmlWriter().writeCharacters(")");
                    }
                }
                xmlWriter().writeEndElement(); // <cxxDefinePrototype>

                xmlWriter().writeStartElement(CXXDEFINENAMELOOKUP);
                writeCharacters(fn->name());
                xmlWriter().writeEndElement(); // <cxxDefineNameLookup>

                if (fn->reimplementedFrom() != 0) {
                    FunctionNode* rfn = (FunctionNode*)fn->reimplementedFrom();
                    xmlWriter().writeStartElement(CXXDEFINEREIMPLEMENTED);
                    xmlWriter().writeAttribute("href",rfn->ditaXmlHref());
                    writeCharacters(marker->plainFullName(rfn));
                    xmlWriter().writeEndElement(); // </cxxDefineReimplemented>
                }

                if (fn->metaness() == FunctionNode::MacroWithParams) {
                    QStringList params = fn->parameterNames();
                    if (!params.isEmpty()) {
                        xmlWriter().writeStartElement(CXXDEFINEPARAMETERS);
                        for (int i = 0; i < params.size(); ++i) {
                            xmlWriter().writeStartElement(CXXDEFINEPARAMETER);
                            xmlWriter().writeStartElement(CXXDEFINEPARAMETERDECLARATIONNAME);
                            writeCharacters(params[i]);
                            xmlWriter().writeEndElement(); // <cxxDefineParameterDeclarationName>

                            // not included: <apiDefNote>

                            xmlWriter().writeEndElement(); // <cxxDefineParameter>
                        }
                        xmlWriter().writeEndElement(); // <cxxDefineParameters>
                    }
                }

                writeLocation(fn);
                xmlWriter().writeEndElement(); // <cxxDefineDefinition>

                writeDetailedDescription(fn, marker, true, QString());

                // not included: <example> or <apiImpl>

                xmlWriter().writeEndElement(); // </cxxDefineDetail>

                // not included: <related-links>

                xmlWriter().writeEndElement(); // </cxxDefine>
            }
        }
        ++m;
    }
}

/*!
  This function writes one parameter of a Q_PROPERTY macro.
  The property is identified by \a tag ("READ" "WRIE" etc),
  and it is found in the 'a nlist.
 */
void DitaXmlGenerator::writePropertyParameter(const QString& tag, const NodeList& nlist)
{
    NodeList::const_iterator n = nlist.begin();
    while (n != nlist.end()) {
        xmlWriter().writeCharacters(" ");
        writeCharacters(tag);
        xmlWriter().writeCharacters(" ");
        writeCharacters((*n)->name());
        ++n;
    }
}

/*!
  Calls beginSubPage() in the base class to open the file.
  Then creates a new XML stream writer using the IO device
  from opened file and pushes the XML writer onto a stackj. 
  Creates the file named \a fileName in the output directory.
  Attaches a QTextStream to the created file, which is written
  to all over the place using out(). Finally, it sets some
  parameters in the XML writer and calls writeStartDocument().
 */
void DitaXmlGenerator::beginSubPage(const Location& location,
                                    const QString& fileName)
{
    PageGenerator::beginSubPage(location,fileName);
    (void) lookupGuidMap(fileName);
    QXmlStreamWriter* writer = new QXmlStreamWriter(out().device());
    xmlWriterStack.push(writer);
    writer->setAutoFormatting(true);
    writer->setAutoFormattingIndent(4);
    writer->writeStartDocument();
}

/*!
  Calls writeEndDocument() and then pops the XML stream writer
  off the stack and deletes it. Then it calls endSubPage() in
  the base class to close the device.
 */
void DitaXmlGenerator::endSubPage()
{
    xmlWriter().writeEndDocument();
    delete xmlWriterStack.pop();
    PageGenerator::endSubPage();
}

/*!
  Returns a reference to the XML stream writer currently in use.
  There is one XML stream writer open for each XML file being
  written, and they are kept on a stack. The one on top of the
  stack is the one being written to at the moment.
 */
QXmlStreamWriter& DitaXmlGenerator::xmlWriter()
{
    return *xmlWriterStack.top();
}

/*!
  Writes the \e {Detailed Description} section(s) for \a node to the
  current XML stream using the code \a marker. if the \a apiDesc flag
  is true, then the first section of the sequence of sections written
  will be an \c {apiDesc>} element with a \e {spectitle} attribute of
  \e {Detailed Description}. Otherwise, the first section will be a
  \c {<section>} element with a \c {<title>} element of \e {Detailed
  Description}. This function calls the Generator::generateBody()
  function to write the XML for the section list.
 */
void DitaXmlGenerator::writeDetailedDescription(const Node* node,
                                                CodeMarker* marker,
                                                bool apiDesc,
                                                const QString& title)
{
    if (!node->doc().isEmpty()) {
        inDetailedDescription = true;
        if (apiDesc) {
            inApiDesc = true;
            xmlWriter().writeStartElement("apiDesc");
            if (!title.isEmpty()) {
                writeGuidAttribute(title);
                xmlWriter().writeAttribute("spectitle",title);
            }
            else
                writeGuidAttribute("Detailed Description");
            xmlWriter().writeAttribute("outputclass","details");
        }
        else {
            inSection = true;
            xmlWriter().writeStartElement("section");
            if (!title.isEmpty()) {
                writeGuidAttribute(title);
                xmlWriter().writeAttribute("outputclass","details");
                xmlWriter().writeStartElement("title");
                xmlWriter().writeAttribute("outputclass","h2");
                writeCharacters(title);
                xmlWriter().writeEndElement(); // </title>
            }
            else {
                writeGuidAttribute("Detailed Description");
                xmlWriter().writeAttribute("outputclass","details");
            }
        }
        generateBody(node, marker);
        if (inApiDesc) {
            xmlWriter().writeEndElement(); // </apiDesc>
            inApiDesc = false;
        }
        else if (inSection) {
            xmlWriter().writeEndElement(); // </section>
            inSection = false;
        }
    }
    inDetailedDescription = false;
}

/*!
  Write the nested class elements.
 */
void DitaXmlGenerator::writeNestedClasses(const Section& s, 
                                          const Node* n)
{
    if (s.members.isEmpty())
        return;
    xmlWriter().writeStartElement("cxxClassNested");
    xmlWriter().writeStartElement("cxxClassNestedDetail");
    
    NodeList::ConstIterator m = s.members.begin();
    while (m != s.members.end()) {
        if ((*m)->type() == Node::Class) {
            xmlWriter().writeStartElement("cxxClassNestedClass");
            QString link = linkForNode((*m), n);
            xmlWriter().writeAttribute("href", link);
            QString name = n->name() + "::" + (*m)->name();
            writeCharacters(name);
            xmlWriter().writeEndElement(); // <cxxClassNestedClass>
        }
        ++m;
    }
    xmlWriter().writeEndElement(); // <cxxClassNestedDetail>
    xmlWriter().writeEndElement(); // <cxxClassNested>
}

/*!
  Recursive writing of DITA XML files from the root \a node.
 */
void
DitaXmlGenerator::generateInnerNode(const InnerNode* node)
{
    if (!node->url().isNull())
        return;

    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode *>(node);
        if (fakeNode->subType() == Node::ExternalPage)
            return;
        if (fakeNode->subType() == Node::Image)
            return;
        if (fakeNode->subType() == Node::QmlPropertyGroup)
            return;
        if (fakeNode->subType() == Node::Page) {
            if (node->count() > 0)
                qDebug("PAGE %s HAS CHILDREN", qPrintable(fakeNode->title()));
        }
    }

    /*
      Obtain a code marker for the source file.
     */
    CodeMarker *marker = CodeMarker::markerForFileName(node->location().filePath());

    if (node->parent() != 0) {
	beginSubPage(node->location(), fileName(node));
	if (node->type() == Node::Namespace || node->type() == Node::Class) {
	    generateClassLikeNode(node, marker);
	}
        else if (node->type() == Node::Fake) {
            if (node->subType() == Node::HeaderFile)
                generateClassLikeNode(node, marker);
            else if (node->subType() == Node::QmlClass)
                generateClassLikeNode(node, marker);
            else
                generateFakeNode(static_cast<const FakeNode*>(node), marker);
	}
	endSubPage();
    }

    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
	if ((*c)->isInnerNode() && (*c)->access() != Node::Private)
	    generateInnerNode((const InnerNode*) *c);
	++c;
    }
}

/*!
  Returns true if \a format is "XML" or "HTML" .
 */
bool DitaXmlGenerator::canHandleFormat(const QString& format)
{
    return (format == "HTML") || (format == this->format());
}

void DitaXmlGenerator::writeDitaMap()
{
    beginSubPage(Location(),"qt-dita-map.xml");

    QString doctype;
    doctype = "<!DOCTYPE cxxAPIMap PUBLIC \"-//NOKIA//DTD DITA C++ API Map Reference Type v0.6.0//EN\" \"dtd/cxxAPIMap.dtd\">";

    xmlWriter().writeDTD(doctype);
    xmlWriter().writeStartElement("cxxAPIMap");
    xmlWriter().writeAttribute("id","Qt-DITA-Map");
    xmlWriter().writeAttribute("title","Qt DITA Map");
    xmlWriter().writeStartElement("topicmeta");
    xmlWriter().writeStartElement("shortdesc");
    xmlWriter().writeCharacters("The top level map for the Qt documentation");
    xmlWriter().writeEndElement(); // </shortdesc>
    xmlWriter().writeEndElement(); // </topicmeta>
    GuidMaps::iterator i = guidMaps.begin();
    while (i != guidMaps.end()) {
        xmlWriter().writeStartElement("topicref");
        xmlWriter().writeAttribute("href",i.key());
        xmlWriter().writeAttribute("type","topic");
        xmlWriter().writeEndElement(); // </topicref>
        ++i;
    }
    endSubPage();
}

QT_END_NAMESPACE
