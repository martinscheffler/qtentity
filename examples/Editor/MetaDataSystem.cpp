#include "MetaDataSystem"


IMPLEMENT_COMPONENT_TYPE(MetaData)


MetaData::MetaData()
    : _entityId(0)
    , _system(nullptr)
{
}


void MetaData::setName(const QString& name)
{
    if(_name != name)
    {
        _name = name;
        if(_system) _system->entityChanged(_entityId, _name, _info);
    }
}


void MetaData::setAdditionalInfo(const QString& info)
{
    if(_info != info)
    {
        _info = info;
        if(_system) _system->entityChanged(_entityId, _name, _info);
    }
}


MetaDataSystem::MetaDataSystem(QtEntity::EntityManager* em)
    : BaseClass(em)
{
    QTE_ADD_PROPERTY("name", QString, MetaData, name, setName);
    QTE_ADD_PROPERTY("additionalInfo", QString, MetaData, additionalInfo, setAdditionalInfo);
}


QtEntity::Component* MetaDataSystem::createComponent(QtEntity::EntityId id, const QVariantMap& properties)
{
    QtEntity::Component* obj = PooledEntitySystem::createComponent(id, properties);
    if(obj != NULL)
    {
        auto md = static_cast<MetaData*>(obj);
        md->_entityId = id;
        md->_system = this;        
        emit entityAdded(id, md->name(), md->additionalInfo());
        
    }
    return obj;
}


bool MetaDataSystem::destroyComponent(QtEntity::EntityId id)
{
    MetaData* entry;
    if(this->component(id, entry))
    {
        emit entityRemoved(id);
    }

    return PooledEntitySystem::destroyComponent(id);
}

