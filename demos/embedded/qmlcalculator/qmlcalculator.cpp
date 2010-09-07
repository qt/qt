/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeView>

#if defined(Q_OS_SYMBIAN)
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif // Q_OS_SYMBIAN

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    const QString mainQmlApp = QLatin1String("calculator.qml");
    QDeclarativeView view;
    view.setSource(QUrl(mainQmlApp));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);

#if defined(QT_KEYPAD_NAVIGATION)
    QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
#endif // QT_KEYPAD_NAVIGATION

#if defined(Q_OS_SYMBIAN)
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAPD(error,
        if (appUi)
            appUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait)
    )
    view.showFullScreen();
#else // Q_OS_SYMBIAN
    view.show();
#endif // Q_OS_SYMBIAN

    return application.exec();
}
