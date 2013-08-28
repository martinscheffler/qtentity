#include <QtEntity/EntityManager>

#include <QtEntity/MetaObjectRegistry>
#include <stdexcept>
#include <new>

namespace QtEntity
{
	EntityManager::EntityManager()
		: _entityCounter(1)
	{
	}


	EntityManager::~EntityManager()
	{
        foreach(EntitySystem* es, _systems)
        {
            delete es;
        }
	}


    EntityId EntityManager::createEntityId()
    {
        EntityId eid = _entityCounter.fetchAndAddRelaxed(1);
        return eid;
    }

    
    void EntityManager::destroyEntity(EntityId id)
    {
        for(auto i = _systems.begin(); i != _systems.end(); ++i)
        {
            i.value()->destroyComponent(id);
        }
    }


    void EntityManager::addSystem(EntitySystem* es)
    {
        ClassTypeId t = es->componentType();
        if(_systems.contains(t))
        {
            throw std::runtime_error("Entity system already added!");
        }
        _systems.insert(t, es);
        es->setEntityManager(this);
    }


    bool EntityManager::hasSystem(ClassTypeId ctype)
    {
        return _systems.contains(ctype);
    }


    bool EntityManager::removeSystem(EntitySystem* es)
    {
        // assert that system is in both maps
        auto j = _systems.find(es->componentType());
        Q_ASSERT(j != _systems.end());
        _systems.erase(j);
        return true;
    }


    EntitySystem* EntityManager::system(const QString& classname) const
    {
        ClassTypeId classtype = ComponentRegistry::classTypeId(classname);
        return system(classtype);
    }


    EntitySystem* EntityManager::system(ClassTypeId componentType) const
    {        
        auto i = _systems.find(componentType);
        if(i == _systems.end()) return nullptr;
        return i.value();
    }

}
