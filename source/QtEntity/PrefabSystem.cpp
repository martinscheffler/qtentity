#include <QtEntity/PrefabSystem>
#include <QtEntity/EntityManager>
#include <QMetaProperty>

namespace QtEntity
{

    Prefab::Prefab(const QString& path, const QVariantMap& components, const QStringList& parameters)
        : _path(path)
        , _components(components)
        , _parameters(parameters)
    {

    }


    PrefabInstance::PrefabInstance(QSharedPointer<Prefab> prefab)
        : _prefab(prefab)
    {

    }


    PrefabSystem::PrefabSystem()
        :QtEntity::SimpleEntitySystem(PrefabInstance::staticMetaObject)
    {
    }


    PrefabSystem::~PrefabSystem()
    {
    }


    void PrefabSystem::addPrefab(const QString& path, const QVariantMap& components, const QStringList& parameters)
    {
        _prefabs[path] = QSharedPointer<Prefab>(new Prefab(path, components, parameters));
    }


    void PrefabSystem::updatePrefab(const QString& path, const QVariantMap& newcomponents, bool updateInstances)
    {
        // fetch prefab by path
        Prefab* prefab;
        {
            auto i = _prefabs.find(path);
            if(i == _prefabs.end())
            {
                return;
            }
            prefab = i.value().data();
        }

        if(updateInstances)
        {

            // update existing components in prefab and delete components no longer in prefab
            for(auto j = prefab->components().begin(); j != prefab->components().end(); ++j)
            {
                EntitySystem* es = entityManager()->system(j.key());
                Q_ASSERT(es);
                if(!es) continue;

                // component is in prefab but not in components map. Delete it from prefab instances.
                if(newcomponents.find(j.key()) == newcomponents.end())
                {
                    // find all prefab instances and destroy the component
                    for(auto k = this->begin(); k != this->end(); ++k)
                    {
                        PrefabInstance* pi = qobject_cast<PrefabInstance*>(k.value());
                        if(pi->prefab() == prefab)
                        {
                            es->destroyComponent(k.key());
                        }
                    }
                }
                else
                {
                    // component exists in component map and in prefab. Update prefab.
                    QVariantMap newvals = newcomponents[j.key()].value<QVariantMap>();

                    // find all prefab instances and update the components
                    for(auto k = this->begin(); k != this->end(); ++k)
                    {
                        PrefabInstance* pi = qobject_cast<PrefabInstance*>(k.value());
                        if(pi->prefab() == prefab)
                        {
                            QObject* component = es->component(k.key());
                            Q_ASSERT(component);
                            if(!component) continue;

                            for(int i = 0; i < es->propertyCount(); ++i)
                            {
                                auto prop = es->property(i);

                                QString n = prop.name();
                                // don't update parameters in prefab parameter list
                                if(prefab->parameters().contains(prop.name()) || prop.name() == "objectName")
                                {
                                    continue;
                                }
                                QVariant current = prop.read(k.key());
                                QVariantMap::iterator newval = newvals.find(prop.name());
                                if(newval != newvals.end() && newval.value() != current)
                                {
                                    prop.write(k.key(), newval.value());
                                }
                            }
                        }
                    }
                }
            }

            // now handle the components that did not exist in prefab yet

            for(auto i = newcomponents.begin(); i != newcomponents.end(); ++i)
            {
                QString key = i.key();
                if(prefab->components().find(key) == prefab->components().end())
                {
                    EntitySystem* es = entityManager()->system(key);
                    Q_ASSERT(es);
                    if(!es) continue;
                    for(auto k = this->begin(); k != this->end(); ++k)
                    {
                        PrefabInstance* pi = qobject_cast<PrefabInstance*>(k.value());
                        if(pi->prefab() == prefab)
                        {
                            es->createComponent(k.key(), i.value().value<QVariantMap>());
                        }
                    }
                }
            }
        }

        // finally update prefab in prefab store
        prefab->setComponents(newcomponents);

    }


    void PrefabSystem::createPrefabComponents(EntityId id, Prefab* prefab) const
    {
        const QVariantMap& c = prefab->components();
        for(auto i = c.begin(); i != c.end(); ++i)
        {
            const QString& classname = i.key();
            const QVariant& var = i.value();
            EntitySystem* es = entityManager()->system(classname);
            if(es)
            {
                es->createComponent(id, var.value<QVariantMap>());
            }
        }
    }


    QObject* PrefabSystem::createComponent(EntityId id, const QVariantMap& properties)
    {
        QString path = properties["path"].toString();
        Prefabs::const_iterator i = _prefabs.find(path);
        if(i == _prefabs.end())
        {
            return nullptr;
        }
        
        QObject* o = SimpleEntitySystem::createComponent(id, properties);
        createPrefabComponents(id, i.value().data());        
        return o;
    }

}
