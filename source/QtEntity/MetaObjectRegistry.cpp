#include <QtEntity/MetaObjectRegistry>

#include <QMutex>
#include <QMutexLocker>
#include <QHash>
#include <QString>
#include <QVector>

namespace QtEntity
{
    QMutex s_metaObjectRegistryMutex;
    QHash<QString,const QMetaObject*> s_metaObjects;
	

    void registerMetaObject(const QMetaObject& mo)
    {
        QMutexLocker m(&s_metaObjectRegistryMutex);
        s_metaObjects[QString(mo.className())] = &mo;
    }


    const QMetaObject* metaObjectByClassName(const QString& classname)
    {
        QMutexLocker m(&s_metaObjectRegistryMutex);
        auto i = s_metaObjects.find(classname);
        return (i == s_metaObjects.end()) ? nullptr : i.value();
    }


    QVector<ComponentConstructor> s_componentConstructors;
    QHash<QString, ClassTypeId> s_classTypeIds;
    QHash<ClassTypeId, QString> s_classNames;
    
    ClassTypeId ComponentRegistry::registerComponent(const QString& classname, const ComponentConstructor& cstr)
    {
        ClassTypeId s = (ClassTypeId) s_componentConstructors.size();
        s_componentConstructors.push_back(cstr);
        s_classTypeIds[classname] = s;
        s_classNames[s] = classname;
        return s;
    }


    Component* ComponentRegistry::createComponent(ClassTypeId typeId)
    {
        if(s_componentConstructors.size() < typeId) return nullptr;
        return s_componentConstructors[typeId]();
    }


    ClassTypeId ComponentRegistry::classTypeId(const QString& classname)
    {
        auto i = s_classTypeIds.find(classname);
        if(i == s_classTypeIds.end()) return -1;
        return i.value();
    }


    QString ComponentRegistry::className(ClassTypeId typeId)
    {
        auto i = s_classNames.find(typeId);
        if(i == s_classNames.end()) return "<Class not registered>";
        return i.value();
    }

}
