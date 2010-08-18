#ifndef BASE_H
#define BASE_H

#include <QWidget>
#include <QTimer>
#include <QDialog>

class base : public QWidget
{
Q_OBJECT
    QTimer *m_timer;
    bool m_modalStarted;
    QDialog *m_modalDialog;
public:
    explicit base(QWidget *parent = 0);

signals:

public slots:
    void periodicTimer();
};

#endif // BASE_H
