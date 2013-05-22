#include <QTEntity/EntitySystem>

namespace qte
{
    EntitySystem::EntitySystem(const QMetaObject& componentMeta)
        : _componentMetaObject(&componentMeta)
    {
    }

    EntitySystem::~EntitySystem()
	{
        foreach(QObject* c, _components)
        {
            delete c;
        }
	}

    const QMetaObject& EntitySystem::componentMetaObject() const
    {
        return *_componentMetaObject;
    }
}
