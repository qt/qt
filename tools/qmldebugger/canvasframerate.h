#ifndef CANVASFRAMERATE_H
#define CANVASFRAMERATE_H

#include <QWidget>

class QmlDebugClient;
class QTabWidget;
class CanvasFrameRate : public QWidget
{
    Q_OBJECT
public:
    CanvasFrameRate(QmlDebugClient *, QWidget *parent = 0);

private slots:
    void newTab();

private:
    QTabWidget *m_tabs;
    QObject *m_plugin;
};

#endif // CANVASFRAMERATE_H

