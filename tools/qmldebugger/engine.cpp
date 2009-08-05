#include "engine.h"
#include <QtDeclarative/qmldebugclient.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTreeWidget>
#include <QTableWidget>
#include <private/qmlenginedebug_p.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxitem.h>

QT_BEGIN_NAMESPACE

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
: QWidget(parent), m_client(client), m_engines(0), m_context(0), m_object(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);

    m_engineView = new QFxView(this);
    m_engineView->rootContext()->setContextProperty("engines", qVariantFromValue(&m_engineItems));
    m_engineView->setContentResizable(true);
    m_engineView->setUrl(QUrl::fromLocalFile("engines.qml"));
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

    m_objTree = new QTreeWidget(this);
    m_objTree->setHeaderHidden(true);
    connect(m_objTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *)));
    hbox->addWidget(m_objTree);

    m_propTable = new QTableWidget(this);
    m_propTable->setColumnCount(2);
    m_propTable->setColumnWidth(0, 150);
    m_propTable->setColumnWidth(1, 400);
    m_propTable->setHorizontalHeaderLabels(QStringList() << "name" << "value");
    hbox->addWidget(m_propTable);
    hbox->setStretchFactor(m_propTable, 2);

    layout->addLayout(hbox);
}

void EnginePane::engineSelected(int id)
{
    qWarning() << "Engine selected" << id;
    queryContext(id);
}

void EnginePane::itemClicked(QTreeWidgetItem *item)
{
    m_propTable->clearContents();

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
    m_propTable->setRowCount(obj.properties().count());
    for (int ii = 0; ii < obj.properties().count(); ++ii) {
        QTableWidgetItem *name = new QTableWidgetItem(obj.properties().at(ii).name());
        m_propTable->setItem(ii, 0, name);
        QTableWidgetItem *value;
        if (!obj.properties().at(ii).binding().isEmpty())
            value = new QTableWidgetItem(obj.properties().at(ii).binding());
        else
            value = new QTableWidgetItem(obj.properties().at(ii).value().toString());
        m_propTable->setItem(ii, 1, value);
    }
    delete m_object; m_object = 0;
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
    QTreeWidgetItem *item = parent ? new QTreeWidgetItem(parent) : new QTreeWidgetItem(m_objTree);
    item->setText(0, obj.className());
    item->setData(0, Qt::UserRole, obj.debugId());

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

