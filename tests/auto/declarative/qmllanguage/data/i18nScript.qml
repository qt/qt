import Qt 4.6

Text {
    Script {
        function val() {
            var áâãäå = 10
            return "Test áâãäå: " + áâãäå
        }
        
    }
    text: val()
}
