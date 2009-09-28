#include "propertyview.h"
#include <QtCore/qdebug.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qtreewidget.h>

QT_BEGIN_NAMESPACE

PropertyView::PropertyView(QWidget *parent)
: QWidget(parent), m_tree(0)
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

class PropertyViewItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    PropertyViewItem(QTreeWidget *widget);
    PropertyViewItem(QTreeWidgetItem *parent);

    QmlDebugPropertyReference property;
};

PropertyViewItem::PropertyViewItem(QTreeWidget *widget)
: QTreeWidgetItem(widget)
{
}

PropertyViewItem::PropertyViewItem(QTreeWidgetItem *parent)
: QTreeWidgetItem(parent)
{
}


void PropertyView::setObject(const QmlDebugObjectReference &object)
{
    m_object = object;
    m_tree->clear();

    QList<QmlDebugPropertyReference> properties = object.properties();
    for (int i=0; i<properties.count(); i++) {
        const QmlDebugPropertyReference &p = properties[i];

        PropertyViewItem *item = new PropertyViewItem(m_tree);
        item->property = p;

        item->setText(0, p.name());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        if (!p.hasNotifySignal())
            item->setForeground(0, Qt::lightGray);

        if (!p.binding().isEmpty()) {
            PropertyViewItem *binding = new PropertyViewItem(item);
            binding->setText(1, p.binding());
            binding->setForeground(1, Qt::darkGreen);
        }

        item->setExpanded(true);
    }

    m_tree->resizeColumnToContents(0);
}

const QmlDebugObjectReference &PropertyView::object() const
{
    return m_object;
}

void PropertyView::clear()
{
    setObject(QmlDebugObjectReference());
}

void PropertyView::updateProperty(const QString &name, const QVariant &value)
{
    for (int i=0; i<m_tree->topLevelItemCount(); i++) {
        PropertyViewItem *item = static_cast<PropertyViewItem *>(m_tree->topLevelItem(i));
        if (item->property.name() == name)
            item->setText(1, value.toString());
    }
}

void PropertyView::setPropertyIsWatched(const QString &name, bool watched)
{
    for (int i=0; i<m_tree->topLevelItemCount(); i++) {
        PropertyViewItem *item = static_cast<PropertyViewItem *>(m_tree->topLevelItem(i));
        if (item->property.name() == name && item->property.hasNotifySignal()) {
            QFont font = m_tree->font();
            font.setBold(watched);
            item->setFont(0, font);
        }
    }
}

void PropertyView::itemActivated(QTreeWidgetItem *i)
{
    PropertyViewItem *item = static_cast<PropertyViewItem *>(i);
    if (!item->property.name().isEmpty() && item->property.hasNotifySignal())
        emit propertyActivated(item->property);
}

QT_END_NAMESPACE

#include "propertyview.moc"
