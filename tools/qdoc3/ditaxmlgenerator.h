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

#ifndef DITAXMLGENERATOR_H
#define DITAXMLGENERATOR_H

#include <qmap.h>
#include <qregexp.h>
#include <QXmlStreamWriter>
#include "codemarker.h"
#include "config.h"
#include "pagegenerator.h"

QT_BEGIN_NAMESPACE

typedef QMultiMap<QString, Node*> NodeMultiMap;
typedef QMap<QString, NodeMultiMap> NewSinceMaps;
typedef QMap<Node*, NodeMultiMap> ParentMaps;
typedef QMap<QString, const Node*> NodeMap;
typedef QMap<QString, NodeMap> NewClassMaps;

typedef QMap<QString, QString> GuidMap;
typedef QMap<QString, GuidMap*> GuidMaps;

class DitaXmlGenerator : public PageGenerator
{
 public:
    enum SinceType { 
        Namespace, 
        Class, 
        MemberFunction,
        NamespaceFunction,
        GlobalFunction,
        Macro,
        Enum, 
        Typedef, 
        Property,
        Variable, 
        QmlClass,
        QmlProperty,
        QmlSignal,
        QmlMethod,
        LastSinceType
    };

 public:
    DitaXmlGenerator();
    ~DitaXmlGenerator();

    virtual void initializeGenerator(const Config& config);
    virtual void terminateGenerator();
    virtual QString format();
    virtual bool canHandleFormat(const QString& format);
    virtual void generateTree(const Tree *tree);

    QString protectEnc(const QString& string);
    static QString protect(const QString& string, const QString& encoding = "ISO-8859-1");
    static QString cleanRef(const QString& ref);
    static QString sinceTitle(int i) { return sinceTitles[i]; }

 protected:
    virtual void startText(const Node* relative, CodeMarker* marker);
    virtual int generateAtom(const Atom* atom, 
                             const Node* relative,
                             CodeMarker* marker);
    virtual void generateClassLikeNode(const InnerNode* inner, CodeMarker* marker);
    virtual void generateFakeNode(const FakeNode* fake, CodeMarker* marker);
    virtual QString fileExtension(const Node* node) const;
    virtual QString refForNode(const Node* node);
    virtual QString guidForNode(const Node* node);
    virtual QString linkForNode(const Node* node, const Node* relative);
    virtual QString refForAtom(Atom* atom, const Node* node);
    
    void writeXrefListItem(const QString& link, const QString& text);
    QString fullQualification(const Node* n);

    void writeCharacters(const QString& text);
    void writeDerivations(const ClassNode* cn, CodeMarker* marker);
    void writeLocation(const Node* n);
    void writeFunctions(const Section& s, 
                        const Node* n, 
                        CodeMarker* marker,
                        const QString& attribute = QString());
    void writeNestedClasses(const Section& s, const Node* n);
    void writeParameters(const FunctionNode* fn);
    void writeEnumerations(const Section& s, 
                           CodeMarker* marker,
                           const QString& attribute = QString());
    void writeTypedefs(const Section& s, 
                       CodeMarker* marker,
                       const QString& attribute = QString());
    void writeDataMembers(const Section& s, 
                          CodeMarker* marker,
                          const QString& attribute = QString());
    void writeProperties(const Section& s, 
                         CodeMarker* marker,
                         const QString& attribute = QString());
    void writeMacros(const Section& s, 
                     CodeMarker* marker,
                     const QString& attribute = QString());
    void writePropertyParameter(const QString& tag, const NodeList& nlist);
    void writeRelatedLinks(const FakeNode* fake, CodeMarker* marker);
    void writeLink(const Node* node, const QString& tex, const QString& role);

 private:
    enum SubTitleSize { SmallSubTitle, LargeSubTitle };

    const QPair<QString,QString> anchorForNode(const Node* node);
    const Node* findNodeForTarget(const QString& target, 
                                  const Node* relative,
                                  CodeMarker* marker, 
                                  const Atom* atom = 0);
    void generateHeader(const Node* node, 
                        const QString& name,
                        bool subpage = false);
    void generateBrief(const Node* node, CodeMarker* marker);
    void generateIncludes(const InnerNode* inner, CodeMarker* marker);
    void generateTableOfContents(const Node* node, 
                                 CodeMarker* marker,
                                 Doc::SectioningUnit sectioningUnit,
                                 int numColumns, 
                                 const Node* relative = 0);
    void generateTableOfContents(const Node* node, 
                                 CodeMarker* marker, 
                                 QList<Section>* sections = 0);
    void generateLowStatusMembers(const InnerNode* inner,
                                  CodeMarker* marker,
                                  CodeMarker::Status status);
    QString generateLowStatusMemberFile(const InnerNode* inner, 
                                        CodeMarker* marker,
                                        CodeMarker::Status status);
    void generateClassHierarchy(const Node* relative, 
                                CodeMarker* marker,
				const NodeMap& classMap);
    void generateAnnotatedList(const Node* relative, 
                               CodeMarker* marker,
			       const NodeMap& nodeMap);
    void generateCompactList(const Node* relative, 
                             CodeMarker* marker,
			     const NodeMap& classMap,
                             bool includeAlphabet,
                             QString commonPrefix = QString());
    void generateFunctionIndex(const Node* relative, CodeMarker* marker);
    void generateLegaleseList(const Node* relative, CodeMarker* marker);
    void generateOverviewList(const Node* relative, CodeMarker* marker);

#ifdef QDOC_QML
    void generateQmlSummary(const Section& section,
                            const Node* relative,
                            CodeMarker* marker);
    void generateQmlItem(const Node* node,
                         const Node* relative,
                         CodeMarker* marker,
                         bool summary);
    void generateDetailedQmlMember(const Node* node,
                                   const InnerNode* relative,
                                   CodeMarker* marker);
    void generateQmlInherits(const QmlClassNode* cn, CodeMarker* marker);
    void generateQmlInheritedBy(const QmlClassNode* cn, CodeMarker* marker);
    void generateQmlInstantiates(const QmlClassNode* qcn, CodeMarker* marker);
    void generateInstantiatedBy(const ClassNode* cn, CodeMarker* marker);
#endif

