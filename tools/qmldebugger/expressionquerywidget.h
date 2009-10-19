#ifndef EXPRESSIONQUERYWIDGET_H
#define EXPRESSIONQUERYWIDGET_H

#include <QWidget>

#include <QtDeclarative/qmldebug.h>

QT_BEGIN_NAMESPACE

class QGroupBox;
class QTextEdit;
class QLineEdit;

class ExpressionQueryWidget : public QWidget
{
    Q_OBJECT
public:
    enum Style {
        Compact,
        Shell
    };

    ExpressionQueryWidget(QmlEngineDebug *client, QWidget *parent = 0);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void setCurrentObject(const QmlDebugObjectReference &obj);

private slots:
    void executeExpression();
    void showResult();

private:
    void appendPrompt();
    void checkCurrentContext();
    void showCurrentContext();
    void updateTitle();

    Style m_style;

    QmlEngineDebug *m_client;
    QmlDebugExpressionQuery *m_query;
    QGroupBox *m_groupBox;
    QTextEdit *m_textEdit;
    QLineEdit *m_lineEdit;
    QString m_prompt;
    QString m_expr;

    QmlDebugObjectReference m_currObject;
    QmlDebugObjectReference m_objectAtLastFocus;
};

QT_END_NAMESPACE

#endif

