#ifndef PROPERTIESTABLEMODEL_H
#define PROPERTIESTABLEMODEL_H

#include <QtDeclarative/qmldebug.h>

#include <QtGui/qwidget.h>

QT_BEGIN_NAMESPACE

class QTreeWidget;
class QTreeWidgetItem;
class QmlDebugConnection;

class ObjectPropertiesView : public QWidget
{
    Q_OBJECT
public:
    ObjectPropertiesView(QmlEngineDebug *client = 0, QWidget *parent = 0);

    void setEngineDebug(QmlEngineDebug *client);
    
signals:
    void activated(const QmlDebugObjectReference &, const QmlDebugPropertyReference &);

public slots:
    void reload(const QmlDebugObjectReference &);
    void watchCreated(QmlDebugWatch *);

private slots:
    void queryFinished();
    void watchStateChanged();
    void valueChanged(const QByteArray &name, const QVariant &value);
    void itemActivated(QTreeWidgetItem *i);

private:
    void setObject(const QmlDebugObjectReference &object);
    void setWatched(const QString &property, bool watched);

    QmlEngineDebug *m_client;
    QmlDebugObjectQuery *m_query;
    QmlDebugWatch *m_watch;

    QTreeWidget *m_tree;
    QmlDebugObjectReference m_object;
};


QT_END_NAMESPACE

#endif
