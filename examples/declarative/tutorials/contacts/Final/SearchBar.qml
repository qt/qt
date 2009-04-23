<Rect id="searchBar"
    color="white">
    <properties>
        <Property name="text" value="{searchEdit.text}"/>
    </properties>
    <Image id="searchIcon"
        anchors.left="{parent.left}" anchors.leftMargin="5"
        anchors.verticalCenter="{parent.verticalCenter}"
        src="../shared/pics/search.png"/>
    <TextEdit id="searchEdit"
        anchors.left="{searchIcon.right}" anchors.right="{parent.right}"
        anchors.leftMargin="5" anchors.rightMargin="5"
        anchors.verticalCenter="{parent.verticalCenter}"
        readOnly="false"
        wrap="false"
        focus="true"/>
</Rect>

