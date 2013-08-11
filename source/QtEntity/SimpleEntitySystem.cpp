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


    QObject* SimpleEntitySystem::getComponent(EntityId id) const
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
        return (this->getComponent(id) != nullptr);
    }


    QObject* SimpleEntitySystem::createComponent(EntityId id, const QVariantMap& propertyVals)
    {
        // check if component already exists
        if(getComponent(id) != nullptr)
        {
            throw std::runtime_error("Component already existss!");
        }

        // use QMetaObject to construct new instance
        QObject* obj = this->createObjectInstance(id, propertyVals);

        // out of memory?
        if(obj == nullptr)
        {
            qCritical() << "Could not construct component. Have you declared a default constructor with Q_INVOKABLE?";
            throw std::runtime_error("Component could not be constructed.");
        }

        // store
        _components[id] = obj;

        emit componentCreated(id, obj);
        return obj;
    }


    bool SimpleEntitySystem::destroyComponent(EntityId id)
    {        
        auto i = _components.find(id);
        if(i == _components.end()) return false;
        emit componentAboutToDestruct(id, i.value());
        delete i.value();
        _components.erase(i);
        return true;
    }


    const QMetaObject& SimpleEntitySystem::componentMetaObject() const
    {
        return *_componentMetaObject;
    }


    QObject* SimpleEntitySystem::createObjectInstance(EntityId id, const QVariantMap& propertyVals)
    {
        QObject* obj = _componentMetaObject->newInstance();
        applyParameters(obj, propertyVals);
        return obj;
    }


    void SimpleEntitySystem::applyParameters(QObject* obj, const QVariantMap& properties)
    {
        if(properties.empty()) return;

        const QMetaObject* meta = obj->metaObject();
        for(int i = 0; i < meta->propertyCount(); ++i)
        {
            QMetaProperty prop = meta->property(i);

            if(!prop.isWritable())
            {
                qWarning() << "Trying to initialize a non-writable property. Name is: " << prop.name();
            }
            else
            {
                auto var = properties.find(prop.name());
                if(var != properties.end())
                {
                    bool success = prop.write(obj, var.value());
                    if(!success)
                    {
                        qWarning() << "Could not set property. Name is: " << prop.name();
                    }
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
