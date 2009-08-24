import Qt 4.6

Item {
    id: Container

    property int rating: 2

    Row {
        Star {
            rating: 0
            onClicked: { Container.rating = rating }
            on: Container.rating >= 0
        }
        Star {
            rating: 1
            onClicked: { Container.rating = rating }
            on: Container.rating >= 1
        }
        Star {
            rating: 2
            onClicked: { Container.rating = rating }
            on: Container.rating >= 2
        }
        Star {
            rating: 3
            onClicked: { Container.rating = rating }
            on: Container.rating >= 3
        }
        Star {
            rating: 4
            onClicked: { Container.rating = rating }
            on: Container.rating >= 4
        }
    }
}
