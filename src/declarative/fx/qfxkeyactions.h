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

#ifndef QFXKEYACTIONS_H
#define QFXKEYACTIONS_H

#include <QtCore/QObject>
#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxKeyActionsPrivate;
class Q_DECLARATIVE_EXPORT QFxKeyActions : public QFxItem
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString keyA READ key_A WRITE setKey_A)
    Q_PROPERTY(QString keyB READ key_B WRITE setKey_B)
    Q_PROPERTY(QString keyC READ key_C WRITE setKey_C)
    Q_PROPERTY(QString keyD READ key_D WRITE setKey_D)
    Q_PROPERTY(QString keyE READ key_E WRITE setKey_E)
    Q_PROPERTY(QString keyF READ key_F WRITE setKey_F)
    Q_PROPERTY(QString keyG READ key_G WRITE setKey_G)
    Q_PROPERTY(QString keyH READ key_H WRITE setKey_H)
    Q_PROPERTY(QString keyI READ key_I WRITE setKey_I)
    Q_PROPERTY(QString keyJ READ key_J WRITE setKey_J)
    Q_PROPERTY(QString keyK READ key_K WRITE setKey_K)
    Q_PROPERTY(QString keyL READ key_L WRITE setKey_L)
    Q_PROPERTY(QString keyM READ key_M WRITE setKey_M)
    Q_PROPERTY(QString keyN READ key_N WRITE setKey_N)
    Q_PROPERTY(QString keyO READ key_O WRITE setKey_O)
    Q_PROPERTY(QString keyP READ key_P WRITE setKey_P)
    Q_PROPERTY(QString keyQ READ key_Q WRITE setKey_Q)
    Q_PROPERTY(QString keyR READ key_R WRITE setKey_R)
    Q_PROPERTY(QString keyS READ key_S WRITE setKey_S)
    Q_PROPERTY(QString keyT READ key_T WRITE setKey_T)
    Q_PROPERTY(QString keyU READ key_U WRITE setKey_U)
    Q_PROPERTY(QString keyV READ key_V WRITE setKey_V)
    Q_PROPERTY(QString keyW READ key_W WRITE setKey_W)
    Q_PROPERTY(QString keyX READ key_X WRITE setKey_X)
    Q_PROPERTY(QString keyY READ key_Y WRITE setKey_Y)
    Q_PROPERTY(QString keyZ READ key_Z WRITE setKey_Z)
    Q_PROPERTY(QString leftArrow READ key_Left WRITE setKey_Left)
    Q_PROPERTY(QString rightArrow READ key_Right WRITE setKey_Right)
    Q_PROPERTY(QString upArrow READ key_Up WRITE setKey_Up)
    Q_PROPERTY(QString downArrow READ key_Down WRITE setKey_Down)
    Q_PROPERTY(QString digit0 READ key_0 WRITE setKey_0)
    Q_PROPERTY(QString digit1 READ key_1 WRITE setKey_1)
    Q_PROPERTY(QString digit2 READ key_2 WRITE setKey_2)
    Q_PROPERTY(QString digit3 READ key_3 WRITE setKey_3)
    Q_PROPERTY(QString digit4 READ key_4 WRITE setKey_4)
    Q_PROPERTY(QString digit5 READ key_5 WRITE setKey_5)
    Q_PROPERTY(QString digit6 READ key_6 WRITE setKey_6)
    Q_PROPERTY(QString digit7 READ key_7 WRITE setKey_7)
    Q_PROPERTY(QString digit8 READ key_8 WRITE setKey_8)
    Q_PROPERTY(QString digit9 READ key_9 WRITE setKey_9)
    Q_PROPERTY(QString asterisk READ key_Asterisk WRITE setKey_Asterisk)
    Q_PROPERTY(QString escape READ key_Escape WRITE setKey_Escape)
    Q_PROPERTY(QString keyReturn READ key_Return WRITE setKey_Return)
    Q_PROPERTY(QString enter READ key_Enter WRITE setKey_Enter)
    Q_PROPERTY(QString delete READ key_Delete WRITE setKey_Delete)
    Q_PROPERTY(QString space READ key_Space WRITE setKey_Space)
    Q_PROPERTY(QString back READ key_Back WRITE setKey_Back)
    Q_PROPERTY(QString select READ key_Select WRITE setKey_Select)
    Q_PROPERTY(QString yes READ key_Yes WRITE setKey_Yes)
    Q_PROPERTY(QString no READ key_No WRITE setKey_No)
    Q_PROPERTY(QString context1 READ key_Context1 WRITE setKey_Context1)
    Q_PROPERTY(QString context2 READ key_Context2 WRITE setKey_Context2)
    Q_PROPERTY(QString context3 READ key_Context3 WRITE setKey_Context3)
    Q_PROPERTY(QString context4 READ key_Context4 WRITE setKey_Context4)
    Q_PROPERTY(QString call READ key_Call WRITE setKey_Call)
    Q_PROPERTY(QString hangup READ key_Hangup WRITE setKey_Hangup)
    Q_PROPERTY(QString flip READ key_Flip WRITE setKey_Flip)
    Q_PROPERTY(QString any READ key_Any WRITE setKey_Any)

