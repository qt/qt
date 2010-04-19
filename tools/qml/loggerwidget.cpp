#include "loggerwidget.h"
#include <qglobal.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

LoggerWidget::LoggerWidget(QWidget *parent) :
    QPlainTextEdit(parent),
    m_keepClosed(false)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowTitle(tr("Qt Declarative UI Viewer - Logger"));
}

void LoggerWidget::append(QtMsgType /*type*/, const char *msg)
{
    appendPlainText(QString::fromAscii(msg));

    if (!m_keepClosed && !isVisible())
        setVisible(true);
}

void LoggerWidget::closeEvent(QCloseEvent *event)
{
    m_keepClosed = true;
    QWidget::closeEvent(event);
}

QT_END_NAMESPACE
