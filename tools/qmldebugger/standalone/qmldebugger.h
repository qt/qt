#ifndef QMLDEBUGGER_H
#define QMLDEBUGGER_H

#include <QtDeclarative/qmldebugclient.h>
#include <QtNetwork/qtcpsocket.h>
#include <QtGui/qwidget.h>

class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QTabWidget;

class EnginePane;

class QmlDebugger : public QWidget
{
    Q_OBJECT
public:
    QmlDebugger(QWidget * = 0);

    void setHost(const QString &host);
    void setPort(quint16 port);
    void showEngineTab();

public slots:
    void connectToHost();
    void disconnectFromHost();

private slots:
    void connectionStateChanged();
    void connectionError(QAbstractSocket::SocketError socketError);

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

#endif
