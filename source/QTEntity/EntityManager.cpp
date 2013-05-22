#include <QTEntity/EntityManager>

namespace qte
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

    EntityId EntityManager::createEntity()
    {
        EntityId eid = _entityCounter.fetchAndAddRelaxed(1);
        return eid;
    }

    void EntityManager::addEntitySystem(EntitySystem* es)
    {
        const QMetaObject* mo = &es->componentMetaObject();
        Q_ASSERT(!_systemsBySystemType.contains(es->metaObject()) && "Entity system already added to entity manager");
        _systemsByComponentType[mo] = es;
        _systemsBySystemType[es->metaObject()] = es;
    }

    bool EntityManager::removeEntitySystem(EntitySystem* es)
    {
        EntitySystemStore::iterator i = _systemsBySystemType.find(es->metaObject());
        if(i == _systemsBySystemType.end()) return false;
        _systemsBySystemType.erase(i);

        // assert that system is in both maps
        EntitySystemStore::iterator j = _systemsByComponentType.find(&i.value()->componentMetaObject());
        Q_ASSERT(j != _systemsByComponentType.end());
        _systemsByComponentType.erase(j);
        return true;
    }

    EntitySystem* EntityManager::getSystemByComponentType(const QMetaObject& componentMetaObject) const
    {
        EntitySystemStore::const_iterator it = _systemsByComponentType.find(&componentMetaObject);
        return (it == _systemsByComponentType.end()) ? nullptr : it.value();
    }

    EntitySystem* EntityManager::getSystemBySystemType(const QMetaObject& systemMetaObject) const
    {
        EntitySystemStore::const_iterator it = _systemsBySystemType.find(&systemMetaObject);
        return (it == _systemsBySystemType.end()) ? nullptr : it.value();
    }
}
