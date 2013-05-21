#include <QTEntity/EntitySystem>

namespace qte
{
    EntitySystem::EntitySystem(const QMetaObject& componentMeta)
        : _componentMetaObject(&componentMeta)
    {
    }

    EntitySystem::~EntitySystem()
	{
	}

    const QMetaObject& EntitySystem::componentMetaObject() const
    {
        return *_componentMetaObject;
    }
}
