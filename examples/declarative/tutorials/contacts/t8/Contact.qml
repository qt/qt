<Item id="contactDetails" anchors.fill="{parent}">
    <properties>
        <Property name="label" onValueChanged="c_label.value = label"/>
        <Property name="contactid" value=""/>
        <Property name="phone" onValueChanged="c_phone.value = phone"/>
        <Property name="email" onValueChanged="c_email.value = email"/>
        <Property name="mode" value="closed"/>
    </properties>
    <signals>
        <Signal name="open"/>
        <Signal name="close"/>
        <Signal name="confirm"/>
        <Signal name="cancel"/>
    </signals>
    <resources>
        <SqlQuery id="updateContactQuery" connection="{contactDatabase}">
            <query>UPDATE contacts SET label = :l, email = :e, phone = :p WHERE recid = :r</query>
            <bindings>
                <SqlBind name=":r" value="{contactid}"/>
                <SqlBind name=":l" value="{c_label.value}"/>
                <SqlBind name=":e" value="{c_email.value}"/>
                <SqlBind name=":p" value="{c_phone.value}"/>
            </bindings>
        </SqlQuery>
        <SqlQuery id="insertContactQuery" connection="{contactDatabase}">
            <query>INSERT INTO contacts (label, email, phone) VALUES(:l, :e, :p)</query>
            <bindings>
                <SqlBind name=":l" value="{c_label.value}"/>
                <SqlBind name=":e" value="{c_email.value}"/>
                <SqlBind name=":p" value="{c_phone.value}"/>
            </bindings>
        </SqlQuery>
    </resources>
    <Connection sender="{contactDetails}" signal="cancel()">
        c_label.value = label;
        c_phone.value = phone;
        c_email.value = email;
        contactDetails.close.emit();
    </Connection>
    <Connection sender="{contactDetails}" signal="confirm()">
        if (c_label.value != '') {
            if (contactid == '') {
                insertContactQuery.exec();
                c_label.value = label;
                c_phone.value = phone;
                c_email.value = email;
            } else {
                updateContactQuery.exec();
            }
            contactList.exec();
        }
        contactDetails.close.emit();
    </Connection>
    <VerticalLayout id="layout" anchors.fill="{parent}" spacing="5" margin="5">
        <Field id="c_label" label="Name"
            editable="{mode == 'opened' ? 1 : 0}"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"/>
        <Field id="c_phone" icon="../shared/pics/phone.png" label="Phone"
            opacity="0" editable="{mode == 'opened' ? 1 : 0}"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"/>
        <Field id="c_email" icon="../shared/pics/email.png" label="Email"
            opacity="0" editable="{mode == 'opened' ? 1 : 0}"
            anchors.left="{layout.left}" anchors.leftMargin="5"
            anchors.right="{layout.right}" anchors.rightMargin="5"/>
    </VerticalLayout>
    <MouseRegion anchors.fill="{contactDetails}" onClicked="contactDetails.open.emit()" z="{mode=='opened' ? -1 : 1}"/>
    <states>
        <State name="opened" when="{mode == 'opened'}">
            <SetProperty target="{c_phone}" property="opacity" value="1"/>
            <SetProperty target="{c_email}" property="opacity" value="1"/>
        </State>
    </states>
    <transitions>
        <Transition fromState="*" toState="opened" reversible="true">
            <NumericAnimation target="{contactFields}" properties="opacity" duration="200"/>
        </Transition>
    </transitions>
</Item>
