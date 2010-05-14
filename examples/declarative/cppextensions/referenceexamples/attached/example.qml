import People 1.0

BirthdayParty {
    host: Boy {
        name: "Bob Jones"
        shoe { size: 12; color: "white"; brand: "Nike"; price: 90.0 }
    }

    // ![1]
    Boy { 
        name: "Leo Hodges" 
        shoe { size: 10; color: "black"; brand: "Reebok"; price: 59.95 }

        BirthdayParty.rsvp: "2009-07-06"
    }
    // ![1]
    Boy { 
        name: "Jack Smith" 
        shoe { size: 8; color: "blue"; brand: "Puma"; price: 19.95 }
    }
    Girl { 
        name: "Anne Brown" 
        shoe.size: 7
        shoe.color: "red"
        shoe.brand: "Marc Jacobs"
        shoe.price: 699.99 

        BirthdayParty.rsvp: "2009-07-01"
    }
}

