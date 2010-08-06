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
  htmlgenerator.cpp
*/

#include "codemarker.h"
#include "codeparser.h"
#include "helpprojectwriter.h"
#include "htmlgenerator.h"
#include "node.h"
#include "separator.h"
#include "tree.h"
#include <ctype.h>

#include <qdebug.h>
#include <qlist.h>
#include <qiterator.h>
#include <qtextcodec.h>

QT_BEGIN_NAMESPACE

#define COMMAND_VERSION                 Doc::alias("version")
int HtmlGenerator::id = 0;
bool HtmlGenerator::debugging_on = false;

#if 0
QString HtmlGenerator::divNavTop = "<div class=\"navTop\"><a href=\"#toc\"><img src=\"./images/bullet_up.png\"></a></div>";
#endif

QString HtmlGenerator::divNavTop = "";

QString HtmlGenerator::sinceTitles[] =
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

static QRegExp linkTag("(<@link node=\"([^\"]+)\">).*(</@link>)");
static QRegExp funcTag("(<@func target=\"([^\"]*)\">)(.*)(</@func>)");
static QRegExp typeTag("(<@(type|headerfile|func)(?: +[^>]*)?>)(.*)(</@\\2>)");
static QRegExp spanTag("</@(?:comment|preprocessor|string|char)>");
static QRegExp unknownTag("</?@[^>]*>");

bool parseArg(const QString &src,
              const QString &tag,
              int *pos,
              int n,
              QStringRef *contents,
              QStringRef *par1 = 0,
              bool debug = false)
{
#define SKIP_CHAR(c) \
    if (debug) \
        qDebug() << "looking for " << c << " at " << QString(src.data() + i, n - i); \
    if (i >= n || src[i] != c) { \
        if (debug) \
            qDebug() << " char '" << c << "' not found"; \
        return false; \
    } \
    ++i;


#define SKIP_SPACE \
    while (i < n && src[i] == ' ') \
        ++i;

    int i = *pos;
    int j = i;

    // assume "<@" has been parsed outside
    //SKIP_CHAR('<');
    //SKIP_CHAR('@');

    if (tag != QStringRef(&src, i, tag.length())) {
        if (0 && debug)
            qDebug() << "tag " << tag << " not found at " << i;
        return false;
    }

    if (debug)
        qDebug() << "haystack:" << src << "needle:" << tag << "i:" <<i;

    // skip tag
    i += tag.length();

    // parse stuff like:  linkTag("(<@link node=\"([^\"]+)\">).*(</@link>)");
    if (par1) {
        SKIP_SPACE;
        // read parameter name
        j = i;
        while (i < n && src[i].isLetter())
            ++i;
        if (src[i] == '=') {
            if (debug)
                qDebug() << "read parameter" << QString(src.data() + j, i - j);
            SKIP_CHAR('=');
            SKIP_CHAR('"');
            // skip parameter name
            j = i;
            while (i < n && src[i] != '"')
                ++i;
            *par1 = QStringRef(&src, j, i - j);
            SKIP_CHAR('"');
            SKIP_SPACE;
        } else {
            if (debug)
                qDebug() << "no optional parameter found";
        }
    }
    SKIP_SPACE;
    SKIP_CHAR('>');

    // find contents up to closing "</@tag>
    j = i;
    for (; true; ++i) {
        if (i + 4 + tag.length() > n)
            return false;
        if (src[i] != '<')
            continue;
        if (src[i + 1] != '/')
            continue;
        if (src[i + 2] != '@')
            continue;
        if (tag != QStringRef(&src, i + 3, tag.length()))
            continue;
        if (src[i + 3 + tag.length()] != '>')
            continue;
        break;
    }

    *contents = QStringRef(&src, j, i - j);

    i += tag.length() + 4;

    *pos = i;
    if (debug)
        qDebug() << " tag " << tag << " found: pos now: " << i;
    return true;
#undef SKIP_CHAR
}

static void addLink(const QString &linkTarget,
                    const QStringRef &nestedStuff,
                    QString *res)
{
    if (!linkTarget.isEmpty()) {
        *res += "<a href=\"";
        *res += linkTarget;
        *res += "\">";
        *res += nestedStuff;
        *res += "</a>";
    }
    else {
        *res += nestedStuff;
    }
}


HtmlGenerator::HtmlGenerator()
    : helpProjectWriter(0),
      inLink(false),
      inContents(false),
      inSectionHeading(false),
      inTableHeader(false),
      numTableRows(0),
      threeColumnEnumValueTable(true),
      offlineDocs(false),
      onlineDocs(false),
      creatorDocs(true),
      funcLeftParen("\\S(\\()"),
      myTree(0),
      slow(false),
      obsoleteLinks(false)
{
}

HtmlGenerator::~HtmlGenerator()
{
    if (helpProjectWriter)
        delete helpProjectWriter;
}

void HtmlGenerator::initializeGenerator(const Config &config)
{
    static const struct {
        const char *key;
        const char *left;
        const char *right;
    } defaults[] = {
        { ATOM_FORMATTING_BOLD, "<b>", "</b>" },
        { ATOM_FORMATTING_INDEX, "<!--", "-->" },
        { ATOM_FORMATTING_ITALIC, "<i>", "</i>" },
        { ATOM_FORMATTING_PARAMETER, "<i>", "</i>" },
        { ATOM_FORMATTING_SUBSCRIPT, "<sub>", "</sub>" },
        { ATOM_FORMATTING_SUPERSCRIPT, "<sup>", "</sup>" },
        { ATOM_FORMATTING_TELETYPE, "<tt>", "</tt>" },
        { ATOM_FORMATTING_UNDERLINE, "<u>", "</u>" },
        { 0, 0, 0 }
    };

    Generator::initializeGenerator(config);
    obsoleteLinks = config.getBool(QLatin1String(CONFIG_OBSOLETELINKS));
    setImageFileExtensions(QStringList() << "png" << "jpg" << "jpeg" << "gif");
    int i = 0;
    while (defaults[i].key) {
        formattingLeftMap().insert(defaults[i].key, defaults[i].left);
        formattingRightMap().insert(defaults[i].key, defaults[i].right);
        i++;
    }

    style = config.getString(HtmlGenerator::format() +
                             Config::dot +
                             HTMLGENERATOR_STYLE);
    postHeader = config.getString(HtmlGenerator::format() +
                                  Config::dot +
                                  HTMLGENERATOR_POSTHEADER);
    postPostHeader = config.getString(HtmlGenerator::format() +
                                      Config::dot +
                                      HTMLGENERATOR_POSTPOSTHEADER);
    creatorPostHeader = config.getString(HtmlGenerator::format() +
                                  Config::dot +
                                  HTMLGENERATOR_CREATORPOSTHEADER);
    creatorPostPostHeader = config.getString(HtmlGenerator::format() +
                                      Config::dot +
                                      HTMLGENERATOR_CREATORPOSTPOSTHEADER);
    footer = config.getString(HtmlGenerator::format() +
                              Config::dot +
                              HTMLGENERATOR_FOOTER);
    address = config.getString(HtmlGenerator::format() +
                               Config::dot +
                               HTMLGENERATOR_ADDRESS);
    pleaseGenerateMacRef = config.getBool(HtmlGenerator::format() +
                                          Config::dot +
                                          HTMLGENERATOR_GENERATEMACREFS);

    project = config.getString(CONFIG_PROJECT);

    onlineDocs = config.getBool(CONFIG_ONLINE);

    offlineDocs = config.getBool(CONFIG_OFFLINE);

    creatorDocs = config.getBool(CONFIG_CREATOR);

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

    stylesheets = config.getStringList(HtmlGenerator::format() +
                                       Config::dot +
                                       HTMLGENERATOR_STYLESHEETS);
    customHeadElements = config.getStringList(HtmlGenerator::format() +
                                              Config::dot +
                                              HTMLGENERATOR_CUSTOMHEADELEMENTS);
    codeIndent = config.getInt(CONFIG_CODEINDENT);

    helpProjectWriter = new HelpProjectWriter(config,
                                              project.toLower() +
                                              ".qhp");
}

void HtmlGenerator::terminateGenerator()
{
    Generator::terminateGenerator();
}

QString HtmlGenerator::format()
{
    return "HTML";
}

/*!
  This is where the html files and dcf files are written.
  \note The html file generation is done in the base class,
  PageGenerator::generateTree().
 */
void HtmlGenerator::generateTree(const Tree *tree, CodeMarker *marker)
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

    dcfClassesRoot.ref = "classes.html";
    dcfClassesRoot.title = "Classes";
    qSort(dcfClassesRoot.subsections);

    dcfOverviewsRoot.ref = "overviews.html";
    dcfOverviewsRoot.title = "Overviews";
    qSort(dcfOverviewsRoot.subsections);

    dcfExamplesRoot.ref = "examples.html";
    dcfExamplesRoot.title = "Tutorial & Examples";
    qSort(dcfExamplesRoot.subsections);

    DcfSection qtRoot;
    appendDcfSubSection(&qtRoot, dcfClassesRoot);
    appendDcfSubSection(&qtRoot, dcfOverviewsRoot);
    appendDcfSubSection(&qtRoot, dcfExamplesRoot);

    generateDcf(project.toLower().simplified().replace(" ", "-"),
                "index.html",
                projectDescription, qtRoot);
    generateDcf("designer",
                "designer-manual.html",
                "Qt Designer Manual",
                dcfDesignerRoot);
    generateDcf("linguist",
                "linguist-manual.html",
                "Qt Linguist Manual",
                dcfLinguistRoot);
    generateDcf("assistant",
                "assistant-manual.html",
                "Qt Assistant Manual",
                dcfAssistantRoot);
    generateDcf("qmake",
                "qmake-manual.html",
                "qmake Manual",
                dcfQmakeRoot);

    QString fileBase = project.toLower().simplified().replace(" ", "-");
    generateIndex(fileBase, projectUrl, projectDescription);
    generatePageIndex(outputDir() + "/" + fileBase + ".pageindex", marker);

    helpProjectWriter->generate(myTree);
}

