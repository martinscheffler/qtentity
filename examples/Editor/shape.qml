import QtQuick 2.1

Rectangle {
    id: rectangle
    x: 0
    y: 0
    width: 106
    height: 90
    color: "#00000000"
    transformOrigin: Item.TopLeft
    smooth: false
    clip : true
    z: 1
    property int clipX : 640
    property int clipY : 640

    property string path : "";

    Image {
        id : texture
        x: -parent.clipX
        y: -parent.clipY
        smooth : false
        width: sourceSize.width
        height: sourceSize.height
        fillMode: Image.Tile
        source: parent.path

    }


}

