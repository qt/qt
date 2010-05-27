#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QtGui/QMainWindow>

class QTableView; //forward declaration

class ModelView : public QMainWindow
{
    Q_OBJECT
private:
    QTableView *tableView;
public:
    ModelView(QWidget *parent = 0);
public slots:
    void showWindowTitle(const QString & title);
};

#endif // MODELVIEW_H
