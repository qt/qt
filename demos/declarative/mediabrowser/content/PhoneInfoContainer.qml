<Flipable id="Container">
    <properties>
        <Property name="frontContainer" value="{ContainerFront}"/>
        <Property name="flickableArea" value="{Flickable}"/>
        <Property name="phoneTitle" value="N/A"/>
        <Property name="phoneDescription" value="..."/>
        <Property name="phoneSpecifications" value=""/>
        <Property name="phoneUrl" value=""/>
        <Property name="rating" value="2"/>
    </properties>

    <signals>
        <Signal name="closed"/>
    </signals>

    <transform>
        <AxisRotation id="Axis" axis.startX="{Container.width / 2}" axis.endX="{Container.width / 2}" axis.endY="1" />
    </transform>

    <front>
        <Item id="ContainerFront" anchors.fill="{Container}">
            <Rect anchors.fill="{parent}" color="black" opacity="0.4" pen.color="white" pen.width="2"/>

            <MediaButton id="BackButton" x="630" y="400" text="Back" onClicked="Container.closed.emit()"/>
            <MediaButton id="MoreButton" x="530" y="400" text="More..." onClicked="Container.state='Back'"/>

            <Text id="TitleText" style="Raised" styleColor="black" color="white" x="420" y="30" width="{parent.width}"
                  text="{Container.phoneTitle}" font.size="22"/>

            <LikeOMeter x="420" y="75" rating="{Container.rating}"/>

            <Flickable id="Flickable" x="420" width="280" height="260" y="120" clip="true" viewportWidth="280"
                       viewportHeight="{DescriptionText.height}">
                <Text id="DescriptionText"  wrap="true" color="white" width="{parent.width}"
                      text="{Container.phoneDescription}" font.size="12"/>
            </Flickable>
            <Text color="white" width="300" x="50" y="300" text="{Container.phoneSpecifications}"/>
            <ScrollBar id="ScrollBar" x="720" y="{Flickable.y}" width="7" height="{Flickable.height}" opacity="0"
                       flickableArea="{Flickable}" clip="true"/>
        </Item>
    </front>

    <back>
        <Item anchors.fill="{Container}">
            <Rect anchors.fill="{parent}" color="black" opacity="0.4" pen.color="white" pen.width="2"/>
            <Flickable x="10" width="{Container.width-20}" height="{Container.height-20}" y="10" clip="true"
                    viewportWidth="{UrlView.width}" viewportHeight="{UrlView.height}">
                <WebView id="UrlView" url="{Container.phoneUrl}" idealWidth="{parent.width}"/>
            </Flickable>
            <MediaButton id="BackButton2" x="630" y="400" text="Back" onClicked="Container.state=''"/>
        </Item>
    </back>

    <states>
        <State name="Back">
            <SetProperty target="{Axis}" property="angle" value="180"/>
        </State>
    </states>

    <transitions>
        <Transition>
            <NumericAnimation easing="easeInOutQuad" properties="angle" duration="500"/>
        </Transition>
    </transitions>

</Flipable>
