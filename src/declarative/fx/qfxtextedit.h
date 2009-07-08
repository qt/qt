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

#ifndef QFXTEXTEDIT_H
#define QFXTEXTEDIT_H

#include <QtDeclarative/qfxtext.h>
#include <QtDeclarative/qfxpainteditem.h>

#include <QtGui/qtextdocument.h>
#include <QtGui/qtextoption.h>
#include <QtGui/qtextcursor.h>
#include <QtGui/qtextformat.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
/*!
WARNING: SHORT TERM CLASS.  INTENDED TO MERGE INTO QFxTextItem
*/
class QFxTextEditPrivate;
class Q_DECLARATIVE_EXPORT QFxTextEdit : public QFxPaintedItem
{
    Q_OBJECT
    Q_ENUMS(VAlignment)
    Q_ENUMS(HAlignment)
    Q_ENUMS(TextFormat)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setHighlightColor)
    Q_PROPERTY(QColor highlightedTextColor READ highlightedTextColor WRITE setHighlightedTextColor)
    Q_PROPERTY(QmlFont * font READ font)
    Q_PROPERTY(HAlignment hAlign READ hAlign WRITE setHAlign)
    Q_PROPERTY(VAlignment vAlign READ vAlign WRITE setVAlign)
    Q_PROPERTY(bool wrap READ wrap WRITE setWrap)
    Q_PROPERTY(TextFormat textFormat READ textFormat WRITE setTextFormat)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool cursorVisible READ isCursorVisible WRITE setCursorVisible)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(QmlComponent* cursorDelegate READ cursorDelegate WRITE setCursorDelegate)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)
    Q_PROPERTY(QString selectedText READ selectedText NOTIFY selectionChanged)
    Q_PROPERTY(bool focusOnPress READ focusOnPress WRITE setFocusOnPress)
    Q_PROPERTY(bool preserveSelection READ preserveSelection WRITE setPreserveSelection)
    Q_PROPERTY(qreal textMargin READ textMargin WRITE setTextMargin)

public:
    QFxTextEdit(QFxItem *parent=0);

    enum HAlignment {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter
    };

    enum VAlignment {
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom,
        AlignVCenter = Qt::AlignVCenter
    };

    enum TextFormat {
        AutoText,
        PlainText,
        RichText,
    };

    QString text() const;
    void setText(const QString &);

    TextFormat textFormat() const;
    void setTextFormat(TextFormat format);

    // leave in, have affect the default text
    QmlFont *font();

    QColor color() const;
    void setColor(const QColor &c);

    QColor highlightColor() const;
    void setHighlightColor(const QColor &c);

    QColor highlightedTextColor() const;
    void setHighlightedTextColor(const QColor &c);

    HAlignment hAlign() const;
    void setHAlign(HAlignment align);

    VAlignment vAlign() const;
    void setVAlign(VAlignment align);

    bool wrap() const;
    void setWrap(bool w);

    bool isCursorVisible() const;
    void setCursorVisible(bool on);

    int cursorPosition() const;
    void setCursorPosition(int pos);

    QmlComponent* cursorDelegate() const;
    void setCursorDelegate(QmlComponent*);

    int selectionStart() const;
    void setSelectionStart(int);

    int selectionEnd() const;
    void setSelectionEnd(int);

    QString selectedText() const;

    bool focusOnPress() const;
    void setFocusOnPress(bool on);

    bool preserveSelection() const;
    void setPreserveSelection(bool on);

    qreal textMargin() const;
    void setTextMargin(qreal margin);

    virtual void dump(int depth);
    virtual QString propertyInfo() const;

    virtual void componentComplete();

    /* FROM EDIT */
    void setReadOnly(bool);
    bool isReadOnly() const;

    void setTextInteractionFlags(Qt::TextInteractionFlags flags);
    Qt::TextInteractionFlags textInteractionFlags() const;

    QTextCursor cursorForPosition(const QPoint &pos) const;
    QRect cursorRect(const QTextCursor &cursor) const;
    QRect cursorRect() const;

    void setTextCursor(const QTextCursor &cursor);
    QTextCursor textCursor() const;

    void moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);

    QVariant inputMethodQuery(Qt::InputMethodQuery property) const;

Q_SIGNALS:
    void textChanged(const QString &);
    void cursorPositionChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void selectionChanged();

public Q_SLOTS:
    void selectAll();

private Q_SLOTS:
    void fontChanged();
    void updateImgCache(const QRectF &rect);
    void q_textChanged();
    void updateSelectionMarkers();
    void moveCursorDelegate();
    void loadCursorDelegate();

private:
    void updateSize();

protected:
    QFxTextEdit(QFxTextEditPrivate &dd, QFxItem *parent);
    virtual void geometryChanged(const QRectF &newGeometry, 
                                 const QRectF &oldGeometry);

    bool event(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

    void focusChanged(bool);

    // mouse filter?
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void inputMethodEvent(QInputMethodEvent *e);

    void drawContents(QPainter *, const QRect &);
private:

    friend class QmlFont;
    Q_DISABLE_COPY(QFxTextEdit)
    Q_DECLARE_PRIVATE(QFxTextEdit)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxTextEdit)

QT_END_HEADER

#endif
