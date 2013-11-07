/*
Copyright (c) 2013 Martin Scheffler
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QtEntity/ReaderWriterJSON>

#include <QtEntity/EntityManager>
#include <QtEntity/EntitySystem>
#include <QtEntity/VecUtils>
#include <QColor>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaProperty>

namespace QtEntity
{
    QJsonObject ReaderWriterJSON::componentToJson(const EntitySystem& es, QtEntity::EntityId id)
    {
        QJsonObject ret;
        int count = es.propertyCount();
        for(int i = 0; i < count; ++i)
        {
            auto prop = es.property(i);
            QString name = prop.name();

            if(name != "objectName")
            {
                QVariant val = prop.read(id);
                ret.insert(name, variantToJson(val));
            }
        }
        return ret;
    }


    QJsonValue ReaderWriterJSON::variantToJson(const QVariant& variant)
    {
        int t = variant.userType();

        // check if a vector of QObjects is stored in the variant
        if(t == qMetaTypeId<QVariantList>())
        {
            // TODO reimplement
            /*
            QJsonArray arr;
            PropertyObjects vars = variant.value<PropertyObjects>();

            // for each object in vector, create a JSON object and
            // set its content by parsing the QObject's properties recursively
            foreach(QVariant var, vars)
            {
                QVariantMap varmap = var.value<QVariantMap>();
                QString classname = varmap["classname"];
               
                if(metaObjectByClassName(classname) == nullptr)
                {
                    qDebug() << "Cannot convert object to json, use QtEntity::registerMetaObject to register its type!";
                    qDebug() << "Classname is: " << classname;
                    continue;
                }

                QJsonObject qobj;

                // write type name of QObject to JSON param
                qobj.insert("classname", QString(classname));

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
            return arr;*/
        }
		else if(t == qMetaTypeId<QColor>())
		{
            QColor c = variant.value<QColor>();
            return QString("%1,%2,%3,%4").arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
        }
		else if(t == qMetaTypeId<Vec2d>())
		{
            Vec2d v2 = variant.value<Vec2d>();
            return QString("%1,%2").arg(x(v2)).arg(y(v2));
        }
        else if(t == qMetaTypeId<Vec4d>())
		{
            Vec3d v3 = variant.value<Vec3d>();
            return QString("%1,%2,%3").arg(x(v3)).arg(y(v3)).arg(z(v3));
        }
        else if(t == qMetaTypeId<Vec4d>())
		{
            Vec4d v4 = variant.value<Vec4d>();
            return QString("%1,%2,%3,%4").arg(x(v4)).arg(y(v4)).arg(z(v4)).arg(w(v4));        
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
        
        for(int i = 0; i < es->propertyCount(); ++i)
        {
            auto prop = es->property(i);
            QString propname = prop.name();
            auto j = json.find(propname);
            if(j != json.end())
            {
                params[propname] = jsonToVariant(prop.variantType(), j.value());
            }
        }

        Component* o = es->createComponent(id, params);
        return (o != nullptr);
    }

    
    QVariant ReaderWriterJSON::jsonToVariant(int t, const QJsonValue& val)
    {
        // check if a vector of QObjects is stored in the variant
        if(t == qMetaTypeId<QVariantList>() && val.isArray())
        {
           // TODO reimplement
            /*PropertyObjects ret;
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
            return QVariant::fromValue(ret);*/
            
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
		else if(t == qMetaTypeId<Vec2d>())
		{
            QString in = val.toString();
            QTextStream str(&in);
            QChar delim;
            double x,y;
            str >> x; str >> delim;
            str >> y;            
            return QVariant::fromValue(Vec2d(x,y));
        }
        else if(t == qMetaTypeId<Vec3d>())
		{
            QString in = val.toString();
            QTextStream str(&in);
            QChar delim;
            double x,y,z;
            str >> x; str >> delim;
            str >> y; str >> delim;
            str >> z;            
            return QVariant::fromValue(Vec3d(x,y,z));
        }
        else if(t == qMetaTypeId<Vec4d>())
		{
            QString in = val.toString();
            QTextStream str(&in);
            QChar delim;
            double x,y,z,w;
            str >> x; str >> delim;
            str >> y; str >> delim;
            str >> z; str >> delim;
            str >> w;            
            return QVariant::fromValue(Vec4d(x,y,z,w)); 
        }

        // unhandled variant types go to Qt provided method
        return val.toVariant();
    }
}
