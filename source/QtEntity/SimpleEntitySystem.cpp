#include <QtEntity/SimpleEntitySystem>
#include <QDebug>
#include <QMetaProperty>
#include <stdexcept>

namespace QtEntity
{

    SimpleEntitySystem::SimpleEntitySystem(const QMetaObject& componentMeta)
        : _entityManager(NULL)
        , _componentMetaObject(&componentMeta)
    {
    }


    SimpleEntitySystem::~SimpleEntitySystem()
	{
        foreach(QObject* c, _components)
        {
            delete c;
        }
	}


    QObject* SimpleEntitySystem::component(EntityId id) const
    {
        auto i = _components.find(id);
        if(i == _components.end())
        {
            return nullptr;
        }
        return i.value();
    }


    bool SimpleEntitySystem::hasComponent(EntityId id) const
    {
        return (this->component(id) != nullptr);
    }


    QObject* SimpleEntitySystem::createComponent(EntityId id, const QVariantMap& propertyVals)
    {
        // check if component already exists
        if(component(id) != nullptr)
        {
            throw std::runtime_error("Component already existss!");
        }


        return createObjectInstance(id, propertyVals);
    }


    QObject* SimpleEntitySystem::createObjectInstance(QtEntity::EntityId id, const QVariantMap& propertyVals)
    {
        // use QMetaObject to construct new instance
        QObject* obj = _componentMetaObject->newInstance();
        // out of memory?
        if(obj == nullptr)
        {
            qCritical() << "Could not construct component. Have you declared a default constructor with Q_INVOKABLE?";
            throw std::runtime_error("Component could not be constructed.");
        }

        // store
        _components[id] = obj;
        applyParameters(id, propertyVals);
        return obj;
    }


    bool SimpleEntitySystem::destroyComponent(EntityId id)
    {        
        auto i = _components.find(id);
        if(i == _components.end()) return false;
        delete i.value();
        _components.erase(i);
        return true;
    }


    const QMetaObject& SimpleEntitySystem::componentMetaObject() const
    {
        return *_componentMetaObject;
    }


    void SimpleEntitySystem::applyParameters(EntityId id, const QVariantMap& vars)
    {
        if(vars.empty()) return;

        for(int i = 0; i < propertyCount(); ++i)
        {
            auto prop = property(i);
            auto var = vars.find(prop.name());
            if(var != vars.end())
            {
                bool success = prop.write(id, var.value());
                if(!success)
                {
                    qWarning() << "Could not set property. Name is: " << prop.name();
                }
            }
        }
    }

    
    size_t SimpleEntitySystem::count() const
    {
        return _components.size();
    }


    EntitySystem::Iterator SimpleEntitySystem::pbegin()
    {
        return EntitySystem::Iterator(new VIteratorImpl<ComponentStore::Iterator>(begin()));
    }


    EntitySystem::Iterator SimpleEntitySystem::pend()
    {
        return EntitySystem::Iterator(new VIteratorImpl<ComponentStore::Iterator>(end()));
    }
}
