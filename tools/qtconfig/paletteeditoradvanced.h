/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
