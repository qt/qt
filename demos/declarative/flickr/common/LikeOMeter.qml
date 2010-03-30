import Qt 4.7

Item {
    id: container

    property int rating: 2

    Row {
        Star {
            rating: 0
            onClicked: { container.rating = rating }
            on: container.rating >= 0
        }
        Star {
            rating: 1
            onClicked: { container.rating = rating }
            on: container.rating >= 1
        }
        Star {
            rating: 2
            onClicked: { container.rating = rating }
            on: container.rating >= 2
        }
        Star {
            rating: 3
            onClicked: { container.rating = rating }
            on: container.rating >= 3
        }
        Star {
            rating: 4
            onClicked: { container.rating = rating }
            on: container.rating >= 4
        }
    }
}
