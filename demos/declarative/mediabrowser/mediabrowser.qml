<?qtfx namespacepath:=content ?>
<Item width="800" height="480" id="MainWindow">
    <properties>
        <Property name="minimized" value="false"/>
    </properties>

    <Item id="Background">
        <Image source="content/pics/background.png" opaque="true"/>

        <Rect id="Menu" x="-150" width="150" height="480" color="#232323">
            <Text id="IconText" x="30" y="122" font.bold="true" font.size="9" text="Coming Soon" color="white" style="Raised" styleColor="black"/>
            <Image source="content/pics/shadow-right-screen.png" x="150" height="480" tile="true"/>
        </Rect>

        <MovieInfoContainer id="MovieInfoContainer" width="750" x="25" y="500" height="440"/>
        <MoviesPathView id="MoviesPathView" model="{MoviesModel}" y="60" width="800" height="360"/>
        <MediaButton id="CloseButton" x="680" y="440" text="Close" onClicked="MainWindow.minimized = true"/>

        <states>
            <State name="Minimized" when="{MainWindow.minimized == true}">
                <SetProperties target="{Background}" x="75"/>
                <SetProperties target="{Menu}" x="-75"/>
            </State>
        </states>
        <transitions>
            <Transition fromState="*" toState="*">
                <NumericAnimation properties="x,y,size" duration="500" easing="easeInOutQuad"/>
            </Transition>
        </transitions>
    </Item>

    <Text id="CategoryText" x="300" y="15" text="Coming Soon" font.size="22" color="white" style="Raised" styleColor="black"/>
    <Item id="Stack" x="50" y="50"/>

</Item>

