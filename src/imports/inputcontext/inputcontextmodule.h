/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef INPUTCONTEXTMODULE_H
#define INPUTCONTEXTMODULE_H

#include <QtCore/qobject.h>
#include <QtCore/qrect.h>
#include <QtGui/qfont.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class DeclarativeInputContext;

class InputContextModule : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool softwareInputPanelVisible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool focus READ hasFocus NOTIFY focusChanged)
    Q_PROPERTY(QString preeditText READ preeditText WRITE setPreeditText NOTIFY preeditTextChanged)
    Q_PROPERTY(QRect microFocus READ microFocus NOTIFY contextUpdated)
    Q_PROPERTY(QFont font READ font NOTIFY contextUpdated)
    Q_PROPERTY(int cursorPosition READ cursorPosition NOTIFY contextUpdated)
    Q_PROPERTY(int anchorPosition READ anchorPosition NOTIFY contextUpdated)
    Q_PROPERTY(int maximumTextLength READ maximumTextLength NOTIFY contextUpdated)
    Q_PROPERTY(QString surroundingText READ surroundingText NOTIFY contextUpdated)
    Q_PROPERTY(QString selectedText READ selectedText NOTIFY contextUpdated)
public:
    explicit InputContextModule(QObject *parent = 0);
    ~InputContextModule();

    bool hasFocus() const;
    void setFocusWidget(QWidget *widget);

    bool isVisible() const;
    void setVisible(bool visible);

    QString preeditText() const;
    void setPreeditText(const QString &text);

    QRect microFocus() const;
    QFont font() const;
    int cursorPosition() const;
    int anchorPosition() const;
    int maximumTextLength() const;
    QString surroundingText() const;
    QString selectedText() const;

    Q_INVOKABLE void sendKeyPress(
            int key, const QString &text, int modifiers = 0, bool autoRepeat = false, int count = 1);
    Q_INVOKABLE void sendKeyRelease(int key, const QString &text, int modifiers = 0, int count = 1);

    Q_INVOKABLE void sendPreedit(const QString &text, int cursor = -1);

    Q_INVOKABLE void commit();
    Q_INVOKABLE void commit(const QString &text, int replacementStart = 0, int replacementEnd = 0);

    Q_INVOKABLE void clear();

    void update();

signals:
    void preeditTextChanged();
    void visibleChanged();
    void contextUpdated();
    void focusChanged();

private:
    QString m_preeditText;
    DeclarativeInputContext *m_inputContext;
    QWidget *m_focusWidget;
    bool m_visible;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
