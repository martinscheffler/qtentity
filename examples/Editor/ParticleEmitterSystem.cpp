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
        m["emitters"] = QVariant::fromValue(e->_emitters);
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
            setEmitters(eid, m["emitters"].value<QtEntityUtils::ItemList>());
        }
    }
}


QVariantMap ParticleEmitterSystem::editingAttributes(int) const
{    
    QVariantMap ret;
    {
        QVariantMap types;
        {
            QVariantMap emittertype;
            {
                QVariantMap emitter;
                {
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
                }
                emittertype["prototype"] = emitter;

                QVariantMap emitterattrs;
                {
                    QVariantMap sizeattrs;
                    {
                        sizeattrs["maximum"] = 10;
                    }                
                    emitterattrs["size"] = sizeattrs;
                }
                emittertype["attributes"] = emitterattrs;
            }
            types["Emitter"] = emittertype;
        }
        ret["__types"] = types;

        QVariantMap emitters;
        {            
            emitters["prototypes"] = QStringList("Emitter");
        }
        ret["emitters"] = emitters;
    }
    
    return ret;

}


void ParticleEmitterSystem::setEmitters(QtEntity::EntityId eid, const QtEntityUtils::ItemList& emitters)
{
    ParticleEmitter* e;
    if(component(eid, e))
    {
        e->_emitters = emitters;

        Shape* shape;
        if(!entityManager()->component(eid, shape)) return;
        _renderer->clearEmitters(shape);

        foreach(QtEntityUtils::Item i, emitters)
        {
            _renderer->addEmitter(shape, i._value.toMap());
        }
    }
}

