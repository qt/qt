/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  main.cpp
*/

#include <qglobal.h>
#include <stdlib.h>
#include "codemarker.h"
#include "codeparser.h"
#include "config.h"
#include "cppcodemarker.h"
#include "cppcodeparser.h"
#include "ditaxmlgenerator.h"
#include "doc.h"
#include "htmlgenerator.h"
#include "jscodemarker.h"
#include "plaincodemarker.h"
#include "puredocparser.h"
#include "qmlcodemarker.h"
#include "qmlcodeparser.h"
#include "tokenizer.h"
#include "tree.h"
#include <qdebug.h>

#include "qtranslator.h"
#ifndef QT_BOOTSTRAPPED
#  include "qcoreapplication.h"
#endif

QT_BEGIN_NAMESPACE

/*
  The default indent for code is 4.
  The default value for false is 0.
  The default language is c++.
  The default output format is html.
  The default tab size is 8.
  And those are all the default values for configuration variables.
 */
static const struct {
    const char *key;
    const char *value;
} defaults[] = {
    { CONFIG_CODEINDENT, "4" },
    { CONFIG_FALSEHOODS, "0" },
    { CONFIG_LANGUAGE, "Cpp" },
    { CONFIG_OUTPUTFORMATS, "HTML" },
    { CONFIG_TABSIZE, "8" },
    { 0, 0 }
};

static bool slow = false;
static bool showInternal = false;
static bool obsoleteLinks = false;
static QStringList defines;
static QHash<QString, Tree *> trees;

/*!
  Print the help message to \c stdout.
 */
static void printHelp()
{
    Location::information(tr("Usage: qdoc [options] file1.qdocconf ...\n"
                              "Options:\n"
                              "    -help         "
                              "Display this information and exit\n"
                              "    -version      "
                              "Display version of qdoc and exit\n"
                              "    -D<name>      "
                              "Define <name> as a macro while parsing sources\n"
                              "    -slow         "
                              "Turn on features that slow down qdoc\n"
                              "    -showinternal "
                              "Include stuff marked internal\n"
                              "    -obsoletelinks "
                              "Report links from obsolete items to non-obsolete items") );
}

/*!
  Prints the qdoc version number to stdout.
 */
static void printVersion()
{
    QString s = tr("qdoc version %1").arg(QT_VERSION_STR);
    Location::information(s);
}

/*!
  Processes the qdoc config file \a fileName. This is the
  controller for all of qdoc.
 */
