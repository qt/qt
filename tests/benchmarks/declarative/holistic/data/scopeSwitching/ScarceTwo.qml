import QtQuick 1.0
import Qt.test 1.0

Item {
    id: scarceResourceConsumer

    property MyScarceResourceProvider a: MyScarceResourceProvider { id: scarceResourceProvider }

    property variant ssr: scarceResourceProvider.smallScarceResource
    property variant lsr: scarceResourceProvider.largeScarceResource
}
