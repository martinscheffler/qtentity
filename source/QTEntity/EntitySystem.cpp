#include <QTEntity/EntitySystem>
#include <QDebug>
#include <stdexcept>

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


    QObject* EntitySystem::getComponent(EntityId id) const
    {
        ComponentStore::const_iterator i = _components.find(id);
        if(i == _components.end())
        {
            return nullptr;
        }
        return i.value();
    }


    bool EntitySystem::hasComponent(EntityId id) const
    {
        return (this->getComponent(id) != nullptr);
    }

    QObject* EntitySystem::createComponent(EntityId id)
    {
        if(getComponent(id) != nullptr)
        {
            throw std::runtime_error("Component already existss!");
        }
        QObject* obj = _componentMetaObject->newInstance();
        if(obj == nullptr)
        {
            qCritical() << "Could not construct component. Have you declared a default constructor with Q_INVOKABLE?";
            throw std::runtime_error("Component could not be constructed.");
        }

        _components[id] = obj;
        return obj;
    }


    bool EntitySystem::destructComponent(EntityId id)
    {
        ComponentStore::iterator i = _components.find(id);
        if(i == _components.end()) return false;
        delete i.value();
        _components.erase(i);
        return true;
    }


    const QMetaObject& EntitySystem::componentMetaObject() const
    {
        return *_componentMetaObject;
    }
}
