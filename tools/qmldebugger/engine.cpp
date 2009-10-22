#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QFile>

#include <private/qmlenginedebug_p.h>
#include <QtDeclarative/qmldebugclient.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxitem.h>
#include <QtDeclarative/qmldebugservice.h>

#include "engine.h"
#include "objectpropertiesview.h"
#include "objecttree.h"
#include "watchtable.h"

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

EnginePane::EnginePane(QmlDebugConnection *conn, QWidget *parent)
: QWidget(parent), m_client(new QmlEngineDebug(conn, this)), m_engines(0), m_context(0), m_watchTableModel(0)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

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
                     this, SLOT(refreshEngines()));

    m_engineView->setVisible(false);
    layout->addWidget(m_engineView);

    QSplitter *splitter = new QSplitter;

    m_objTree = new ObjectTree(m_client, this);
    m_propertiesView = new ObjectPropertiesView(m_client);
    m_watchTableModel = new WatchTableModel(m_client, this);

    m_watchTableView = new WatchTableView(m_watchTableModel);
    m_watchTableView->setModel(m_watchTableModel);
    WatchTableHeaderView *header = new WatchTableHeaderView(m_watchTableModel);
    m_watchTableView->setHorizontalHeader(header);

    connect(m_objTree, SIGNAL(currentObjectChanged(QmlDebugObjectReference)),
            m_propertiesView, SLOT(reload(QmlDebugObjectReference)));
    connect(m_objTree, SIGNAL(expressionWatchRequested(QmlDebugObjectReference,QString)),
            m_watchTableModel, SLOT(expressionWatchRequested(QmlDebugObjectReference,QString)));

    connect(m_propertiesView, SIGNAL(activated(QmlDebugObjectReference,QmlDebugPropertyReference)),
            m_watchTableModel, SLOT(togglePropertyWatch(QmlDebugObjectReference,QmlDebugPropertyReference)));

    connect(m_watchTableModel, SIGNAL(watchCreated(QmlDebugWatch*)),
            m_propertiesView, SLOT(watchCreated(QmlDebugWatch*)));

    connect(m_watchTableView, SIGNAL(objectActivated(int)),
            m_objTree, SLOT(setCurrentObject(int)));

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(m_propertiesView, tr("Properties"));
    m_tabs->addTab(m_watchTableView, tr("Watched"));

    splitter->addWidget(m_objTree);
    splitter->addWidget(m_tabs);
    splitter->setStretchFactor(1, 2);
    layout->addWidget(splitter);
}

void EnginePane::engineSelected(int id)
{
    qWarning() << "Engine selected" << id;
    queryContext(id);
}

void EnginePane::queryContext(int id)
{
    if (m_context) {
        delete m_context;
        m_context = 0;
    }

    m_context = m_client->queryRootContexts(QmlDebugEngineReference(id), this);
    if (!m_context->isWaiting())
        contextChanged();
    else
        QObject::connect(m_context, SIGNAL(stateChanged(State)),
                         this, SLOT(contextChanged()));
}

void EnginePane::contextChanged()
{
    //dump(m_context->rootContext(), 0);

    foreach (const QmlDebugObjectReference &object, m_context->rootContext().objects())
        m_objTree->reload(object.debugId());

    delete m_context; m_context = 0;
}

void EnginePane::refreshEngines()
{
    if (m_engines)
        return;

    m_engines = m_client->queryAvailableEngines(this);
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

    if (engines.isEmpty())
        qWarning("qmldebugger: no engines found!");

    for (int ii = 0; ii < engines.count(); ++ii)
        m_engineItems << new DebuggerEngineItem(engines.at(ii).name(),
                                                engines.at(ii).debugId());

    m_engineView->rootContext()->setContextProperty("engines", qVariantFromValue(&m_engineItems));

    m_engineView->setVisible(m_engineItems.count() > 1);
    if (m_engineItems.count() == 1)
        engineSelected(qobject_cast<DebuggerEngineItem*>(m_engineItems.at(0))->engineId());
}


#include "engine.moc"

QT_END_NAMESPACE

