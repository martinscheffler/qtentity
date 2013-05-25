#include <QtEntity/ReaderWriterJSON>

#include <QJsonArray>
#include <QtEntity/EntityManager>
#include <QMetaProperty>
#include <QColor>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QDebug>

namespace QtEntity
{
    QJsonObject ReaderWriterJSON::componentToJson(const QObject& component)
    {
        QJsonObject ret;
        const QMetaObject* meta = component.metaObject();
        int count = meta->propertyCount();
        for(int i = 0; i < count; ++i)
        {
            QMetaProperty prop = meta->property(i);

            QString name = prop.name();

            if(name != "objectName")
            {
                QVariant val = prop.read(&component);
                ret.insert(name, variantToJson(val));
            }
        }
        return ret;
    }


    QJsonValue ReaderWriterJSON::variantToJson(const QVariant& variant)
    {
        int t = variant.userType();

        // check if a vector of QObjects is stored in the variant
        if(t == qMetaTypeId<QtEntity::PropertyObjects>())
        {
            QJsonArray arr;
            PropertyObjects objs = variant.value<PropertyObjects>();

            // for each object in vector, create a JSON object and
            // set its content by parsing the QObject's properties recursively
            foreach(PropertyObjectPointer obj, objs)
            {
                const QMetaObject* meta = obj->metaObject();

                QJsonObject qobj;

                // write class name of QObject to JSON param
                qobj.insert("classname", QString(meta->className()));

                // convert the properties of the QObject
                for(unsigned int i = 0; i < meta->propertyCount(); ++i)
                {
                    QMetaProperty prop = meta->property(i);

                    if(!prop.isStored()) continue;

                    // don't encode object name to JSON
                    if(QString(prop.name()) == "objectName") continue;

                    QVariant val = prop.read(obj.data());
                    qobj.insert(prop.name(), variantToJson(val));
                }
                arr.push_back(qobj);
            }
            return arr;
        }

        // Can handle the rest of the types static
        switch(t)
        {
        case qMetaTypeId<QColor>(): {
            QColor c = variant.value<QColor>();
            return QString("%1,%2,%3,%4").arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
        }
        case qMetaTypeId<QVector2D>(): {
            QVector2D v2 = variant.value<QVector2D>();
            return QString("%1,%2").arg(v2.x()).arg(v2.y());
        }
        case qMetaTypeId<QVector3D>(): {
            QVector3D v3 = variant.value<QVector3D>();
            return QString("%1,%2,%3").arg(v3.x()).arg(v3.y()).arg(v3.z());
        }
        case qMetaTypeId<QVector4D>(): {
            QVector4D v4 = variant.value<QVector4D>();
            return QString("%1,%2,%3,%4").arg(v4.x()).arg(v4.y()).arg(v4.z()).arg(v4.w());
        }
        }

        // unhandled variant types go to Qt provided method
        return QJsonValue::fromVariant(variant);
    }


    bool ReaderWriterJSON::jsonToComponent(EntityManager& em, EntityId id, const QJsonObject& json)
    {
        qCritical() << "Not implemented!";
        /*QJsonObject::const_iterator n = json.find("classname");
        if(n == json.end()) return false;
        QString classname = *n;
        QMetaType::Type id = QMetaType::type(classname);
        if(id == 0)
        {
            return false;
        }*/
    }
}
