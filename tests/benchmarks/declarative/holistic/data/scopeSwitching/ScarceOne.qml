import QtQuick 1.0
import Qt.test 1.0

Item {
    id: scarceResourceConsumer

    property MyScarceResourceProvider a: MyScarceResourceProvider { id: scarceResourceProvider }

    property variant ssr;
    property variant lsr;

    function copyScarceResources() {
        ssr = scarceResourceProvider.smallScarceResource;
        lsr = scarceResourceProvider.largeScarceResource;
    }
}
