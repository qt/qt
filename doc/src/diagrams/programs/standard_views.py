#!/usr/bin/env python

import sys
from PyQt4.QtCore import QDir, Qt
from PyQt4.QtGui import *

app = QApplication(sys.argv)

background = QWidget()
palette = QPalette()
palette.setColor(QPalette.Window, QColor(Qt.white))
background.setPalette(palette)

model = QFileSystemModel()
model.setRootPath(QDir.currentPath())

treeView = QTreeView(background)
treeView.setModel(model)
treeView.setRootIndex(model.index(QDir.currentPath()))

listView = QListView(background)
listView.setModel(model)
listView.setRootIndex(model.index(QDir.currentPath()))

tableView = QTableView(background)
tableView.setModel(model)
tableView.setRootIndex(model.index(QDir.currentPath()))

selection = QItemSelectionModel(model)
treeView.setSelectionModel(selection)
listView.setSelectionModel(selection)
tableView.setSelectionModel(selection)

layout = QHBoxLayout(background)
layout.addWidget(listView)
layout.addSpacing(24)
layout.addWidget(treeView, 1)
layout.addSpacing(24)
layout.addWidget(tableView)
background.show()

sys.exit(app.exec_())
