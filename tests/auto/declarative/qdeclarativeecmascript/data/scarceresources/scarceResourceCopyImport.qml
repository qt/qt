import QtQuick 1.0
import Qt.test 1.0
import "scarceResourceCopyImport.js" as ScarceResourceCopyImportJs

QtObject {
    // this binding is evaluated once, prior to the resource being released
    property variant scarceResourceCopy: ScarceResourceCopyImportJs.importScarceResource()

    // this code is evaluated on completion, and so copy one should be valid, copy two invalid.
    property variant scarceResourceAssignedCopyOne;
    property variant scarceResourceAssignedCopyTwo;
    Component.onCompleted: {
        scarceResourceAssignedCopyOne = ScarceResourceCopyImportJs.importScarceResource();
        ScarceResourceCopyImportJs.destroyScarceResource();
        scarceResourceAssignedCopyTwo = ScarceResourceCopyImportJs.importScarceResource();
    }
}

