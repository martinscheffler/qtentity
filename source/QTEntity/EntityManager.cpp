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
        QString cn = es->metaObject()->className();
            Q_ASSERT(!_systems.contains(cn) && "Entity system already added to entity manager");
        _systems[cn] = es;
    }
}
