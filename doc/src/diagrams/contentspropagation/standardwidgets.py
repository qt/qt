#!/usr/bin/env python

import os, sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *


def createGroupBox(parent, attributes = None, fill = False, fake = False):

    background = CustomWidget(parent, fake)
    backgroundLayout = QVBoxLayout()
    backgroundLayout.setMargin(4)
    background.setLayout(backgroundLayout)
    
    groupBox = QGroupBox("&Options")
    layout = QGridLayout()
    groupBox.setLayout(layout)
    layout.addWidget(QCheckBox("C&ase sensitive"), 0, 0)
    layout.addWidget(QCheckBox("W&hole words"), 0, 1)
    checkedBox = QCheckBox("Search &forwards")
    checkedBox.setChecked(True)
    layout.addWidget(checkedBox, 1, 0)
    layout.addWidget(QCheckBox("From &start of text"), 1, 1)
    
    backgroundLayout.addWidget(groupBox)
    
    if attributes:
        for attr in attributes:
            groupBox.setAttribute(attr, True)
            if not fake:
                background.setAttribute(attr, True)
    
    groupBox.setAutoFillBackground(fill)
    background.setAutoFillBackground(fill)
    
    return background
    
class CustomWidget(QWidget):

    def __init__(self, parent, fake = False):
    
        QWidget.__init__(self, parent)
        self.fake = fake
        self.fakeBrush = QBrush(Qt.red, Qt.DiagCrossPattern)

    def paintEvent(self, event):
    
        painter = QPainter()
        painter.begin(self)
        painter.setRenderHint(QPainter.Antialiasing)
        if self.fake:
            painter.fillRect(event.rect(), QBrush(Qt.white))
            painter.fillRect(event.rect(), self.fakeBrush)
        painter.end()


if __name__ == "__main__":

    try:
        qt = sys.argv[1]
    except IndexError:
        qt = "4.1"
    
    if qt != "4.0" and qt != "4.1":
        sys.stderr.write("Usage: %s [4.0|4.1]\n" % sys.argv[0])
        sys.exit(1)
    
    app = QApplication(sys.argv)
    exec_dir = os.path.split(os.path.abspath(sys.argv[0]))[0]
    label = QLabel()
    label.setPixmap(QPixmap(os.path.join(exec_dir, "lightbackground.png")))
    
    layout = QGridLayout()
    label.setLayout(layout)
    if qt == "4.0":
        layout.addWidget(createGroupBox(label), 0, 0, Qt.AlignCenter)
        caption = QLabel("Opaque (Default)", label)
        caption.setMargin(2)
        layout.addWidget(caption, 1, 0, Qt.AlignCenter | Qt.AlignTop)
    elif qt == "4.1":
        layout.addWidget(createGroupBox(label), 0, 0, Qt.AlignCenter)
        caption = QLabel("Contents Propagated (Default)", label)
        caption.setAutoFillBackground(True)
        caption.setMargin(2)
        layout.addWidget(caption, 1, 0, Qt.AlignCenter | Qt.AlignTop)
    
    if qt == "4.0":
        contentsWidget = createGroupBox(label)
        contentsWidget.setAttribute(Qt.WA_ContentsPropagated, True)
        layout.addWidget(contentsWidget, 0, 1, Qt.AlignCenter)
        caption = QLabel("With WA_ContentsPropagated set", label)
        caption.setMargin(2)
        layout.addWidget(caption, 1, 1, Qt.AlignCenter | Qt.AlignTop)
    elif qt == "4.1":
        autoFillWidget = createGroupBox(label, fill = True)
        layout.addWidget(autoFillWidget, 0, 1, Qt.AlignCenter)
        caption = QLabel("With autoFillBackground set", label)
        caption.setAutoFillBackground(True)
        caption.setMargin(2)
        layout.addWidget(caption, 1, 1, Qt.AlignCenter | Qt.AlignTop)
    
#    if qt == "4.0":
#        noBackgroundWidget = createGroupBox(
#            label, attributes = [Qt.WA_NoBackground], fake = True)
#        layout.addWidget(noBackgroundWidget, 2, 0, Qt.AlignCenter)
#        caption = QLabel("With WA_NoBackground set", label)
#        caption.setWordWrap(True)
#        caption.setMargin(2)
#        layout.addWidget(caption, 3, 0, Qt.AlignCenter | Qt.AlignTop)
#    elif qt == "4.1":
#        opaqueWidget = createGroupBox(
#            label, attributes = [Qt.WA_OpaquePaintEvent], fake = True)
#        layout.addWidget(opaqueWidget, 2, 0, Qt.AlignCenter)
#        caption = QLabel("With WA_OpaquePaintEvent set", label)
#        caption.setAutoFillBackground(True)
#        caption.setMargin(2)
#        layout.addWidget(caption, 3, 0, Qt.AlignCenter | Qt.AlignTop)
#    
#    if qt == "4.0":
#        contentsNoBackgroundWidget = createGroupBox(
#            label, attributes = [Qt.WA_ContentsPropagated, Qt.WA_NoBackground],
#            fake = True)
#        layout.addWidget(contentsNoBackgroundWidget, 2, 1, Qt.AlignCenter)
#        caption = QLabel("With WA_ContentsPropagated and WA_NoBackground set", label)
#        caption.setMargin(2)
#        layout.addWidget(caption, 3, 1, Qt.AlignCenter | Qt.AlignTop)
#    elif qt == "4.1":
#        opaqueAutoFillWidget = createGroupBox(
#            label, attributes = [Qt.WA_OpaquePaintEvent], fill = True, fake = True)
#        layout.addWidget(opaqueAutoFillWidget, 2, 1, Qt.AlignCenter)
#        caption = QLabel("With WA_OpaquePaintEvent and autoFillBackground set", label)
#        caption.setWordWrap(True)
#        caption.setAutoFillBackground(True)
#        caption.setMargin(2)
#        layout.addWidget(caption, 3, 1, Qt.AlignCenter | Qt.AlignTop)
    
    if qt == "4.0":
        label.setWindowTitle("Qt 4.0: Painting Standard Qt Widgets")
    elif qt == "4.1":
        label.setWindowTitle("Qt 4.1: Painting Standard Qt Widgets")
    
    label.resize(480, 140)
    label.show()
    sys.exit(app.exec_())
