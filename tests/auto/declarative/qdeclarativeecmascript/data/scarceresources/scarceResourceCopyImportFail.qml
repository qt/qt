import QtQuick 1.0
import Qt.test 1.0
import "scarceResourceCopyImportFail.js" as ScarceResourceCopyImportFailJs

QtObject {
    property variant scarceResourceCopy: ScarceResourceCopyImportFailJs.importScarceResource()
}

