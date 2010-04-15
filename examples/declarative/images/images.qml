import Qt 4.7

Rectangle {
    color: "white"
    width: grid.width + 50
    height: grid.height + 50

    Grid {
        id: grid
        x: 25; y: 25
        columns: 3

        Image {
            source: "content/lemonade.jpg"
        }

        Image {
            sourceSize.width: 50
            sourceSize.height: 50
            source: "content/lemonade.jpg"
        }

        Image {
            sourceSize.width: 50
            sourceSize.height: 50
            smooth: true
            source: "content/lemonade.jpg"
        }

        Image {
            scale: 1/3
            source: "content/lemonade.jpg"
        }

        Image {
            scale: 1/3
            sourceSize.width: 50
            sourceSize.height: 50
            source: "content/lemonade.jpg"
        }

        Image {
            scale: 1/3
            sourceSize.width: 50
            sourceSize.height: 50
            smooth: true
            source: "content/lemonade.jpg"
        }

        Image {
            width: 50; height: 50
            transform: Translate { x: 50 }
            source: "content/lemonade.jpg"
        }

        Image {
            width: 50; height: 50
            transform: Translate { x: 50 }
            sourceSize.width: 50
            sourceSize.height: 50
            source: "content/lemonade.jpg"
        }

        Image {
            width: 50; height: 50
            transform: Translate { x: 50 }
            sourceSize: "50x50" // syntactic sugar
            smooth: true
            source: "content/lemonade.jpg"
        }
    }
}
