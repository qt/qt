/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q3MULTILINEEDIT_H
#define Q3MULTILINEEDIT_H

#include <Qt3Support/q3textedit.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_MULTILINEEDIT

class Q3MultiLineEditCommand;
class QValidator;
class Q3MultiLineEditData;

class Q_COMPAT_EXPORT Q3MultiLineEdit : public Q3TextEdit
{
    Q_OBJECT
    Q_PROPERTY(int numLines READ numLines)
    Q_PROPERTY(bool atBeginning READ atBeginning)
    Q_PROPERTY(bool atEnd READ atEnd)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(bool edited READ edited WRITE setEdited DESIGNABLE false)

public:
    Q3MultiLineEdit(QWidget* parent=0, const char* name=0);
    ~Q3MultiLineEdit();

    QString textLine(int line) const;
    int numLines() const;

    virtual void insertLine(const QString &s, int line = -1);
    virtual void insertAt(const QString &s, int line, int col) {
        insertAt(s, line, col, false);
    }
    virtual void insertAt(const QString &s, int line, int col, bool mark);
    virtual void removeLine(int line);
    virtual void setCursorPosition(int line, int col) {
        setCursorPosition(line, col, false);
    }
    virtual void setCursorPosition(int line, int col, bool mark);
    bool atBeginning() const;
    bool atEnd() const;

    void setAlignment(Qt::Alignment flags);
    Qt::Alignment alignment() const;

    void setEdited(bool);
    bool edited() const;

    bool hasMarkedText() const;
    QString markedText() const;

    void cursorWordForward(bool mark);
    void cursorWordBackward(bool mark);

    // noops
    bool autoUpdate() const { return true; }
    virtual void setAutoUpdate(bool) {}

    int totalWidth() const { return contentsWidth(); }
    int totalHeight() const { return contentsHeight(); }

    int maxLines() const { return QWIDGETSIZE_MAX; }
    void setMaxLines(int) {}

public Q_SLOTS:
    void deselect() { selectAll(false); }

protected:
    QPoint cursorPoint() const;
    virtual void insertAndMark(const QString&, bool mark);
    virtual void newLine();
    virtual void killLine();
    virtual void pageUp(bool mark=false);
    virtual void pageDown(bool mark=false);
    virtual void cursorLeft(bool mark=false, bool wrap = true);
    virtual void cursorRight(bool mark=false, bool wrap = true);
    virtual void cursorUp(bool mark=false);
    virtual void cursorDown(bool mark=false);
    virtual void backspace();
    virtual void home(bool mark=false);
    virtual void end(bool mark=false);

    bool getMarkedRegion(int *line1, int *col1, int *line2, int *col2) const;
    int lineLength(int row) const;

private:
    Q_DISABLE_COPY(Q3MultiLineEdit)

    Q3MultiLineEditData *d;
};

#endif // QT_NO_MULTILINEEDIT

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3MULTILINEEDIT_H
