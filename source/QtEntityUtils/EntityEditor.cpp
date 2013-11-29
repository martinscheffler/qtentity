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
#include <qttreepropertybrowser.h>
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


    void EntityEditor::displayEntity(QtEntity::EntityId id, const QVariantMap& data, const QVariantMap& attributes)
    {
        // changing property editors triggers signals which cause the component to be updated in the game.
        // ignore these signals while initially creating the property editors
        _ignorePropertyChanges = true;
        _entityId = id;
        clear();        
        
        for(auto i = data.begin(); i != data.end(); ++i)
        {
            QString componenttype = i.key();
            QVariant component = i.value();
            QVariantMap attrs = attributes.value(componenttype, QVariantMap()).toMap();

            QtProperty* item = _propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), componenttype);
            _editor->addProperty(item);
            
            QVariantMap props = component.toMap();
            for(auto j = props.begin(); j != props.end(); ++j)
            {
                QString name = j.key();
                QVariant val = j.value();

                QtVariantProperty* propitem = _propertyManager->addProperty(val.userType(), name);
				if(propitem)
				{
					propitem->setValue(val);

                    if(attrs.find(name) != attrs.end())
                    {
                        QVariantMap a = attrs[name].toMap();
                        // fetch property attributes
                        for(auto k = a.begin(); k != a.end(); ++k)
                        {
                            propitem->setAttribute(k.key(), k.value());                        
                        }	
                    }
					item->addSubProperty(propitem);
				}
				else
				{
					qDebug() << "Could not create property editor for property " << name;
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


    void EntityEditor::fetchEntityData(const QtEntity::EntityManager& em, QtEntity::EntityId eid, QVariantMap& components, QVariantMap& attributes)
    {
        for(auto i = em.begin(); i != em.end(); ++i)
        {
            QtEntity::EntitySystem* es = i->second;
            if(!es->component(eid)) continue;
            components[es->componentName()] = es->properties(eid);
            attributes[es->componentName()] = es->propertyAttributes();            
        }
    }


    void EntityEditor::applyEntityData(QtEntity::EntityManager& em, QtEntity::EntityId eid, const QVariantMap& values)
    {
        foreach(const QString &componenttype, values.keys())
        {
            QtEntity::EntitySystem* es = em.system(componenttype);
            if(es == nullptr)
            {
                qDebug() << "Could not apply entity data, no entity system of type " << componenttype;
                return;
            }
            es->setProperties(eid, values.value(componenttype).toMap());               
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
            QVariantMap prop;
            prop[property->propertyName()] = val;
            QVariantMap components;
            components[componentName] = prop;
            emit entityDataChanged(_entityId, components);                
        }
    }
}
