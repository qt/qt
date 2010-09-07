/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7

Component {
    id: listDelegate
    Item {
        id: wrapper; width: wrapper.ListView.view.width; height: if(txt.height > 60){txt.height+10}else{60} //50+5+5
        function handleLink(link){
            if(link.slice(0,3) == 'app'){
                screen.setUser(link.slice(7));
            }else if(link.slice(0,4) == 'http'){
                Qt.openUrlExternally(link);
            }
        }
        function addTags(str){
            var ret = str.replace(/@[a-zA-Z0-9_]+/g, '<a href="app://$&">$&</a>');//click to jump to user?
            var ret2 = ret.replace(/http:\/\/[^ \n\t]+/g, '<a href="$&">$&</a>');//surrounds http links with html link tags
            return ret2;
        }

        // Strip away paranthesis
        function userName(str) {
            var user = str.replace(/\([\S|\s]*\)/gi, "");
            return user.trim();
        }

        Item {
            id: moveMe; height: parent.height
            Rectangle {
                id: blackRect
                color: "black"; opacity: wrapper.ListView.index % 2 ? 0.2 : 0.3; height: wrapper.height-2; width: wrapper.width; y: 1
            }
            Item {
                id: image; x: 6; width: 48; height: 48; smooth: true
                anchors.verticalCenter: parent.verticalCenter

                Loading { x: 1; y: 1; width: 48; height: 48; visible: realImage.status != Image.Ready }
                Image {
                    id: realImage;
                    source: userImage; x: 1; y: 1;
                    width:48; height:48; opacity:0 ;
                    onStatusChanged: {
                        if(status==Image.Ready)
                            image.state="loaded"
                    }
                }
                states: State {
                    name: "loaded";
                    PropertyChanges { target: realImage ; opacity:1 }
                }
                transitions: Transition { NumberAnimation { target: realImage; property: "opacity"; duration: 200 } }

            }
            Text { id:txt; y:4; x: 56
                text: '<html><style type="text/css">a:link {color:"#aaccaa"}; a:visited {color:"#336633"}</style>'
                    + '<a href="app://@'+userName(name)+'"><b>'+userName(name) + "</b></a> from " +source
                    + "<br /><b>" + statusText + "</b></html>";
                textFormat: Qt.RichText
                color: "#cccccc"; style: Text.Raised; styleColor: "black"; wrapMode: Text.WordWrap
                anchors.left: image.right; anchors.right: blackRect.right; anchors.leftMargin: 6; anchors.rightMargin: 6
                onLinkActivated: wrapper.handleLink(link)
            }
        }
    }
}
