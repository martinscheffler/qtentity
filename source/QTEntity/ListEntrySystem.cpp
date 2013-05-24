#include <QTEntity/ListEntrySystem>

namespace qte
{
    ListEntry::ListEntry(ListEntrySystem* ls)
        : _system(ls)
    {

    }


    ListEntrySystem::ListEntrySystem()
        : EntitySystem(ListEntry::staticMetaObject)
    {

    }


    QObject* ListEntrySystem::createComponent(EntityId id, const QVariantMap& propertyVals)
    {
        QObject* obj = EntitySystem::createComponent(id, propertyVals);
        if(obj != NULL)
        {
            ListEntry* entry;
            if(this->getComponent(id, entry))
            {
                emit listEntryAdded(id, entry->name());
            }
        }
        return obj;
    }


    QObject* ListEntrySystem::createObjectInstance()
    {
        return new ListEntry(this);
    }


    bool ListEntrySystem::destructComponent(EntityId id)
    {
        ListEntry* entry;
        if(this->getComponent(id, entry))
        {
            emit listEntryRemoved(id, entry->name());
        }

        return EntitySystem::destructComponent(id);
    }
}
