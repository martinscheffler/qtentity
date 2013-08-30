#include <QtEntity/EntityManager>

#include <QtEntity/EntitySystem>
#include <QDebug>

namespace QtEntity
{
	EntityManager::EntityManager()
		: _entityCounter(1)
	{
	}


	EntityManager::~EntityManager()
	{
        for(auto i = _systems.begin(); i != _systems.end(); ++i)
        {
            delete i->second;
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
            i->second->destroyComponent(id);
        }
    }


    void EntityManager::addSystem(EntitySystem* es)
    {
        ClassTypeId ctype = es->componentType();
        _systems[ctype] = es;
        es->setEntityManager(this);
    }


    bool EntityManager::hasSystem(ClassTypeId ctype)
    {
        return _systems.find(ctype) != _systems.end();
    }


    bool EntityManager::removeSystem(EntitySystem* es)
    {
        // assert that system is in both maps
        auto j = _systems.find(es->componentType());
        assert(j != _systems.end());
        assert(es == j->second);
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
        assert(i->second->componentType() == componentType);
        return i->second;
    }


    Component* EntityManager::component(EntityId id, ClassTypeId tid) const
    {
        EntitySystem* s = this->system(tid);
        return (s == nullptr) ? nullptr : s->component(id);
    }


    Component* EntityManager::createComponent(EntityId id, ClassTypeId cid, const QVariantMap& properties)
    {
        EntitySystem* s = this->system(cid);

        if(s == nullptr) return nullptr;

        if(s->component(id) != nullptr)
        {
            qDebug() << "Component already exists! ComponentType:" << s->componentName() << " EntityId " << id;
            return nullptr;
        }

        try
        {
            return s->createComponent(id, properties);
        }
        catch(std::bad_alloc&)
        {
            qCritical() << "Could not create component, bad allocation!";
            return nullptr;
        }
    }


    bool EntityManager::destroyComponent(EntityId id, ClassTypeId cid)
    {
        EntitySystem* s = this->system(cid);
        if(s == nullptr) return false;
        return s->destroyComponent(id);
    }

}
