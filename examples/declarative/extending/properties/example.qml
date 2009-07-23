import Qt 4.6

// ![0]
BirthdayParty {
    celebrant: Person {
        name: "Bob Jones"
        shoeSize: 12
    }
    guests: [
        Person { name: "Joan Hodges" },
        Person { name: "Jack Smith" },
        Person { name: "Anne Brown" }
    ]
}
// ![0]
