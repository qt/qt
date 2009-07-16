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

#ifndef QFXLINEEDIT_H
#define QFXLINEEDIT_H

#include "qfxtext.h"
#include "qfxpainteditem.h"
#include <QGraphicsSceneMouseEvent>
#include <QIntValidator>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxLineEditPrivate;
class QValidator;
class Q_DECLARATIVE_EXPORT QFxLineEdit : public QFxPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QmlFont *font READ font)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    /*
    Q_PROPERTY(QFxText::TextStyle style READ style WRITE setStyle)
    Q_PROPERTY(QColor styleColor READ styleColor WRITE setStyleColor)
    Q_PROPERTY(QFxText::HAlignment hAlign READ hAlign WRITE setHAlign)
    Q_PROPERTY(QFxText::VAlignment vAlign READ vAlign WRITE setVAlign)
    */

    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly);
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength);
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged);
    Q_PROPERTY(int selectionLength READ selectionLength WRITE setSelectionLength NOTIFY selectionLengthChanged);
    Q_PROPERTY(QString selectedText READ selectedText NOTIFY selectedTextChanged);

    Q_PROPERTY(QObject* validator READ validator WRITE setValidator);
    Q_PROPERTY(QString inputMask READ inputMask WRITE setInputMask);
    Q_PROPERTY(bool acceptableInput READ hasAcceptableInput NOTIFY acceptableInputChanged);
    Q_PROPERTY(uint echoMode READ echoMode WRITE setEchoMode);

    Q_PROPERTY(QmlComponent *cursorDelegate READ cursorDelegate WRITE setCursorDelegate);
    /*
    Q_PROPERTY(int scrollDuration READ scrollDuration SET setScrollDuration NOTIFY scrollDurationChanged);
    */
    //### Requested by Aaron K.(Remove before release?)
    Q_PROPERTY(bool awesome READ isAwesome WRITE setAwesome);
public:
    QFxLineEdit(QFxItem* parent=0);
    ~QFxLineEdit();

    QString text() const;
    void setText(const QString &);

    QmlFont *font();

    QColor color() const;
    void setColor(const QColor &c);

    //### Should we have this function or x variants of properties?
    Q_INVOKABLE int xToPos(int x);

    /*
    QFxText::TextStyle style() const;
    void setStyle(QFxText::TextStyle style);

    QColor styleColor() const;
    void setStyleColor(const QColor &c);
    */

    QFxText::HAlignment hAlign() const;
    void setHAlign(QFxText::HAlignment align);

    QFxText::VAlignment vAlign() const;
    void setVAlign(QFxText::VAlignment align);

    bool isReadOnly() const;
    void setReadOnly(bool);

    int maxLength() const;
    void setMaxLength(int ml);

    int cursorPosition() const;
    void setCursorPosition(int cp);

    int selectionLength() const;
    void setSelectionLength(int len);

    QString selectedText() const;

    QObject * validator() const;
    void setValidator(QObject* v);

    QString inputMask() const;
    void setInputMask(const QString &im);

    uint echoMode() const;
    void setEchoMode(uint echo);

    QmlComponent* cursorDelegate() const;
    void setCursorDelegate(QmlComponent*);

    /*
    int scrollDuration() const;
    void setScrollDuration(int);
    */

    bool hasAcceptableInput() const;

    bool isAwesome() const;
    void setAwesome(bool a);

    void drawContents(QPainter *p,const QRect &r);
Q_SIGNALS:
    void textChanged();
    void cursorPositionChanged();
    void selectionLengthChanged();
    void selectedTextChanged();
    void accepted();
    void acceptableInputChanged();

protected:
    QFxLineEdit(QFxLineEditPrivate &dd, QFxItem *parent);
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent* ev);
    bool event(QEvent *e);

    void focusChanged(bool hasFocus);

private slots:
    void updateSize();
    void q_textChanged();
    void selectionChanged();
    void updateAll();
    void createCursor();
    void moveCursor();
    void rainbowRedraw();

private:
    Q_DECLARE_PRIVATE(QFxLineEdit);
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxLineEdit)
QML_DECLARE_TYPE(QIntValidator)

QT_END_HEADER
#endif // QFXLINEEDIT_H