static void processQdocconfFile(const QString &fileName)
{
#ifndef QT_NO_TRANSLATION
    QList<QTranslator *> translators;
#endif

    /*
      The Config instance represents the configuration data for qdoc.
      All the other classes are initialized with the config. Here we
      initialize the configuration with some default values.
     */
    Config config(tr("qdoc"));
    int i = 0;
    while (defaults[i].key) {
	config.setStringList(defaults[i].key,
                             QStringList() << defaults[i].value);
	++i;
    }
    config.setStringList(CONFIG_SYNTAXHIGHLIGHTING, QStringList(slow ? "true" : "false"));
    config.setStringList(CONFIG_SHOWINTERNAL,
                         QStringList(showInternal ? "true" : "false"));
    config.setStringList(CONFIG_OBSOLETELINKS,
                         QStringList(obsoleteLinks ? "true" : "false"));

    /*
      With the default configuration values in place, load
      the qdoc configuration file. Note that the configuration
      file may include other configuration files.

      The Location class keeps track of the current location
      in the file being processed, mainly for error reporting
      purposes.
     */
    Location::initialize(config);
    config.load(fileName);

    /*
      Add the defines to the configuration variables.
     */
    QStringList defs = defines + config.getStringList(CONFIG_DEFINES);
    config.setStringList(CONFIG_DEFINES,defs);
    Location::terminate();

    QString prevCurrentDir = QDir::currentPath();
    QString dir = QFileInfo(fileName).path();
    if (!dir.isEmpty())
	QDir::setCurrent(dir);

    /*
      Initialize all the classes and data structures with the
      qdoc configuration.
     */
    Location::initialize(config);
    Tokenizer::initialize(config);
    Doc::initialize(config);
    CodeMarker::initialize(config);
    CodeParser::initialize(config);
    Generator::initialize(config);

#ifndef QT_NO_TRANSLATION
    /*
      Load the language translators, if the configuration specifies any.
     */
    QStringList fileNames = config.getStringList(CONFIG_TRANSLATORS);
    QStringList::Iterator fn = fileNames.begin();
    while (fn != fileNames.end()) {
	QTranslator *translator = new QTranslator(0);
	if (!translator->load(*fn))
	    config.lastLocation().error(tr("Cannot load translator '%1'")
					 .arg(*fn));
	QCoreApplication::instance()->installTranslator(translator);
	translators.append(translator);
	++fn;
    }
#endif

    //QSet<QString> outputLanguages = config.getStringSet(CONFIG_OUTPUTLANGUAGES);

    /*
      Get the source language (Cpp) from the configuration
      and the location in the configuration file where the
      source language was set.
     */
    QString lang = config.getString(CONFIG_LANGUAGE);
    Location langLocation = config.lastLocation();

    /*
      Initialize the tree where all the parsed sources will be stored.
      The tree gets built as the source files are parsed, and then the
      documentation output is generated by traversing the tree.
     */
    Tree *tree = new Tree;
    tree->setVersion(config.getString(CONFIG_VERSION));

    /*
      By default, the only output format is HTML.
     */
    QSet<QString> outputFormats = config.getStringSet(CONFIG_OUTPUTFORMATS);
    Location outputFormatsLocation = config.lastLocation();

    /*
      Read some XML indexes containing definitions from other documentation sets.
     */
    QStringList indexFiles = config.getStringList(CONFIG_INDEXES);
    tree->readIndexes(indexFiles);

#if 0
    /*
      I think we won't beusing this...

      Read the list of DITA excluded directories.
     */
    QSet<QString> ditaExcludedDirs;
    QStringList ditaExcludedDirsList = config.getStringList("dita.metadata.excludedirs");
    foreach (const QString &t, ditaExcludedDirsList)
        ditaExcludedDirs.insert(QDir::fromNativeSeparators(t));

    if (!ditaExcludedDirs.isEmpty()) {
        QSet<QString>::iterator i = ditaExcludedDirs.begin();
        while (i != ditaExcludedDirs.end()) {
            qDebug() << "DITA EXCLUDED DIR:" << (*i);
            ++i;
        }
    }
#endif

    /*
      Read the list of excluded directories.
     */
    QSet<QString> excludedDirs;
    QStringList excludedDirsList = config.getStringList(CONFIG_EXCLUDEDIRS);
    foreach (const QString &excludeDir, excludedDirsList)
        excludedDirs.insert(QDir::fromNativeSeparators(excludeDir));

    /*
      Get all the header files: "*.ch *.h *.h++ *.hh *.hpp *.hxx"
      Put them in a set.
     */
    QSet<QString> headers = QSet<QString>::fromList(
        config.getAllFiles(CONFIG_HEADERS, CONFIG_HEADERDIRS, excludedDirs));

    /*
      Get all the source text files: "*.cpp *.qdoc *.mm"
      Put them in a set.
     */
    QSet<QString> sources = QSet<QString>::fromList(
        config.getAllFiles(CONFIG_SOURCES, CONFIG_SOURCEDIRS, excludedDirs));

    /*
      Parse each header file in the set using the appropriate parser and add it
      to the big tree.
     */
    QSet<CodeParser *> usedParsers;

    QSet<QString>::ConstIterator h = headers.begin();
    while (h != headers.end()) {
        CodeParser *codeParser = CodeParser::parserForHeaderFile(*h);
        if (codeParser) {
	    codeParser->parseHeaderFile(config.location(), *h, tree);
            usedParsers.insert(codeParser);
        }
	++h;
    }

    foreach (CodeParser *codeParser, usedParsers)
        codeParser->doneParsingHeaderFiles(tree);

    usedParsers.clear();
    /*
      Parse each source text file in the set using the appropriate parser and
      add it to the big tree.
     */
    QSet<QString>::ConstIterator s = sources.begin();
    while (s != sources.end()) {
        CodeParser *codeParser = CodeParser::parserForSourceFile(*s);
        if (codeParser) {
	    codeParser->parseSourceFile(config.location(), *s, tree);
            usedParsers.insert(codeParser);
        }
	++s;
    }

    foreach (CodeParser *codeParser, usedParsers)
        codeParser->doneParsingSourceFiles(tree);

    /*
      Now the big tree has been built from all the header and
      source files. Resolve all the class names, function names,
      targets, URLs, links, and other stuff that needs resolving.
     */
    tree->resolveGroups();
    tree->resolveTargets();

    /*
      Now the tree has been built, and all the stuff that needed
      resolving has been resolved. Now it is time to traverse
      the big tree and generate the documentation output.
     */
    QSet<QString>::ConstIterator of = outputFormats.begin();
    while (of != outputFormats.end()) {
        Generator *generator = Generator::generatorForFormat(*of);
        if (generator == 0)
            outputFormatsLocation.fatal(tr("Unknown output format '%1'")
                                        .arg(*of));
        generator->generateTree(tree);
        ++of;
    }

    /*
      Generate the XML tag file, if it was requested.
     */
    QString tagFile = config.getString(CONFIG_TAGFILE);
    if (!tagFile.isEmpty()) {
        tree->generateTagFile(tagFile);
    }

    tree->setVersion("");
    Generator::terminate();
    CodeParser::terminate();
    CodeMarker::terminate();
    Doc::terminate();
    Tokenizer::terminate();
    Location::terminate();
    QDir::setCurrent(prevCurrentDir);

#ifndef QT_NO_TRANSLATION
    qDeleteAll(translators);
#endif
#ifdef DEBUG_SHUTDOWN_CRASH    
    qDebug() << "main(): Delete tree";
#endif    
    delete tree;
#ifdef DEBUG_SHUTDOWN_CRASH    
    qDebug() << "main(): Tree deleted";
#endif
}

