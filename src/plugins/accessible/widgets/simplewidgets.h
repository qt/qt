/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SIMPLEWIDGETS_H
#define SIMPLEWIDGETS_H

#include <QtCore/qcoreapplication.h>
#include <QtGui/qaccessible2.h>
#include <QtGui/qaccessiblewidget.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ACCESSIBILITY

class QAbstractButton;
class QLineEdit;
class QToolButton;
class QGroupBox;
class QProgressBar;

class QAccessibleButton : public QAccessibleWidgetEx, public QAccessibleActionInterface
{
    Q_ACCESSIBLE_OBJECT
    Q_DECLARE_TR_FUNCTIONS(QAccessibleButton)
public:
    QAccessibleButton(QWidget *w, Role r);

    QString text(Text t, int child) const;
    State state(int child) const;

    QString actionText(int action, Text text, int child) const;
    bool doAction(int action, int child, const QVariantList &params);

    // QAccessibleActionInterface
    int actionCount();
    void doAction(int actionIndex);
    QString description(int actionIndex);
    QString name(int actionIndex);
    QString localizedName(int actionIndex);
    QStringList keyBindings(int actionIndex);

protected:
    QAbstractButton *button() const;
};

#ifndef QT_NO_TOOLBUTTON
class QAccessibleToolButton : public QAccessibleButton
{
public:
    QAccessibleToolButton(QWidget *w, Role role);

    enum ToolButtonElements {
        ToolButtonSelf        = 0,
        ButtonExecute,
        ButtonDropMenu
    };

    Role role(int child) const;
    State state(int child) const;

    int childCount() const;
    QRect rect(int child) const;

    QString text(Text t, int child) const;

    int actionCount(int child) const;
    QString actionText(int action, Text text, int child) const;
    bool doAction(int action, int child, const QVariantList &params);

protected:
    QToolButton *toolButton() const;

    bool isSplitButton() const;
};
#endif // QT_NO_TOOLBUTTON

class QAccessibleDisplay : public QAccessibleWidgetEx, public QAccessibleImageInterface
{
    Q_ACCESSIBLE_OBJECT
public:
    explicit QAccessibleDisplay(QWidget *w, Role role = StaticText);

    QString text(Text t, int child) const;
    Role role(int child) const;

    Relation relationTo(int child, const QAccessibleInterface *other, int otherChild) const;
    int navigate(RelationFlag, int entry, QAccessibleInterface **target) const;

    // QAccessibleImageInterface
    QString imageDescription();
    QSize imageSize();
    QRect imagePosition(QAccessible2::CoordinateType coordType);
};

#ifndef QT_NO_GROUPBOX
class QAccessibleGroupBox : public QAccessibleWidgetEx,
                            public QAccessibleActionInterface
{
    Q_ACCESSIBLE_OBJECT
public:
    explicit QAccessibleGroupBox(QWidget *w);

    State state(int child) const;
    Role role(int child) const;
    QString text(Text t, int child) const;

    Relation relationTo(int child, const QAccessibleInterface *other, int otherChild) const;
    int navigate(RelationFlag rel, int entry, QAccessibleInterface** target) const;

    //QAccessibleActionInterface
    int actionCount();
    QString description(int actionIndex);
    void doAction(int actionIndex);
    QString name(int actionIndex);
    QString localizedName(int actionIndex);
    QStringList keyBindings(int actionIndex);

private:
    QGroupBox *groupBox() const;
};
#endif

#ifndef QT_NO_LINEEDIT
class QAccessibleLineEdit : public QAccessibleWidgetEx, public QAccessibleTextInterface,
                            public QAccessibleSimpleEditableTextInterface
{
    Q_ACCESSIBLE_OBJECT
public:
    explicit QAccessibleLineEdit(QWidget *o, const QString &name = QString());

    QString text(Text t, int child) const;
    void setText(Text t, int control, const QString &text);
    State state(int child) const;
    QVariant invokeMethodEx(QAccessible::Method method, int child, const QVariantList &params);

    // QAccessibleTextInterface
    void addSelection(int startOffset, int endOffset);
    QString attributes(int offset, int *startOffset, int *endOffset);
    int cursorPosition();
    QRect characterRect(int offset, QAccessible2::CoordinateType coordType);
    int selectionCount();
    int offsetAtPoint(const QPoint &point, QAccessible2::CoordinateType coordType);
    void selection(int selectionIndex, int *startOffset, int *endOffset);
    QString text(int startOffset, int endOffset);
    QString textBeforeOffset (int offset, QAccessible2::BoundaryType boundaryType,
            int *startOffset, int *endOffset);
    QString textAfterOffset(int offset, QAccessible2::BoundaryType boundaryType,
            int *startOffset, int *endOffset);
    QString textAtOffset(int offset, QAccessible2::BoundaryType boundaryType,
            int *startOffset, int *endOffset);
    void removeSelection(int selectionIndex);
    void setCursorPosition(int position);
    void setSelection(int selectionIndex, int startOffset, int endOffset);
    int characterCount();
    void scrollToSubstring(int startIndex, int endIndex);

protected:
    QLineEdit *lineEdit() const;
};
#endif // QT_NO_LINEEDIT

#ifndef QT_NO_PROGRESSBAR
class QAccessibleProgressBar : public QAccessibleDisplay, public QAccessibleValueInterface
{
    Q_ACCESSIBLE_OBJECT
public:
    explicit QAccessibleProgressBar(QWidget *o);

    // QAccessibleValueInterface
    QVariant currentValue();
    QVariant maximumValue();
    QVariant minimumValue();
    inline void setCurrentValue(const QVariant &) {}

protected:
    QProgressBar *progressBar() const;
};
#endif

#endif // QT_NO_ACCESSIBILITY

QT_END_NAMESPACE

#endif // SIMPLEWIDGETS_H
