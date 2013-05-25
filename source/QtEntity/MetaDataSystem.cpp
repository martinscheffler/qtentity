#include <QtEntity/MetaDataSystem>

namespace qte
{
    MetaData::MetaData(EntityId id, MetaDataSystem* ls)
        : _entityId(id)
        , _system(ls)
    {

    }


    void MetaData::setName(const QString& name)
    {
        if(_name != name)
        {
            _name = name;
            QMetaObject::invokeMethod(_system, "entityChanged", Q_ARG(EntityId, _entityId),
                Q_ARG(QString, _name), Q_ARG(QString, _info));
        }
    }


    void MetaData::setAdditionalInfo(const QString& info) { _info = info; }


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


    QObject* MetaDataSystem::createObjectInstance(EntityId id)
    {
        return new MetaData(id, this);
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
