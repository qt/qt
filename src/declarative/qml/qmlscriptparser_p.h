/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#ifndef QMLSCRIPTPARSER_P_H
#define QMLSCRIPTPARSER_P_H

#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtDeclarative/qmlerror.h>
#include "qmlparser_p.h"


QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QByteArray;

class QmlScriptParserJsASTData;
class QmlScriptParser
{
public:
    class Import
    {
    public:
        Import() {}

        QString uri;
        QmlParser::LocationSpan location;
    };

    QmlScriptParser();
    ~QmlScriptParser();

    bool parse(const QByteArray &data, const QUrl &url = QUrl());

    QMap<QString,QString> nameSpacePaths() const;
    QStringList types() const;

    QmlParser::Object *tree() const;
    QList<Import> imports() const;

    void clear();

    QList<QmlError> errors() const;

    QList<int> automaticSemicolonOffsets() const { return _automaticSemicolonOffsets; }
    void addAutomaticSemicolonOffset(int offset) { _automaticSemicolonOffsets.append(offset); }

// ### private:
    int findOrCreateTypeId(const QString &name);
    void setTree(QmlParser::Object *tree);

    void setScriptFile(const QString &filename) {_scriptFile = filename; }
    QString scriptFile() const { return _scriptFile; }

    void addNamespacePath(const QString &path);

// ### private:
    QList<QmlError> _errors;

    QMap<QString,QString> _nameSpacePaths;
    QmlParser::Object *root;
    QList<Import> _imports;
    QStringList _typeNames;
    QString _scriptFile;
    QmlScriptParserJsASTData *data;
    QList<int> _automaticSemicolonOffsets;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QMLSCRIPTPARSER_P_H
