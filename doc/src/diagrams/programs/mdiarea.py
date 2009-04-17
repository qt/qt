#!/usr/bin/env python

import sys
from PyQt4.QtCore import SIGNAL
from PyQt4.QtGui import QApplication, QColor, QIcon, QLabel, QMdiArea, QPixmap, \
                        QPushButton, QTableWidget, QTableWidgetItem, QTextEdit


class Changer:

    def __init__(self, mdiArea):
    
        self.mdiArea = mdiArea
        self.state = 0
    
    def change(self):
    
        if self.state == 0:
            self.mdiArea.cascadeSubWindows()
            self.mdiArea.setWindowTitle("Cascade")
        elif self.state == 1:
            self.mdiArea.tileSubWindows()
            self.mdiArea.setWindowTitle("Tile")
        self.state = (self.state + 1) % 2


if __name__ == "__main__":

    app = QApplication(sys.argv)
    pixmap = QPixmap(16, 16)
    pixmap.fill(QColor(0, 0, 0, 0))
    icon = QIcon(pixmap)
    app.setWindowIcon(icon)
    
    mdiArea = QMdiArea()
    
    textEdit = QTextEdit()
    textEdit.setPlainText("Qt Quarterly is a paper-based newsletter "
                          "exclusively available to Qt customers. Every "
                          "quarter we mail out an issue that we hope will "
                          "bring added insight and pleasure to your Qt "
                          "programming, with high-quality technical articles "
                          "written by Qt experts.")
    textWindow = mdiArea.addSubWindow(textEdit)
    textWindow.setWindowTitle("A Text Editor")
    
    label = QLabel()
    label.setPixmap(QPixmap("../../images/qt-logo.png"))
    labelWindow = mdiArea.addSubWindow(label)
    labelWindow.setWindowTitle("A Label")
    
    items = (("Henry", 23), ("Bill", 56), ("Susan", 19), ("Jane", 47))
    table = QTableWidget(len(items), 2)
    
    for i in range(len(items)):
        name, age = items[i]
        item = QTableWidgetItem(name)
        table.setItem(i, 0, item)
        item = QTableWidgetItem(str(age))
        table.setItem(i, 1, item)
    
    tableWindow = mdiArea.addSubWindow(table)
    tableWindow.setWindowTitle("A Table Widget")
    
    mdiArea.show()
    
    changer = Changer(mdiArea)
    button = QPushButton("Cascade")
    button.connect(button, SIGNAL("clicked()"), changer.change)
    button.show()
    sys.exit(app.exec_())