public:
    QFxKeyActions(QFxItem *parent=0);
    virtual ~QFxKeyActions();

    bool enabled() const;
    void setEnabled(bool);

    QString key_A() const;
    void setKey_A(const QString &);

    QString key_B() const;
    void setKey_B(const QString &);

    QString key_C() const;
    void setKey_C(const QString &);

    QString key_D() const;
    void setKey_D(const QString &);

    QString key_E() const;
    void setKey_E(const QString &);

    QString key_F() const;
    void setKey_F(const QString &);

    QString key_G() const;
    void setKey_G(const QString &);

    QString key_H() const;
    void setKey_H(const QString &);

    QString key_I() const;
    void setKey_I(const QString &);

    QString key_J() const;
    void setKey_J(const QString &);

    QString key_K() const;
    void setKey_K(const QString &);

    QString key_L() const;
    void setKey_L(const QString &);

    QString key_M() const;
    void setKey_M(const QString &);

    QString key_N() const;
    void setKey_N(const QString &);

    QString key_O() const;
    void setKey_O(const QString &);

    QString key_P() const;
    void setKey_P(const QString &);

    QString key_Q() const;
    void setKey_Q(const QString &);

    QString key_R() const;
    void setKey_R(const QString &);

    QString key_S() const;
    void setKey_S(const QString &);

    QString key_T() const;
    void setKey_T(const QString &);

    QString key_U() const;
    void setKey_U(const QString &);

    QString key_V() const;
    void setKey_V(const QString &);

    QString key_W() const;
    void setKey_W(const QString &);

    QString key_X() const;
    void setKey_X(const QString &);

    QString key_Y() const;
    void setKey_Y(const QString &);

    QString key_Z() const;
    void setKey_Z(const QString &);

    QString key_Left() const;
    void setKey_Left(const QString &);

    QString key_Right() const;
    void setKey_Right(const QString &);

    QString key_Up() const;
    void setKey_Up(const QString &);

    QString key_Down() const;
    void setKey_Down(const QString &);

    QString key_0() const;
    void setKey_0(const QString &);

    QString key_1() const;
    void setKey_1(const QString &);

    QString key_2() const;
    void setKey_2(const QString &);

    QString key_3() const;
    void setKey_3(const QString &);

    QString key_4() const;
    void setKey_4(const QString &);

    QString key_5() const;
    void setKey_5(const QString &);

    QString key_6() const;
    void setKey_6(const QString &);

    QString key_7() const;
    void setKey_7(const QString &);

    QString key_8() const;
    void setKey_8(const QString &);

    QString key_9() const;
    void setKey_9(const QString &);

    QString key_Asterisk() const;
    void setKey_Asterisk(const QString &);

    QString key_Escape() const;
    void setKey_Escape(const QString &);

    QString key_Return() const;
    void setKey_Return(const QString &);

    QString key_Enter() const;
    void setKey_Enter(const QString &);

    QString key_Delete() const;
    void setKey_Delete(const QString &);

    QString key_Space() const;
    void setKey_Space(const QString &);

    QString key_Back() const;
    void setKey_Back(const QString &);

    QString key_Select() const;
    void setKey_Select(const QString &);

    QString key_Yes() const;
    void setKey_Yes(const QString &);

    QString key_No() const;
    void setKey_No(const QString &);

    QString key_Context1() const;
    void setKey_Context1(const QString &);

    QString key_Context2() const;
    void setKey_Context2(const QString &);

    QString key_Context3() const;
    void setKey_Context3(const QString &);

    QString key_Context4() const;
    void setKey_Context4(const QString &);

    QString key_Call() const;
    void setKey_Call(const QString &);

    QString key_Hangup() const;
    void setKey_Hangup(const QString &);

    QString key_Flip() const;
    void setKey_Flip(const QString &);

    QString key_Any() const;
    void setKey_Any(const QString &);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

Q_SIGNALS:
    void enabledChanged();

private:
    Q_DISABLE_COPY(QFxKeyActions)
    QFxKeyActionsPrivate *d;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxKeyActions)

QT_END_HEADER

#endif // QFXKEYACTIONS_H
