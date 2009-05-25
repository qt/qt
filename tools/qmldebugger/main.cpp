#include <QtNetwork/qtcpsocket.h>
#include <QtGui/qapplication.h>
#include <QtGui/qwidget.h>
#include <QtGui/qpainter.h>
#include <QtGui/qscrollbar.h>
#include <QtDeclarative/qmldebugclient.h>
#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatastream.h>
#include "canvasframerate.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTabWidget>
#include <QSpinBox>

class Shell : public QWidget
{
Q_OBJECT
public:
    Shell(QWidget * = 0);

private slots:
    void connectToHost();
    void disconnectFromHost();

private:
    QmlDebugClient client;

    QLineEdit *m_host;
    QSpinBox *m_port;
    QPushButton *m_connectButton;
    QPushButton *m_disconnectButton;
};

Shell::Shell(QWidget *parent)
: QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);


    QHBoxLayout *connectLayout = new QHBoxLayout;
    layout->addLayout(connectLayout);
    connectLayout->addStretch(2);

    m_host = new QLineEdit(this);
    m_host->setText("localhost");
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

    QTabWidget *tabs = new QTabWidget(this);
    layout->addWidget(tabs);

    CanvasFrameRate *cfr = new CanvasFrameRate(&client, this);
    tabs->addTab(cfr, tr("Frame Rate"));
}

void Shell::connectToHost()
{
    m_connectButton->setEnabled(false);
    m_disconnectButton->setEnabled(true);
    client.connectToHost(m_host->text(), m_port->value());
}

void Shell::disconnectFromHost()
{
    m_connectButton->setEnabled(true);
    m_disconnectButton->setEnabled(false);
    client.disconnectFromHost();
}

int main(int argc, char ** argv)
{
    if(argc != 3) {
        qWarning() << "Usage:" << argv[0] << "host port";
        return -1;
    }

    QApplication app(argc, argv);

    Shell shell;
//    shell.setFixedSize(1024, 768);
    shell.show();


    return app.exec();
}

#include "main.moc"
