#include "ParticleEmitterSystem"

#include <QtEntity/EntityManager>
#include "Renderer"
#include "ShapeSystem"

ParticleEmitter::ParticleEmitter()
{
}


ParticleEmitterSystem::ParticleEmitterSystem(QtEntity::EntityManager* em, Renderer* renderer)
    : BaseClass(em)
    , _renderer(renderer)
{
}


QVariantMap ParticleEmitterSystem::toVariantMap(QtEntity::EntityId eid, int)
{

    QVariantMap m;
    ParticleEmitter* e;
    if(component(eid, e))
    {
        //m["attackMode"] = QVariant::fromValue(attackMode(eid));
        m["emitters"] = e->_emitters;
    }
    return m;
}


void ParticleEmitterSystem::fromVariantMap(QtEntity::EntityId eid, const QVariantMap& m, int)
{
    ParticleEmitter* e;
    if(component(eid, e))
    {

        if(m.contains("emitters"))
        {
            setEmitters(eid, m["emitters"].toList());
        }
    }
}


QVariantMap ParticleEmitterSystem::editingAttributes(int) const
{    

    QVariantMap emitter;
    emitter["x"] = 0.0f;
    emitter["y"] = 0.0f;
    emitter["emitRate"] = 10.0f;
    emitter["lifeSpan"] = 500.0f;
    emitter["lifeSpanVariation"] = 500.0f;
    emitter["enabled"] = true;
    emitter["velocity.angle"] = 0;
    emitter["velocity.angleVariation"] = 360;
    emitter["velocity.magnitude"] = 64;
    emitter["velocity.magnitudeVariation"] = 0;

    emitter["size"] = 24;
    emitter["sizeVariation"] = 8;

    QVariantMap prototypes;
    prototypes["Emitter"] = emitter;

    QVariantMap emitters;
    emitters["prototypes"] = prototypes;
    QVariantMap m;
    m["emitters"] = emitters;
    return m;

}


void ParticleEmitterSystem::setEmitters(QtEntity::EntityId eid, const QVariantList &emitters)
{
    ParticleEmitter* e;
    if(component(eid, e))
    {
        e->_emitters = emitters;

        Shape* shape;
        if(!entityManager()->component(eid, shape)) return;
        _renderer->clearEmitters(shape);

        for(auto i = emitters.begin(); i != emitters.end(); ++i)
        {
            QVariantMap entry = i->toMap()["value"].toMap();
            _renderer->addEmitter(shape, entry);
        }
    }
}

