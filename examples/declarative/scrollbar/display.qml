<Rect width="640" height="480">

    <!-- Create a flickable to view a large image.  -->
    <Flickable id="View" anchors.fill="{parent}">
        <Image id="Picture" src="pics/niagara_falls.jpg"/>
        <viewportWidth>{Picture.width}</viewportWidth>
        <viewportHeight>{Picture.height}</viewportHeight>

        <!-- Only show the scrollbars when the view is moving.  -->
        <states>
            <State name="ShowBars" when="{View.moving}">
                <SetProperty target="{SBV}" property="opacity" value="1" />
                <SetProperty target="{SBH}" property="opacity" value="1" />
            </State>
        </states>
        <transitions>
            <Transition fromState="*" toState="*">
                <NumericAnimation properties="opacity" duration="400"/>
            </Transition>
        </transitions>

    </Flickable>

    <!-- Attach scrollbars to the right and bottom edges of the view.  -->
    <ScrollBar id="SBV" opacity="0" orientation="Vertical" position="{View.pageYPosition}" pageSize="{View.pageHeight}" width="12" height="{View.height-12}" anchors.right="{View.right}"/>
    <ScrollBar id="SBH" opacity="0" orientation="Horizontal" position="{View.pageXPosition}" pageSize="{View.pageWidth}" height="12" width="{View.width-12}" anchors.bottom="{View.bottom}"/>

</Rect>
