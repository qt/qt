#ifndef OBJECTTREE_H
#define OBJECTTREE_H

#include <QtGui/qtreewidget.h>

QT_BEGIN_NAMESPACE

class QTreeWidgetItem;

class QmlEngineDebug;
class QmlDebugObjectReference;
class QmlDebugObjectQuery;
class QmlDebugContextReference;


class ObjectTree : public QTreeWidget
{
    Q_OBJECT
public:
    ObjectTree(QmlEngineDebug *client, QWidget *parent = 0);

signals:
    void objectSelected(const QmlDebugObjectReference &);
    void expressionWatchRequested(const QmlDebugObjectReference &, const QString &);

public slots:
    void reload(int objectDebugId);
    void selectObject(int debugId);

protected:
    virtual void mousePressEvent(QMouseEvent *);

private slots:
    void objectFetched();
    void handleItemClicked(QTreeWidgetItem *);

private:
    QTreeWidgetItem *findItemByObjectId(int debugId) const;
    QTreeWidgetItem *findItem(QTreeWidgetItem *item, int debugId) const;
    void dump(const QmlDebugContextReference &, int);
    void dump(const QmlDebugObjectReference &, int);
    void buildTree(const QmlDebugObjectReference &, QTreeWidgetItem *parent);

    QmlEngineDebug *m_client;
    QmlDebugObjectQuery *m_query;
};

QT_END_NAMESPACE


#endif
