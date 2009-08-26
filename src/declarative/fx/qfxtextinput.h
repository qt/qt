/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QFXTEXTINPUT_H
#define QFXTEXTINPUT_H

#include "qfxtext.h"
#include "qfxpainteditem.h"
#include <QGraphicsSceneMouseEvent>
#include <QIntValidator>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxTextInputPrivate;
class QValidator;
class Q_DECLARATIVE_EXPORT QFxTextInput : public QFxPaintedItem
{
    Q_OBJECT
    Q_ENUMS(HAlignment)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setHighlightColor) //### selectionColor
    Q_PROPERTY(QColor highlightedTextColor READ highlightedTextColor WRITE setHighlightedTextColor) //### selectedTextColor
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(HAlignment horizontalAlignment READ hAlign WRITE setHAlign)

    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool cursorVisible READ isCursorVisible WRITE setCursorVisible)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(QmlComponent *cursorDelegate READ cursorDelegate WRITE setCursorDelegate)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)
    Q_PROPERTY(QString selectedText READ selectedText NOTIFY selectedTextChanged)

    Q_PROPERTY(int maximumLength READ maxLength WRITE setMaxLength)
    Q_PROPERTY(QValidator* validator READ validator WRITE setValidator)
    Q_PROPERTY(QString inputMask READ inputMask WRITE setInputMask)
    Q_PROPERTY(bool acceptableInput READ hasAcceptableInput NOTIFY acceptableInputChanged)
    Q_PROPERTY(uint echoMode READ echoMode WRITE setEchoMode) //### enum
    //### Q_PROPERTY(bool focusOnPress READ focusOnPress WRITE setFocusOnPress)

public:
    QFxTextInput(QFxItem* parent=0);
    ~QFxTextInput();

    enum HAlignment {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter
    };

    //### Should we have this function, x based properties,
    //### or copy TextEdit with x instead of QTextCursor?
    Q_INVOKABLE int xToPos(int x);

    QString text() const;
    void setText(const QString &);

    QFont font() const;
    void setFont(const QFont &font);

    QColor color() const;
    void setColor(const QColor &c);

    QColor highlightColor() const;
    void setHighlightColor(const QColor &c);

    QColor highlightedTextColor() const;
    void setHighlightedTextColor(const QColor &c);

    HAlignment hAlign() const;
    void setHAlign(HAlignment align);

    bool isReadOnly() const;
    void setReadOnly(bool);

    bool isCursorVisible() const;
    void setCursorVisible(bool on);

    int cursorPosition() const;
    void setCursorPosition(int cp);

    int selectionStart() const;
    void setSelectionStart(int);

    int selectionEnd() const;
    void setSelectionEnd(int);

    QString selectedText() const;

    int maxLength() const;
    void setMaxLength(int ml);

    QValidator * validator() const;
    void setValidator(QValidator* v);

    QString inputMask() const;
    void setInputMask(const QString &im);

    uint echoMode() const;
    void setEchoMode(uint echo);

    QmlComponent* cursorDelegate() const;
    void setCursorDelegate(QmlComponent*);

    bool hasAcceptableInput() const;

    void drawContents(QPainter *p,const QRect &r);
Q_SIGNALS:
    void textChanged();
    void cursorPositionChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void selectedTextChanged();
    void accepted();
    void acceptableInputChanged();

protected:
    QFxTextInput(QFxTextInputPrivate &dd, QFxItem *parent);
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent* ev);
    bool event(QEvent *e);

    void focusChanged(bool hasFocus);

public Q_SLOTS:
    void selectAll();

private Q_SLOTS:
    void updateSize();
    void q_textChanged();
    void selectionChanged();
    void updateAll();
    void createCursor();
    void moveCursor();
    void cursorPosChanged();

private:
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QFxTextInput);
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxTextInput)
QML_DECLARE_TYPE(QValidator)
QML_DECLARE_TYPE(QIntValidator)

QT_END_HEADER

#endif // QFXTEXTINPUT_H