void HtmlGenerator::startText(const Node * /* relative */,
                              CodeMarker * /* marker */)
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
int HtmlGenerator::generateAtom(const Atom *atom,
                                const Node *relative,
                                CodeMarker *marker)
{
    int skipAhead = 0;
    static bool in_para = false;
	
    switch (atom->type()) {
    case Atom::AbstractLeft:
        break;
    case Atom::AbstractRight:
        break;
    case Atom::AutoLink:
        if (!inLink && !inContents && !inSectionHeading) {
            const Node *node = 0;
            QString link = getLink(atom, relative, marker, &node);
            if (!link.isEmpty()) {
                beginLink(link, node, relative, marker);
                generateLink(atom, relative, marker);
                endLink();
            }
            else {
                out() << protectEnc(atom->string());
            }
        }
        else {
            out() << protectEnc(atom->string());
        }
        break;
    case Atom::BaseName:
        break;
    case Atom::BriefLeft:
        if (relative->type() == Node::Fake) {
            skipAhead = skipAtoms(atom, Atom::BriefRight);
            break;
        }

        out() << "<p>";
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
            out() << "This ";
            if (relative->type() == Node::Property)
                out() << "property";
            else
                out() << "variable";
            QStringList words = str.split(" ");
            if (!(words.first() == "contains" || words.first() == "specifies"
                || words.first() == "describes" || words.first() == "defines"
                || words.first() == "holds" || words.first() == "determines"))
                out() << " holds ";
            else
                out() << " ";
            out() << str << ".";
        }
        break;
    case Atom::BriefRight:
        if (relative->type() != Node::Fake)
            out() << "</p>\n";
        break;
    case Atom::C:
        out() << formattingLeftMap()[ATOM_FORMATTING_TELETYPE];
        if (inLink) {
            out() << protectEnc(plainCode(atom->string()));
        }
        else {
            out() << highlightedCode(atom->string(), marker, relative);
        }
        out() << formattingRightMap()[ATOM_FORMATTING_TELETYPE];
        break;
    case Atom::Code:
	out() << "<pre class=\"highlightedCode brush: cpp\">"
              << trimmedTrailing(highlightedCode(indent(codeIndent,atom->string()),
                                                 marker,relative))
              << "</pre>\n";
	break;
#ifdef QDOC_QML
    case Atom::Qml:
	out() << "<pre class=\"highlightedCode brush: cpp\">"
              << trimmedTrailing(highlightedCode(indent(codeIndent,atom->string()),
                                                 marker,relative))
              << "</pre>\n";
	break;
#endif
    case Atom::CodeNew:
        out() << "<p>you can rewrite it as</p>\n"
              << "<pre class=\"highlightedCode brush: cpp\">"
              << trimmedTrailing(highlightedCode(indent(codeIndent,atom->string()),
                                                 marker,relative))
              << "</pre>\n";
        break;
    case Atom::CodeOld:
        out() << "<p>For example, if you have code like</p>\n";
        // fallthrough
    case Atom::CodeBad:
        out() << "<pre class=\"highlightedCode brush: cpp\">"
              << trimmedTrailing(protectEnc(plainCode(indent(codeIndent,atom->string()))))
              << "</pre>\n";
	break;
    case Atom::FootnoteLeft:
        // ### For now
        if (in_para) {
            out() << "</p>\n";
            in_para = false;
        }
        out() << "<!-- ";
        break;
    case Atom::FootnoteRight:
        // ### For now
        out() << "-->";
        break;
    case Atom::FormatElse:
    case Atom::FormatEndif:
    case Atom::FormatIf:
        break;
    case Atom::FormattingLeft:
        out() << formattingLeftMap()[atom->string()];
        if (atom->string() == ATOM_FORMATTING_PARAMETER) {
            if (atom->next() != 0 && atom->next()->type() == Atom::String) {
                QRegExp subscriptRegExp("([a-z]+)_([0-9n])");
                if (subscriptRegExp.exactMatch(atom->next()->string())) {
                    out() << subscriptRegExp.cap(1) << "<sub>"
                          << subscriptRegExp.cap(2) << "</sub>";
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
            out() << formattingRightMap()[atom->string()];
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
                out() << "<ul>\n";
                s = sections.constBegin();
                while (s != sections.constEnd()) {
                    if (!(*s).members.isEmpty()) {

                        out() << "<li>"
                              << "<a href=\"#"
                              << Doc::canonicalTitle((*s).name)
                              << "\">"
                              << (*s).name
                              << "</a></li>\n";
                    }
                    ++s;
                }
                out() << "</ul>\n";

                int idx = 0;
                s = sections.constBegin();
                while (s != sections.constEnd()) {
                    if (!(*s).members.isEmpty()) {
                        out() << "<a name=\""
                              << Doc::canonicalTitle((*s).name)
                              << "\"></a>\n";
                        out() << "<h3>" << protectEnc((*s).name) << "</h3>\n";
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
                                out() << "<p>Class ";

                                out() << "<a href=\""
                                      << linkForNode(pmap.key(), 0)
                                      << "\">";
                                QStringList pieces = fullName(pmap.key(), 0, marker).split("::");
                                out() << protectEnc(pieces.last());
                                out() << "</a>"  << ":</p>\n";

                                generateSection(nlist, 0, marker, CodeMarker::Summary);
                                out() << "<br/>";
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
            if (atom->type() == Atom::Image)
                out() << "<p class=\"centerAlign\">";
            if (fileName.isEmpty()) {
                out() << "<font color=\"red\">[Missing image "
                      << protectEnc(atom->string()) << "]</font>";
            }
            else {
                out() << "<img src=\"" << protectEnc(fileName) << "\"";
                if (!text.isEmpty())
                    out() << " alt=\"" << protectEnc(text) << "\"";
                out() << " />";
                helpProjectWriter->addExtraFile(fileName);
            }
            if (atom->type() == Atom::Image)
                out() << "</p>";
        }
        break;
    case Atom::ImageText:
        break;
    case Atom::LegaleseLeft:
        out() << "<div class=\"LegaleseLeft\">";
        break;
    case Atom::LegaleseRight:
        out() << "</div>";
        break;
    case Atom::LineBreak:
        out() << "<br/>";
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
            out() << "</p>\n";
            in_para = false;
        }
        if (atom->string() == ATOM_LIST_BULLET) {
            out() << "<ul>\n";
        }
        else if (atom->string() == ATOM_LIST_TAG) {
            out() << "<dl>\n";
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            threeColumnEnumValueTable = isThreeColumnEnumValueTable(atom);
            if (threeColumnEnumValueTable) {
                out() << "<table class=\"valuelist\">";
                    //  << "<tr>"
				if (++numTableRows % 2 == 1)
					out() << "<tr class=\"odd\">";
				else
					out() << "<tr class=\"even\">";

					out() << "<tr><th class=\"tblConst\">Constant</th>"
                      << "<th class=\"tblval\">Value</th>"
                      << "<th class=\"tbldscr\">Description</th></tr>\n";
            }
            else {
                out() << "<table class=\"valuelist\">"
                      << "<tr><th class=\"tblConst\">Constant</th><th class=\"tblVal\">Value</th></tr>\n";
            }
        }
        else {
            out() << "<ol type=";
            if (atom->string() == ATOM_LIST_UPPERALPHA) {
                out() << "\"A\"";
            } /* why type? */
            else if (atom->string() == ATOM_LIST_LOWERALPHA) {
                out() << "\"a\"";
            }
            else if (atom->string() == ATOM_LIST_UPPERROMAN) {
                out() << "\"I\"";
            }
            else if (atom->string() == ATOM_LIST_LOWERROMAN) {
                out() << "\"i\"";
            }
            else { // (atom->string() == ATOM_LIST_NUMERIC)
                out() << "\"1\"";
            }
            if (atom->next() != 0 && atom->next()->string().toInt() != 1)
                out() << " start=\"" << atom->next()->string() << "\"";
            out() << ">\n";
        }
        break;
    case Atom::ListItemNumber:
        break;
    case Atom::ListTagLeft:
        if (atom->string() == ATOM_LIST_TAG) {
            out() << "<dt>";
        }
        else { // (atom->string() == ATOM_LIST_VALUE)
            // ### Trenton

            out() << "<tr><td  class=\"topAlign\"><tt>"
                  << protectEnc(plainCode(marker->markedUpEnumValue(atom->next()->string(),
                                                                 relative)))
                  << "</tt></td><td class=\" topAlign\">";

            QString itemValue;
            if (relative->type() == Node::Enum) {
                const EnumNode *enume = static_cast<const EnumNode *>(relative);
                itemValue = enume->itemValue(atom->next()->string());
            }

            if (itemValue.isEmpty())
                out() << "?";
            else
                out() << "<tt>" << protectEnc(itemValue) << "</tt>";

            skipAhead = 1;
        }
        break;
    case Atom::ListTagRight:
        if (atom->string() == ATOM_LIST_TAG)
            out() << "</dt>\n";
        break;
    case Atom::ListItemLeft:
        if (atom->string() == ATOM_LIST_TAG) {
            out() << "<dd>";
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            if (threeColumnEnumValueTable) {
                out() << "</td><td  class=\"topAlign\">";
                if (matchAhead(atom, Atom::ListItemRight))
                    out() << "&nbsp;";
            }
        }
        else {
            out() << "<li>";
        }
        if (matchAhead(atom, Atom::ParaLeft))
            skipAhead = 1;
        break;
    case Atom::ListItemRight:
        if (atom->string() == ATOM_LIST_TAG) {
            out() << "</dd>\n";
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            out() << "</td></tr>\n";
        }
        else {
            out() << "</li>\n";
        }
        break;
    case Atom::ListRight:
        if (atom->string() == ATOM_LIST_BULLET) {
            out() << "</ul>\n";
        }
        else if (atom->string() == ATOM_LIST_TAG) {
            out() << "</dl>\n";
        }
        else if (atom->string() == ATOM_LIST_VALUE) {
            out() << "</table>\n";
        }
        else {
            out() << "</ol>\n";
        }
        break;
    case Atom::Nop:
        break;
    case Atom::ParaLeft:
        out() << "<p>";
        in_para = true;
        break;
    case Atom::ParaRight:
        endLink();
        if (in_para) {
            out() << "</p>\n";
            in_para = false;
        }
        //if (!matchAhead(atom, Atom::ListItemRight) && !matchAhead(atom, Atom::TableItemRight))
        //    out() << "</p>\n";
        break;
    case Atom::QuotationLeft:
        out() << "<blockquote>";
        break;
    case Atom::QuotationRight:
        out() << "</blockquote>\n";
        break;
    case Atom::RawString:
        out() << atom->string();
        break;
    case Atom::SectionLeft:
#if 0
        {
            int nextLevel = atom->string().toInt();
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
            out() << "<a name=\"sec-" << sectionNumber.join("-") << "\"></a>" << divNavTop << "\n";
        }
#else
        out() << "<a name=\"" << Doc::canonicalTitle(Text::sectionHeading(atom).toString())
              << "\"></a>" << divNavTop << "\n";
#endif
        break;
    case Atom::SectionRight:
        break;
    case Atom::SectionHeadingLeft:
        out() << "<h" + QString::number(atom->string().toInt() + hOffset(relative)) + ">";
        inSectionHeading = true;
        break;
    case Atom::SectionHeadingRight:
        out() << "</h" + QString::number(atom->string().toInt() + hOffset(relative)) + ">\n";
        inSectionHeading = false;
        break;
    case Atom::SidebarLeft:
        break;
    case Atom::SidebarRight:
        break;
    case Atom::String:
        if (inLink && !inContents && !inSectionHeading) {
            generateLink(atom, relative, marker);
        }
        else {
            out() << protectEnc(atom->string());
        }
        break;
    case Atom::TableLeft:
        if (in_para) {
            out() << "</p>\n";
            in_para = false;
        }
        if (!atom->string().isEmpty()) {
            if (atom->string().contains("%"))
                out() << "<table class=\"generic\">\n "; // width=\"" << atom->string() << "\">\n ";
            else {
                out() << "<table class=\"generic\">\n";
            }
        }
        else {
            out() << "<table class=\"generic\">\n";
        }
        numTableRows = 0;
        break;
    case Atom::TableRight:
        out() << "</table>\n";
        break;
    case Atom::TableHeaderLeft:
        out() << "<thead><tr class=\"qt-style topAlign\">";
        inTableHeader = true;
        break;
    case Atom::TableHeaderRight:
        out() << "</tr>";
        if (matchAhead(atom, Atom::TableHeaderLeft)) {
            skipAhead = 1;
            out() << "\n<tr class=\"qt-style topAlign\">";
        }
        else {
            out() << "</thead>\n";
            inTableHeader = false;
        }
        break;
    case Atom::TableRowLeft:
        if (++numTableRows % 2 == 1)
            out() << "<tr class=\"odd topAlign\">";
        else
            out() << "<tr class=\"even topAlign\">";
        break;
    case Atom::TableRowRight:
        out() << "</tr>\n";
        break;
    case Atom::TableItemLeft:
        {
            if (inTableHeader)
                out() << "<th ";
            else
                out() << "<td ";

            QStringList spans = atom->string().split(",");
            if (spans.size() == 2) {
                if (spans.at(0) != "1")
                    out() << " colspan=\"" << spans.at(0) << "\"";
                if (spans.at(1) != "1")
                    out() << " rowspan=\"" << spans.at(1) << "\"";
            if (inTableHeader)
                out() << ">";
            else
                out() << "><p>"; 
            }
            if (matchAhead(atom, Atom::ParaLeft))
                skipAhead = 1;
        }
        break;
    case Atom::TableItemRight:
        if (inTableHeader)
            out() << "</th>";
        else
            out() << "</p></td>";
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
        out() << "<a name=\"" << Doc::canonicalTitle(atom->string()) << "\"></a>";
        break;
    case Atom::UnhandledFormat:
        out() << "<b class=\"redFont\">&lt;Missing HTML&gt;</b>";
        break;
    case Atom::UnknownCommand:
        out() << "<b class=\"redFont\"><code>\\" << protectEnc(atom->string())
              << "</code></b>";
        break;
#ifdef QDOC_QML
    case Atom::QmlText:
    case Atom::EndQmlText:
        // don't do anything with these. They are just tags.
        break;
#endif
    default:
        unknownAtom(atom);
    }
    return skipAhead;
}

/*!
  Generate a reference page for a C++ class.
 */
void HtmlGenerator::generateClassLikeNode(const InnerNode *inner,
                                          CodeMarker *marker)
{
    QList<Section> sections;
    QList<Section>::ConstIterator s;

    const ClassNode *classe = 0;
    const NamespaceNode *namespasse = 0;

    QString title;
    QString rawTitle;
    QString fullTitle;
    if (inner->type() == Node::Namespace) {
        namespasse = static_cast<const NamespaceNode *>(inner);
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Namespace";
    }
    else if (inner->type() == Node::Class) {
        classe = static_cast<const ClassNode *>(inner);
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Class Reference";
    }

    DcfSection classSection;
    classSection.title = title;
    classSection.ref = linkForNode(inner, 0);
    classSection.keywords += qMakePair(inner->name(), classSection.ref);

    Text subtitleText;
    if (rawTitle != fullTitle)
        subtitleText << "(" << Atom(Atom::AutoLink, fullTitle) << ")"
                     << Atom(Atom::LineBreak);

    generateHeader(title, inner, marker);
    sections = marker->sections(inner, CodeMarker::Summary, CodeMarker::Okay);
    generateTableOfContents(inner,marker,&sections);
    generateTitle(title, subtitleText, SmallSubTitle, inner, marker);    
    generateBrief(inner, marker);
    generateIncludes(inner, marker);
    generateStatus(inner, marker);
    if (classe) {
        generateInherits(classe, marker);
        generateInheritedBy(classe, marker);
#ifdef QDOC_QML
        if (!classe->qmlElement().isEmpty()) {
            generateInstantiatedBy(classe,marker);
        }
#endif
    }
    generateThreadSafeness(inner, marker);
    generateSince(inner, marker);

    out() << "<ul>\n";

    QString membersLink = generateListOfAllMemberFile(inner, marker);
    if (!membersLink.isEmpty())
        out() << "<li><a href=\"" << membersLink << "\">"
              << "List of all members, including inherited members</a></li>\n";

    QString obsoleteLink = generateLowStatusMemberFile(inner,
                                                       marker,
                                                       CodeMarker::Obsolete);
    if (!obsoleteLink.isEmpty())
        out() << "<li><a href=\"" << obsoleteLink << "\">"
              << "Obsolete members</a></li>\n";

    QString compatLink = generateLowStatusMemberFile(inner,
                                                     marker,
                                                     CodeMarker::Compat);
    if (!compatLink.isEmpty())
        out() << "<li><a href=\"" << compatLink << "\">"
              << "Qt 3 support members</a></li>\n";

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
               // out() << "<hr />\n";
                out() << "<a name=\""
                      << registerRef((*s).name.toLower())
                      << "\"></a>" << divNavTop << "\n";
                out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";
                generateSection(s->members, inner, marker, CodeMarker::Summary);
            }
            if (!s->reimpMembers.isEmpty()) {
                QString name = QString("Reimplemented ") + (*s).name;
              //  out() << "<hr />\n";
                out() << "<a name=\""
                      << registerRef(name.toLower())
                      << "\"></a>" << divNavTop << "\n";
                out() << "<h2>" << protectEnc(name) << "</h2>\n";
                generateSection(s->reimpMembers, inner, marker, CodeMarker::Summary);
            }

            if (!s->inherited.isEmpty()) {
                out() << "<ul>\n";
                generateSectionInheritedList(*s, inner, marker);
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

    out() << "<a name=\"" << registerRef("details") << "\"></a>" << divNavTop << "\n";

    if (!inner->doc().isEmpty()) {
        generateExtractionMark(inner, DetailedDescriptionMark);
        //out() << "<hr />\n"
        out() << "<div class=\"descr\">\n" // QTBUG-9504
              << "<h2>" << "Detailed Description" << "</h2>\n";
        generateBody(inner, marker);
        out() << "</div>\n"; // QTBUG-9504
        generateAlsoList(inner, marker);
        generateExtractionMark(inner, EndMark);
    }

    sections = marker->sections(inner, CodeMarker::Detailed, CodeMarker::Okay);
    s = sections.begin();
    while (s != sections.end()) {
        //out() << "<hr />\n";
        if (!(*s).divClass.isEmpty())
            out() << "<div class=\"" << (*s).divClass << "\">\n"; // QTBUG-9504
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
                foreach (const QString &name, names)
                    classSection.keywords += qMakePair(name,linkForNode(*m,0));
            }
            ++m;
        }
        if (!(*s).divClass.isEmpty())
            out() << "</div>\n"; // QTBUG-9504
        ++s;
    }
    generateFooter(inner);

    if (!membersLink.isEmpty()) {
        DcfSection membersSection;
        membersSection.title = "List of all members";
        membersSection.ref = membersLink;
        appendDcfSubSection(&classSection, membersSection);
    }
    if (!obsoleteLink.isEmpty()) {
        DcfSection obsoleteSection;
        obsoleteSection.title = "Obsolete members";
        obsoleteSection.ref = obsoleteLink;
        appendDcfSubSection(&classSection, obsoleteSection);
    }
    if (!compatLink.isEmpty()) {
        DcfSection compatSection;
        compatSection.title = "Qt 3 support members";
        compatSection.ref = compatLink;
        appendDcfSubSection(&classSection, compatSection);
    }

    appendDcfSubSection(&dcfClassesRoot, classSection);
}

