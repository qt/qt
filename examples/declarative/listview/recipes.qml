import Qt 4.6
import "content"

// This example illustrates expanding a list item to show a more detailed view

Rectangle {
    id: page
    width: 400; height: 240; color: "black"

    // Delegate for the recipes.  This delegate has two modes:
    // 1. the list mode (default), which just shows the picture and title of the recipe.
    // 2. the details mode, which also shows the ingredients and method.
    Component {
        id: recipeDelegate
        Item {
            id: wrapper
            width: list.width

            // Create a property to contain the visibility of the details.
            // We can bind multiple element's opacity to this one property,
            // rather than having a "PropertyChanges" line for each element we
            // want to fade.
            property real detailsOpacity : 0

            // A simple rounded rectangle for the background
            Rectangle {
                id: background
                x: 1; y: 2; width: parent.width - 2; height: parent.height - 4
                color: "#FEFFEE"; border.color: "#FFBE4F"; radius: 5
            }

            // This mouse region covers the entire delegate.
            // When clicked it changes mode to 'Details'.  If we are already
            // in Details mode, then no change will happen.
            MouseArea {
                id: pageMouse
                anchors.fill: parent
                onClicked: wrapper.state = 'Details';
            }

            // Layout the page.  Picture, title and ingredients at the top, method at the
            // bottom.  Note that elements that should not be visible in the list
            // mode have their opacity set to wrapper.detailsOpacity.
            Row {
                id: topLayout
                x: 10; y: 10; height: recipePic.height; width: parent.width
                spacing: 10

                Image {
                    id: recipePic
                    source: picture; width: 48; height: 48
                }

                Column {
                    height: recipePic.height; width: background.width-recipePic.width-20
                    spacing: 5
                    Text { id: name; text: title; font.bold: true; font.pointSize: 16 }
                    Text {
                        text: "Ingredients"; font.pointSize: 12; font.bold: true
                        opacity: wrapper.detailsOpacity
                    }
                    Text {
                        text: ingredients; wrap: true; width: parent.width
                        opacity: wrapper.detailsOpacity
                    }
                }
            }

            Item {
                id: details
                x: 10; width: parent.width-20
                anchors.top: topLayout.bottom; anchors.topMargin: 10
                anchors.bottom: parent.bottom; anchors.bottomMargin: 10
                opacity: wrapper.detailsOpacity

                Text {
                    id: methodTitle
                    text: "Method"; font.pointSize: 12; font.bold: true
                    anchors.top: parent.top
                }
                Flickable {
                    id: flick
                    anchors.top: methodTitle.bottom; anchors.bottom: parent.bottom
                    width: parent.width; contentHeight: methodText.height; clip: true
                    Text { id: methodText; text: method; wrap: true; width: details.width }
                }
                Image {
                    anchors.right: flick.right; anchors.top: flick.top
                    source: "content/pics/moreUp.png"; opacity: flick.atYBeginning ? 0 : 1
                }
                Image {
                    anchors.right: flick.right; anchors.bottom: flick.bottom
                    source: "content/pics/moreDown.png"; opacity: flick.atYEnd ? 0 : 1
                }
            }

            // A button to close the detailed view, i.e. set the state back to default ('').
            MediaButton {
                anchors.right: background.right; anchors.rightMargin: 5
                y: 10; opacity: wrapper.detailsOpacity
                text: "Close"; onClicked: wrapper.state = '';
            }

            // Make the default height equal the hight of the picture, plus margin.
            height: 68

            states: State {
                name: "Details"
                PropertyChanges { target: background; color: "white" }
                // Make the picture bigger
                PropertyChanges { target: recipePic; width: 128; height: 128 }
                // Make details visible
                PropertyChanges { target: wrapper; detailsOpacity: 1; x: 0 }
                // Make the detailed view fill the entire list area
                PropertyChanges { target: wrapper; height: list.height }
                // Move the list so that this item is at the top.
                PropertyChanges { target: wrapper.ListView.view; explicit: true; contentY: wrapper.y }
                // Disallow flicking while we're in detailed view
                PropertyChanges { target: wrapper.ListView.view; interactive: false }
            }

            transitions: Transition {
                // Make the state changes smooth
                ParallelAnimation {
                    ColorAnimation { property: "color"; duration: 500 }
                    NumberAnimation {
                        duration: 300; properties: "detailsOpacity,x,contentY,height,width"
                    }
                }
            }
        }
    }

    // The actual list
    ListView {
        id: list
        model: Recipes; delegate: recipeDelegate
        anchors.fill: parent; clip: true
    }
}
