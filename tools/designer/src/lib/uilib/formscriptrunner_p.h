/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef FORMSCRIPTRUNNER_H
#define FORMSCRIPTRUNNER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtDesigner/uilib_global.h>
#include <QtCore/QList>
#include <QtCore/QFlags>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE

class QWidget;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal
{
#endif

class DomWidget;

class QDESIGNER_UILIB_EXPORT QFormScriptRunner
{
public:
    QFormScriptRunner();
    ~QFormScriptRunner();

    typedef QList<QWidget*> WidgetList;

    bool run(const DomWidget *domWidget,
             const QString &customWidgetScript,
             QWidget *widget, const WidgetList &children,
             QString *errorMessage);

    struct Error {
        QString objectName;
        QString script;
        QString errorMessage;
    };
    typedef QList<Error> Errors;
    Errors errors() const;
    void clearErrors();

    enum Option {
         NoOptions = 0x0,
         DisableWarnings = 0x1,
         DisableScripts = 02
     };
     Q_DECLARE_FLAGS(Options, Option)

    Options options() const;
    void setOptions(Options options);

private:
    class QFormScriptRunnerPrivate;
    QFormScriptRunnerPrivate *m_impl;

    QFormScriptRunner(const QFormScriptRunner &);
    void operator = (const QFormScriptRunner &);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QFormScriptRunner::Options)

#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

QT_END_NAMESPACE

#endif // FORMSCRIPTRUNNER_H
