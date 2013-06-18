#include <QtEntity/PrefabSystem>
#include <QtEntity/EntityManager>

namespace QtEntity
{

    Prefab::Prefab(const QString& path, const QVariantMap& components, const QStringList& parameters)
        : _path(path)
        , _components(components)
        , _parameters(parameters)
    {

    }


    PrefabSystem::PrefabSystem()
        :QtEntity::EntitySystem(PrefabInstance::staticMetaObject)
    {

    }


    PrefabSystem::~PrefabSystem()
    {
    }


    void PrefabSystem::createPrefabComponents(EntityId id, Prefab* prefab) const
    {
        const QVariantMap& c = prefab->components();
        for(auto i = c.begin(); i != c.end(); ++i)
        {
            const QString& classname = i.key();
            const QVariant& var = i.value();
            EntitySystem* es = entityManager()->getSystemByComponentClassName(classname);
            if(es)
            {
                QObject* component = es->createComponent(id, var.value<QVariantMap>());
            }
        }

    }


    QObject* PrefabSystem::createObjectInstance(QtEntity::EntityId id, const QVariantMap& propertyVals)
    {
        QString path = propertyVals["path"].toString();
        Prefabs::const_iterator i = _prefabs.find(path);
        if(i == _prefabs.end())
        {
            return nullptr;
        }

        createPrefabComponents(id, i.value().data());
        return new PrefabInstance(i.value());
    }

}
