import Qt 4.7

Item{
    width:600;
    height:300;
    Column{
        TextInput{ 
            text: "Jackdaws love my big sphinx of quartz"; 
            horizontalAlignment: TextInput.AlignLeft;
        }
        TextInput{ 
            text: "Jackdaws love my big sphinx of quartz"; 
            horizontalAlignment: TextInput.AlignHCenter;
        }
        TextInput{ 
            text: "Jackdaws love my big sphinx of quartz"; 
            horizontalAlignment: TextInput.AlignRight;
        }
        Rectangle{ width: 600; height: 10; color: "pink" }
        TextInput{ 
            height: 30;
            width: 600;
            text: "Jackdaws love my big sphinx of quartz"; 
            horizontalAlignment: TextInput.AlignLeft;
        }
        TextInput{ 
            height: 30;
            width: 600;
            text: "Jackdaws love my big sphinx of quartz"; 
            horizontalAlignment: TextInput.AlignHCenter;
        }
        TextInput{ 
            height: 30;
            width: 600;
            text: "Jackdaws love my big sphinx of quartz"; 
            horizontalAlignment: TextInput.AlignRight;
        }
    }
}