/*!
  Generate the html page for a qdoc file that doesn't map
  to an underlying c++ file.
 */
void HtmlGenerator::generateFakeNode(const FakeNode *fake, CodeMarker *marker)
{
    SubTitleSize subTitleSize = LargeSubTitle;
    DcfSection fakeSection;
    fakeSection.title = fake->fullTitle();
    fakeSection.ref = linkForNode(fake, 0);

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

    generateHeader(htmlTitle, fake, marker);
        
    /*
      Generate the TOC for the new doc format.
      Don't generate a TOC for the home page.
    */
    const QmlClassNode* qml_cn = 0;
    if (fake->subType() == Node::QmlClass) {
        qml_cn = static_cast<const QmlClassNode*>(fake);
        sections = marker->qmlSections(qml_cn,CodeMarker::Summary);
        generateTableOfContents(fake,marker,&sections);
    }
    else if (fake->name() != QString("index.html"))
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
            out() << "<a name=\"" << registerRef("namespaces") << "\"></a>" << divNavTop << "\n";
            out() << "<h2>Namespaces</h2>\n";
            generateAnnotatedList(fake, marker, moduleNamespaceMap[fake->name()]);
        }
        if (moduleClassMap.contains(fake->name())) {
            out() << "<a name=\"" << registerRef("classes") << "\"></a>" << divNavTop << "\n";
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
            out() << "<li><a href=\"" << membersLink << "\">"
                  << "List of all members, including inherited members</a></li>\n";

        QString obsoleteLink = generateLowStatusMemberFile(fake,
                                                           marker,
                                                           CodeMarker::Obsolete);
        if (!obsoleteLink.isEmpty())
            out() << "<li><a href=\"" << obsoleteLink << "\">"
                  << "Obsolete members</a></li>\n";

        QString compatLink = generateLowStatusMemberFile(fake,
                                                         marker,
                                                         CodeMarker::Compat);
        if (!compatLink.isEmpty())
            out() << "<li><a href=\"" << compatLink << "\">"
                  << "Qt 3 support members</a></li>\n";

        out() << "</ul>\n";

        if (!membersLink.isEmpty()) {
            DcfSection membersSection;
            membersSection.title = "List of all members";
            membersSection.ref = membersLink;
            appendDcfSubSection(&fakeSection, membersSection);
        }
        if (!obsoleteLink.isEmpty()) {
            DcfSection obsoleteSection;
            obsoleteSection.title = "Obsolete members";
            obsoleteSection.ref = obsoleteLink;
            appendDcfSubSection(&fakeSection, obsoleteSection);
        }
        if (!compatLink.isEmpty()) {
            DcfSection compatSection;
            compatSection.title = "Qt 3 support members";
            compatSection.ref = compatLink;
            appendDcfSubSection(&fakeSection, compatSection);
        }
    }
