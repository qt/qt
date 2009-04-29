<PathView id="Container">

    <path>
        <Path startX="-50" startY="40">
            <PathAttribute name="scale" value="0.4"/>
            <PathCubic x="400" y="220" control1X="140" control1Y="40" control2X="210" control2Y="220"/>
            <PathAttribute name="scale" value="0.7"/>
            <PathCubic x="850" y="40" control2X="660" control2Y="40" control1X="590" control1Y="220"/>
            <PathAttribute name="scale" value="0.4"/>
        </Path>
    </path>

    <delegate>
        <Component id="MovieDelegate">
            <Item id="Wrapper" width="220" height="310" scale="{Wrapper.PathView.scale}">
                <Connection sender="{MovieInfoContainer}" script="if (Wrapper.state == 'Details') Wrapper.state = ''" signal="closed()"/>

                <Script>
                    function movieClicked() {
                        if (MainWindow.minimized == true) {
                            MainWindow.minimized = false;
                        } else {
                            MovieInfoContainer.movieTitle = title;
                            MovieInfoContainer.flickableArea.yPosition = 0;
                            MovieInfoContainer.movieDescription = description;
                            MovieInfoContainer.rating = rating;
                            Wrapper.state = "Details";
                        }
                    }
                </Script>

                <Rect id="Dvd" anchors.fill="{parent}" color="white">
                    <Image source="{thumb}" width="215" height="305" anchors.centeredIn="{parent}" opaque="true"/>
                    <Image source="pics/reflection.png" anchors.centeredIn="{parent}"/>
                    <Image source="pics/shadow-right.png" x="220"/>
                    <Image source="pics/shadow-bottom.png" y="310"/>
                    <Image source="pics/shadow-corner.png" x="220" y="310"/>
                </Rect>

                <MouseRegion anchors.fill="{Wrapper}" onClicked="movieClicked()"/>

                <states>
                    <State name="Details">
                        <ParentChange target="{Wrapper}" parent="{MovieInfoContainer.frontContainer}"/>
                        <SetProperties target="{Wrapper}" x="50" y="60" scale="1"/>
                        <SetProperties target="{MovieInfoContainer}" y="20"/>
                        <SetProperties target="{Container}" y="-480"/>
                        <SetProperties target="{CloseButton}" opacity="0"/>
                        <SetProperties target="{CategoryText}" y="-50"/>
                    </State>
                    <State name="Stacked" when="{MainWindow.minimized == true}">
                        <ParentChange target="{Wrapper}" parent="{Stack}"/>
                        <SetProperties target="{Wrapper}" x="0" y="0" scale="0.2"/>
                        <SetProperties target="{CloseButton}" opacity="0"/>
                        <SetProperties target="{CategoryText}" y="-50"/>
                    </State>
                </states>

                <transitions>
                    <Transition fromState="" toState="Details,Stacked">
                        <ParentChangeAction/>
                        <NumericAnimation properties="x,y,scale,opacity" duration="500" easing="easeInOutQuad"/>
                    </Transition>
                    <Transition fromState="Details,Stacked" toState="">
                        <ParentChangeAction/>
                        <NumericAnimation properties="x,y,scale,opacity" duration="500" easing="easeInOutQuad"/>
                    </Transition>
                </transitions>

            </Item>
        </Component>
    </delegate>

</PathView>
