#ifndef LOGGERWIDGET_H
#define LOGGERWIDGET_H

#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE

class LoggerWidget : public QPlainTextEdit {
Q_OBJECT
public:
    LoggerWidget(QWidget *parent=0);
    void append(QtMsgType type, const char *msg);
protected:
    void closeEvent(QCloseEvent *event);
private:
    bool m_keepClosed;
};

QT_END_NAMESPACE

#endif // LOGGERWIDGET_H