    void generateSection(const NodeList& nl,
                         const Node* relative,
                         CodeMarker* marker,
                         CodeMarker::SynopsisStyle style);
    QString getMarkedUpSynopsis(const Node* node, 
                                const Node* relative, 
                                CodeMarker* marker,
                                CodeMarker::SynopsisStyle style);
    void generateSectionInheritedList(const Section& section, 
                                      const Node* relative,
                                      CodeMarker* marker);
    void writeText(const QString& markedCode, 
                   CodeMarker* marker, 
                   const Node* relative);

    void generateFullName(const Node* apparentNode, 
                          const Node* relative, 
                          CodeMarker* marker,
			  const Node* actualNode = 0);
    void generateLink(const Atom* atom, 
                      const Node* relative, 
                      CodeMarker* marker);
    void generateStatus(const Node* node, CodeMarker* marker);
    
    QString registerRef(const QString& ref);
    QString fileBase(const Node *node) const;
    QString fileName(const Node *node);
    void findAllClasses(const InnerNode *node);
    void findAllFunctions(const InnerNode *node);
    void findAllLegaleseTexts(const InnerNode *node);
    void findAllNamespaces(const InnerNode *node);
    void findAllSince(const InnerNode *node);
    static int hOffset(const Node *node);
    static bool isThreeColumnEnumValueTable(const Atom *atom);
    virtual QString getLink(const Atom *atom, 
                            const Node *relative, 
                            CodeMarker *marker, 
                            const Node **node);
    virtual void generateIndex(const QString& fileBase, 
                               const QString& url,
                               const QString& title);
#ifdef GENERATE_MAC_REFS    
    void generateMacRef(const Node* node, CodeMarker* marker);
#endif
    void beginLink(const QString& link); 
    void endLink();
    QString writeGuidAttribute(QString text);
    void writeGuidAttribute(Node* node);
    QString lookupGuid(QString text);
    QString lookupGuid(const QString& fileName, const QString& text);
    GuidMap* lookupGuidMap(const QString& fileName);
    virtual void beginSubPage(const Location& location, const QString& fileName);
    virtual void endSubPage();
    virtual void generateInnerNode(const InnerNode* node);
    QXmlStreamWriter& xmlWriter();
    void writeDetailedDescription(const Node* node,
                                  CodeMarker* marker,
                                  bool apiDesc,
                                  const QString& title);
    void addLink(const QString& href, const QStringRef& text);
    void writeDitaMap();

 private:
    QMap<QString, QString> refMap;
    QMap<QString, QString> name2guidMap;
    GuidMaps guidMaps;
    int codeIndent;
    bool inLink;
    bool inObsoleteLink;
    bool inContents;
    bool inSectionHeading;
    bool inTableHeader;
    bool inTableBody;
    int numTableRows;
    bool threeColumnEnumValueTable;
    bool offlineDocs;
    QString link;
    QStringList sectionNumber;
    QRegExp funcLeftParen;
    QString style;
    QString postHeader;
    QString postPostHeader;
    QString footer;
    QString address;
    bool pleaseGenerateMacRef;
    QString project;
    QString projectDescription;
    QString projectUrl;
    QString navigationLinks;
    QStringList stylesheets;
    QStringList customHeadElements;
    const Tree* myTree;
    bool obsoleteLinks;
    bool noLinks;
    int tableColumnCount;
    QMap<QString, NodeMap > moduleClassMap;
    QMap<QString, NodeMap > moduleNamespaceMap;
    NodeMap nonCompatClasses;
    NodeMap mainClasses;
    NodeMap compatClasses;
    NodeMap obsoleteClasses;
    NodeMap namespaceIndex;
    NodeMap serviceClasses;
#ifdef QDOC_QML    
    NodeMap qmlClasses;
#endif
    QMap<QString, NodeMap > funcIndex;
    QMap<Text, const Node*> legaleseTexts;
    NewSinceMaps newSinceMaps;
    static QString sinceTitles[];
    NewClassMaps newClassMaps;
    NewClassMaps newQmlClassMaps;
    static int id;
    static bool inApiDesc;
    static bool inSection;
    static bool inDetailedDescription;
    static bool inLegaleseText;

    QStack<QXmlStreamWriter*> xmlWriterStack;
};

#define DITAXMLGENERATOR_ADDRESS           "address"
#define DITAXMLGENERATOR_FOOTER            "footer"
#define DITAXMLGENERATOR_GENERATEMACREFS   "generatemacrefs" // ### document me
#define DITAXMLGENERATOR_POSTHEADER        "postheader"
#define DITAXMLGENERATOR_POSTPOSTHEADER    "postpostheader"
#define DITAXMLGENERATOR_STYLE             "style"
#define DITAXMLGENERATOR_STYLESHEETS       "stylesheets"
#define DITAXMLGENERATOR_CUSTOMHEADELEMENTS "customheadelements"

QT_END_NAMESPACE

#endif
