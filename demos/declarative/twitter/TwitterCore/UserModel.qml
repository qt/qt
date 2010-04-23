import Qt 4.7

//This "model" gets the user information about the searched user. Mainly for the icon.
//Copied from RssModel

Item { id: wrapper
    property variant model: xmlModel
    property string user : ""
    property int status: xmlModel.status
    function reload() { xmlModel.reload(); }
XmlListModel {
    id: xmlModel

    source: {if(user!="") {"http://twitter.com/users/show.xml?screen_name="+user;}else{"";}}
    query: "/user"

    XmlRole { name: "name"; query: "name/string()" }
    XmlRole { name: "screenName"; query: "screen_name/string()" }
    XmlRole { name: "image"; query: "profile_image_url/string()" }
    XmlRole { name: "location"; query: "location/string()" }
    XmlRole { name: "description"; query: "description/string()" }
    XmlRole { name: "followers"; query: "followers_count/string()" }
    //XmlRole { name: "protected"; query: "protected/bool()" }
    //TODO: Could also get the user's color scheme, timezone and a few other things
}
}
