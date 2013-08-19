#include "MetaDataSystem"

MetaData::MetaData(QtEntity::EntityId id, const QString& name, const QString& info, MetaDataSystem* ls)
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
        _system->entityChanged(_entityId, _name, _info);
    }
}


void MetaData::setAdditionalInfo(const QString& info)
{
    if(_info != info)
    {
        _info = info;
        _system->entityChanged(_entityId, _name, _info);
    }
}


MetaDataSystem::MetaDataSystem()
    : QtEntity::SimpleEntitySystem(MetaData::staticMetaObject)
{

}


QObject* MetaDataSystem::createComponent(QtEntity::EntityId id, const QVariantMap& propertyVals)
{
    QObject* obj = SimpleEntitySystem::createComponent(id, propertyVals);
    if(obj != NULL)
    {
        MetaData* entry;
        if(this->component(id, entry))
        {
            emit entityAdded(id, entry->name(), entry->additionalInfo());
        }
    }
    return obj;
}


QObject* MetaDataSystem::createObjectInstance(QtEntity::EntityId id, const QVariantMap& propertyVals)
{
    QString name = propertyVals["name"].toString();
    QString info = propertyVals["additionalInfo"].toString();
    auto obj = new MetaData(id, name, info, this);
    _components[id] = obj;
    applyParameters(id, propertyVals);
    return obj;
}


bool MetaDataSystem::destroyComponent(QtEntity::EntityId id)
{
    MetaData* entry;
    if(this->component(id, entry))
    {
        emit entityRemoved(id);
    }

    return SimpleEntitySystem::destroyComponent(id);
}

