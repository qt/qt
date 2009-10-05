#include "lineedit.h"
#include <qml.h>

LineEditExtension::LineEditExtension(QObject *object)
: QObject(object), m_lineedit(static_cast<QLineEdit *>(object))
{
}

int LineEditExtension::leftMargin() const
{
    int l, r, t, b;
    m_lineedit->getTextMargins(&l, &t, &r, &b);
    return l;
}

void LineEditExtension::setLeftMargin(int m)
{
    int l, r, t, b;
    m_lineedit->getTextMargins(&l, &t, &r, &b);
    m_lineedit->setTextMargins(m, t, r, b);
}

int LineEditExtension::rightMargin() const
{
    int l, r, t, b;
    m_lineedit->getTextMargins(&l, &t, &r, &b);
    return r;
}

void LineEditExtension::setRightMargin(int m) 
{
    int l, r, t, b;
    m_lineedit->getTextMargins(&l, &t, &r, &b);
    m_lineedit->setTextMargins(l, t, m, b);
}

int LineEditExtension::topMargin() const
{
    int l, r, t, b;
    m_lineedit->getTextMargins(&l, &t, &r, &b);
    return t;
}

void LineEditExtension::setTopMargin(int m) 
{
    int l, r, t, b;
    m_lineedit->getTextMargins(&l, &t, &r, &b);
    m_lineedit->setTextMargins(l, m, r, b);
}

int LineEditExtension::bottomMargin() const
{
    int l, r, t, b;
    m_lineedit->getTextMargins(&l, &t, &r, &b);
    return b;
}

void LineEditExtension::setBottomMargin(int m) 
{
    int l, r, t, b;
    m_lineedit->getTextMargins(&l, &t, &r, &b);
    m_lineedit->setTextMargins(l, t, r, m);
}

QML_DECLARE_TYPE(QLineEdit);
QML_DEFINE_EXTENDED_TYPE(People, 1, 0, 0, QLineEdit, QLineEdit, LineEditExtension);
