<Item id="contactField"
    clip="true"
    height="30">
    <RemoveButton2 id="removeButton"
        anchors.right="{parent.right}"
        anchors.top="{parent.top}" anchors.bottom="{parent.bottom}"
        expandedWidth="{contactField.width}"
        onConfirmed="print('Clear field text'); fieldText.text=''"/>
    <Text id="fieldText"
        width="{contactField.width-70}"
        anchors.right="{removeButton.left}" anchors.rightMargin="5"
        anchors.verticalCenter="{parent.verticalCenter}"
        font.bold="true"
        color="white"
        text="Phone Number"/>
    <Image src="../shared/pics/phone.png"
        anchors.right="{fieldText.left}" anchors.rightMargin="5"
        anchors.verticalCenter="{parent.verticalCenter}"/>
</Item>
