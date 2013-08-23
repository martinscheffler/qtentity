#include "DamageSystem"
#include "ShapeSystem"

#include <QtEntity/EntityManager>

IMPLEMENT_COMPONENT_TYPE(Damage)

DamageSystem::DamageSystem()
{
    QTE_ADD_PROPERTY("energy", int, Damage, energy, setEnergy);
}


void DamageSystem::step(int frameNumber, int totalTime, int delta)
{
    for(auto i = begin(); i != end(); ++i)
    {
        Shape* shape;
        _entityManager->component(i->first, shape);
    }
}


const QVariantMap DamageSystem::attributesForProperty(const QString& name) const
{
    QVariantMap r;
    if(name == "energy")
    {
        r["minimum"] = 0;
        r["maximum"] = 1000;
    }
    return r;
}
