import Qt 4.7

// ListModel allows free form list models to be defined and populated.

ListModel {
    id: petsModel
    ListElement {
        name: "Polly"
        type: "Parrot"
        age: 12
        size: "Small"
    }
    ListElement {
        name: "Penny"
        type: "Turtle"
        age: 4
        size: "Small"
    }
    ListElement {
        name: "Warren"
        type: "Rabbit"
        age: 2
        size: "Small"
    }
    ListElement {
        name: "Spot"
        type: "Dog"
        age: 9
        size: "Medium"
    }
    ListElement {
        name: "Schrödinger"
        type: "Cat"
        age: 2
        size: "Medium"
    }
    ListElement {
        name: "Joey"
        type: "Kangaroo"
        age: 1
        size: "Medium"
    }
    ListElement {
        name: "Kimba"
        type: "Bunny"
        age: 65
        size: "Large"
    }
    ListElement {
        name: "Rover"
        type: "Dog"
        age: 5
        size: "Large"
    }
    ListElement {
        name: "Tiny"
        type: "Elephant"
        age: 15
        size: "Large"
    }
}
