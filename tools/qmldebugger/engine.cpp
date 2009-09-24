#include "engine.h"
#include "propertyview.h"
#include "watchtablemodel.h"
#include <QtDeclarative/qmldebugclient.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTabWidget>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
#include <QInputDialog>
#include <QFile>
#include <QPointer>
#include <private/qmlenginedebug_p.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxitem.h>
#include <QtDeclarative/qmldebugservice.h>

QT_BEGIN_NAMESPACE


class QmlObjectTree : public QTreeWidget
{
    Q_OBJECT
public:
    enum AdditionalRoles {
        ContextIdRole = Qt::UserRole + 1
    };

    QmlObjectTree(QWidget *parent = 0);

    QTreeWidgetItem *findItemByObjectId(int debugId) const;

signals:
    void addExpressionWatch(int debugId, const QString &);

protected:
    virtual void mousePressEvent(QMouseEvent *);

private:
    QTreeWidgetItem *findItem(QTreeWidgetItem *item, int debugId) const;
};

QmlObjectTree::QmlObjectTree(QWidget *parent)
: QTreeWidget(parent)
{
}

QTreeWidgetItem *QmlObjectTree::findItemByObjectId(int debugId) const
{
    for (int i=0; i<topLevelItemCount(); i++) {
        QTreeWidgetItem *item = findItem(topLevelItem(i), debugId);
        if (item)
            return item;
    }

    return 0;
}

QTreeWidgetItem *QmlObjectTree::findItem(QTreeWidgetItem *item, int debugId) const
{
    if (item->data(0, Qt::UserRole).toInt() == debugId)
        return item;

    QTreeWidgetItem *child;
    for (int i=0; i<item->childCount(); i++) {
        child = findItem(item->child(i), debugId);
        if (child)
            return child;
    }

    return 0;
}

void QmlObjectTree::mousePressEvent(QMouseEvent *me)
{
    QTreeWidget::mousePressEvent(me);
    if (!currentItem())
        return;
    if(me->button()  == Qt::RightButton && me->type() == QEvent::MouseButtonPress) {
        QAction action(tr("Add watch..."), 0);
        QList<QAction *> actions;
        actions << &action;
        int debugId = currentItem()->data(0, Qt::UserRole).toInt();
        if (debugId >= 0 && QMenu::exec(actions, me->globalPos())) {
            bool ok = false;
            QString watch = QInputDialog::getText(this, tr("Watch expression"),
                    tr("Expression:"), QLineEdit::Normal, QString(), &ok);
            if (ok && !watch.isEmpty()) 
                emit addExpressionWatch(debugId, watch);
        }
    } 
}



class WatchTableView : public QTableView
{
    Q_OBJECT
public:
    WatchTableView(QWidget *parent);

signals:
    void stopWatching(int column);

protected:
    void mousePressEvent(QMouseEvent *me);
};

WatchTableView::WatchTableView(QWidget *parent)
    : QTableView(parent)
{
}

void WatchTableView::mousePressEvent(QMouseEvent *me)
{
    QTableView::mousePressEvent(me);
    if (me->button()  == Qt::RightButton && me->type() == QEvent::MouseButtonPress) {
        int col = columnAt(me->x());
        if (col >= 0) {
            QAction action(tr("Stop watching"), 0);
            QList<QAction *> actions;
            actions << &action;
            if (QMenu::exec(actions, me->globalPos()))
                emit stopWatching(col);
        }
    } 
}


class DebuggerEngineItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT);
    Q_PROPERTY(int engineId READ engineId CONSTANT);

public:
    DebuggerEngineItem(const QString &name, int id)
    : m_name(name), m_engineId(id) {}

    QString name() const { return m_name; }
    int engineId() const { return m_engineId; }

private:
    QString m_name;
    int m_engineId;
};

