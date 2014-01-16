import QtQuick 2.0
import QtQuick.Particles 2.0

Emitter {
    system: particles
    x: 100
    y: 100
    emitRate: 100
    lifeSpan: 2000
    enabled: true
    velocity: AngleDirection{magnitude: 64; angleVariation: 360}
    size: 24
    sizeVariation: 8
}
