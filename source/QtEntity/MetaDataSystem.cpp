#include <QtEntity/MetaDataSystem>

namespace QtEntity
{
    MetaData::MetaData(EntityId id, const QString& name, const QString& info, MetaDataSystem* ls)
        : _entityId(id)
        , _name(name)
        , _info(info)
        , _system(ls)
    {
    }


    void MetaData::setName(const QString& name)
    {
        if(_name != name)
        {
            _name = name;
            QMetaObject::invokeMethod(_system, "entityChanged", Q_ARG(QtEntity::EntityId, _entityId),
                Q_ARG(QString, _name), Q_ARG(QString, _info));
        }
    }


    void MetaData::setAdditionalInfo(const QString& info)
    {
        if(_info != info)
        {
            _info = info;
            QMetaObject::invokeMethod(_system, "entityChanged", Q_ARG(QtEntity::EntityId, _entityId),
                Q_ARG(QString, _name), Q_ARG(QString, _info));
        }
    }


    MetaDataSystem::MetaDataSystem()
        : EntitySystem(MetaData::staticMetaObject)
    {

    }


    QObject* MetaDataSystem::createComponent(EntityId id, const QVariantMap& propertyVals)
    {
        QObject* obj = EntitySystem::createComponent(id, propertyVals);
        if(obj != NULL)
        {
            MetaData* entry;
            if(this->getComponent(id, entry))
            {
                emit entityAdded(id, entry->name(), entry->additionalInfo());
            }
        }
        return obj;
    }


    QObject* MetaDataSystem::createObjectInstance(EntityId id, const QVariantMap& propertyVals)
    {
        QString name = propertyVals["name"].toString();
        QString info = propertyVals["additionalInfo"].toString();
        return new MetaData(id, name, info, this);
    }


    bool MetaDataSystem::destructComponent(EntityId id)
    {
        MetaData* entry;
        if(this->getComponent(id, entry))
        {
            emit entityRemoved(id);
        }

        return EntitySystem::destructComponent(id);
    }
}
