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

#include <QtEntity/EntityManager>
#include <QtEntity/EntitySystem>
#include <QtEntityUtils/VariantFactory>
#include <QtEntityUtils/VariantManager>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QLocale>
#include <QHBoxLayout>
#include <QMenu>
#include <QMetaProperty>
#include <QUuid>
#include <qttreepropertybrowser.h>


namespace QtEntityUtils
{
    bool findSubProperty(QtProperty *property, QtProperty *toFind)
    {
        if(toFind == property) return true;
        foreach(auto prop, property->subProperties())
        {
            if(findSubProperty(prop, toFind))
            {
                return true;
            }
        }
        return false;
    }

    QtProperty* findComponentProperty(QtTreePropertyBrowser* editor, QtProperty *property)
    {
        QList<QtBrowserItem *> items = editor->topLevelItems();
        foreach(auto item, items)
        {
            if(findSubProperty(item->property(), property))
            {
                return item->property();
            }
        }
        return nullptr;
    }

    QtProperty* findParentPropertyRec(const QtProperty* property, QtProperty* tree)
    {
        QList<QtProperty *> items = tree->subProperties();
        foreach(auto item, items)
        {
            if(item == property) return tree;
            QtProperty* parent = findParentPropertyRec(property, item);
            if(parent) return parent;
        }
        return nullptr;
    }

    QtProperty* findParentProperty(QtTreePropertyBrowser* editor, const QtProperty *property)
    {
        QList<QtBrowserItem *> items = editor->topLevelItems();
        foreach(auto item, items)
        {
            if(item->property() == property) return nullptr;
            QtProperty* parent = findParentPropertyRec(property, item->property());
            if(parent) return parent;
        }
        return nullptr;
    }

    EntityEditor::EntityEditor()
        : _entityId(0)
        , _propertyManager(new VariantManager(this))
        , _editor(new QtTreePropertyBrowser(this))
        , _ignorePropertyChanges(false)
    {

        QtVariantEditorFactory* variantFactory = new VariantFactory();

        _editor->setFactoryForManager(_propertyManager, variantFactory);
        _editor->setPropertiesWithoutValueMarked(false);
        _editor->setRootIsDecorated(false);
        _editor->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(_editor, &QWidget::customContextMenuRequested, this, &EntityEditor::showContextMenu);

        setLayout(new QHBoxLayout());
        layout()->addWidget(_editor);

        connect(_propertyManager, &VariantManager::valueChanged, this, &EntityEditor::propertyValueChanged);

        _appendListEntry = new QAction("Add Entry", this);
        _removeListEntry = new QAction("Remove Entry", this);

    }

    void EntityEditor::showContextMenu(const QPoint& pos)
    {
        QtVariantProperty* prop = dynamic_cast<QtVariantProperty*>(_editor->propertyAt(pos));
        if(!prop)
        {
            return;
        }

        if(prop->valueType() == VariantManager::listId())
        {
            QMenu menu(this);
            menu.addAction(_appendListEntry);
            menu.exec(_editor->mapToGlobal(pos));
        }
        QtVariantProperty* parent = dynamic_cast<QtVariantProperty*>(findParentProperty(_editor, prop));
        if(parent && parent->valueType() == VariantManager::listId())
        {
            QMenu menu(this);
            menu.addAction(_removeListEntry);
            menu.exec(_editor->mapToGlobal(pos));
        }
    }


    QtProperty* EntityEditor::addWidgetsRecursively(const QString& name, const QVariant& data, const QVariantMap& attributes)
    {
        if(data.type() == QVariant::Map)
        {
            QVariantMap props = data.toMap();

            QtProperty* grp = _propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), name);

            for(auto j = props.begin(); j != props.end(); ++j)
            {
                QVariantMap subattribs = attributes[j.key()].toMap();

                auto prop = this->addWidgetsRecursively(j.key(), j.value(), subattribs);
                if(prop)
                {
                    grp->addSubProperty(prop);
                }
            }
            return grp;
        }
        else if(data.type() == QVariant::List)
        {
            QVariantList items = data.toList();

            QtProperty* grp = _propertyManager->addProperty(VariantManager::listId(), name);

            int count = 0;
            for(auto j = items.begin(); j != items.end(); ++j)
            {
                QVariantMap subattribs;// = attributes[j.key()].toMap();

                auto prop = this->addWidgetsRecursively(QString("%1").arg(count++), *j, subattribs);
                if(prop)
                {
                    grp->addSubProperty(prop);
                }
            }
            return grp;
        }
        else
        {
            QtVariantProperty* propitem = _propertyManager->addProperty(data.userType(), name);
            if(propitem)
            {

                propitem->setValue(data);

                // set property attributes
                for(auto k = attributes.begin(); k != attributes.end(); ++k)
                {
                    propitem->setAttribute(k.key(), k.value());
                }
            }
            else
            {
                qDebug() << "Could not create property editor for property " << name;
            }
            return propitem;
        }
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
            QVariantMap attrs = attributes.value(i.key(), QVariantMap()).toMap();

            auto item = this->addWidgetsRecursively(i.key(), i.value(), attrs);
            if(item)
            {
                _editor->addProperty(item);
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

    void EntityEditor::propertyValueChanged(QtProperty *property, const QVariant &val)
    {
        // changing property editors triggers signals which cause the component to be updated in the game.
        // ignore these signals while initially creating the property editors
        if(_ignorePropertyChanges) return;

        QtProperty* changedComponent = findComponentProperty(_editor, property);
        Q_ASSERT(changedComponent);
        QtProperty* changedProp = nullptr;
        foreach(auto prop, changedComponent->subProperties())
        {
            if(findSubProperty(prop, property))
            {
                changedProp = prop;
            }
        }
        
        Q_ASSERT(changedProp);

        QVariantMap prop;
        prop[changedProp->propertyName()] =  _propertyManager->value(changedProp);
        QVariantMap components;
        components[changedComponent->propertyName()] = prop;
        emit entityDataChanged(_entityId, components);

    }
}
