#ifndef ENGINE_H
#define ENGINE_H

#include <QWidget>
#include <QtCore/qpointer.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qfxview.h>
#include <QtDeclarative/qmldebug.h>

QT_BEGIN_NAMESPACE

class QmlDebugConnection;
class QmlDebugPropertyReference;
class QmlDebugWatch;
class QmlObjectTree;
class EngineClientPlugin;
class WatchTableModel;
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

    void propertyDoubleClicked(QTableWidgetItem *);
    void watchedItemActivated(const QModelIndex &index);

private:
    void dump(const QmlDebugContextReference &, int);
    void dump(const QmlDebugObjectReference &, int);
    void buildTree(const QmlDebugObjectReference &, QTreeWidgetItem *parent);
    bool togglePropertyWatch(const QmlDebugObjectReference &object, const QmlDebugPropertyReference &property);

    QmlEngineDebug m_client;
    QmlDebugEnginesQuery *m_engines;
    QmlDebugRootContextQuery *m_context;
    QmlDebugObjectQuery *m_object;

    QLineEdit *m_text;
    QmlObjectTree *m_objTree;
    QTabWidget *m_tabs;
    QTableWidget *m_propTable;
    QTableView *m_watchTable;

    QFxView *m_engineView;
    QList<QObject *> m_engineItems;

    QmlDebugWatch *m_watchedObject;
    WatchTableModel *m_watchTableModel;
    QHash< QPair<int, QString>, QPointer<QmlDebugWatch> > m_watchedProps;
};

QT_END_NAMESPACE

#endif // ENGINE_H

