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

#ifndef QFXTEXT_H
#define QFXTEXT_H

#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QFxTextPrivate;
class Q_DECLARATIVE_EXPORT QFxText : public QFxItem
{
    Q_OBJECT
    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)
    Q_ENUMS(TextStyle)
    Q_ENUMS(TextFormat)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(TextStyle style READ style WRITE setStyle)
    Q_PROPERTY(QColor styleColor READ styleColor WRITE setStyleColor)
    Q_PROPERTY(HAlignment hAlign READ hAlign WRITE setHAlign) // ### horizontalAlignment?
    Q_PROPERTY(VAlignment vAlign READ vAlign WRITE setVAlign) // ### verticalAlignment?
    Q_PROPERTY(bool wrap READ wrap WRITE setWrap) //### there are several wrap modes in Qt
    Q_PROPERTY(TextFormat textFormat READ textFormat WRITE setTextFormat)
    Q_PROPERTY(Qt::TextElideMode elide READ elideMode WRITE setElideMode) //### elideMode?

public:
    QFxText(QFxItem *parent=0);
    ~QFxText();

    enum HAlignment { AlignLeft = Qt::AlignLeft,
                       AlignRight = Qt::AlignRight,
                       AlignHCenter = Qt::AlignHCenter };
    enum VAlignment { AlignTop = Qt::AlignTop,
                       AlignBottom = Qt::AlignBottom,
                       AlignVCenter = Qt::AlignVCenter };
    enum TextStyle { Normal,
                      Outline,
                      Raised,
                      Sunken };
    enum TextFormat { PlainText = Qt::PlainText,
                      RichText = Qt::RichText,
                      AutoText = Qt::AutoText };

    QString text() const;
    void setText(const QString &);

    QFont font() const;
    void setFont(const QFont &font);

    QColor color() const;
    void setColor(const QColor &c);

    TextStyle style() const;
    void setStyle(TextStyle style);

    QColor styleColor() const;
    void setStyleColor(const QColor &c);

    HAlignment hAlign() const;
    void setHAlign(HAlignment align);

    VAlignment vAlign() const;
    void setVAlign(VAlignment align);

    bool wrap() const;
    void setWrap(bool w);

    TextFormat textFormat() const;
    void setTextFormat(TextFormat format);

    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    virtual void componentComplete();

Q_SIGNALS:
    void textChanged(const QString &text);
    void linkActivated(const QString &link);

protected:
    QFxText(QFxTextPrivate &dd, QFxItem *parent);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

private:
    Q_DISABLE_COPY(QFxText)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QFxText)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxText)

QT_END_HEADER

#endif
