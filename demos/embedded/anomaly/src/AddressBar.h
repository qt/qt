/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demos of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
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