QT_END_NAMESPACE

int main(int argc, char **argv)
{
    QT_USE_NAMESPACE

#ifndef QT_BOOTSTRAPPED
    QCoreApplication app(argc, argv);
#endif

    /*
      Create code parsers for the languages to be parsed,
      and create a tree for C++.
     */
    CppCodeParser cppParser;
    QmlCodeParser qmlParser;
    PureDocParser docParser;

    /*
      Create code markers for plain text and C++.
     */
    PlainCodeMarker plainMarker;
    CppCodeMarker cppMarker;
    JsCodeMarker jsMarker;
    QmlCodeMarker qmlMarker;

    HtmlGenerator htmlGenerator;
    DitaXmlGenerator ditaxmlGenerator;

    QStringList qdocFiles;
    QString opt;
    int i = 1;

    while (i < argc) {
        opt = argv[i++];

	if (opt == "-help") {
	    printHelp();
	    return EXIT_SUCCESS;
	}
        else if (opt == "-version") {
	    printVersion();
	    return EXIT_SUCCESS;
	}
        else if (opt == "--") {
	    while (i < argc)
		qdocFiles.append(argv[i++]);
        }
        else if (opt.startsWith("-D")) {
            QString define = opt.mid(2);
            defines += define;
        }
        else if (opt == "-slow") {
            slow = true;
	}
        else if (opt == "-showinternal") {
            showInternal = true;
        }
        else if (opt == "-obsoletelinks") {
            obsoleteLinks = true;
        }
        else {
	    qdocFiles.append(opt);
	}
    }

	if (qdocFiles.isEmpty()) {
        printHelp();
        return EXIT_FAILURE;
    }

    /*
      Main loop.
     */
    foreach (QString qf, qdocFiles) {
        //qDebug() << "PROCESSING:" << qf;
	processQdocconfFile(qf);
    }

    qDeleteAll(trees);
    return EXIT_SUCCESS;
}

