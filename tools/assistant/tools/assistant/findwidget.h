/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QCheckBox;
class QLabel;
class QLineEdit;
class QToolButton;

class FindWidget : public QWidget
{
    Q_OBJECT
public:
    FindWidget(QWidget *parent = 0);
    ~FindWidget();

    void show();
    void showAndClear();

    QString text() const;
    bool caseSensitive() const;

    void setPalette(bool found);
    void setTextWrappedVisible(bool visible);

signals:
    void escapePressed();

    void findNext();
    void findPrevious();
    void find(const QString &text, bool forward);

protected:
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent * event);

private slots:
    void updateButtons();
    void textChanged(const QString &text);

private:
    bool eventFilter(QObject *object, QEvent *e);
    QToolButton* setupToolButton(const QString &text, const QString &icon);

private:
    QPalette appPalette;

    QLineEdit *editFind;
    QCheckBox *checkCase;
    QLabel *labelWrapped;
    QToolButton *toolNext;
    QToolButton *toolClose;
    QToolButton *toolPrevious;
};

QT_END_NAMESPACE

#endif  // FINDWIDGET_H
