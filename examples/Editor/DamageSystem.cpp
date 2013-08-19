#include "DamageSystem"
#include "ShapeSystem"

#include <QtEntity/EntityManager>

DamageSystem::DamageSystem()
    :QtEntity::SimpleEntitySystem(Damage::staticMetaObject)
{
    QTE_ADD_PROPERTY("energy", int, Damage, energy, setEnergy);
}


void DamageSystem::step(int frameNumber, int totalTime, int delta)
{
    for(auto i = begin(); i != end(); ++i)
    {
        Shape* shape;
        _entityManager->component(i.key(), shape);
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
