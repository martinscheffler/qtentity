#include <QtEntityUtils/EntityEditor>
#include <QtEntityUtils/VariantFactory>
#include <QtEntityUtils/VariantManager>
#include <QtEntityUtils/PropertyObjectsEdit>
#include <QtPropertyBrowser/QtTreePropertyBrowser>
#include <QtEntity/EntityManager>
#include <QtEntity/MetaObjectRegistry>
#include <QDate>
#include <QLocale>
#include <QHBoxLayout>
#include <QMetaProperty>
#include <QUuid>

namespace QtEntityUtils
{
    EntityEditor::EntityEditor()
        : _entityId(0)
        , _propertyManager(new VariantManager(this))
        , _editor(new QtTreePropertyBrowser())
        , _ignorePropertyChanges(false)
    {

        QtVariantEditorFactory* variantFactory = new VariantFactory();

        _editor->setFactoryForManager(_propertyManager, variantFactory);
        _editor->setPropertiesWithoutValueMarked(true);
        _editor->setRootIsDecorated(false);
        setLayout(new QHBoxLayout());
        layout()->addWidget(_editor);

        connect(_propertyManager, &VariantManager::valueChanged, this, &EntityEditor::propertyValueChanged);
    }


    void EntityEditor::displayEntity(QtEntity::EntityId id, const QVariant& data)
    {
        // changing property editors triggers signals which cause the component to be updated in the game.
        // ignore these signals while initially creating the property editors
        _ignorePropertyChanges = true;
        _entityId = id;
        clear();

        if(!data.canConvert<QVariantMap>()) return;
        
        QVariantMap components = data.value<QVariantMap>();
        for(auto i = components.begin(); i != components.end(); ++i)
        {
            QString componenttype = i.key();
            QtProperty* item = _propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), componenttype);
            _editor->addProperty(item);
            QVariant variant = i.value();
            if(!variant.canConvert<QVariantMap>()) continue;
            QVariantMap props = variant.value<QVariantMap>();
            for(auto j = props.begin(); j != props.end(); ++j)
            {
                QString propname = j.key();

                // skip property attributes
                if(propname.left(2) == "#|") continue;
                QVariant propval = j.value();

                int t = propval.userType();
                
                // qvariant has no float type :(

                if(t == (int)QMetaType::Float)
                {
                    t = QVariant::Double;
                }

                QtVariantProperty* propitem = _propertyManager->addProperty(t, propname);
				if(propitem)
				{
					propitem->setValue(propval);

					// fetch property attributes
					auto i = props.find(QString("#|%1").arg(propname));
					if(i != props.end())
					{
						QVariantMap attrs = i.value().value<QVariantMap>();
						for(auto j = attrs.begin(); j != attrs.end(); ++j)
						{
							propitem->setAttribute(j.key(), j.value());
						}
					}

					item->addSubProperty(propitem);
				}
				else
				{
					qDebug() << "Could not create property editor for property " << propname;
				}                
            }
        }
        _ignorePropertyChanges = false;
    }


    void EntityEditor::clear()
    {
        _propertyManager->clear();
        _editor->clear();
    }


    QVariantMap replacePropertyObjectsWithVariantLists(const QVariantMap& in)
    {
        QVariantMap out;
        for(auto i = in.begin(); i != in.end(); ++i)
        {
            if(i.value().userType() == VariantManager::propertyObjectsId())
            {
                QVariantList vl;

                QtEntity::PropertyObjects po = i.value().value<QtEntity::PropertyObjects>();
                foreach(auto o, po)
                {
                    QVariantMap m;
                    const QMetaObject& meta = *o->metaObject();
                    m["classname"] = meta.className();
                    for(int j = 0; j < meta.propertyCount(); ++j)
                    {
                        QMetaProperty prop = meta.property(j);
                        m[prop.name()] = prop.read(o.data());
                    }
                    vl.append(m);
                }
                out[i.key()] = vl;
            }
            else
            {
                out[i.key()] = i.value();
            }
        }
        return out;
    }


    QVariant EntityEditor::fetchEntityData(const QtEntity::EntityManager& em, QtEntity::EntityId eid)
    {
        QVariantMap components;
        for(auto i = em.begin(); i != em.end(); ++i)
        {
            QtEntity::EntitySystem* es = *i;
            if(!es->component(eid)) continue;
            QVariantMap vals = QtEntity::propertyValues(es, eid);
            vals = replacePropertyObjectsWithVariantLists(vals);
            QVariantMap attribs = QtEntity::propertyAttributes(es);
            for(auto j = attribs.begin(); j != attribs.end(); ++j)
            {
                vals[QString("#|%1").arg(j.key())] = j.value();
            }
            components[es->componentName()] = vals;
        }
        return components;
    }


    void EntityEditor::applyEntityData(QtEntity::EntityManager& em, QtEntity::EntityId eid, const QString& componenttype, const QString& propertyname, const QVariant& value)
    {
        QtEntity::EntitySystem* es = em.system(componenttype);
        if(es == nullptr)
        {
            qDebug() << "Could not apply entity data, no entity system of type " << componenttype;
            return;
        }
        QtEntity::PropertyAccessor prop;

        for(int j = 0; j < es->propertyCount(); ++j)
        {
            if(es->property(j).name() == propertyname)
            {
                prop = es->property(j);
                break;
            }
        }

        if(!prop.hasSetter())
        {
            qDebug() << "No property named " << propertyname << " on object of type " << es->componentName();
            return;
        }

        if(prop.variantType() == VariantManager::propertyObjectsId())
        {
            QVariantList vars = value.value<QVariantList>();
            QtEntity::PropertyObjects objs = prop.read(eid).value<QtEntity::PropertyObjects>();
            QtEntity::PropertyObjects objsnew = PropertyObjectEditor::updatePropertyObjectsFromVariantList(objs, vars);
            prop.write(eid, QVariant::fromValue(objsnew));
        }
        else
        {
            prop.write(eid, value);
        }
    }


    QString componentNameForProperty(QtTreePropertyBrowser* editor, QtProperty *property)
    {
        QList<QtBrowserItem *> items = editor-> topLevelItems();
        foreach(auto item, items)
        {
            
            QString tmpname= item->property()->propertyName();
            foreach(auto prop, item->property()->subProperties())
            {
				Q_ASSERT(prop != NULL);
				QString tmpname2 = prop->propertyName();
                if(prop == property)
                {
                     return item->property()->propertyName();
                }
            }
        }
        return "";
    }


    void EntityEditor::propertyValueChanged(QtProperty *property, const QVariant &val)
    {
        // changing property editors triggers signals which cause the component to be updated in the game.
        // ignore these signals while initially creating the property editors
        if(_ignorePropertyChanges) return;

        QString componentName = componentNameForProperty(_editor, property);

        // only send changes for full components
        //QtVariantProperty* var = dynamic_cast<QtVariantProperty*>(property);
        //if(var->propertyType() != QtVariantPropertyManager::groupTypeId()) return;

        
        if(!componentName.isEmpty())
        {              
            emit entityDataChanged(_entityId, componentName, property->propertyName(), val);                
        }
    }
}
