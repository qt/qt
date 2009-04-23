<?qtfx namespacepath:=content?>
<Item width="320" height="500" id="webbrowser" state="Normal">
    <properties><Property name="url" value="http://www.qtsoftware.com"/></properties>
    <Script>
        function zoomOut() {
            webbrowser.state = "ZoomedOut";
        }
        function toggleZoom() {
            if(webbrowser.state == "ZoomedOut") {
                flick.centerX = webview.mouseX;
                flick.centerY = webview.mouseY;
                webbrowser.state = "Normal";
            } else {
                zoomOut();
            }
        }
    </Script>

    <Item id="webpanel" anchors.fill="{parent}" clip="true">
        <Rect color="#555555" anchors.fill="{parent}"/>
        <Image src="content/pics/softshadow-bottom.png" width="{webpanel.width}" height="16" />
        <Image src="content/pics/softshadow-top.png" width="{webpanel.width}" anchors.bottom="{footer.top}" height="16" />
        <RectSoftShadow x="{-flick.xPosition}" y="{-flick.yPosition}"
            width="{webview.width*webview.scale}" height="{flick.y+webview.height*webview.scale}"/>
        <Item id="headerspace" width="{parent.width}" height="60" z="1">
            <Image id="header" width="{parent.width}" state="normal"
                    x="{flick.xPosition &lt; 0
                                ? -flick.xPosition
                                : flick.xPosition &gt; flick.viewportWidth-flick.width
                                    ? -flick.xPosition+flick.viewportWidth-flick.width
                                    : 0}"
                    y="{flick.yPosition &lt; 0 ? -flick.yPosition : progressoff*(flick.yPosition>height?-height:-flick.yPosition)}"
                    height="60" src="content/pics/header.png">
                <Text id="header_text" text="{webview.title!='' || webview.progress == 1.0 ? webview.title : 'Loading...'}"
                    color="#000000"
                    font.family="Helvetica" font.size="9" font.bold="true" elide="ElideRight"
                    anchors.left="{header.left}" anchors.right="{header.right}"
                    anchors.leftMargin="4" anchors.rightMargin="4"
                    anchors.top="{header.top}" anchors.topMargin="4" hAlign="AlignHCenter"/>
                <Item anchors.top="{header_text.bottom}" anchors.topMargin="2" anchors.bottom="{parent.bottom}" width="{parent.width}">
                    <Item id="urlbox" anchors.left="{parent.left}" anchors.leftMargin="12" anchors.right="{parent.right}" anchors.rightMargin="12" height="31" anchors.top="{parent.top}" clip="true">
                        <Image src="content/pics/addressbar.sci" anchors.fill="{urlbox}"/>
                        <Image id="urlboxhl" src="content/pics/addressbar-filled.sci" opacity="{1-header.progressoff}" clip="true" width="{parent.width*webview.progress}" height="{parent.height}"/>
                        <KeyProxy id="proxy" anchors.left="{urlbox.left}" anchors.fill="{urlbox}" focusable="true" targets="{[keyActions,editurl]}"/>
                        <KeyActions id="keyActions" return="webbrowser.url = editurl.text; proxy.focus=false;"/>
                        <TextEdit id="editurl" color="#555555" text="{webview.url == '' ? ' ' : webview.url}"
                            anchors.left="{urlbox.left}" anchors.right="{urlbox.right}" anchors.leftMargin="6"
                            anchors.verticalCenter="{urlbox.verticalCenter}" anchors.verticalCenterOffset="1"
                            font.size="11" wrap="false" opacity="0"/>
                        <Text id="showurl" color="#555555" text="{webview.url == '' ? ' ' : webview.url}"
                            anchors.left="{urlbox.left}" anchors.right="{urlbox.right}" anchors.leftMargin="6"
                            anchors.verticalCenter="{urlbox.verticalCenter}" anchors.verticalCenterOffset="1"
                            font.size="11" />
                    </Item>
                    <MouseRegion anchors.fill="{urlbox}" onClicked="proxy.focus=true"/>
                </Item>
                <properties>
                    <Property name="progressoff" value="1" type="Real"/>
                </properties>
                <states>
                    <State name="normal" when="{webview.progress == 1.0}">
                        <SetProperty target="{header}" property="progressoff" value="1"/>
                    </State>
                    <State name="progressshown" when="{webview.progress &lt; 1.0}">
                        <SetProperty target="{header}" property="progressoff" value="0"/>
                    </State>
                </states>
                <transitions>
                    <Transition>
                        <NumericAnimation target="{header}" properties="progressoff" easing="easeInOutQuad" duration="300"/>
                    </Transition>
                </transitions>
            </Image>
        </Item>
        <Flickable id="flick"
                anchors.top="{headerspace.bottom}" anchors.bottom="{footer.top}"
                anchors.left="{parent.left}" anchors.right="{parent.right}"
                width="{parent.width}"
                viewportWidth="{Math.max(parent.width,webview.width*webview.scale)}"
                viewportHeight="{Math.max(parent.height,webview.height*webview.scale)}"
                >
            <properties>
                <Property name="centerX" value="0" type="Real"/>
                <Property name="centerY" value="0" type="Real"/>
            </properties>
            <WebView id="webview"
                cacheSize="4000000"
                smooth="true"
                url="{webbrowser.url}"
                onDoubleClick="toggleZoom()"
                focusable="true"
                focus="true"
                idealWidth="{flick.width}"
                idealHeight="{flick.height/scale}"
                onUrlChanged="flick.xPosition=0; flick.yPosition=0; zoomOut()"
                scale="{(width > 0) ? flick.width/width*zoomedout+(1-zoomedout) : 1}"
                >
                <properties>
                    <Property name="zoomedout" type="real" value="1"/>
                </properties>
            </WebView>
            <Rect id="webviewTint" anchors.fill="{webview}" color="black" opacity="0">
                <MouseRegion anchors.fill="{webviewTint}" onClicked="proxy.focus=false"/>
            </Rect>
        </Flickable>
        <Image id="footer" width="{parent.width}" anchors.bottom="{parent.bottom}"
                height="43" src="content/pics/footer.png">
            <Rect y="-1" width="{parent.width}" height="1" color="#555555"/>
            <Item id="backbutton" anchors.right="{reload.left}" anchors.rightMargin="10" anchors.verticalCenter="{parent.verticalCenter}" width="{back_e.width}" height="{back_e.height}">
                <Image anchors.fill="{parent}" id="back_e" src="content/pics/back.png" />
                <Image anchors.fill="{parent}" id="back_d" src="content/pics/back-disabled.png" />
                <states>
                    <State name="Enabled" when="{webview.back.enabled==true}">
                        <SetProperty target="{back_e}" property="opacity" value="1"/>
                        <SetProperty target="{back_d}" property="opacity" value="0"/>
                    </State>
                    <State name="Disabled" when="{webview.back.enabled==false}">
                        <SetProperty target="{back_e}" property="opacity" value="0"/>
                        <SetProperty target="{back_d}" property="opacity" value="1"/>
                    </State>
                </states>
                <transitions>
                    <Transition>
                        <NumericAnimation properties="opacity" easing="easeInOutQuad" duration="300"/>
                    </Transition>
                </transitions>
                <MouseRegion anchors.fill="{back_e}" onClicked="if (webview.back.enabled) webview.back.trigger()"/>
            </Item>
            <Image id="reload" src="content/pics/reload.png" anchors.horizontalCenter="{parent.horizontalCenter}" anchors.verticalCenter="{parent.verticalCenter}"/>
            <MouseRegion anchors.fill="{reload}" onClicked="webview.reload.trigger()"/>
            <Item id="forwardbutton" anchors.left="{reload.right}" anchors.leftMargin="10" anchors.verticalCenter="{parent.verticalCenter}" width="{forward_e.width}" height="{forward_e.height}">
                <Image anchors.fill="{parent}" anchors.verticalCenter="{parent.verticalCenter}" id="forward_e" src="content/pics/forward.png" />
                <Image anchors.fill="{parent}" id="forward_d" src="content/pics/forward-disabled.png" />
                <states>
                    <State name="Enabled" when="{webview.forward.enabled==true}">
                        <SetProperty target="{forward_e}" property="opacity" value="1"/>
                        <SetProperty target="{forward_d}" property="opacity" value="0"/>
                    </State>
                    <State name="Disabled" when="{webview.forward.enabled==false}">
                        <SetProperty target="{forward_e}" property="opacity" value="0"/>
                        <SetProperty target="{forward_d}" property="opacity" value="1"/>
                    </State>
                </states>
                <transitions>
                    <Transition>
                        <NumericAnimation properties="opacity" easing="easeInOutQuad" duration="320"/>
                    </Transition>
                </transitions>
                <MouseRegion anchors.fill="{parent}" onClicked="if (webview.forward.enabled) webview.forward.trigger()"/>
            </Item>
        </Image>
    </Item>
    <states>
        <State name="Normal">
            <SetProperty target="{webview}" property="zoomedout" value="0"/>
            <SetProperty target="{flick}" property="xPosition" value="{Math.min(webview.width-flick.width,Math.max(0,flick.centerX-flick.width/2))}"/>
            <SetProperty target="{flick}" property="yPosition" value="{Math.min(webview.height-flick.height,Math.max(0,flick.centerY-flick.height/2))}"/>
        </State>
        <State name="ZoomedOut">
            <SetProperty target="{webview}" property="zoomedout" value="1"/>
        </State>
    </states>
    <transitions>
        <Transition>
            <SequentialAnimation>
                <SetPropertyAction target="{webview}" property="smooth" value="false" />
                <ParallelAnimation>
                    <NumericAnimation target="{webview}" properties="zoomedout" easing="easeInOutQuad" duration="200"/>
                    <NumericAnimation target="{flick}" properties="xPosition,yPosition" easing="easeInOutQuad" duration="200"/>
                </ParallelAnimation>
                <SetPropertyAction target="{webview}" property="smooth" value="true" />
            </SequentialAnimation>
        </Transition>
    </transitions>
</Item>
