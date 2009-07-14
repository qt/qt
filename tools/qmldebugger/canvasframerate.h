#ifndef CANVASFRAMERATE_H
#define CANVASFRAMERATE_H

#include <QWidget>

QT_BEGIN_NAMESPACE

class QmlDebugConnection;
class QTabWidget;
class CanvasFrameRate : public QWidget
{
    Q_OBJECT
public:
    CanvasFrameRate(QmlDebugConnection *, QWidget *parent = 0);

private slots:
    void newTab();
    void stateChanged(int);

private:
    QTabWidget *m_tabs;
    QObject *m_plugin;
};

QT_END_NAMESPACE

#endif // CANVASFRAMERATE_H

