#include <QtNetwork/qtcpsocket.h>
#include <QtGui/qapplication.h>
#include <QtGui/qwidget.h>
#include <QtGui/qpainter.h>
#include <QtGui/qscrollbar.h>
#include <QtDeclarative/qmldebugclient.h>
#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qtimer.h>
#include "canvasframerate.h"
#include "engine.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTabWidget>
#include <QSpinBox>
#include <QLabel>

class Shell : public QWidget
{
Q_OBJECT
public:
    Shell(QWidget * = 0);

    void setHost(const QString &host);
    void setPort(quint16 port);
    void showEngineTab();

public slots:
    void connectToHost();
    void disconnectFromHost();

private slots:
    void connectionStateChanged();

private:
    QmlDebugConnection client;

    QLabel *m_connectionState;
    QLineEdit *m_host;
    QSpinBox *m_port;
    QPushButton *m_connectButton;
    QPushButton *m_disconnectButton;

    EnginePane *m_enginePane;
    QTabWidget *m_tabs;
};

Shell::Shell(QWidget *parent)
: QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);


    QHBoxLayout *connectLayout = new QHBoxLayout;
    layout->addLayout(connectLayout);
    connectLayout->addStretch(2);

    m_connectionState = new QLabel(this);
    connectLayout->addWidget(m_connectionState);
    m_host = new QLineEdit(this);
    m_host->setText("127.0.0.1");
    connectLayout->addWidget(m_host);
    m_port = new QSpinBox(this);
    m_port->setMinimum(1024);
    m_port->setMaximum(20000);
    m_port->setValue(3768);
    connectLayout->addWidget(m_port);
    m_connectButton = new QPushButton(tr("Connect"), this);
    QObject::connect(m_connectButton, SIGNAL(clicked()), 
                     this, SLOT(connectToHost()));
    connectLayout->addWidget(m_connectButton);
    m_disconnectButton = new QPushButton(tr("Disconnect"), this);
    QObject::connect(m_disconnectButton, SIGNAL(clicked()), 
                     this, SLOT(disconnectFromHost()));
    m_disconnectButton->setEnabled(false);
    connectLayout->addWidget(m_disconnectButton);

    m_tabs = new QTabWidget(this);
    layout->addWidget(m_tabs);

    CanvasFrameRate *cfr = new CanvasFrameRate(&client, this);
    m_tabs->addTab(cfr, tr("Frame Rate"));

    m_enginePane = new EnginePane(&client, this);
    m_tabs->addTab(m_enginePane, tr("QML Engine"));

    QObject::connect(&client, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectionStateChanged()));
    connectionStateChanged();
}

void Shell::setHost(const QString &host)
{
    m_host->setText(host);
}

void Shell::setPort(quint16 port)
{
    m_port->setValue(port);
}

void Shell::showEngineTab()
{
    m_tabs->setCurrentWidget(m_enginePane);
}

void Shell::connectionStateChanged()
{
    switch (client.state()) {
    default:
    case QAbstractSocket::UnconnectedState:
        m_connectionState->setText(tr("Disconnected"));
        m_connectButton->setEnabled(true);
        m_disconnectButton->setEnabled(false);
        break;
    case QAbstractSocket::HostLookupState:
        m_connectionState->setText(tr("Resolving"));
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(true);
        break;
    case QAbstractSocket::ConnectingState:
        m_connectionState->setText(tr("Connecting"));
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(true);
        break;
    case QAbstractSocket::ConnectedState:
        m_connectionState->setText(tr("Connected"));
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(true);

        QTimer::singleShot(0, m_enginePane, SLOT(refreshEngines()));
        break;
    case QAbstractSocket::ClosingState:
        m_connectionState->setText(tr("Closing"));
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(false);
        break;
    }
}

void Shell::connectToHost()
{
    client.connectToHost(m_host->text(), m_port->value());
}

void Shell::disconnectFromHost()
{
    client.disconnectFromHost();
}

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    QStringList args = app.arguments();

    Shell shell;
    if (args.contains("--engine"))
        shell.showEngineTab();

    if (args.count() > 1 && args.at(1).contains(':')) {
        QStringList hostAndPort = args.at(1).split(':');
        bool ok = false;
        quint16 port = hostAndPort[1].toInt(&ok);
        if (ok) {
            shell.setHost(hostAndPort[0]);
            shell.setPort(port);
            shell.connectToHost();
        }
    }

    shell.show();

    return app.exec();
}

#include "main.moc"
