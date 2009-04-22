<Item height="30" width="{parent.width}">
    <properties>
        <Property name="text" value="{searchEdit.text}"/>
    </properties>
    <Rect color="white" anchors.fill="{parent}">
        <Image id="searchIcon" src="../shared/pics/search.png"
            anchors.left="{parent.left}" anchors.leftMargin="5"
            anchors.verticalCenter="{parent.verticalCenter}"/>
        <TextEdit id="searchEdit" focus="{Page.listShown == 1}" 
            anchors.left="{searchIcon.right}" anchors.right="{parent.right}"
            anchors.leftMargin="5" anchors.rightMargin="5"
            anchors.verticalCenter="{parent.verticalCenter}"
            readOnly="false" wrap="false"/>
    </Rect>
</Item>

