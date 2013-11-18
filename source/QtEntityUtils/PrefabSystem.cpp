/*
Copyright (c) 2013 Martin Scheffler
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QtEntityUtils/PrefabSystem>

#include <QtEntity/EntityManager>
#include <QtEntity/PropertyAccessor>
#include <QMetaProperty>

namespace QtEntityUtils
{

    
    Prefab::Prefab(const QString& path, const QVariantMap& components, const QStringList& parameters)
        : _path(path)
        , _components(components)
        , _parameters(parameters)
    {

    }


    IMPLEMENT_COMPONENT_TYPE(PrefabInstance)


    PrefabInstance::PrefabInstance()
    {

    }

    PrefabInstance::~PrefabInstance()
    {
    }

    
    PrefabSystem::PrefabSystem(QtEntity::EntityManager* em)
        : BaseClass(em)
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
                        PrefabInstance* pi = static_cast<PrefabInstance*>(k->second);
                        if(pi->prefab() == prefab)
                        {
                            es->destroyComponent(k->first);
                        }
                    }
                }
                else
                {
                    // component exists in component map and in prefab. Update prefab.
                    QVariantMap newvals = newcomponents[j.key()].toMap();

                    // find all prefab instances and update the components
                    for(auto k = this->begin(); k != this->end(); ++k)
                    {
                        PrefabInstance* pi = static_cast<PrefabInstance*>(k->second);
                        if(pi->prefab() == prefab)
                        {
                            QtEntity::Component* component = es->component(k->first);
                            Q_ASSERT(component);
                            if(!component) continue;

                            for(int i = 0; i < es->propertyCount(); ++i)
                            {
                                auto prop = es->property(i);

                                QString n = prop->name();
                                // don't update parameters in prefab parameter list
                                if(prefab->parameters().contains(n) || n == "objectName")
                                {
                                    continue;
                                }
                                QVariant current = prop->read(k->first);
                                QVariantMap::iterator newval = newvals.find(n);
                                if(newval != newvals.end() && newval.value() != current)
                                {
                                    prop->write(k->first, newval.value());
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
                        PrefabInstance* pi = static_cast<PrefabInstance*>(k->second);
                        if(pi->prefab() == prefab)
                        {
                            es->createComponent(k->first, i.value().toMap());
                        }
                    }
                }
            }
        }

        // finally update prefab in prefab store
        prefab->setComponents(newcomponents);

    }


    void PrefabSystem::createPrefabComponents(QtEntity::EntityId id, Prefab* prefab) const
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


    QtEntity::Component* PrefabSystem::createComponent(QtEntity::EntityId id, const QVariantMap& properties)
    {
        QString path = properties["path"].toString();
        Prefabs::const_iterator i = _prefabs.find(path);
        if(i == _prefabs.end())
        {
            return nullptr;
        }
        
        QtEntity::Component* o = SimpleEntitySystem::createComponent(id, properties);
        static_cast<PrefabInstance*>(o)->_prefab = *i;
        createPrefabComponents(id, i.value().data());        
        return o;
    }

}
