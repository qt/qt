<!-- This example illustrates expanding a list item to show a more detailed view -->
<?qtfx namespacepath:=content ?>
<Rect id="page" width="400" height="280" color="black">
    <resources>
        <!--
        Delegate for the recipes.  This delegate has two modes:
        1. the list mode (default), which just shows the picture and title of the recipe.
        2. the details mode, which also shows the ingredients and method.
        -->
        <Component id="recipeDelegate">
            <Item id="wrapper" x="10" width="{List.width-20}">
                <!--
                Create a property to contain the visibility of the details.
                We can bind multiple element's opacity to this one property,
                rather than having a "SetProperty" line for each element we
                want to fade.
                -->
                <properties><Property name="detailsOpacity" value="0" type="real"/></properties>
                <!-- A simple rounded rectangle for the background -->
                <Rect id="background" x="1" y="2" width="{parent.width-2}" height="{parent.height-4}" color="#FEFFEE" pen.color="#FFBE4F" radius="5"/>
                <!--
                This mouse region covers the entire delegate.
                When clicked it changes mode to 'Details'.  If we are already
                in Details mode, then no change will happen.
                -->
                <MouseRegion id="pageMouse" anchors.fill="{parent}" onClicked="wrapper.state = 'Details'"/> 
                <!--
                Layout the page.  Picture, title and ingredients at the top, method at the
                bottom.  Note that elements that should not be visible in the list
                mode have their opacity set to wrapper.detailsOpacity.
                -->
                <VerticalLayout id="topLayout" width="{parent.width-20}" margin="10" spacing="10">
                    <HorizontalLayout spacing="10" height="{recipePic.height}">
                        <Image id="recipePic" src="{picture}" width="48" height="48"/>
                        <VerticalLayout height="{recipePic.height}" spacing="5" width="{background.width-recipePic.width-20}">
                            <Text id="name" text="{title}" font.bold="true" font.size="16"/>
                            <Text opacity="{wrapper.detailsOpacity}" text="Ingredients" font.size="12" font.bold="true"/>
                            <Text opacity="{wrapper.detailsOpacity}" text="{ingredients}" wrap="true" width="{parent.width}"/>
                        </VerticalLayout>
                    </HorizontalLayout>
                    <VerticalLayout id="details" opacity="{wrapper.detailsOpacity}">
                        <Text text="Method" font.size="12" font.bold="true"/>
                        <Text text="{method}" wrap="true" width="{topLayout.width}"/>
                    </VerticalLayout>
                </VerticalLayout>
                <!-- A button to close the detailed view, i.e. set the state back to default (''). -->
                <MediaButton anchors.right="{background.right}" anchors.rightMargin="5" y="10" opacity="{wrapper.detailsOpacity}" text="Close" onClicked="wrapper.state = ''"/>
                <!-- Make the default height equal the hight of the picture, plus margin. -->
                <height>68</height>

                <states>
                    <State name="Details">
                        <SetProperty target="{background}" property="color" value="white" />
                        <!-- Make the picture bigger -->
                        <SetProperties target="{recipePic}" width="128" height="128" />
                        <!-- Make details visible -->
                        <SetProperties target="{wrapper}" detailsOpacity="1" x="0"/>
                        <!-- Make the detailed view fill the entire list area -->
                        <SetProperty target="{wrapper}" property="height" value="{List.height}"/>
                        <SetProperty target="{wrapper}" property="width" value="{List.width}"/>
                        <!-- Move the list so that this item is at the top. -->
                        <SetProperty target="{wrapper.ListView.view}" property="yPosition" value="{wrapper.y}"/>
                        <!-- Disallow flicking while we're in detailed view -->
                        <SetProperty target="{wrapper.ListView.view}" property="locked" value="1"/>
                    </State>
                </states>

                <transitions>
                    <Transition>
                        <!-- Make the state changes smooth -->
                        <ParallelAnimation>
                            <ColorAnimation duration="500" />
                            <NumericAnimation duration="300" properties="detailsOpacity,x,yPosition,height,width"/>
                        </ParallelAnimation>
                    </Transition>
                </transitions>
            </Item>
        </Component>
    </resources>

    <!-- The actual list -->
    <ListView id="List" model="{Recipies}" anchors.fill="{parent}" clip="true" delegate="{recipeDelegate}"/>
</Rect>
