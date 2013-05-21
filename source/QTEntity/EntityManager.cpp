#include <QTEntity/EntityManager>

namespace qte
{
	EntityManager::EntityManager()
		: _entityCounter(1)
	{
	}

	EntityManager::~EntityManager()
	{
	}

    void EntityManager::addEntitySystem(EntitySystem* es)
    {
        const QMetaObject* mo = &es->componentMetaObject();
        Q_ASSERT(!_systems.contains(mo) && "Entity system already added to entity manager");
        _systems[mo] = es;
    }
}
