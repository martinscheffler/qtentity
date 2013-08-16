#include <QtEntity/ReaderWriterJSON>

#include <QtEntity/EntityManager>
#include <QtEntity/MetaObjectRegistry>
#include <QColor>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaProperty>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

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

                if(metaObjectByClassName(meta->className()) == nullptr)
                {
                    qDebug() << "Cannot convert object to json, use QtEntity::registerMetaObject to register its type!";
                    qDebug() << "Classname is: " << meta->className();
                    continue;
                }

                QJsonObject qobj;

                // write type name of QObject to JSON param
                qobj.insert("classname", QString(meta->className()));

                // convert the properties of the QObject
                for(int i = 0; i < meta->propertyCount(); ++i)
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
		else if(t == qMetaTypeId<QColor>())
		{
            QColor c = variant.value<QColor>();
            return QString("%1,%2,%3,%4").arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
        }
		else if(t == qMetaTypeId<QVector2D>())
		{
            QVector2D v2 = variant.value<QVector2D>();
            return QString("%1,%2").arg(v2.x()).arg(v2.y());
        }
        else if(t == qMetaTypeId<QVector3D>())
		{
            QVector3D v3 = variant.value<QVector3D>();
            return QString("%1,%2,%3").arg(v3.x()).arg(v3.y()).arg(v3.z());
        }
        else if(t == qMetaTypeId<QVector4D>())
		{
            QVector4D v4 = variant.value<QVector4D>();
            return QString("%1,%2,%3,%4").arg(v4.x()).arg(v4.y()).arg(v4.z()).arg(v4.w());        
        }

        // unhandled variant types go to Qt provided method
        return QJsonValue::fromVariant(variant);
    }


    bool ReaderWriterJSON::jsonToComponent(EntityManager& em, EntityId id, const QJsonObject& json)
    {
        QJsonObject::const_iterator n = json.find("classname");
        if(n == json.end()) return false;
        QString classname = n.value().toString();
        EntitySystem* es = em.system(classname);
        if(es == 0)
        {
            return false;
        }

        QVariantMap params;
        QMetaObject mo = es->componentMetaObject();
        for(int i = 0; i < mo.propertyCount(); ++i)
        {
            QMetaProperty prop = mo.property(i);
            const char* propname = prop.name();
            auto j = json.find(propname);
            if(j != json.end())
            {
                params[propname] = jsonToVariant(prop.userType(), j.value());
            }
        }

        QObject* o = es->createComponent(id, params);
        return (o != nullptr);
    }

    
    QVariant ReaderWriterJSON::jsonToVariant(int t, const QJsonValue& val)
    {
        // check if a vector of QObjects is stored in the variant
        if(t == qMetaTypeId<QtEntity::PropertyObjects>() && val.isArray())
        {
            PropertyObjects ret;
            QJsonArray arr = val.toArray();
            for(auto i = arr.begin(); i != arr.end(); ++i)
            {
                QJsonValue val = *i;
                if(!val.isObject()) continue;
                QJsonObject obj = val.toObject();
                if(!obj.contains("classname")) continue;
                QString classname = obj.value("classname").toString();
                
                const QMetaObject* mo = metaObjectByClassName(classname);
                if(mo == nullptr)
                {
                    qDebug() << "Could not restore object from JSon,  use QtEntity::registerMetaObject to register its type!";
                    qDebug() << "Classname is: " << classname;
                    continue;
                }
                QObject* retobj = mo->newInstance();
                ret.push_back(QSharedPointer<QObject>(retobj));

                for(int j = 0; j < mo->propertyCount(); ++j)
                {

                    QMetaProperty prop = mo->property(j);
                    const char* propname = prop.name();
                    auto k = obj.find(propname);
                    if(k != obj.end())
                    {
                        prop.write(retobj, jsonToVariant(prop.type(), k.value()));
                    }
                }   
            }
            return QVariant::fromValue(ret);
            
        }
        else if(t == qMetaTypeId<QColor>() && val.isString())
		{            
            QString in = val.toString();
            QTextStream str(&in);
            QChar delim;
            qreal r,g,b,a;
            str >> r; str >> delim;
            str >> g; str >> delim;
            str >> b; str >> delim;
            str >> a;
            return QColor(r,g,b,a);
        }
		else if(t == qMetaTypeId<QVector2D>())
		{
            QString in = val.toString();
            QTextStream str(&in);
            QChar delim;
            float x,y;
            str >> x; str >> delim;
            str >> y;            
            return QVector2D(x,y);
        }
        else if(t == qMetaTypeId<QVector3D>())
		{
            QString in = val.toString();
            QTextStream str(&in);
            QChar delim;
            float x,y,z;
            str >> x; str >> delim;
            str >> y; str >> delim;
            str >> z;            
            return QVector3D(x,y,z);
        }
        else if(t == qMetaTypeId<QVector4D>())
		{
            QString in = val.toString();
            QTextStream str(&in);
            QChar delim;
            float x,y,z,w;
            str >> x; str >> delim;
            str >> y; str >> delim;
            str >> z; str >> delim;
            str >> w;            
            return QVector4D(x,y,z,w); 
        }

        // unhandled variant types go to Qt provided method
        return val.toVariant();
    }
}