#ifdef QDOC_QML
    else if (fake->subType() == Node::QmlClass) {
        const ClassNode* cn = qml_cn->classNode();
        generateBrief(qml_cn, marker);
        generateQmlInherits(qml_cn, marker);
        generateQmlInheritedBy(qml_cn, marker);
        generateQmlInstantiates(qml_cn, marker);
        s = sections.begin();
        while (s != sections.end()) {
            out() << "<a name=\"" << registerRef((*s).name.toLower())
                  << "\"></a>" << divNavTop << "\n";
            out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";
            generateQmlSummary(*s,fake,marker);
            ++s;
        }

        generateExtractionMark(fake, DetailedDescriptionMark);
        out() << "<a name=\"" << registerRef("details") << "\"></a>" << divNavTop << "\n";
        out() << "<h2>" << "Detailed Description" << "</h2>\n";
        generateBody(fake, marker);
        if (cn)
            generateQmlText(cn->doc().body(), cn, marker, fake->name());
        generateAlsoList(fake, marker);
        generateExtractionMark(fake, EndMark);
        //out() << "<hr />\n";

        sections = marker->qmlSections(qml_cn,CodeMarker::Detailed);
        s = sections.begin();
        while (s != sections.end()) {
            out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";
            NodeList::ConstIterator m = (*s).members.begin();
            while (m != (*s).members.end()) {
                generateDetailedQmlMember(*m, fake, marker);
                out() << "<br/>\n";
                fakeSection.keywords += qMakePair((*m)->name(),
                                                  linkForNode(*m,0));
                ++m;
            }
            ++s;
        }
        generateFooter(fake);
        return;
    }
#endif
    
    sections = marker->sections(fake, CodeMarker::Summary, CodeMarker::Okay);
    s = sections.begin();
    while (s != sections.end()) {
        out() << "<a name=\"" << registerRef((*s).name) << "\"></a>" << divNavTop << "\n";
        out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";
        generateSectionList(*s, fake, marker, CodeMarker::Summary);
        ++s;
    }

    Text brief = fake->doc().briefText();
    if (fake->subType() == Node::Module && !brief.isEmpty()) {
        generateExtractionMark(fake, DetailedDescriptionMark);
        out() << "<a name=\"" << registerRef("details") << "\"></a>" << divNavTop << "\n";
        out() << "<div class=\"descr\">\n"; // QTBUG-9504
        out() << "<h2>" << "Detailed Description" << "</h2>\n";
    }
    else {
        generateExtractionMark(fake, DetailedDescriptionMark);
        out() << "<div class=\"descr\"> <a name=\"" << registerRef("details") << "\"></a>\n"; // QTBUG-9504
    }

    generateBody(fake, marker);
    out() << "</div>\n"; // QTBUG-9504
    generateAlsoList(fake, marker);
    generateExtractionMark(fake, EndMark);

    if (!fake->groupMembers().isEmpty()) {
        NodeMap groupMembersMap;
        foreach (const Node *node, fake->groupMembers()) {
            if (node->type() == Node::Class || node->type() == Node::Namespace)
                groupMembersMap[node->name()] = node;
        }
        generateAnnotatedList(fake, marker, groupMembersMap);
    }

    fakeSection.keywords += qMakePair(fakeSection.title, fakeSection.ref);

    sections = marker->sections(fake, CodeMarker::Detailed, CodeMarker::Okay);
    s = sections.begin();
    while (s != sections.end()) {
        //out() << "<hr />\n";
        out() << "<h2>" << protectEnc((*s).name) << "</h2>\n";

        NodeList::ConstIterator m = (*s).members.begin();
        while (m != (*s).members.end()) {
            generateDetailedMember(*m, fake, marker);
            fakeSection.keywords += qMakePair((*m)->name(), linkForNode(*m, 0));
            ++m;
        }
        ++s;
    }
    generateFooter(fake);

    if (fake->subType() == Node::Example) {
        appendDcfSubSection(&dcfExamplesRoot, fakeSection);
    }
    else if (fake->subType() != Node::File) {
        QString contentsPage = fake->links().value(Node::ContentsLink).first;

        if (contentsPage == "Qt Designer Manual") {
            appendDcfSubSection(&dcfDesignerRoot, fakeSection);
        }
        else if (contentsPage == "Qt Linguist Manual") {
            appendDcfSubSection(&dcfLinguistRoot, fakeSection);
        }
        else if (contentsPage == "Qt Assistant Manual") {
            appendDcfSubSection(&dcfAssistantRoot, fakeSection);
        }
        else if (contentsPage == "qmake Manual") {
            appendDcfSubSection(&dcfQmakeRoot, fakeSection);
        }
        else {
            appendDcfSubSection(&dcfOverviewsRoot, fakeSection);
        }
    }
}

/*!
  Returns "html" for this subclass of Generator.
 */
QString HtmlGenerator::fileExtension(const Node * /* node */) const
{
    return "html";
}

/*!
  Output breadcrumb list in the html file.
 */
void HtmlGenerator::generateBreadCrumbs(const QString& title,
                                        const Node *node,
                                        CodeMarker *marker)
{
    Text breadcrumb;
    if (node->type() == Node::Class) {
        const ClassNode* cn = static_cast<const ClassNode*>(node);
        QString name =  node->moduleName();
        out() << "              <li><a href=\"modules.html\">Modules</a></li>";
        if (!name.isEmpty()) {
            out() << "              <li>";
            breadcrumb << Atom(Atom::AutoLink,name);
            generateText(breadcrumb, node, marker);
            out() << "</li>\n";
        }
        if (!cn->name().isEmpty())
            out() << "              <li>" << cn->name() << "</li>\n";
    }
    else if (node->type() == Node::Fake) {
        const FakeNode* fn = static_cast<const FakeNode*>(node);
        if (node->subType() == Node::Module) {
            out() << "              <li><a href=\"modules.html\">Modules</a></li>";
            QString name =  node->name();
            if (!name.isEmpty())
                out() << "              <li>" << name << "</li>\n";
        }
        else if (node->subType() == Node::Group) {
            if (fn->name() == QString("modules"))
                out() << "              <li>Modules</li>";
            else {
                out() << "              <li>" << title << "</li>";
            }
        }
        else if (node->subType() == Node::Page) {
            if (fn->name() == QString("qdeclarativeexamples.html")) {
                out() << "              <li><a href=\"all-examples.html\">Examples</a></li>";
                out() << "              <li>QML Examples & Demos</li>";
            }
            else if (fn->name().startsWith("examples-")) {
                out() << "              <li><a href=\"all-examples.html\">Examples</a></li>";
                out() << "              <li>" << title << "</li>";
            }
            else if (fn->name() == QString("namespaces.html")) {
                out() << "              <li>Namespaces</li>";
            }
            else {
                out() << "              <li>" << title << "</li>";
            }
        }
        else if (node->subType() == Node::QmlClass) {
            out() << "              <li><a href=\"qdeclarativeelements.html\">QML Elements</a></li>";
            out() << "              <li>" << title << "</li>";
        }
        else if (node->subType() == Node::Example) {
            out() << "              <li><a href=\"all-examples.html\">Examples</a></li>";
            QStringList sl = fn->name().split('/');
            if (sl.contains("declarative"))
                out() << "              <li><a href=\"qdeclarativeexamples.html\">QML Examples & Demos</a></li>";
            else {
                QString name = "examples-" + sl.at(0) + ".html";
                QString t = CodeParser::titleFromName(name);
                out() << "              <li><a href=\"" << name << "\">"
                      << t << "</a></li>";
            }
            out() << "              <li>" << title << "</li>";
        }
    }
    else if (node->type() == Node::Namespace) {
        out() << "              <li><a href=\"namespaces.html\">Namespaces</a></li>";
        out() << "              <li>" << title << "</li>";
    }
}

