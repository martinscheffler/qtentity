#include "DamageSystem"
#include "ShapeSystem"

#include <QtEntity/EntityManager>

IMPLEMENT_COMPONENT_TYPE(Damage)

DamageSystem::DamageSystem(QtEntity::EntityManager* em)
    : BaseClass(em)
{
    QTE_ADD_PROPERTY("energy", int, Damage, energy, setEnergy);
}


void DamageSystem::step(int frameNumber, int totalTime, int delta)
{
    for(auto i = begin(); i != end(); ++i)
    {
        Shape* shape;
        entityManager()->component(i->first, shape);
    }
}


