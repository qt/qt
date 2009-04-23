<Item id="contactDetails"
    anchors.fill="{parent}">
    <properties>
        <Property name="contactid" value=""/>
        <Property name="label" onValueChanged="labelField.value = label"/>
        <Property name="phone" onValueChanged="phoneField.value = phone"/>
        <Property name="email" onValueChanged="emailField.value = email"/>
    </properties>
    <signals>
        <Signal name="update"/>
        <Signal name="insert"/>
    </signals>
    <resources>
        <SqlQuery id="updateContactQuery" connection="{contactDatabase}">
            <query>UPDATE contacts SET label = :l, email = :e, phone = :p WHERE recid = :r</query>
            <bindings>
                <SqlBind name=":r" value="{contactid}"/>
                <SqlBind name=":l" value="{labelField.value}"/>
                <SqlBind name=":e" value="{emailField.value}"/>
                <SqlBind name=":p" value="{phoneField.value}"/>
            </bindings>
        </SqlQuery>
        <SqlQuery id="insertContactQuery" connection="{contactDatabase}">
            <query>INSERT INTO contacts (label, email, phone) VALUES(:l, :e, :p)</query>
            <bindings>
                <SqlBind name=":l" value="{labelField.value}"/>
                <SqlBind name=":e" value="{emailField.value}"/>
                <SqlBind name=":p" value="{phoneField.value}"/>
            </bindings>
        </SqlQuery>
    </resources>
    <Connection sender="{contactDetails}" signal="update()">
        updateContactQuery.exec();
    </Connection>
    <Connection sender="{contactDetails}" signal="insert()">
        insertContactQuery.exec();
    </Connection>
    <VerticalLayout id="layout"
        anchors.fill="{parent}"
        spacing="5"
        margin="5">
        <ContactField id="labelField"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"
            label="Name"/>
        <ContactField id="phoneField"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"
            icon="../shared/pics/phone.png"
            label="Phone"/>
        <ContactField id="emailField"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"
            icon="../shared/pics/email.png"
            label="Email"/>
    </VerticalLayout>
</Item>
