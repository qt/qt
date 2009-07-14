#ifndef ENGINE_H
#define ENGINE_H

#include <QWidget>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qfxview.h>
#include <QtDeclarative/qmldebug.h>

QT_BEGIN_NAMESPACE

class QmlDebugConnection;
class EngineClientPlugin;
class QLineEdit;
class EnginePane : public QWidget
{
Q_OBJECT
public:
    EnginePane(QmlDebugConnection *, QWidget *parent = 0);

private slots:
    void queryEngines();
    void enginesChanged();

    void queryContext(int);
    void contextChanged();

    void fetchClicked();
    void fetchObject(int);
    void objectFetched();

    void engineSelected(int);

private:
    void dump(const QmlDebugContextReference &, int);
    void dump(const QmlDebugObjectReference &, int);

    QmlEngineDebug m_client;
    QmlDebugEnginesQuery *m_engines;
    QmlDebugRootContextQuery *m_context;
    QmlDebugObjectQuery *m_object;

    QLineEdit *m_text;

    QFxView *m_engineView;
    QList<QObject *> m_engineItems;
};

QT_END_NAMESPACE

#endif // ENGINE_H

