/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Anomaly project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef ADDRESSBAR_H
#define ADDRESSBAR_H

#include <QWidget>

class QLineEdit;
class QToolButton;

class AddressBar : public QWidget
{
    Q_OBJECT

public:
    AddressBar(QWidget *parent = 0);
    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);
    void focusInEvent(QFocusEvent *event);

signals:
    void addressEntered(const QString &address);

private slots:
    void processAddress();

private:
    QLineEdit *m_lineEdit;
    QToolButton *m_toolButton;
};

#endif // ADDRESSBAR_H