void HtmlGenerator::generateHeader(const QString& title,
                                   const Node *node,
                                   CodeMarker *marker)
{
    out() << QString("<?xml version=\"1.0\" encoding=\"%1\"?>\n").arg(outputEncoding);
    out() << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";
    out() << QString("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%1\" lang=\"%1\">\n").arg(naturalLanguage);
    out() << "<head>\n";
    out() << "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
    QString shortVersion;
    shortVersion = project + " " + shortVersion + ": ";
    if (node && !node->doc().location().isEmpty())
        out() << "<!-- " << node->doc().location().fileName() << " -->\n";

    shortVersion = myTree->version();
    if (shortVersion.count(QChar('.')) == 2)
        shortVersion.truncate(shortVersion.lastIndexOf(QChar('.')));
    if (!shortVersion.isEmpty()) {
        if (project == "QSA")
            shortVersion = "QSA " + shortVersion + ": ";
        else
            shortVersion = "Qt " + shortVersion + ": ";
    }

    // Generating page title
    out() << "  <title>" << shortVersion << protectEnc(title) << "</title>\n";
    // Adding style sheet
    out() << "  <link rel=\"stylesheet\" type=\"text/css\" href=\"style/style.css\" />\n";
    // Adding jquery and functions - providing online tools and search features
    out() << "  <script src=\"scripts/jquery.js\" type=\"text/javascript\"></script>\n";
    out() << "  <script src=\"scripts/functions.js\" type=\"text/javascript\"></script>\n";
    // Adding style and js for small windows
    out() << "  <script src=\"./scripts/superfish.js\" type=\"text/javascript\"></script>\n";
    out() << "  <link rel=\"stylesheet\" type=\"text/css\" href=\"style/superfish.css\" />";
    out() << "  <script src=\"./scripts/narrow.js\" type=\"text/javascript\"></script>\n";
    out() << "  <link rel=\"stylesheet\" type=\"text/css\" href=\"style/narrow.css\" />\n";
	
    // Adding syntax highlighter 	// future release
	
    // Setting some additional style sheet related details depending on configuration (e.g. online/offline)

    
    if(onlineDocs==true) // onlineDocs is for the web
    {
        // Browser spec styles
	out() << "  <!--[if IE]>\n";
	out() << "<meta name=\"MSSmartTagsPreventParsing\" content=\"true\">\n";
	out() << "<meta http-equiv=\"imagetoolbar\" content=\"no\">\n";
	out() << "<![endif]-->\n";
	out() << "<!--[if lt IE 7]>\n";
	out() << "<link rel=\"stylesheet\" type=\"text/css\" href=\"style/style_ie6.css\">\n";
	out() << "<![endif]-->\n";
	out() << "<!--[if IE 7]>\n";
	out() << "<link rel=\"stylesheet\" type=\"text/css\" href=\"style/style_ie7.css\">\n";
	out() << "<![endif]-->\n";
	out() << "<!--[if IE 8]>\n";
	out() << "<link rel=\"stylesheet\" type=\"text/css\" href=\"style/style_ie8.css\">\n";
	out() << "<![endif]-->\n";
		
	out() << "</head>\n";
	// CheckEmptyAndLoadList activating search
	out() << "<body class=\"\" onload=\"CheckEmptyAndLoadList();\">\n";
    }   
    else if (offlineDocs == true) // offlineDocs is for ???
    {
	out() << "</head>\n";
	out() << "<body class=\"offline \">\n"; // offline		
    }	  
    else if (creatorDocs == true) // creatorDocs is for Assistant/Creator
    {
	out() << "</head>\n";
	out() << "<body class=\"offline narrow creator\">\n"; // offline narrow
    }	
    // default -- not used except if one forgets to set any of the above settings to true
    else
    {
	out() << "</head>\n";
	out() << "<body>\n";		
    }

#ifdef GENERATE_MAC_REFS    
    if (mainPage)
        generateMacRef(node, marker);
#endif   
 

    if(onlineDocs==true) // onlineDocs is for the web
    {
        out() << QString(postHeader).replace("\\" + COMMAND_VERSION, myTree->version());
        generateBreadCrumbs(title,node,marker);
        out() << QString(postPostHeader).replace("\\" + COMMAND_VERSION, myTree->version());
    }   
    else if (offlineDocs == true) // offlineDocs is for ???
    {
        out() << QString(creatorPostHeader).replace("\\" + COMMAND_VERSION, myTree->version());
        generateBreadCrumbs(title,node,marker);
        out() << QString(creatorPostPostHeader).replace("\\" + COMMAND_VERSION, myTree->version());	
    }	  
    else if (creatorDocs == true) // creatorDocs is for Assistant/Creator
    {
        out() << QString(creatorPostHeader).replace("\\" + COMMAND_VERSION, myTree->version());
        generateBreadCrumbs(title,node,marker);
        out() << QString(creatorPostPostHeader).replace("\\" + COMMAND_VERSION, myTree->version());	
    }	
    // default -- not used except if one forgets to set any of the above settings to true
    else
    {
        out() << QString(creatorPostHeader).replace("\\" + COMMAND_VERSION, myTree->version());
        generateBreadCrumbs(title,node,marker);
        out() << QString(creatorPostPostHeader).replace("\\" + COMMAND_VERSION, myTree->version());	
    }

        navigationLinks.clear();

    if (node && !node->links().empty()) {
        QPair<QString,QString> linkPair;
        QPair<QString,QString> anchorPair;
        const Node *linkNode;

        if (node->links().contains(Node::PreviousLink)) {
            linkPair = node->links()[Node::PreviousLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            if (!linkNode || linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);

            out() << "  <link rel=\"prev\" href=\""
                  << anchorPair.first << "\" />\n";

            navigationLinks += "[Previous: <a href=\"" + anchorPair.first + "\">";
            if (linkPair.first == linkPair.second && !anchorPair.second.isEmpty())
                navigationLinks += protect(anchorPair.second);
            else
                navigationLinks += protect(linkPair.second);
            navigationLinks += "</a>]\n";
        }
        if (node->links().contains(Node::NextLink)) {
            linkPair = node->links()[Node::NextLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            if (!linkNode || linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);

            out() << "  <link rel=\"next\" href=\""
                  << anchorPair.first << "\" />\n";

            navigationLinks += "[Next: <a href=\"" + anchorPair.first + "\">";
            if (linkPair.first == linkPair.second && !anchorPair.second.isEmpty())
                navigationLinks += protect(anchorPair.second);
            else
                navigationLinks += protect(linkPair.second);
            navigationLinks += "</a>]\n";
        }
        if (node->links().contains(Node::StartLink)) {
            linkPair = node->links()[Node::StartLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            if (!linkNode || linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);
            out() << "  <link rel=\"start\" href=\""
                  << anchorPair.first << "\" />\n";
        }
    }

#if 0 // Removed for new doc format. MWS
    if (node && !node->links().empty())
        out() << "<p>\n" << navigationLinks << "</p>\n";
#endif    
}

void HtmlGenerator::generateTitle(const QString& title,
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

void HtmlGenerator::generateFooter(const Node *node)
{
    if (node && !node->links().empty())
        out() << "<p>\n" << navigationLinks << "</p>\n";

    out() << QString(footer).replace("\\" + COMMAND_VERSION, myTree->version())
          << QString(address).replace("\\" + COMMAND_VERSION, myTree->version());
	
	    if (onlineDocs == true)
	    {
	    	    out() << "  <script src=\"scripts/functions.js\" type=\"text/javascript\"></script>\n";
	    	    out() << "  <!-- <script type=\"text/javascript\">\n";
	    	    out() << "  var _gaq = _gaq || [];\n";
	    	    out() << "  _gaq.push(['_setAccount', 'UA-4457116-5']);\n";
	    	    out() << "  _gaq.push(['_trackPageview']);\n";
	    	    out() << "  (function() {\n";
	    	    out() << "  var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;\n";
	    	    out() << "  ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';\n";
	    	    out() << "  var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);\n";
	    	    out() << "  })();\n";
	    	    out() << "  </script> -->\n";
	    	    out() << "</body>\n";
	    }	
	    else if (offlineDocs == true)
	    {
	    	    out() << "</body>\n";
	    }
	    else if (creatorDocs == true)
	    {
	    	    out() << "</body>\n";
	    }
	    else
	    {
	    	    out() << "</body>\n";
	    }
          out() <<   "</html>\n";
}

void HtmlGenerator::generateBrief(const Node *node, CodeMarker *marker,
                                  const Node *relative)
{
    Text brief = node->doc().briefText();
    if (!brief.isEmpty()) {
        generateExtractionMark(node, BriefMark);
        out() << "<p>";
        generateText(brief, node, marker);

        if (!relative || node == relative)
            out() << " <a href=\"#";
        else
            out() << " <a href=\"" << linkForNode(node, relative) << "#";
        out() << registerRef("details") << "\">More...</a></p>\n";


        generateExtractionMark(node, EndMark);
    }
}

void HtmlGenerator::generateIncludes(const InnerNode *inner, CodeMarker *marker)
{
    if (!inner->includes().isEmpty()) {
        out() << "<pre class=\"highlightedCode brush: cpp\">"
              << trimmedTrailing(highlightedCode(indent(codeIndent,
                                                        marker->markedUpIncludes(inner->includes())),
                                                 marker,inner))
              << "</pre>";
    }
}

/*!
  Generates a table of contents begining at \a node.
 */
void HtmlGenerator::generateTableOfContents(const Node *node,
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
        out() << "<a href=\""
              << nodeName
              << "#"
              << Doc::canonicalTitle(headingText.toString())
              << "\">";
        generateAtomList(headingText.firstAtom(), node, marker, true, numAtoms);
        out() << "</a></li>\n";

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
void HtmlGenerator::generateTableOfContents(const Node *node,
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
    out() << "<h3><a name=\"toc\">Contents</a></h3>\n";
    sectionNumber.append("1");
    out() << "<ul>\n";

    if (node->subType() == Node::Module) {
        if (moduleNamespaceMap.contains(node->name())) {
            out() << "<li class=\"level"
                  << sectionNumber.size()
                  << "\"><a href=\"#"
                  << registerRef("namespaces")
                  << "\">Namespaces</a></li>\n";
        }
        if (moduleClassMap.contains(node->name())) {
            out() << "<li class=\"level"
                  << sectionNumber.size()
                  << "\"><a href=\"#"
                  << registerRef("classes")
                  << "\">Classes</a></li>\n";
        }
        out() << "<li class=\"level"
              << sectionNumber.size()
              << "\"><a href=\"#"
              << registerRef("details")
              << "\">Detailed Description</a></li>\n";
        for (int i = 0; i < toc.size(); ++i) {
            if (toc.at(i)->string().toInt() == 1) {
                detailsBase = 1;
                break;
            }
        }
    }
    else if (sections && ((node->type() == Node::Class) ||
                          (node->type() == Node::Namespace) ||
                          (node->subType() == Node::QmlClass))) {
        QList<Section>::ConstIterator s = sections->begin();
        while (s != sections->end()) {
            if (!s->members.isEmpty() || !s->reimpMembers.isEmpty()) {
                out() << "<li class=\"level"
                      << sectionNumber.size()
                      << "\"><a href=\"#"
                      << registerRef((*s).pluralMember)
                      << "\">" << (*s).name
                      << "</a></li>\n";
            }
            ++s;
        }
        out() << "<li class=\"level"
              << sectionNumber.size()
              << "\"><a href=\"#"
              << registerRef("details")
              << "\">Detailed Description</a></li>\n";
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
        out() << "<a href=\""
              << "#"
              << Doc::canonicalTitle(s)
              << "\">";
        generateAtomList(headingText.firstAtom(), node, marker, true, numAtoms);
        out() << "</a></li>\n";
    }
    while (!sectionNumber.isEmpty()) {
        sectionNumber.removeLast();
    }
    out() << "</ul>\n";
    out() << "</div>\n";
    inContents = false;
    inLink = false;
}

#if 0
void HtmlGenerator::generateNavigationBar(const NavigationBar& bar,
                                           const Node *node,
                                           CodeMarker *marker)
{
    if (bar.prev.begin() != 0 || bar.current.begin() != 0 ||
         bar.next.begin() != 0) {
        out() << "<p class=\"rightAlign\">";
        if (bar.prev.begin() != 0) {
#if 0
            out() << "[<a href=\"" << section.previousBaseName()
                  << ".html\">Prev: ";
            generateText(section.previousHeading(), node, marker);
            out() << "</a>]\n";
#endif
        }
		if (fake->name() != QString("index.html"))
			{
        if (bar.current.begin() != 0) {
            out() << "[<a href=\"" << "home"
                  << ".html\">Home</a>]\n";
        }
        if (bar.next.begin() != 0) {
            out() << "[<a href=\"" << fileBase(node, bar.next)
                  << ".html\">Next: ";
            generateText(Text::sectionHeading(bar.next.begin()), node, marker);
            out() << "</a>]\n";
        }
        out() << "</p>\n";
		}
    }
}
#endif

QString HtmlGenerator::generateListOfAllMemberFile(const InnerNode *inner,
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
    generateHeader(title, inner, marker);
    generateTitle(title, Text(), SmallSubTitle, inner, marker);
    out() << "<p>This is the complete list of members for ";
    generateFullName(inner, 0, marker);
    out() << ", including inherited members.</p>\n";

    Section section = sections.first();
    generateSectionList(section, 0, marker, CodeMarker::SeparateList);

    generateFooter();
    endSubPage();
    return fileName;
}

QString HtmlGenerator::generateLowStatusMemberFile(const InnerNode *inner,
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
    generateHeader(title, inner, marker);
    generateTitle(title, Text(), SmallSubTitle, inner, marker);

    if (status == CodeMarker::Compat) {
        out() << "<p><b>The following class members are part of the "
                 "<a href=\"qt3support.html\">Qt 3 support layer</a>.</b> "
                 "They are provided to help you port old code to Qt 4. We advise against "
                 "using them in new code.</p>\n";
    }
    else {
        out() << "<p><b>The following class members are obsolete.</b> "
              << "They are provided to keep old source code working. "
              << "We strongly advise against using them in new code.</p>\n";
    }

    out() << "<p><ul><li><a href=\""
          << linkForNode(inner, 0) << "\">"
          << protectEnc(inner->name())
          << " class reference</a></li></ul></p>\n";

    for (i = 0; i < sections.size(); ++i) {
        out() << "<h2>" << protectEnc(sections.at(i).name) << "</h2>\n";
        generateSectionList(sections.at(i), inner, marker, CodeMarker::Summary);
    }

    sections = marker->sections(inner, CodeMarker::Detailed, status);
    for (i = 0; i < sections.size(); ++i) {
        //out() << "<hr />\n";
        out() << "<h2>" << protectEnc(sections.at(i).name) << "</h2>\n";

        NodeList::ConstIterator m = sections.at(i).members.begin();
        while (m != sections.at(i).members.end()) {
            if ((*m)->access() != Node::Private)
                generateDetailedMember(*m, inner, marker);
            ++m;
        }
    }

    generateFooter();
    endSubPage();
    return fileName;
}

void HtmlGenerator::generateClassHierarchy(const Node *relative,
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

void HtmlGenerator::generateAnnotatedList(const Node *relative,
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
        out() << "<td class=\"tblName\"><p>";
        generateFullName(node, relative, marker);
        out() << "</p></td>";

        if (!(node->type() == Node::Fake)) {
            Text brief = node->doc().trimmedBriefText(name);
            if (!brief.isEmpty()) {
                out() << "<td class=\"tblDescr\"><p>";
                generateText(brief, node, marker);
                out() << "</p></td>";
            }
        }
        else {
            out() << "<td class=\"tblDescr\"><p>";
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
void HtmlGenerator::generateCompactList(const Node *relative,
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
                out() << QString("<a href=\"#%1\">%2</a>&nbsp;").arg(ch).arg(ch.toUpper());
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
            out() << "<a href=\"" << linkForNode(it.value(), relative) << "\">";
            
            QStringList pieces;
            if (it.value()->subType() == Node::QmlClass)
                pieces << it.value()->name();
            else
                pieces = fullName(it.value(), relative, marker).split("::");
            out() << protectEnc(pieces.last());
            out() << "</a>";
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

void HtmlGenerator::generateFunctionIndex(const Node *relative,
                                          CodeMarker *marker)
{
    out() << "<p  class=\"centerAlign functionIndex\"><b>";
    for (int i = 0; i < 26; i++) {
        QChar ch('a' + i);
        out() << QString("<a href=\"#%1\">%2</a>&nbsp;").arg(ch).arg(ch.toUpper());
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

void HtmlGenerator::generateLegaleseList(const Node *relative,
                                         CodeMarker *marker)
{
    QMap<Text, const Node *>::ConstIterator it = legaleseTexts.begin();
    while (it != legaleseTexts.end()) {
        Text text = it.key();
        //out() << "<hr />\n";
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

#ifdef QDOC_QML
void HtmlGenerator::generateQmlItem(const Node *node,
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
    out() << highlightedCode(marked, marker, relative, false, node);
}
#endif

void HtmlGenerator::generateOverviewList(const Node *relative, CodeMarker * /* marker */)
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
            out() << QString("<h3><a href=\"%1\">%2</a></h3>\n").arg(
                        linkForNode(groupNode, relative)).arg(
                        protectEnc(groupNode->fullTitle()));

            if (fakeNodeMap[groupNode].count() == 0)
                continue;

            out() << "<ul>\n";

            foreach (const FakeNode *fakeNode, fakeNodeMap[groupNode]) {
                QString title = fakeNode->fullTitle();
                if (title.startsWith("The "))
                    title.remove(0, 4);
                out() << "<li><a href=\"" << linkForNode(fakeNode, relative) << "\">"
                      << protectEnc(title) << "</a></li>\n";
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
            out() << "<li><a href=\"" << linkForNode(fakeNode, relative) << "\">"
                  << protectEnc(title) << "</a></li>\n";
        }
        out() << "</ul>\n";
    }
}

void HtmlGenerator::generateSection(const NodeList& nl,
                                    const Node *relative,
                                    CodeMarker *marker,
                                    CodeMarker::SynopsisStyle style)
{
    bool alignNames = true;
    if (!nl.isEmpty()) {
        bool twoColumn = false;
        if (style == CodeMarker::SeparateList) {
            alignNames = false;
            twoColumn = (nl.count() >= 16);
        }
        else if (nl.first()->type() == Node::Property) {
            twoColumn = (nl.count() >= 5);
            alignNames = false;
        }
        if (alignNames) {
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

            if (alignNames) {
                out() << "<tr><td class=\"memItemLeft rightAlign topAlign\"> ";
            }
            else {
                if (twoColumn && i == (int) (nl.count() + 1) / 2)
                    out() << "</ul></td><td  class=\"topAlign\"><ul>\n";
                out() << "<li class=\"fn\">";
            }

            generateSynopsis(*m, relative, marker, style, alignNames);
            if (alignNames)
                out() << "</td></tr>\n";
            else
                out() << "</li>\n";
            i++;
            ++m;
        }
        if (alignNames)
            out() << "</table>\n";
        else {
            out() << "</ul>\n";
            if (twoColumn)
                out() << "</td></tr>\n</table>\n";
        }
    }
}

void HtmlGenerator::generateSectionList(const Section& section,
                                        const Node *relative,
                                        CodeMarker *marker,
                                        CodeMarker::SynopsisStyle style)
{
    bool alignNames = true;
    if (!section.members.isEmpty()) {
        bool twoColumn = false;
        if (style == CodeMarker::SeparateList) {
            alignNames = false;
            twoColumn = (section.members.count() >= 16);
        }
        else if (section.members.first()->type() == Node::Property) {
            twoColumn = (section.members.count() >= 5);
            alignNames = false;
        }
        if (alignNames) {
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

            if (alignNames) {
                out() << "<tr><td class=\"memItemLeft topAlign rightAlign\"> ";
            }
            else {
                if (twoColumn && i == (int) (section.members.count() + 1) / 2)
                    out() << "</ul></td><td class=\"topAlign\"><ul>\n";
                out() << "<li class=\"fn\">";
            }

            generateSynopsis(*m, relative, marker, style, alignNames);
            if (alignNames)
                out() << "</td></tr>\n";
            else
                out() << "</li>\n";
            i++;
            ++m;
        }
        if (alignNames)
            out() << "</table>\n";
        else {
            out() << "</ul>\n";
            if (twoColumn)
                out() << "</td></tr>\n</table>\n";
        }
    }

    if (style == CodeMarker::Summary && !section.inherited.isEmpty()) {
        out() << "<ul>\n";
        generateSectionInheritedList(section, relative, marker);
        out() << "</ul>\n";
    }
}

void HtmlGenerator::generateSectionInheritedList(const Section& section,
                                                 const Node *relative,
                                                 CodeMarker *marker)
{
    QList<QPair<ClassNode *, int> >::ConstIterator p = section.inherited.begin();
    while (p != section.inherited.end()) {
        out() << "<li class=\"fn\">";
        out() << (*p).second << " ";
        if ((*p).second == 1) {
            out() << section.singularMember;
        }
        else {
            out() << section.pluralMember;
        }
        out() << " inherited from <a href=\"" << fileName((*p).first)
              << "#" << HtmlGenerator::cleanRef(section.name.toLower()) << "\">"
              << protectEnc(marker->plainFullName((*p).first, relative))
              << "</a></li>\n";
        ++p;
    }
}

void HtmlGenerator::generateSynopsis(const Node *node,
                                     const Node *relative,
                                     CodeMarker *marker,
                                     CodeMarker::SynopsisStyle style,
                                     bool alignNames)
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
    out() << highlightedCode(marked, marker, relative, alignNames);
}

QString HtmlGenerator::highlightedCode(const QString& markedCode,
                                       CodeMarker* marker,
                                       const Node* relative,
                                       bool alignNames,
                                       const Node* self)
{
    QString src = markedCode;
    QString html;
    QStringRef arg;
    QStringRef par1;

    const QChar charLangle = '<';
    const QChar charAt = '@';

    static const QString typeTag("type");
    static const QString headerTag("headerfile");
    static const QString funcTag("func");
    static const QString linkTag("link");
    
    // replace all <@link> tags: "(<@link node=\"([^\"]+)\">).*(</@link>)"
    bool done = false;
    for (int i = 0, srcSize = src.size(); i < srcSize;) {
        if (src.at(i) == charLangle && src.at(i + 1).unicode() == '@') {
            if (alignNames && !done) {// && (i != 0)) Why was this here?
                html += "</td><td class=\"memItemRight bottomAlign\">";
                done = true;
            }
            i += 2;
            if (parseArg(src, linkTag, &i, srcSize, &arg, &par1)) {
                html += "<b>";
                const Node* n = CodeMarker::nodeForString(par1.toString());
                QString link = linkForNode(n, relative);
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
        for (int i = 0, srcSize = src.size(); i < srcSize;) {
            if (src.at(i) == charLangle && src.at(i + 1) == charAt) {
                i += 2;
                if (parseArg(src, funcTag, &i, srcSize, &arg, &par1)) {
                    const Node* n = marker->resolveTarget(par1.toString(),
                                                          myTree,
                                                          relative);
                    QString link = linkForNode(n, relative);
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

    for (int i=0, srcSize=src.size(); i<srcSize;) {
        if (src.at(i) == charLangle && src.at(i+1) == charAt) {
            i += 2;
            bool handled = false;
            if (parseArg(src, typeTag, &i, srcSize, &arg, &par1)) {
                par1 = QStringRef();
                const Node* n = marker->resolveTarget(arg.toString(), myTree, relative, self);
                addLink(linkForNode(n,relative), arg, &html);
                handled = true;
            }
            else if (parseArg(src, headerTag, &i, srcSize, &arg, &par1)) {
                par1 = QStringRef();
                const Node* n = marker->resolveTarget(arg.toString(), myTree, relative);
                addLink(linkForNode(n,relative), arg, &html);
                handled = true;
            }
            else if (parseArg(src, funcTag, &i, srcSize, &arg, &par1)) {
                par1 = QStringRef();
                const Node* n = marker->resolveTarget(arg.toString(), myTree, relative);
                addLink(linkForNode(n,relative), arg, &html);
                handled = true;
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

void HtmlGenerator::generateLink(const Atom* atom,
                                 const Node* /* relative */,
                                 CodeMarker* marker)
{
    static QRegExp camelCase("[A-Z][A-Z][a-z]|[a-z][A-Z0-9]|_");

    if (funcLeftParen.indexIn(atom->string()) != -1 && marker->recognizeLanguage("Cpp")) {
        // hack for C++: move () outside of link
        int k = funcLeftParen.pos(1);
        out() << protectEnc(atom->string().left(k));
        if (link.isEmpty()) {
            if (showBrokenLinks)
                out() << "</i>";
        } else {
            out() << "</a>";
        }
        inLink = false;
        out() << protectEnc(atom->string().mid(k));
    } else if (marker->recognizeLanguage("Java")) {
	// hack for Java: remove () and use <tt> when appropriate
        bool func = atom->string().endsWith("()");
        bool tt = (func || atom->string().contains(camelCase));
        if (tt)
            out() << "<tt>";
        if (func) {
            out() << protectEnc(atom->string().left(atom->string().length() - 2));
        } else {
            out() << protectEnc(atom->string());
        }
        out() << "</tt>";
    } else {
        out() << protectEnc(atom->string());
    }
}

QString HtmlGenerator::cleanRef(const QString& ref)
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

QString HtmlGenerator::registerRef(const QString& ref)
{
    QString clean = HtmlGenerator::cleanRef(ref);

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

QString HtmlGenerator::protectEnc(const QString &string)
{
    return protect(string, outputEncoding);
}

QString HtmlGenerator::protect(const QString &string, const QString &outputEncoding)
{
#define APPEND(x) \
    if (html.isEmpty()) { \
        html = string; \
        html.truncate(i); \
    } \
    html += (x);

    QString html;
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
            html += QString::number(ch.unicode(), 16);
            html += QLatin1Char(';');
        } else {
            if (!html.isEmpty())
                html += ch;
        }
    }

    if (!html.isEmpty())
        return html;
    return string;

#undef APPEND
}

QString HtmlGenerator::fileBase(const Node *node)
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
QString HtmlGenerator::fileBase(const Node *node,
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

QString HtmlGenerator::fileName(const Node *node)
{
    if (node->type() == Node::Fake) {
        if (static_cast<const FakeNode *>(node)->subType() == Node::ExternalPage)
            return node->name();
        if (static_cast<const FakeNode *>(node)->subType() == Node::Image)
            return node->name();
    }
    return PageGenerator::fileName(node);
}

QString HtmlGenerator::refForNode(const Node *node)
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

QString HtmlGenerator::linkForNode(const Node *node, const Node *relative)
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
/*    if (!node->url().isEmpty())
        return fn;*/
#if 0
    // ### reintroduce this test, without breaking .dcf files
    if (fn != outFileName())
#endif
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

QString HtmlGenerator::refForAtom(Atom *atom, const Node * /* node */)
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

void HtmlGenerator::generateFullName(const Node *apparentNode,
                                     const Node *relative,
                                     CodeMarker *marker,
                                     const Node *actualNode)
{
    if (actualNode == 0)
        actualNode = apparentNode;
    out() << "<a href=\"" << linkForNode(actualNode, relative);
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
    out() << "</a>";
}

void HtmlGenerator::generateDetailedMember(const Node *node,
                                           const InnerNode *relative,
                                           CodeMarker *marker)
{
    const EnumNode *enume;

#ifdef GENERATE_MAC_REFS    
    generateMacRef(node, marker);
#endif    
    generateExtractionMark(node, MemberMark);
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
        out() << "</h3>" << divNavTop << "\n";
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
                  << "<a href=\"qflags.html\">QFlags</a>&lt;"
                  << protectEnc(enume->name())
                  << "&gt;. It stores an OR combination of "
                  << protectEnc(enume->name())
                  << " values.</p>\n";
        }
    }
    generateAlsoList(node, marker);
    generateExtractionMark(node, EndMark);
}

void HtmlGenerator::findAllClasses(const InnerNode *node)
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
void HtmlGenerator::findAllSince(const InnerNode *node)
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

void HtmlGenerator::findAllFunctions(const InnerNode *node)
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

void HtmlGenerator::findAllLegaleseTexts(const InnerNode *node)
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

void HtmlGenerator::findAllNamespaces(const InnerNode *node)
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

int HtmlGenerator::hOffset(const Node *node)
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

bool HtmlGenerator::isThreeColumnEnumValueTable(const Atom *atom)
{
    while (atom != 0 && !(atom->type() == Atom::ListRight && atom->string() == ATOM_LIST_VALUE)) {
        if (atom->type() == Atom::ListItemLeft && !matchAhead(atom, Atom::ListItemRight))
            return true;
        atom = atom->next();
    }
    return false;
}

const Node *HtmlGenerator::findNodeForTarget(const QString &target,
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

const QPair<QString,QString> HtmlGenerator::anchorForNode(const Node *node)
{
    QPair<QString,QString> anchorPair;

    anchorPair.first = PageGenerator::fileName(node);
    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode*>(node);
        anchorPair.second = fakeNode->title();
    }

    return anchorPair;
}

QString HtmlGenerator::getLink(const Atom *atom,
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

void HtmlGenerator::generateDcf(const QString &fileBase,
                                const QString &startPage,
                                const QString &title,
                                DcfSection &dcfRoot)
{
    dcfRoot.ref = startPage;
    dcfRoot.title = title;
    generateDcfSections(dcfRoot, outputDir() + "/" + fileBase + ".dcf", fileBase + "/reference");
}

void HtmlGenerator::generateIndex(const QString &fileBase,
                                  const QString &url,
                                  const QString &title)
{
    myTree->generateIndex(outputDir() + "/" + fileBase + ".index", url, title);
}

void HtmlGenerator::generateStatus(const Node *node, CodeMarker *marker)
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
void HtmlGenerator::generateMacRef(const Node *node, CodeMarker *marker)
{
    if (!pleaseGenerateMacRef || marker == 0)
        return;

    QStringList macRefs = marker->macRefsForNode(node);
    foreach (const QString &macRef, macRefs)
        out() << "<a name=\"" << "//apple_ref/" << macRef << "\"></a>\n";
}
#endif

void HtmlGenerator::beginLink(const QString &link,
                              const Node *node,
                              const Node *relative,
                              CodeMarker *marker)
{
    Q_UNUSED(marker)
    Q_UNUSED(relative)

    this->link = link;
    if (link.isEmpty()) {
        if (showBrokenLinks)
            out() << "<i>";
    }
    else if (node == 0 || (relative != 0 &&
                           node->status() == relative->status())) {
        out() << "<a href=\"" << link << "\">";
    }
    else {
        switch (node->status()) {
        case Node::Obsolete:
            out() << "<a href=\"" << link << "\" class=\"obsolete\">";
            break;
        case Node::Compat:
            out() << "<a href=\"" << link << "\" class=\"compat\">";
            break;
        default:
            out() << "<a href=\"" << link << "\">";
        }
    }
    inLink = true;
}

void HtmlGenerator::endLink()
{
    if (inLink) {
        if (link.isEmpty()) {
            if (showBrokenLinks)
                out() << "</i>";
        }
        else {
            if (inObsoleteLink) {
                out() << "<sup>(obsolete)</sup>";
            }
            out() << "</a>";
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
void HtmlGenerator::generateQmlSummary(const Section& section,
                                       const Node *relative,
                                       CodeMarker *marker)
{
    if (!section.members.isEmpty()) {
        NodeList::ConstIterator m;
        int count = section.members.size();
        bool twoColumn = false;
        if (section.members.first()->type() == Node::QmlProperty) {
            twoColumn = (count >= 5);
            twoColumn = false;
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
void HtmlGenerator::generateDetailedQmlMember(const Node *node,
                                              const InnerNode *relative,
                                              CodeMarker *marker)
{
    const QmlPropertyNode* qpn = 0;
#ifdef GENERATE_MAC_REFS    
    generateMacRef(node, marker);
#endif    
    generateExtractionMark(node, MemberMark);
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
				
                out() << "<td class=\"tblQmlPropNode\"><p>";

                out() << "<a name=\"" + refForNode(qpn) + "\"></a>";

                if (!qpn->isWritable(myTree)) {
                    out() << "<span class=\"qmlreadonly\">read-only</span>";
                }
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
        out() << "<td class=\"tblQmlFuncNode\"><p>";
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
        out() << "<td class=\"tblQmlFuncNode\"><p>";
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
    generateExtractionMark(node, EndMark);
}

/*!
  Output the "Inherits" line for the QML element,
  if there should be one.
 */
void HtmlGenerator::generateQmlInherits(const QmlClassNode* cn,
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
                Text text;
                text << Atom::ParaLeft << "Inherits ";
                text << Atom(Atom::LinkNode,CodeMarker::stringForNode(qcn));
                text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
                text << Atom(Atom::String, linkPair.second);
                text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
                text << Atom::ParaRight;
                generateText(text, cn, marker);
            }
        }
    }
}

/*!
  Output the "Inherit by" list for the QML element,
  if it is inherited by any other elements.
 */
void HtmlGenerator::generateQmlInheritedBy(const QmlClassNode* cn,
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
void HtmlGenerator::generateQmlInstantiates(const QmlClassNode* qcn,
                                            CodeMarker* marker)
{
    const ClassNode* cn = qcn->classNode();
    if (cn && (cn->status() != Node::Internal)) {
        Text text;
        text << Atom::ParaLeft;
        text << Atom(Atom::LinkNode,CodeMarker::stringForNode(qcn));
        text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
        QString name = qcn->name();
        if (name.startsWith(QLatin1String("QML:")))
            name = name.mid(4); // remove the "QML:" prefix
        text << Atom(Atom::String, name);
        text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
        text << " instantiates the C++ class ";
        text << Atom(Atom::LinkNode,CodeMarker::stringForNode(cn));
        text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
        text << Atom(Atom::String, cn->name());
        text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
        text << Atom::ParaRight;
        generateText(text, qcn, marker);
    }
}

/*!
  Output the "[QmlGraphicsXxx is instantiated by QML element Xxx]"
  line for the class, if there should be one.

  If there is no QML element, or if the class node status
  is set to Node::Internal, do nothing. 
 */
void HtmlGenerator::generateInstantiatedBy(const ClassNode* cn,
                                           CodeMarker* marker)
{
    if (cn &&  cn->status() != Node::Internal && !cn->qmlElement().isEmpty()) {
        const Node* n = myTree->root()->findNode(cn->qmlElement(),Node::Fake);
        if (n && n->subType() == Node::QmlClass) {
            Text text;
            text << Atom::ParaLeft;
            text << Atom(Atom::LinkNode,CodeMarker::stringForNode(cn));
            text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
            text << Atom(Atom::String, cn->name());
            text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
            text << " is instantiated by QML element ";
            text << Atom(Atom::LinkNode,CodeMarker::stringForNode(n));
            text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
            text << Atom(Atom::String, n->name());
            text << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
            text << Atom::ParaRight;
            generateText(text, cn, marker);
        }
    }
}

/*!
  Generate the <page> element for the given \a node using the \a writer.
  Return true if a <page> element was written; otherwise return false.
 */
bool HtmlGenerator::generatePageElement(QXmlStreamWriter& writer,
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
void HtmlGenerator::generatePageElements(QXmlStreamWriter& writer, const Node* node, CodeMarker* marker) const
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
void HtmlGenerator::generatePageIndex(const QString& fileName, CodeMarker* marker) const
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

void HtmlGenerator::generateExtractionMark(const Node *node, ExtractionMarkType markType)
{
    if (markType != EndMark) {
        out() << "<!-- $$$" + node->name();
        if (markType == MemberMark) {
            if (node->type() == Node::Function) {
                const FunctionNode *func = static_cast<const FunctionNode *>(node);
                if (!func->associatedProperty()) {
                    if (func->overloadNumber() == 1)
                        out() << "[overload1]";
                    out() << "$$$" + func->name() + func->rawParameters().remove(' ');
                }
            } else if (node->type() == Node::Property) {
                out() << "-prop";
                const PropertyNode *prop = static_cast<const PropertyNode *>(node);
                const NodeList &list = prop->functions();
                foreach (const Node *propFuncNode, list) {
                    if (propFuncNode->type() == Node::Function) {
                        const FunctionNode *func = static_cast<const FunctionNode *>(propFuncNode);
                        out() << "$$$" + func->name() + func->rawParameters().remove(' ');
                    }
                }
            } else if (node->type() == Node::Enum) {
                const EnumNode *enumNode = static_cast<const EnumNode *>(node);
                foreach (const EnumItem &item, enumNode->items())
                    out() << "$$$" + item.name();
            }
        } else if (markType == BriefMark) {
            out() << "-brief";
        } else if (markType == DetailedDescriptionMark) {
            out() << "-description";
        }
        out() << " -->\n";
    } else {
        out() << "<!-- @@@" + node->name() + " -->\n";
    }
}

#endif

 QT_END_NAMESPACE
