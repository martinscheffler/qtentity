import QtQuick 2.1
import QtQuick.Particles 2.0

Item {
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
    ParticleSystem {
        id: particles
        anchors.fill: parent
        ImageParticle {
            source: "qrc:///assets/star.png"
            alpha: 0
            colorVariation: 0.6
        }

        Emitter {
            id: burstEmitter
            x: 100
            y: 100
            emitRate: 1000
            lifeSpan: 2000
            enabled: true
            velocity: AngleDirection{magnitude: 64; angleVariation: 360}
            size: 24
            sizeVariation: 8
        }
        Emitter {
            id: pulseEmitter
            x: 100
            y: 120
            emitRate: 1000
            lifeSpan: 2000
            enabled: true
            velocity: AngleDirection{magnitude: 64; angleVariation: 360}
            size: 24
            sizeVariation: 8
        }

    }
}
