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
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QMLSCRIPTPARSER_P_H
