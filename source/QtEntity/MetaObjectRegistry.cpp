#include <QtEntity/MetaObjectRegistry>

#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include <QString>

namespace QtEntity
{
    QMutex s_metaObjectRegistryMutex;
    QMap<QString,const QMetaObject*> s_metaObjects;
	
    void registerMetaObject(const QMetaObject& mo)
    {
        s_metaObjects[QString(mo.className())] = &mo;
    }

    const QMetaObject* metaObjectByClassName(const QString& classname)
    {
        auto i = s_metaObjects.find(classname);
        return (i == s_metaObjects.end()) ? nullptr : i.value();
    }

}
