Item {
    id: Container
    properties: Property {
        name: "rating"
        value: 2
    }
    HorizontalLayout {
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
