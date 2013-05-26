#include "PhysicalSystem"

PhysicalSystem::PhysicalSystem()
    :QtEntity::EntitySystem(Physical::staticMetaObject)
{

}


void PhysicalSystem::step(int delta)
{
    for(auto i = begin(); i != end(); ++i)
    {

    }
}
