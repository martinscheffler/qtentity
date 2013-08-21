#include <QtEntity/EntityManager>
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
        foreach(EntitySystem* es, _systemsByComponentType)
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
        for(auto i = _systemsByComponentType.begin(); i != _systemsByComponentType.end(); ++i)
        {
            i.value()->destroyComponent(id);
        }
    }


    void EntityManager::addSystem(EntitySystem* es)
    {
        ClassTypeId t = es->componentType();
        if(_systemsByComponentType.contains(t))
        {
            throw std::runtime_error("Entity system already added!");
        }
        _systemsBySystemType.insert(es->metaObject(), es);
        _systemsByComponentType.insert(t, es);
        es->setEntityManager(this);
    }


    bool EntityManager::hasSystem(EntitySystem* es)
    {
        return _systemsBySystemType.contains(es->metaObject());
    }


    bool EntityManager::removeSystem(EntitySystem* es)
    {
        EntitySystemStore::iterator i = _systemsBySystemType.find(es->metaObject());
        if(i == _systemsBySystemType.end()) return false;
        _systemsBySystemType.erase(i);

        // assert that system is in both maps
        auto j = _systemsByComponentType.find(es->componentType());
        Q_ASSERT(j != _systemsByComponentType.end());
        _systemsByComponentType.erase(j);
        return true;
    }


    EntitySystem* EntityManager::system(const QString& classname) const
    {
        ClassTypeId classtype = ComponentRegistry::classTypeId(classname);
        return system(classtype);
    }


    EntitySystem* EntityManager::system(ClassTypeId componentType) const
    {        
        auto i = _systemsByComponentType.find(componentType);
        if(i == _systemsByComponentType.end()) return nullptr;
        return i.value();
    }


    EntitySystem* EntityManager::systemBySystemType(const QMetaObject& systemMetaObject) const
    {
        EntitySystemStore::const_iterator it = _systemsBySystemType.find(&systemMetaObject);
        return (it == _systemsBySystemType.end()) ? nullptr : it.value();
    }

}
