import QtQuick 2.1

Rectangle {
    id: rectangle1
    x: 0
    y: 0
    width: 106
    height: 90
    transformOrigin: Item.BottomLeft
    smooth: false
    clip : true
    z: 1
    Image {
        x: -374
        y: -360
        smooth : false
        width: sourceSize.width
        height: sourceSize.height
        fillMode: Image.Tile
        source: "spaceArt.svg"

    }
}

