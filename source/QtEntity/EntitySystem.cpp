#include <QtEntity/EntitySystem>


namespace QtEntity
{
    EntitySystem::EntitySystem()
        : _entityManager(nullptr)
    {
    }


    EntitySystem::~EntitySystem()
    {
    }


    QString EntitySystem::componentName() const
    { 
        return ComponentRegistry::className(componentType()); 
    }


    int EntitySystem::propertyCount() const 
    { 
        return _properties.size(); 
    }
    

    PropertyAccessor EntitySystem::property(int idx) 
    { 
        return _properties.at(idx); 
    }


    const PropertyAccessor EntitySystem::property(int idx) const 
    { 
        return _properties.at(idx); 
    }


    void EntitySystem::addProperty(const PropertyAccessor& a) 
    { 
        _properties.push_back(a); 
    }

}
