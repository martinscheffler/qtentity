#include <QtEntity/SimpleEntitySystem>
#include <QDebug>
#include <QMetaProperty>
#include <stdexcept>

namespace QtEntity
{

    SimpleEntitySystem::SimpleEntitySystem(ClassTypeId componentType)
        : _entityManager(NULL)
        , _componentType(componentType)
    {
    }


    SimpleEntitySystem::~SimpleEntitySystem()
	{
        foreach(QObject* c, _components)
        {
            delete c;
        }
	}


    QObject* SimpleEntitySystem::component(EntityId id) const
    {
        auto i = _components.find(id);
        if(i == _components.end())
        {
            return nullptr;
        }
        return i.value();
    }


    bool SimpleEntitySystem::hasComponent(EntityId id) const
    {
        return (this->component(id) != nullptr);
    }


    QObject* SimpleEntitySystem::createComponent(EntityId id, const QVariantMap& propertyVals)
    {
        // check if component already exists
        if(component(id) != nullptr)
        {
            throw std::runtime_error("Component already existss!");
        }

        // use QMetaObject to construct new instance
        Component* obj = ComponentRegistry::createComponent(_componentType);
        // out of memory?
        if(obj == nullptr)
        {
            qCritical() << "Could not construct component. Have you registered the component type?";
            throw std::runtime_error("Component could not be constructed.");
        }

        // store
        _components[id] = obj;
        applyPropertyValues(this, id, propertyVals);
        return obj;       
    }


    bool SimpleEntitySystem::destroyComponent(EntityId id)
    {        
        auto i = _components.find(id);
        if(i == _components.end()) return false;
        delete i.value();
        _components.erase(i);
        return true;
    }


    size_t SimpleEntitySystem::count() const
    {
        return _components.size();
    }


    EntitySystem::Iterator SimpleEntitySystem::pbegin()
    {
        return EntitySystem::Iterator(new VIteratorImpl<ComponentStore::Iterator>(begin()));
    }


    EntitySystem::Iterator SimpleEntitySystem::pend()
    {
        return EntitySystem::Iterator(new VIteratorImpl<ComponentStore::Iterator>(end()));
    }
}
