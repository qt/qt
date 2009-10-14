#include <QtGui/qtreewidget.h>
#include <QtGui/qlayout.h>

#include <QtDeclarative/qmldebugservice.h>
#include <QtDeclarative/qmldebug.h>
#include <QtDeclarative/qmldebugclient.h>

#include "objectpropertiesview.h"

QT_BEGIN_NAMESPACE

class PropertiesViewItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    PropertiesViewItem(QTreeWidget *widget);
    PropertiesViewItem(QTreeWidgetItem *parent);

    QmlDebugPropertyReference property;
};

PropertiesViewItem::PropertiesViewItem(QTreeWidget *widget)
: QTreeWidgetItem(widget)
{
}

PropertiesViewItem::PropertiesViewItem(QTreeWidgetItem *parent)
: QTreeWidgetItem(parent)
{
}

ObjectPropertiesView::ObjectPropertiesView(QmlEngineDebug *client, QWidget *parent)
    : QWidget(parent),
      m_client(client),
      m_query(0),
      m_watch(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    m_tree = new QTreeWidget(this);
    m_tree->setExpandsOnDoubleClick(false);
    m_tree->setHeaderLabels(QStringList() << tr("Property") << tr("Value"));
    QObject::connect(m_tree, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
                     this, SLOT(itemActivated(QTreeWidgetItem *)));

    m_tree->setColumnCount(2);

    layout->addWidget(m_tree);
}

void ObjectPropertiesView::reload(const QmlDebugObjectReference &obj)
{
    m_query = m_client->queryObjectRecursive(obj, this);
    if (!m_query->isWaiting())
        queryFinished();
    else
        QObject::connect(m_query, SIGNAL(stateChanged(State)),
                         this, SLOT(queryFinished()));
}

void ObjectPropertiesView::queryFinished()
{
    if (m_watch) {
        m_client->removeWatch(m_watch);
        delete m_watch;
        m_watch = 0;
    }

    QmlDebugObjectReference obj = m_query->object();

    QmlDebugWatch *watch = m_client->addWatch(obj, this);
    if (watch->state() != QmlDebugWatch::Dead) {
        m_watch = watch;
        QObject::connect(watch, SIGNAL(valueChanged(QByteArray,QVariant)),
                        this, SLOT(valueChanged(QByteArray,QVariant)));
    }

    delete m_query;
    m_query = 0;

    setObject(obj);
}

void ObjectPropertiesView::setObject(const QmlDebugObjectReference &object)
{
    m_object = object;
    m_tree->clear();

    QList<QmlDebugPropertyReference> properties = object.properties();
    for (int i=0; i<properties.count(); i++) {
        const QmlDebugPropertyReference &p = properties[i];

        PropertiesViewItem *item = new PropertiesViewItem(m_tree);
        item->property = p;

        item->setText(0, p.name());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        if (!p.hasNotifySignal())
            item->setForeground(0, Qt::lightGray);

        if (!p.binding().isEmpty()) {
            PropertiesViewItem *binding = new PropertiesViewItem(item);
            binding->setText(1, p.binding());
            binding->setForeground(1, Qt::darkGreen);
        }

        item->setExpanded(true);
    }

    m_tree->resizeColumnToContents(0);
}

void ObjectPropertiesView::watchCreated(QmlDebugWatch *watch)
{
    if (watch->objectDebugId() == m_object.debugId()
            && qobject_cast<QmlDebugPropertyWatch*>(watch)) {
        connect(watch, SIGNAL(stateChanged(State)), SLOT(watchStateChanged()));
        setWatched(qobject_cast<QmlDebugPropertyWatch*>(watch)->name(), true);
    }
}

void ObjectPropertiesView::watchStateChanged()
{
    QmlDebugWatch *watch = qobject_cast<QmlDebugWatch*>(sender());

    if (watch->objectDebugId() == m_object.debugId()
            && qobject_cast<QmlDebugPropertyWatch*>(watch)
            && watch->state() == QmlDebugWatch::Inactive) {
        setWatched(qobject_cast<QmlDebugPropertyWatch*>(watch)->name(), false);
    }
}

void ObjectPropertiesView::setWatched(const QString &property, bool watched)
{
    for (int i=0; i<m_tree->topLevelItemCount(); i++) {
        PropertiesViewItem *item = static_cast<PropertiesViewItem *>(m_tree->topLevelItem(i));
        if (item->property.name() == property && item->property.hasNotifySignal()) {
            QFont font = m_tree->font();
            font.setBold(watched);
            item->setFont(0, font);
        }
    }
}

void ObjectPropertiesView::valueChanged(const QByteArray &name, const QVariant &value)
{
    for (int i=0; i<m_tree->topLevelItemCount(); i++) {
        PropertiesViewItem *item = static_cast<PropertiesViewItem *>(m_tree->topLevelItem(i));
        if (item->property.name() == name)
            item->setText(1, value.toString());
    }
}

void ObjectPropertiesView::itemActivated(QTreeWidgetItem *i)
{
    PropertiesViewItem *item = static_cast<PropertiesViewItem *>(i);
    if (!item->property.name().isEmpty() && item->property.hasNotifySignal())
        emit activated(m_object, item->property);
}

QT_END_NAMESPACE

#include "objectpropertiesview.moc"