EnginePane::EnginePane(QmlDebugConnection *client, QWidget *parent)
: QWidget(parent), m_client(client), m_engines(0), m_context(0), m_object(0), m_watchedObject(0), m_watchTableModel(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);

    QFile enginesFile(":/engines.qml");
    enginesFile.open(QFile::ReadOnly);
    Q_ASSERT(enginesFile.isOpen());

    m_engineView = new QmlView(this);
    m_engineView->rootContext()->setContextProperty("engines", qVariantFromValue(&m_engineItems));
    m_engineView->setContentResizable(true);
    m_engineView->setQml(enginesFile.readAll());
    m_engineView->execute();
    m_engineView->setFixedHeight(100);
    QObject::connect(m_engineView->root(), SIGNAL(engineClicked(int)),
                     this, SLOT(engineSelected(int)));
    QObject::connect(m_engineView->root(), SIGNAL(refreshEngines()),
                     this, SLOT(queryEngines()));

    layout->addWidget(m_engineView);


    m_text = new QLineEdit(this);
    layout->addWidget(m_text);

    QPushButton *query = new QPushButton("Fetch object", this);
    QObject::connect(query, SIGNAL(clicked()), this, SLOT(fetchClicked()));
    layout->addWidget(query);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setContentsMargins(0, 0, 0, 0);

    m_objTree = new QmlObjectTree(this);
    m_objTree->setHeaderHidden(true);
    connect(m_objTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *)));
    connect(m_objTree, SIGNAL(addExpressionWatch(int,QString)), this, SLOT(addExpressionWatch(int,QString)));
    hbox->addWidget(m_objTree);

    m_propView = new PropertyView(this);
    connect(m_propView, SIGNAL(propertyDoubleClicked(QmlDebugPropertyReference)),
            this, SLOT(propertyDoubleClicked(QmlDebugPropertyReference)));

    m_watchTableModel = new WatchTableModel(this);
    m_watchTable = new WatchTableView(this);
    m_watchTable->setModel(m_watchTableModel);
    QObject::connect(m_watchTable, SIGNAL(activated(QModelIndex)),
                     this, SLOT(watchedItemActivated(QModelIndex)));
    QObject::connect(m_watchTable, SIGNAL(stopWatching(int)),
                     this, SLOT(stopWatching(int)));

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(m_propView, tr("Properties"));
    m_tabs->addTab(m_watchTable, tr("Watching"));

    hbox->addWidget(m_tabs);
    hbox->setStretchFactor(m_tabs, 2);

    layout->addLayout(hbox);
}

void EnginePane::engineSelected(int id)
{
    qWarning() << "Engine selected" << id;
    queryContext(id);
}

void EnginePane::itemClicked(QTreeWidgetItem *item)
{
    m_propView->clear();

    if (m_object) {
        delete m_object;
        m_object = 0;
    }

    m_object = m_client.queryObjectRecursive(QmlDebugObjectReference(item->data(0, Qt::UserRole).toInt()), this);
    if (!m_object->isWaiting())
        showProperties();
    else
        QObject::connect(m_object, SIGNAL(stateChanged(State)),
                         this, SLOT(showProperties()));
}

void EnginePane::showProperties()
{
    QmlDebugObjectReference obj = m_object->object();
    m_propView->setObject(obj);

    if (m_watchedObject) {
        m_client.removeWatch(m_watchedObject);
        delete m_watchedObject;
        m_watchedObject = 0;
    }

    QmlDebugWatch *watch = m_client.addWatch(obj, this);
    m_watchedObject = watch;
    QObject::connect(watch, SIGNAL(valueChanged(QByteArray,QVariant)),
                     this, SLOT(valueChanged(QByteArray,QVariant)));

    delete m_object; m_object = 0;
}

void EnginePane::addExpressionWatch(int debugId, const QString &expr)
{
    QmlDebugWatch *watch = m_client.addWatch(QmlDebugObjectReference(debugId), expr, this);

    QObject::connect(watch, SIGNAL(valueChanged(QByteArray,QVariant)),
                    this, SLOT(valueChanged(QByteArray,QVariant)));
    m_watchTableModel->addWatch(watch, expr);
    m_watchTable->resizeColumnsToContents();
}

void EnginePane::valueChanged(const QByteArray &propertyName, const QVariant &value)
{
    QmlDebugWatch *watch = qobject_cast<QmlDebugWatch*>(sender());

    m_watchTableModel->updateWatch(watch, value);

    if (!propertyName.isEmpty()) {
        if (watch->objectDebugId() == m_propView->object().debugId())
            m_propView->updateProperty(propertyName, value);
    }
}

void EnginePane::propertyDoubleClicked(const QmlDebugPropertyReference &property)
{
    PropertyView *view = qobject_cast<PropertyView*>(sender());
    if (!view)
        return;

    QmlDebugObjectReference object = view->object();
    QmlDebugWatch *watch = m_watchTableModel->findWatch(object.debugId(), property.name());
    if (watch) {
        m_watchTableModel->removeWatch(watch);
        m_client.removeWatch(watch);
        delete watch;
    } else {
        QmlDebugWatch *watch = m_client.addWatch(property, this);
        QObject::connect(watch, SIGNAL(stateChanged(State)),
                        this, SLOT(propertyWatchStateChanged()));
        QObject::connect(watch, SIGNAL(valueChanged(QByteArray,QVariant)),
                        this, SLOT(valueChanged(QByteArray,QVariant)));
        QString desc = property.name()
                + QLatin1String(" on\n")
                + object.className()
                + QLatin1String(": ")
                + (object.name().isEmpty() ? QLatin1String("<unnamed>") : object.name());
        m_watchTableModel->addWatch(watch, desc);
        m_watchTable->resizeColumnsToContents();
    }
}

void EnginePane::propertyWatchStateChanged()
{
    QmlDebugPropertyWatch *watch = qobject_cast<QmlDebugPropertyWatch*>(sender());
    if (watch && watch->objectDebugId() == m_propView->object().debugId())
        m_propView->setPropertyIsWatched(watch->name(), watch->state() == QmlDebugWatch::Active);
}

