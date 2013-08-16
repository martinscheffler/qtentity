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


    void EntityManager::addEntitySystem(EntitySystem* es)
    {
        const QMetaObject* mo = &es->componentMetaObject();
        if(_systemsBySystemType.contains(es->metaObject()))
        {
            throw std::runtime_error("Entity system already added!");
        }
        _systemsBySystemType.insert(es->metaObject(), es);
        _systemsByComponentType.insert(mo, es);
        es->setEntityManager(this);
    }


    bool EntityManager::hasEntitySystem(EntitySystem* es)
    {
        return _systemsBySystemType.contains(es->metaObject());
    }


    bool EntityManager::removeEntitySystem(EntitySystem* es)
    {
        EntitySystemStore::iterator i = _systemsBySystemType.find(es->metaObject());
        if(i == _systemsBySystemType.end()) return false;
        _systemsBySystemType.erase(i);

        // assert that system is in both maps
        EntitySystemStore::iterator j = _systemsByComponentType.find(&es->componentMetaObject());
        Q_ASSERT(j != _systemsByComponentType.end());
        _systemsByComponentType.erase(j);
        return true;
    }


    EntitySystem* EntityManager::system(const QString& classname) const
    {
        for(auto i = _systemsByComponentType.begin(); i != _systemsByComponentType.end(); ++i)
        {
            if(i.key()->className() == classname)
            {
                return i.value();
            }
        }
        return nullptr;
    }


    EntitySystem* EntityManager::systemByComponentType(const QMetaObject& componentMetaObject) const
    {
        EntitySystemStore::const_iterator it = _systemsByComponentType.find(&componentMetaObject);
        return (it == _systemsByComponentType.end()) ? nullptr : it.value();
    }


    EntitySystem* EntityManager::systemBySystemType(const QMetaObject& systemMetaObject) const
    {
        EntitySystemStore::const_iterator it = _systemsBySystemType.find(&systemMetaObject);
        return (it == _systemsBySystemType.end()) ? nullptr : it.value();
    }


    QObject* EntityManager::createComponentByType(EntityId id, const QMetaObject& componentMetaObject,
                                                  const QVariantMap& props)
    {
        EntitySystem* s = this->systemByComponentType(componentMetaObject);

        if(s == nullptr || s->getComponent(id) != nullptr)
        {
            return nullptr;
        }
        try
        {
            QObject* component = s->createComponent(id, props);
            return component;
        }
        catch(std::bad_alloc&)
        {
            qCritical() << "Could not create component, bad allocation!";
            return nullptr;
        }
    }
}
