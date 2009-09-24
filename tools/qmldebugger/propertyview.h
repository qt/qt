#ifndef PROPERTYVIEW_H
#define PROPERTYVIEW_H

#include <QtGui/qwidget.h>
#include <QtCore/qpointer.h>
#include <QtDeclarative/qmldebug.h>

QT_BEGIN_NAMESPACE

class QTreeWidget;
class QTreeWidgetItem;

class PropertyView : public QWidget
{
    Q_OBJECT
public:
    PropertyView(QWidget *parent = 0);

    void setObject(const QmlDebugObjectReference &object);
    const QmlDebugObjectReference &object() const;

    void updateProperty(const QString &name, const QVariant &value);
    void setPropertyIsWatched(const QString &name, bool watched);

    void clear();

signals:
    void propertyDoubleClicked(const QmlDebugPropertyReference &property);

private slots:
    void itemDoubleClicked(QTreeWidgetItem *);

private:
    QmlDebugObjectReference m_object;
    QTreeWidget *m_tree;
};

QT_END_NAMESPACE

#endif // PROPERTYVIEW_H
