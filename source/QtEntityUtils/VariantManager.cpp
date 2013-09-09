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

#include <QtEntityUtils/VariantManager>
#include <QtEntity/DataTypes>
#include <QTextStream>
#include <QMetaProperty>

namespace QtEntityUtils
{

    QtVariantProperty* VariantManager::addProperty(int propertyType, const QString &name)
    {
        if(propertyType == qMetaTypeId<float>())
        {
            propertyType = qMetaTypeId<double>();
        }
        return QtVariantPropertyManager::addProperty(propertyType, name);
    }

    
    int VariantManager::filePathTypeId()
    {
        return qMetaTypeId<FilePath>();
    }


    int VariantManager::propertyObjectsId()
    {
        return qMetaTypeId<QtEntity::PropertyObjects>();
    }


    int VariantManager::variantListId()
    {
        return QVariant::List;
    }


    bool VariantManager::isPropertyTypeSupported(int propertyType) const
    {
        if (propertyType == filePathTypeId() ||
            propertyType == variantListId())
        {
            return true;
        }
        return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
    }


    int VariantManager::valueType(int propertyType) const
    {
        if (propertyType == filePathTypeId() ||
            propertyType == variantListId())
        {
            return QVariant::String;
        }
        return QtVariantPropertyManager::valueType(propertyType);
    }


    QVariant VariantManager::value(const QtProperty *property) const
    {
        if (_filePathValues.contains(property))
        {
            return _filePathValues[property].value;
        }
        if (_propertyObjectsValues.contains(property))
        {
            return _propertyObjectsValues[property].value;
        }
        return QtVariantPropertyManager::value(property);
    }


    QStringList VariantManager::attributes(int propertyType) const
    {
        if (propertyType == filePathTypeId())
        {
            QStringList attr;
            attr << QLatin1String("filter");
            return attr;
        }

        if (propertyType == variantListId())
        {
            QStringList attr;
            attr << QLatin1String("classnames");
            return attr;
        }
        return QtVariantPropertyManager::attributes(propertyType);
    }


    int VariantManager::attributeType(int propertyType, const QString &attribute) const
    {
        if (propertyType == filePathTypeId())
        {
            if (attribute == QLatin1String("filter"))
                return QVariant::String;
            return 0;
        }

        if (propertyType == variantListId())
        {
            if (attribute == QLatin1String("classnames"))
               return QVariant::StringList;
            return 0;
        }
        return QtVariantPropertyManager::attributeType(propertyType, attribute);
    }


    QVariant VariantManager::attributeValue(const QtProperty *property, const QString &attribute) const
    {
        if (_filePathValues.contains(property))
        {
            if (attribute == QLatin1String("filter"))
                return _filePathValues[property].filter;
            return QVariant();
        }
        if (_propertyObjectsValues.contains(property))
        {
            if (attribute == QLatin1String("classnames"))
                return _propertyObjectsValues[property].classnames;
            return QVariant();
        }
        return QtVariantPropertyManager::attributeValue(property, attribute);
    }


    QString VariantManager::valueText(const QtProperty *property) const
    {
        if (_filePathValues.contains(property))
        {
            return _filePathValues[property].value;
        }
        else if (_propertyObjectsValues.contains(property))
        {
            const QVariantList&  val = _propertyObjectsValues[property].value;
            QString ret = "[";
            bool first = true;
            foreach(auto o, val)
            {
                if(!o.canConvert<QVariantMap>()) continue;
                QVariantMap obj = o.value<QVariantMap>();
                if(first)
                    first = false;
                else
                    ret.append(";");
                ret.append(obj["classname"].toString());
            }
            ret.append("]");
            return ret;
        }
        else
        {
            return QtVariantPropertyManager::valueText(property);
        }
    }


    void VariantManager::setValue(QtProperty *property, const QVariant &val)
    {
        if (_filePathValues.contains(property))
        {
            if (val.type() != QVariant::String && !val.canConvert(QVariant::String))
                return;
            QString str = val.value<QString>();
            FilePathData d = _filePathValues[property];
            if (d.value == str)
                return;
            d.value = str;
            _filePathValues[property] = d;
            emit propertyChanged(property);
            emit valueChanged(property, str);
            return;
        }
        if (_propertyObjectsValues.contains(property))
        {
            if (!val.canConvert<QVariantList>())
                return;
            QVariantList obs = val.value<QVariantList>();
            PropertyObjectsData d = _propertyObjectsValues[property];
            if (d.value == obs)
                return;
            d.value = obs;
            _propertyObjectsValues[property] = d;
            emit propertyChanged(property);
            emit valueChanged(property, QVariant::fromValue(obs));
            return;
        }
        QtVariantPropertyManager::setValue(property, val);
    }


    void VariantManager::setAttribute(QtProperty *property,
                    const QString &attribute, const QVariant &val)
    {
        if (_filePathValues.contains(property))
        {
            if (attribute == QLatin1String("filter")) {
                if (val.type() != QVariant::String && !val.canConvert(QVariant::String))
                    return;
                QString str = val.value<QString>();
                FilePathData d = _filePathValues[property];
                if (d.filter == str)
                    return;
                d.filter = str;
                _filePathValues[property] = d;
                emit attributeChanged(property, attribute, str);
            }
            return;
        }
        if (_propertyObjectsValues.contains(property))
        {
            if (attribute == QLatin1String("classnames")) {
                if (val.type() != QVariant::StringList && !val.canConvert(QVariant::StringList))
                    return;
                QStringList str = val.value<QStringList>();
                PropertyObjectsData d = _propertyObjectsValues[property];
                if (d.classnames == str)
                    return;
                d.classnames = str;
                _propertyObjectsValues[property] = d;
                emit attributeChanged(property, attribute, str);
            }
            return;
        }
        QtVariantPropertyManager::setAttribute(property, attribute, val);
    }


    void VariantManager::initializeProperty(QtProperty *property)
    {
        if (propertyType(property) == filePathTypeId())
            _filePathValues[property] = FilePathData();
        if (propertyType(property) == variantListId())
            _propertyObjectsValues[property] = PropertyObjectsData();
        QtVariantPropertyManager::initializeProperty(property);
    }


    void VariantManager::uninitializeProperty(QtProperty *property)
    {
        _filePathValues.remove(property);
        QtVariantPropertyManager::uninitializeProperty(property);
    }

}
