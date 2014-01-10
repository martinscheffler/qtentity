import QtQuick 2.1
import QtQuick.Particles 2.0

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

    Particles {
             y: 0
             width: parent.width
             height: 30
             source: "star.png"
             lifeSpan: 5000
             count: 50
             angle: 70
             angleDeviation: 36
             velocity: 30
             velocityDeviation: 10
             ParticleMotionWander {
                 xvariance: 30
                 pace: 100
             }
         }
         Particles {
             y: 300
             x: 120
             width: 1
             height: 1
             source: "star.png"
             lifeSpan: 5000
             count: 200
             angle: 270
             angleDeviation: 45
             velocity: 50
             velocityDeviation: 30
             ParticleMotionGravity {
                 yattractor: 1000
                 xattractor: 0
                 acceleration: 25
             }
         }


}

