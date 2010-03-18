#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMenuBar>
#include <QMenu>

namespace Ui {
    class Widget;
}

class Widget : public QWidget {
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void switchToFullScreen();
    void switchToNormalScreen();
    void addMenuBar();
    void removeMenuBar();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Widget *ui;
    QMenuBar *menuBar;
};

#endif // WIDGET_H
