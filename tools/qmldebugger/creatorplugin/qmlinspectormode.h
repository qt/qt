#ifndef QMLINSPECTORMODE_H
#define QMLINSPECTORMODE_H

#include <QAction>

#include <coreplugin/basemode.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QToolButton;
class QLineEdit;
class QSpinBox;
class QLabel;

class QmlEngineDebug;
class QmlDebugConnection;
class QmlDebugEnginesQuery;
class QmlDebugRootContextQuery;
class ObjectTree;
class WatchTableModel;
class WatchTableView;
class ObjectPropertiesView;
class CanvasFrameRate;
class ExpressionQueryWidget;
class EngineSpinBox;
    

class QmlInspectorMode : public Core::BaseMode
{
    Q_OBJECT

public:
    QmlInspectorMode(QObject *parent = 0);
    
    
    quint16 viewerPort() const;
    
signals:
    void startViewer();
    void stopViewer();
    void statusMessage(const QString &text);
    
public slots: 
    void connectToViewer(); // using host, port from widgets
    void disconnectFromViewer(); 

private slots:
    void connectionStateChanged();
    void connectionError();
    void reloadEngines();
    void enginesChanged();
    void queryEngineContext(int);
    void contextChanged();

private:
    struct Actions {
        QAction *startAction;
        QAction *stopAction;
    };
    
    void initActions();
    QWidget *createModeWindow();
    QWidget *createMainView();
    void initWidgets();
    QWidget *createBottomWindow();
    QToolButton *createToolButton(QAction *action);
    
    Actions m_actions;
    
    QmlDebugConnection *m_conn;
    QmlEngineDebug *m_client;

    QmlDebugEnginesQuery *m_engineQuery;
    QmlDebugRootContextQuery *m_contextQuery;
    
    ObjectTree *m_objectTreeWidget;
    ObjectPropertiesView *m_propertiesWidget;
    WatchTableModel *m_watchTableModel;
    WatchTableView *m_watchTableView;
    CanvasFrameRate *m_frameRateWidget;
    ExpressionQueryWidget *m_expressionWidget;

    QLineEdit *m_addressEdit;
    QSpinBox *m_portSpinBox;
    EngineSpinBox *m_engineSpinBox;
};

QT_END_NAMESPACE

#endif
