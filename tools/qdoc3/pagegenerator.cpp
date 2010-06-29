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
  pagegenerator.cpp
*/

#include <qfile.h>
#include <qfileinfo.h>
#include <qdebug.h>
#include "pagegenerator.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

/*!
  Nothing to do in the constructor.
 */
PageGenerator::PageGenerator()
{
    // nothing.
}

/*!
  The destructor 
 */
PageGenerator::~PageGenerator()
{
    while (!outStreamStack.isEmpty())
	endSubPage();
}

static QRegExp linkTag("(<@link node=\"([^\"]+)\">).*(</@link>)");
static QRegExp funcTag("(<@func target=\"([^\"]*)\">)(.*)(</@func>)");
static QRegExp typeTag("(<@(type|headerfile|func)(?: +[^>]*)?>)(.*)(</@\\2>)");
static QRegExp spanTag("</@(?:comment|preprocessor|string|char)>");
static QRegExp unknownTag("</?@[^>]*>");

bool PageGenerator::parseArg(const QString& src,
                             const QString& tag,
                             int* pos,
                             int n,
                             QStringRef* contents,
                             QStringRef* par1,
                             bool debug)
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

/*!
  This function is recursive.
 */
void PageGenerator::generateTree(const Tree *tree, CodeMarker *marker)
{
    generateInnerNode(tree->root(), marker);
}

QString PageGenerator::fileBase(const Node *node) const
{
    if (node->relates())
	node = node->relates();
    else if (!node->isInnerNode())
	node = node->parent();
#ifdef QDOC_QML
    if (node->subType() == Node::QmlPropertyGroup) {
        node = node->parent();
    }
#endif        

    QString base = node->doc().baseName();
    if (!base.isEmpty())
        return base;

    const Node *p = node;

    forever {
        const Node *pp = p->parent();
        base.prepend(p->name());
#ifdef QDOC_QML
        /*
          To avoid file name conflicts in the html directory,
          we prepend "qml-" to the file name of QML element doc
          files.
         */
        if ((p->subType() == Node::QmlClass) ||
            (p->subType() == Node::QmlBasicType)) {
            if (!base.startsWith(QLatin1String("QML:")))
                base.prepend("qml-");
        }
#endif        
        if (!pp || pp->name().isEmpty() || pp->type() == Node::Fake)
            break;
        base.prepend(QLatin1Char('-'));
        p = pp;
    }
    
    if (node->type() == Node::Fake) {
#ifdef QDOC2_COMPAT
        if (base.endsWith(".html"))
            base.truncate(base.length() - 5);
#endif
    }

    // the code below is effectively equivalent to:
    //   base.replace(QRegExp("[^A-Za-z0-9]+"), " ");
    //   base = base.trimmed();
    //   base.replace(" ", "-");
    //   base = base.toLower();
    // as this function accounted for ~8% of total running time
    // we optimize a bit...

    QString res;
    // +5 prevents realloc in fileName() below
    res.reserve(base.size() + 5);
    bool begun = false;
    for (int i = 0; i != base.size(); ++i) {
        QChar c = base.at(i);
        uint u = c.unicode();
        if (u >= 'A' && u <= 'Z')
            u -= 'A' - 'a';
        if ((u >= 'a' &&  u <= 'z') || (u >= '0' && u <= '9')) {
            res += QLatin1Char(u);
            begun = true;
        }
        else if (begun) {
            res += QLatin1Char('-');
            begun = false;
        }
    }
    while (res.endsWith(QLatin1Char('-')))
        res.chop(1);
    return res;
}

QString PageGenerator::fileName(const Node *node) const
{
    if (!node->url().isEmpty())
        return node->url();

    QString name = fileBase(node);
    name += QLatin1Char('.');
    name += fileExtension(node);
    return name;
}

QString PageGenerator::outFileName()
{
    return QFileInfo(static_cast<QFile *>(out().device())->fileName()).fileName();
}

void PageGenerator::beginSubPage(const Location& location,
                                 const QString& fileName)
{
    QFile *outFile = new QFile(outputDir() + "/" + fileName);
    if (!outFile->open(QFile::WriteOnly))
	location.fatal(tr("Cannot open output file '%1'")
			.arg(outFile->fileName()));
    QTextStream *out = new QTextStream(outFile);
    out->setCodec(outputCodec);
    outStreamStack.push(out);
}

void PageGenerator::endSubPage()
{
    outStreamStack.top()->flush();
    delete outStreamStack.top()->device();
    delete outStreamStack.pop();
}

QTextStream &PageGenerator::out()
{
    return *outStreamStack.top();
}

/*!
  Recursive writing of html files from the root \a node.
 */
void
PageGenerator::generateInnerNode(const InnerNode* node, CodeMarker* marker)
{
    if (!node->url().isNull())
        return;

    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode *>(node);
        if (fakeNode->subType() == Node::ExternalPage)
            return;
#ifdef QDOC_QML            
        if (fakeNode->subType() == Node::QmlPropertyGroup)
            return;
#endif            
        if (fakeNode->subType() == Node::Page) {
            if (node->count() > 0)
                qDebug("PAGE %s HAS CHILDREN", qPrintable(fakeNode->title()));
        }
    }

    if (node->parent() != 0) {
	beginSubPage(node->location(), fileName(node));
	if (node->type() == Node::Namespace || node->type() == Node::Class) {
	    generateClassLikeNode(node, marker);
	}
        else if (node->type() == Node::Fake) {
	    generateFakeNode(static_cast<const FakeNode *>(node), marker);
	}
	endSubPage();
    }

    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
	if ((*c)->isInnerNode() && (*c)->access() != Node::Private)
	    generateInnerNode((const InnerNode *) *c, marker);
	++c;
    }
}

QT_END_NAMESPACE
