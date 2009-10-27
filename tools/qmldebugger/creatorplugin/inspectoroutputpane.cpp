#include <QtGui/qtextedit.h>

#include "inspectoroutputpane.h"

InspectorOutputPane::InspectorOutputPane(QObject *parent)
    : Core::IOutputPane(parent),
      m_textEdit(new QTextEdit)
{
}

InspectorOutputPane::~InspectorOutputPane()
{
    delete m_textEdit;
}

QWidget *InspectorOutputPane::outputWidget(QWidget *parent)
{
    Q_UNUSED(parent);
    return m_textEdit;
}

QList<QWidget*> InspectorOutputPane::toolBarWidgets() const
{
    return QList<QWidget *>();
}

QString InspectorOutputPane::name() const
{
    return tr("Inspector Output");
}

int InspectorOutputPane::priorityInStatusBar() const
{
    return 1;
}

void InspectorOutputPane::clearContents()
{
    m_textEdit->clear();
}

void InspectorOutputPane::visibilityChanged(bool visible)
{
    Q_UNUSED(visible);
}

void InspectorOutputPane::setFocus()
{
    m_textEdit->setFocus();
}

bool InspectorOutputPane::hasFocus()
{
    return m_textEdit->hasFocus();
}

bool InspectorOutputPane::canFocus()
{
    return true;
}

bool InspectorOutputPane::canNavigate()
{
    return false;
}

bool InspectorOutputPane::canNext()
{
    return false;
}

bool InspectorOutputPane::canPrevious()
{
    return false;
}

void InspectorOutputPane::goToNext()
{
}

void InspectorOutputPane::goToPrev()
{
}

void InspectorOutputPane::addOutput(RunControl *, const QString &text)
{
    m_textEdit->insertPlainText(text);
    m_textEdit->moveCursor(QTextCursor::End);
}

void InspectorOutputPane::addOutputInline(RunControl *, const QString &text)
{
    m_textEdit->insertPlainText(text);
    m_textEdit->moveCursor(QTextCursor::End);
}

void InspectorOutputPane::addErrorOutput(RunControl *, const QString &text)
{
    m_textEdit->append(text);
    m_textEdit->moveCursor(QTextCursor::End);
}

void InspectorOutputPane::addInspectorStatus(const QString &text)
{
    m_textEdit->append(text);
    m_textEdit->moveCursor(QTextCursor::End);
}

