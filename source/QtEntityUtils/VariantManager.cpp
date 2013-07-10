#include <QtEntityUtils/VariantManager>
#include <QtEntity/DataTypes>

namespace QtEntityUtils
{

    int VariantManager::filePathTypeId()
    {
        return qMetaTypeId<FilePath>();
    }


    int VariantManager::propertyObjectTypeId()
    {
        return qMetaTypeId<QtEntity::PropertyObjects>();
    }


    bool VariantManager::isPropertyTypeSupported(int propertyType) const
    {
        if (propertyType == filePathTypeId() ||
            propertyType == propertyObjectTypeId())
        {
            return true;
        }
        return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
    }


    int VariantManager::valueType(int propertyType) const
    {
        if (propertyType == filePathTypeId() ||
            propertyType == propertyObjectTypeId())
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
            return QVariant::fromValue(_propertyObjectsValues[property].value);
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

        if (propertyType == propertyObjectTypeId())
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

        if (propertyType == propertyObjectTypeId())
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
            return _filePathValues[property].value;
        if (_propertyObjectsValues.contains(property))
            return QString("%1").arg(_propertyObjectsValues[property].value.count());
        return QtVariantPropertyManager::valueText(property);
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
            if (!val.canConvert<QtEntity::PropertyObjects>())
                return;
            QtEntity::PropertyObjects obs = val.value<QtEntity::PropertyObjects>();
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
        if (propertyType(property) == propertyObjectTypeId())
            _propertyObjectsValues[property] = PropertyObjectsData();
        QtVariantPropertyManager::initializeProperty(property);
    }


    void VariantManager::uninitializeProperty(QtProperty *property)
    {
        _filePathValues.remove(property);
        QtVariantPropertyManager::uninitializeProperty(property);
    }

}
