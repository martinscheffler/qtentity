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

#include <QtEntityUtils/EntityEditor>
#include <QtEntityUtils/VariantFactory>
#include <QtEntityUtils/VariantManager>
#include <QtEntityUtils/PropertyObjectsEdit>
#include <QtPropertyBrowser/QtTreePropertyBrowser>
#include <QtEntity/EntityManager>
#include <QtEntity/EntitySystem>
#include <QDate>
#include <QLocale>
#include <QHBoxLayout>
#include <QMetaProperty>
#include <QUuid>
#include <QDebug>

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
        
        QVariantMap components = data.toMap();
        for(auto i = components.begin(); i != components.end(); ++i)
        {
            QString componenttype = i.key();
            QVariant component = i.value();

            QtProperty* item = _propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), componenttype);
            _editor->addProperty(item);
            
            if(!component.canConvert<QVariantMap>()) continue;
            QVariantMap props = component.toMap();
            for(auto j = props.begin(); j != props.end(); ++j)
            {
                QString propname = j.key();
                QVariantMap propvals = j.value().toMap();

                QVariant propval = propvals["__value__"];
                int typeval = propvals["__type__"].toInt();
                QVariantMap attribsval = propvals["__attributes__"].toMap();

               
                // qvariant has no float type :(

                if(typeval == (int)QMetaType::Float)
                {
                    typeval = QVariant::Double;
                }

                QtVariantProperty* propitem = _propertyManager->addProperty(typeval, propname);
				if(propitem)
				{
					propitem->setValue(propval);

					// fetch property attributes
                    for(auto k = attribsval.begin(); k != attribsval.end(); ++k)
                    {
                        propitem->setAttribute(k.key(), k.value());                        
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


   /* QVariantMap replacePropertyObjectsWithVariantLists(const QVariantMap& in)
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
    }*/


    QVariant EntityEditor::fetchEntityData(const QtEntity::EntityManager& em, QtEntity::EntityId eid)
    {
        QVariantMap components;
        for(auto i = em.begin(); i != em.end(); ++i)
        {
            QtEntity::EntitySystem* es = i->second;
            if(!es->component(eid)) continue;
                
            QVariantMap component;
            for(int i = 0; i < es->propertyCount(); ++i)
            {
                auto prop = es->property(i);
                QVariantMap param;
                param["__value__"] = prop.read(eid);
                param["__type__"] = prop.variantType();
                param["__attributes__"] = prop.attributes();
                component[prop.name()] = param;
            }
            components[es->componentName()] = component;
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
        prop.write(eid, value);        
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
