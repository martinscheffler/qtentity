#include "DamageSystem"
#include "ShapeSystem"

#include <QtEntity/EntityManager>

DamageSystem::DamageSystem()
    :QtEntity::EntitySystem(Damage::staticMetaObject)
{

}


void DamageSystem::step(int frameNumber, int totalTime, int delta)
{
    for(auto i = begin(); i != end(); ++i)
    {
        Shape* shape;
        _entityManager->getComponent(i.key(), shape);
    }
}


QVariantMap DamageSystem::attributesForProperty(const QString& name) const
{
    QVariantMap r;
    if(name == "energy")
    {
        r["minimum"] = 0;
        r["maximum"] = 1000;
    }
    return r;
}
