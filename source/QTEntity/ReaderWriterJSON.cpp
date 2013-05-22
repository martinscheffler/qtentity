#include <QTEntity/ReaderWriterJSON>
#include <QMetaProperty>
#include <QColor>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QDebug>

namespace qte
{
    QJsonObject ReaderWriterJSON::componentToJSON(const QObject& component)
    {
        QJsonObject ret;
        const QMetaObject* meta = component.metaObject();
        int count = meta->propertyCount();
        for(int i = 0; i < count; ++i)
        {
            QMetaProperty prop = meta->property(i);
            int utype = prop.userType();
            QString name = prop.name();

            if(name != "objectName")
            {
                QVariant val = prop.read(&component);
                ret.insert(name, variantToJSON(val));
            }
        }
        return ret;
    }

    QJsonValue ReaderWriterJSON::variantToJSON(const QVariant& variant)
    {
        int t = variant.type();
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

        return QJsonValue::fromVariant(variant);

    }
}
