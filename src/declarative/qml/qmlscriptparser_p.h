#ifndef QMLSCRIPTPARSER_P_H
#define QMLSCRIPTPARSER_P_H

#include <qml.h>
#include <private/qmlcomponent_p.h>
#include <private/qmlparser_p.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QByteArray;

class QmlScriptParser
{
public:
    QmlScriptParser();
    ~QmlScriptParser();

    bool parse(const QByteArray &data, const QUrl &url = QUrl());
    QString errorDescription() const;
    int errorLine() const;

    QMap<QString,QString> nameSpacePaths() const;
    QStringList types() const;

    QmlParser::Object *tree() const;

// ### private:
    int findOrCreateTypeId(const QString &name);
    void setTree(QmlParser::Object *tree);

    void setScriptFile(const QString &filename) {_scriptFile = filename; }
    QString scriptFile() const { return _scriptFile; }

    void addNamespacePath(const QString &path);

private:
    QMap<QString,QString> _nameSpacePaths;
    QmlParser::Object *root;
    QStringList _typeNames;
    QString _error;
    int _errorLine;
    QString _scriptFile;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QMLSCRIPTPARSER_P_H
