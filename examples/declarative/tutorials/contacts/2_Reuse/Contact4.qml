<Item id="contactDetails">
    <properties>
        <Property name="contactid" value=""/>
        <Property name="label" onValueChanged="labelField.value = label"/>
        <Property name="phone" onValueChanged="phoneField.value = phone"/>
        <Property name="email" onValueChanged="emailField.value = email"/>
    </properties>
    <VerticalLayout id="layout"
        anchors.fill="{parent}"
        spacing="5"
        margin="5">
        <ContactField4 id="labelField"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"
            label="Name"/>
        <ContactField4 id="phoneField"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"
            icon="../shared/pics/phone.png"
            label="Phone"/>
        <ContactField4 id="emailField"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"
            icon="../shared/pics/email.png"
            label="Email"/>
    </VerticalLayout>
</Item>
