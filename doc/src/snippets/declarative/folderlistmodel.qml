//![0]
import Qt 4.7
import Qt.labs.folderlistmodel 1.0

ListView {
    FolderListModel {
        id: foldermodel
        nameFilters: ["*.qml"]
    }
    Component {
        id: filedelegate
        Text { text: fileName }
    }
    model: foldermodel
    delegate: filedelegate
}
//![0]
