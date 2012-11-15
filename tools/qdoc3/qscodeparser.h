/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  qscodeparser.h
*/

#ifndef QSCODEPARSER_H
#define QSCODEPARSER_H

#include "cppcodeparser.h"
#include "cpptoqsconverter.h"

QT_BEGIN_NAMESPACE

class QsCodeParser : public CppCodeParser
{
 public:
    QsCodeParser(Tree *cppTree);
    ~QsCodeParser();

    virtual void initializeParser(const Config& config);
    virtual void terminateParser();
    virtual QString language();
    virtual QString headerFileNameFilter();
    virtual QString sourceFileNameFilter();
    virtual void parseHeaderFile(const Location& location,
                                 const QString& filePath, Tree *tree);
    virtual void parseSourceFile(const Location& location,
                                 const QString& filePath, Tree *tree);
    virtual void doneParsingHeaderFiles(Tree *tree);
    virtual void doneParsingSourceFiles(Tree *tree);

    FunctionNode *findFunctionNode(const QString& synopsis, Tree *tree);

 protected:
    virtual QSet<QString> topicCommands();
    virtual Node *processTopicCommand(const Doc& doc, const QString& command,
                                      const QString& arg);
    virtual QSet<QString> otherMetaCommands();
    virtual void processOtherMetaCommand(const Doc& doc,
                                         const QString& command,
                                         const QString& arg, Node *node);

 private:
    ClassNode *tryClass(const QString& className);
    FunctionNode *findKernelFunction(const QStringList& parentPath,
                                     const QString& name);
    void extractRegExp(const QRegExp& regExp, QString& source,
                       const Doc& doc);
    void extractTarget(const QString& target, QString& source,
                       const Doc& doc);
    void renameParameters(QString& source, const Doc& doc,
                          const QStringList& qtNames,
                          const QStringList& quickNames);
    void applyReplacementList(QString& source, const Doc& doc);
    void quickifyClass(ClassNode *quickClass);
    void quickifyEnum(ClassNode *quickClass, EnumNode *enume);
    void quickifyFunction(ClassNode *quickClass, ClassNode *qtClass,
                          FunctionNode *func, bool onBlackList);
    void quickifyProperty(ClassNode *quickClass, ClassNode *qtClass,
                          PropertyNode *property);
    QString quickifiedDoc(const QString& source);
    void setQtDoc(Node *quickNode, const Doc& doc);
    void setQuickDoc(Node *quickNode, const Doc& doc,
                     const QStringList& qtParams = QStringList(),
                     const QStringList& quickParams = QStringList());
    bool makeFunctionNode(const QString& synopsis, QStringList *parentPathPtr,
                          FunctionNode **funcPtr);

    static bool isWord(QChar ch);
    static bool leftWordBoundary(const QString& str, int pos);
    static bool rightWordBoundary(const QString& str, int pos);

    QMap<QString,Node::Type>    nodeTypeMap;
    QMap<QString,Node*>         classesWithNoQuickDoc;
    QList<QRegExp>              replaceBefores;
    QStringList                 replaceAfters;
    QSet<QString>               classesWithNoQ;
    Tree*                       cppTre;
    Tree*                       qsTre;
    QRegExp                     replaceRegExp;
    CppToQsConverter            cpp2qs;

    static int                  tabSize;
};

QT_END_NAMESPACE

#endif
