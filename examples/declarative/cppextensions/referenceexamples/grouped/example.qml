import People 1.0

// ![0]
BirthdayParty {
    host: Boy {
        name: "Bob Jones"
        shoe { size: 12; color: "white"; brand: "Nike"; price: 90.0 }
    }

    Boy { 
        name: "Leo Hodges" 
        shoe { size: 10; color: "black"; brand: "Reebok"; price: 59.95 }
    }
    // ![1]
    Boy { 
        name: "Jack Smith" 
        shoe { 
            size: 8
            color: "blue"
            brand: "Puma" 
            price: 19.95 
        }
    }
    // ![1]
    Girl { 
        name: "Anne Brown" 
        shoe.size: 7
        shoe.color: "red"
        shoe.brand: "Marc Jacobs"
        shoe.price: 699.99 
    }
}
// ![0]
