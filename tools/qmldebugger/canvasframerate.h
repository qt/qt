#ifndef CANVASFRAMERATE_H
#define CANVASFRAMERATE_H

#include <QWidget>

QT_BEGIN_NAMESPACE

class QmlDebugConnection;
class QTabWidget;
class QSpinBox;
class CanvasFrameRate : public QWidget
{
    Q_OBJECT
public:
    CanvasFrameRate(QmlDebugConnection *, QWidget *parent = 0);

    void setSizeHint(const QSize &);
    virtual QSize sizeHint() const;

private slots:
    void newTab();
    void stateChanged(int);

private:
    QTabWidget *m_tabs;
    QSpinBox *m_spin;
    QObject *m_plugin;
    QSize m_sizeHint;
};

QT_END_NAMESPACE

#endif // CANVASFRAMERATE_H

