#ifndef CANVASFRAMERATE_H
#define CANVASFRAMERATE_H

#include <QtCore/qpointer.h>
#include <QtGui/qwidget.h>

#include <private/qmldebugclient_p.h>

QT_BEGIN_NAMESPACE

class QTabWidget;
class QSpinBox;
class QCheckBox;

class CanvasFrameRatePlugin;

class CanvasFrameRate : public QWidget
{
    Q_OBJECT
public:
    CanvasFrameRate(QWidget *parent = 0);
    
    void reset(QmlDebugConnection *conn);

    void setSizeHint(const QSize &);
    virtual QSize sizeHint() const;

private slots:
    void newTab();
    void enabledStateChanged(int);
    void connectionStateChanged(QAbstractSocket::SocketState state);

private:
    void handleConnected(QmlDebugConnection *conn);

    QTabWidget *m_tabs;
    QSpinBox *m_spin;
    CanvasFrameRatePlugin *m_plugin;
    QSize m_sizeHint;
    QCheckBox *m_enabledCheckBox;
};

QT_END_NAMESPACE

#endif // CANVASFRAMERATE_H

