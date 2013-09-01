#include <QtEntity/MetaObjectRegistry>

#include <QMutex>
#include <QMutexLocker>
#include <unordered_map>

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

}
