import Qt 4.6
import "../../flickr/common"

Component {
    id: ListDelegate
    Item {
        id: Wrapper; width: Wrapper.ListView.view.width; height: if(txt.height > 58){txt.height+8}else{58}//50+4+4
        Script {
            function handleLink(link){
                if(link.slice(0,3) == 'app'){
                    setUser(link.slice(7));
                    Screen.setMode(true);
                }else if(link.slice(0,4) == 'http'){
                    Qt.DesktopServices.openUrl(link);
                }
            }
            function addTags(str){
                ret = str.replace(/@[a-zA-Z0-9_]+/g, '<a href="app://$&">$&</a>');//click to jump to user?
                ret2 = ret.replace(/http:\/\/[^ \n\t]+/g, '<a href="$&">$&</a>');//surrounds http links with html link tags
                return ret2;
            }
        }
        Item {
            id: MoveMe; height: parent.height
            Rectangle { 
                id: BlackRect
                color: "black"; opacity: Wrapper.ListView.index % 2 ? 0.2 : 0.3; height: Wrapper.height-2; width: Wrapper.width; y: 1 
            }
            Rectangle {
                id: WhiteRect; x: 6; width: 50; height: 50; color: "white"; smooth: true
                anchors.verticalCenter: parent.verticalCenter

                Loading { x: 1; y: 1; width: 48; height: 48; visible: RealImage.status != 1 } 
                Image { id: RealImage; source: userImage; x: 1; y: 1; width:48; height:48 }
            }
            Text { id:txt; y:4; x: 56
                text: '<html><style type="text/css">a:link {color:"#aaccaa"}; a:visited {color:"#336633"}</style>'
                    + '<a href="app://@'+userScreenName+'"><b>'+userScreenName + "</b></a>  from " +source
                    + "<br /><b>" + addTags(statusText) + "</b></html>";
                textFormat: Qt.RichText
                color: "white"; color: "#cccccc"; style: "Raised"; styleColor: "black"; wrap: true
                anchors.left: WhiteRect.right; anchors.right: BlackRect.right; anchors.leftMargin: 6; anchors.rightMargin: 6
                onLinkActivated: handleLink(link)
            }
        }
    }
}
