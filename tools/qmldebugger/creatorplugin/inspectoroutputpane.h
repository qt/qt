#ifndef INSPECTOROUTPUTPANE_H
#define INSPECTOROUTPUTPANE_H 

#include <QtCore/QObject>

#include <coreplugin/ioutputpane.h>


QT_BEGIN_NAMESPACE

class QTextEdit;

class RunControl;

class InspectorOutputPane : public Core::IOutputPane
{
    Q_OBJECT
public:
    InspectorOutputPane(QObject *parent = 0);
    virtual ~InspectorOutputPane();

    virtual QWidget *outputWidget(QWidget *parent);
    virtual QList<QWidget*> toolBarWidgets() const;
    virtual QString name() const;

    virtual int priorityInStatusBar() const;

    virtual void clearContents();
    virtual void visibilityChanged(bool visible);

    virtual void setFocus();
    virtual bool hasFocus();
    virtual bool canFocus();

    virtual bool canNavigate();
    virtual bool canNext();
    virtual bool canPrevious();
    virtual void goToNext();
    virtual void goToPrev();

public slots:
    void addOutput(RunControl *, const QString &text);
    void addOutputInline(RunControl *, const QString &text);

    void addErrorOutput(RunControl *, const QString &text);
    void addInspectorStatus(const QString &text);

private:
    QTextEdit *m_textEdit;
};

QT_END_NAMESPACE

#endif

