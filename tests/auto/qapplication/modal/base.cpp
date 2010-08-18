#include "base.h"

base::base(QWidget *parent) :
    QWidget(parent)
{
    m_timer = new QTimer(this);
    m_modalStarted = false;
    m_timer->setSingleShot(false);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(periodicTimer()));
    m_timer->start(5000);
}

void base::periodicTimer()
{
    if(m_modalStarted)
        exit(0);
    m_modalDialog = new QDialog(this);
    m_modalDialog->setModal(true);
    m_modalDialog->show();
    m_modalStarted = true;
}
