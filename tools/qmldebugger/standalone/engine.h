#ifndef ENGINE_H
#define ENGINE_H

#include <QWidget>
#include <QtCore/qpointer.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlview.h>
#include <private/qmldebug_p.h>

QT_BEGIN_NAMESPACE

class ObjectPropertiesView;
class QmlDebugConnection;
class QmlDebugPropertyReference;
class QmlDebugWatch;
class ObjectTree;
class WatchTableModel;
class WatchTableView;
class ExpressionQueryWidget;

class QTabWidget;

class EnginePane : public QWidget
{
Q_OBJECT
public:
    EnginePane(QmlDebugConnection *, QWidget *parent = 0);

public slots:
    void refreshEngines();

private slots:
    void enginesChanged();

    void queryContext(int);
    void contextChanged();

    void engineSelected(int);

private:
    QmlEngineDebug *m_client;
    QmlDebugEnginesQuery *m_engines;
    QmlDebugRootContextQuery *m_context;

    ObjectTree *m_objTree;
    QTabWidget *m_tabs;
    WatchTableView *m_watchTableView;
    WatchTableModel *m_watchTableModel;
    ExpressionQueryWidget *m_exprQueryWidget;

    QmlView *m_engineView;
    QList<QObject *> m_engineItems;

    ObjectPropertiesView *m_propertiesView;
};

QT_END_NAMESPACE

#endif // ENGINE_H

