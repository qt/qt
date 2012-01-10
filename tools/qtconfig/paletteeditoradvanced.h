/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PALETTEEDITORADVANCED_H
#define PALETTEEDITORADVANCED_H

#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

namespace Ui {
    class PaletteEditorAdvanced;
}

class ColorButton;

class PaletteEditorAdvanced : public QDialog
{
    Q_OBJECT
public:
    PaletteEditorAdvanced(QWidget *parent = 0);
    ~PaletteEditorAdvanced();

    static QPalette getPalette(bool *ok, const QPalette &pal,
                               QPalette::ColorRole backgroundRole = QPalette::Window,
                               QWidget *parent = 0);

    static QPalette buildEffect(QPalette::ColorGroup colorGroup, const QPalette &basePalette);

protected slots:
    void paletteSelected(int);

    void onCentral(int);
    void onEffect(int);

    void onChooseCentralColor();
    void onChooseEffectColor();

    void onToggleBuildEffects(bool);
    void onToggleBuildInactive(bool);
    void onToggleBuildDisabled(bool);

protected:
    void buildEffect(QPalette::ColorGroup);
    void build(QPalette::ColorGroup);

private:
    void updateColorButtons();
    void setupBackgroundRole(QPalette::ColorRole);

    QPalette pal() const;
    void setPal(const QPalette &);

    static QPalette::ColorGroup groupFromIndex(int);
    static QPalette::ColorRole centralFromIndex(int);
    static QPalette::ColorRole effectFromIndex(int);
    QPalette editPalette;

    Ui::PaletteEditorAdvanced *ui;

    int selectedPalette;
    ColorButton *buttonCentral;
    ColorButton *buttonEffect;
};

QT_END_NAMESPACE

#endif // PALETTEEDITORADVANCED_H
