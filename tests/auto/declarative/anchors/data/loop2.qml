import Qt 4.6

Rect {
  id: container;
  width: 600;
  height: 600;

  Image {
    id: Image1
    source: "http://labs.trolltech.com/blogs/wp-content/uploads/2009/03/3311388091_ac2a257feb.jpg"
    anchors.right: Image2.left
  }

  Image  {
    id: Image2
    source: "http://labs.trolltech.com/blogs/wp-content/uploads/2009/03/oslo_groupphoto.jpg"
    anchors.left: Image1.right
    anchors.leftMargin: 20
  }
}
