#include <QtEntity/MetaObjectRegistry>

#include <QMutex>
#include <QMutexLocker>
#include <unordered_map>
#include <QString>
#include <QVector>

namespace QtEntity
{
    QMutex s_metaObjectRegistryMutex;
    std::unordered_map<std::string,const QMetaObject*> s_metaObjects;
	

    void registerMetaObject(const QMetaObject& mo)
    {
        QMutexLocker m(&s_metaObjectRegistryMutex);
        s_metaObjects[mo.className()] = &mo;
    }


    const QMetaObject* metaObjectByClassName(const QString& classname)
    {
        QMutexLocker m(&s_metaObjectRegistryMutex);
        auto i = s_metaObjects.find(classname.toStdString());
        return (i == s_metaObjects.end()) ? nullptr : i->second;
    }


    std::unordered_map<std::string, ClassTypeId> s_classTypeIds;
    std::unordered_map<ClassTypeId, std::string> s_classNames;
    
    ClassTypeId ComponentRegistry::registerComponent(const QString& classname)
    {
        Q_ASSERT(s_classTypeIds.find(classname.toStdString()) == s_classTypeIds.end());
        ClassTypeId s = (ClassTypeId) s_classTypeIds.size();        
        s_classTypeIds[classname.toStdString()] = s;
        s_classNames[s] = classname.toStdString();
        return s;
    }

    ClassTypeId ComponentRegistry::classTypeId(const QString& classname)
    {
        auto i = s_classTypeIds.find(classname.toStdString());
        if(i == s_classTypeIds.end()) return -1;
        return i->second;
    }


    QString ComponentRegistry::className(ClassTypeId typeId)
    {
        auto i = s_classNames.find(typeId);
        if(i == s_classNames.end()) return "<Class not registered>";
        return QString::fromStdString(i->second);
    }

}
