/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
class AppendText : public QUndoCommand
{
public:
    AppendText(QString *doc, const QString &text)
        : m_document(doc), m_text(text) { setText("append text"); }
    virtual void undo()
        { m_document->chop(m_text.length()); }
    virtual void redo()
        { m_document->append(m_text); }
private:
    QString *m_document;
    QString m_text;
};
//! [0]


//! [1]
MyCommand *command1 = new MyCommand();
stack->push(command1);
MyCommand *command2 = new MyCommand();
stack->push(command2);

stack->undo();

MyCommand *command3 = new MyCommand();
stack->push(command3); // command2 gets deleted
//! [1]


//! [2]
QUndoCommand *insertRed = new QUndoCommand(); // an empty command
insertRed->setText("insert red text");

new InsertText(document, idx, text, insertRed); // becomes child of insertRed
new SetColor(document, idx, text.length(), Qt::red, insertRed);

stack.push(insertRed);
//! [2]


//! [3]
bool AppendText::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) // make sure other is also an AppendText command
        return false;
    m_text += static_cast<const AppendText*>(other)->m_text;
    return true;
}
//! [3]


//! [4]
stack.beginMacro("insert red text");
stack.push(new InsertText(document, idx, text));
stack.push(new SetColor(document, idx, text.length(), Qt::red));
stack.endMacro(); // indexChanged() is emitted
//! [4]


//! [5]
QUndoCommand *insertRed = new QUndoCommand(); // an empty command
insertRed->setText("insert red text");

new InsertText(document, idx, text, insertRed); // becomes child of insertRed
new SetColor(document, idx, text.length(), Qt::red, insertRed);

stack.push(insertRed);
//! [5]
