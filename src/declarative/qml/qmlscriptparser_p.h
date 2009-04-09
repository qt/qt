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

    QMap<QString,QString> nameSpacePaths() const;
    QStringList types() const;

    QmlParser::Object *tree() const;

// ### private:
    int findOrCreateTypeId(const QString &name);
    void setTree(QmlParser::Object *tree);

private:
    QMap<QString,QString> _nameSpacePaths;
    QmlParser::Object *root;
    QStringList _typeNames;
    QString _error;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QMLSCRIPTPARSER_P_H
