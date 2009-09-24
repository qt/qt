#ifndef ENGINE_H
#define ENGINE_H

#include <QWidget>
#include <QtCore/qpointer.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlview.h>
#include <QtDeclarative/qmldebug.h>

QT_BEGIN_NAMESPACE

class QmlDebugConnection;
class QmlDebugPropertyReference;
class QmlDebugWatch;
class QmlObjectTree;
class EngineClientPlugin;
class PropertyView;
class WatchTableModel;
class WatchTableView;
class QLineEdit;
class QModelIndex;
class QTreeWidget;
class QTreeWidgetItem;
class QTabWidget;
class QTableWidget;
class QTableView;
class QTableWidgetItem;

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

    void itemClicked(QTreeWidgetItem *);
    void showProperties();
    void addExpressionWatch(int debugId, const QString &expr);

    void valueChanged(const QByteArray &property, const QVariant &value);

    void propertyDoubleClicked(const QmlDebugPropertyReference &property);
    void propertyWatchStateChanged();
    void watchedItemActivated(const QModelIndex &index);
    void stopWatching(int column);

private:
    void dump(const QmlDebugContextReference &, int);
    void dump(const QmlDebugObjectReference &, int);
    void buildTree(const QmlDebugObjectReference &, QTreeWidgetItem *parent);

    QmlEngineDebug m_client;
    QmlDebugEnginesQuery *m_engines;
    QmlDebugRootContextQuery *m_context;
    QmlDebugObjectQuery *m_object;

    QLineEdit *m_text;
    QmlObjectTree *m_objTree;
    QTabWidget *m_tabs;
    PropertyView *m_propView;
    WatchTableView *m_watchTable;

    QmlView *m_engineView;
    QList<QObject *> m_engineItems;

    QmlDebugWatch *m_watchedObject;
    WatchTableModel *m_watchTableModel;
};

QT_END_NAMESPACE

#endif // ENGINE_H