void EnginePane::stopWatching(int column)
{
    QmlDebugWatch *watch = m_watchTableModel->findWatch(column);
    if (watch) {
        m_watchTableModel->removeWatch(watch);
        m_client.removeWatch(watch);
        delete watch;
    }
}

void EnginePane::watchedItemActivated(const QModelIndex &index)
{
    QmlDebugWatch *watch = m_watchTableModel->findWatch(index.column());
    if (!watch)
        return;
    QTreeWidgetItem *item = m_objTree->findItemByObjectId(watch->objectDebugId());
    if (item) {
        m_objTree->setCurrentItem(item);
        item->setExpanded(true);
    }
}

void EnginePane::queryContext(int id)
{
    if (m_context) {
        delete m_context;
        m_context = 0;
    }

    m_context = m_client.queryRootContexts(QmlDebugEngineReference(id), this);
    if (!m_context->isWaiting())
        contextChanged();
    else
        QObject::connect(m_context, SIGNAL(stateChanged(State)), 
                         this, SLOT(contextChanged()));
}

void EnginePane::contextChanged()
{
    dump(m_context->rootContext(), 0);
    foreach (const QmlDebugObjectReference &object, m_context->rootContext().objects())
        fetchObject(object.debugId());

    delete m_context; m_context = 0;
}

void EnginePane::dump(const QmlDebugContextReference &ctxt, int ind)
{
    QByteArray indent(ind * 4, ' ');
    qWarning().nospace() << indent.constData() << ctxt.debugId() << " " 
                         << qPrintable(ctxt.name());

    for (int ii = 0; ii < ctxt.contexts().count(); ++ii)
        dump(ctxt.contexts().at(ii), ind + 1);

    for (int ii = 0; ii < ctxt.objects().count(); ++ii)
        dump(ctxt.objects().at(ii), ind);
}

void EnginePane::dump(const QmlDebugObjectReference &obj, int ind)
{
    QByteArray indent(ind * 4, ' ');
    qWarning().nospace() << indent.constData() << qPrintable(obj.className())
                         << " " << qPrintable(obj.name()) << " " 
                         << obj.debugId();

    for (int ii = 0; ii < obj.children().count(); ++ii)
        dump(obj.children().at(ii), ind + 1);
}


void EnginePane::buildTree(const QmlDebugObjectReference &obj, QTreeWidgetItem *parent)
{
    if (!parent)
        m_objTree->clear();

    QTreeWidgetItem *item = parent ? new QTreeWidgetItem(parent) : new QTreeWidgetItem(m_objTree);
    item->setText(0, obj.className());
    item->setData(0, Qt::UserRole, obj.debugId());
    item->setData(0, QmlObjectTree::ContextIdRole, obj.contextDebugId());

    if (parent && obj.contextDebugId() >= 0
            && obj.contextDebugId() != parent->data(0, QmlObjectTree::ContextIdRole).toInt()) {
        QmlDebugFileReference source = obj.source();
        if (!source.url().isEmpty()) {
            QString toolTipString = QLatin1String("URL: ") + source.url().toString();
            item->setToolTip(0, toolTipString);
        }
        item->setForeground(0, QColor("orange"));
    } else {
        item->setExpanded(true);
    }

    if (obj.contextDebugId() < 0)
        item->setForeground(0, Qt::lightGray);

    for (int ii = 0; ii < obj.children().count(); ++ii)
        buildTree(obj.children().at(ii), item);
}

void EnginePane::queryEngines()
{
    if (m_engines)
        return;

    m_engines = m_client.queryAvailableEngines(this);
    if (!m_engines->isWaiting())
        enginesChanged();
    else
        QObject::connect(m_engines, SIGNAL(stateChanged(State)), 
                         this, SLOT(enginesChanged()));
}

void EnginePane::enginesChanged()
{
    qDeleteAll(m_engineItems);
    m_engineItems.clear();

    QList<QmlDebugEngineReference> engines = m_engines->engines();
    delete m_engines; m_engines = 0;

    for (int ii = 0; ii < engines.count(); ++ii)
        m_engineItems << new DebuggerEngineItem(engines.at(ii).name(),
                                                engines.at(ii).debugId());

    m_engineView->rootContext()->setContextProperty("engines", qVariantFromValue(&m_engineItems));
}

void EnginePane::fetchClicked()
{
    int id = m_text->text().toInt();
    fetchObject(id);
}

void EnginePane::fetchObject(int id)
{
    if (m_object) {
        delete m_object;
        m_object = 0;
    }

    m_object = m_client.queryObjectRecursive(QmlDebugObjectReference(id), this);
    if (!m_object->isWaiting())
        objectFetched();
    else
        QObject::connect(m_object, SIGNAL(stateChanged(State)), 
                         this, SLOT(objectFetched()));
}

void EnginePane::objectFetched()
{
    dump(m_object->object(), 0);
    buildTree(m_object->object(), 0);
    delete m_object; m_object = 0;
}


#include "engine.moc"

QT_END_NAMESPACE

