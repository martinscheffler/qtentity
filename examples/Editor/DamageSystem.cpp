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
