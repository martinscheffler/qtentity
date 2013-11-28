import QtQuick 2.1

Rectangle
{
    Image {
      z : 2
      height:3200
      smooth : false
      fillMode: Image.Tile
      source: "qrc:/assets/GalaxyUno.jpg"
      RotationAnimation on y {
               loops: Animation.Infinite
               from: -1600
               to: 0
               duration: 10000
           }
    }
}
