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
        , _variantManager(new VariantManager(this))
        , _editor(new QtTreePropertyBrowser(this))
        , _ignorePropertyChanges(false)
    {

        QtVariantEditorFactory* variantFactory = new VariantFactory();

        _editor->setFactoryForManager(_variantManager, variantFactory);
        _editor->setPropertiesWithoutValueMarked(false);
        _editor->setRootIsDecorated(false);
        _editor->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(_editor, &QWidget::customContextMenuRequested, this, &EntityEditor::showContextMenu);

        QHBoxLayout* l = new QHBoxLayout();
        l->setMargin(0);
        l->setSpacing(0);
        l->setContentsMargins(0,0,0,0);
        setLayout(l);
        layout()->addWidget(_editor);

        connect(_variantManager, &VariantManager::valueChanged, this, &EntityEditor::propertyValueChanged);

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
            QVariant prototypes = _variantManager->attributeValue(prop, "prototypes");
            QVariantMap protos = prototypes.toMap();
            if(!protos.isEmpty())
            {
                QMenu menu(this);
                foreach(QString prototype, protos.keys())
                {
                    QAction* action = menu.addAction(QString("Add %1").arg(prototype));
                    // connect to lambda
                    connect(action, &QAction::triggered, [this, prototype, prop]() {
                        this->addListItem(prototype, prop);
                    });
                }
                menu.exec(_editor->mapToGlobal(pos));
            }
        }
        QtVariantProperty* parent = dynamic_cast<QtVariantProperty*>(findParentProperty(_editor, prop));
        if(parent && parent->valueType() == VariantManager::listId())
        {
            QMenu menu(this);
            QAction* action = menu.addAction("Remove entry");
            // connect to lambda
            connect(action, &QAction::triggered, [this, prop]() {
                this->removeListItem(prop);
            });
            menu.exec(_editor->mapToGlobal(pos));
        }
    }


    void EntityEditor::addListItem(const QString& prototype, QtVariantProperty* prop)
    {
        QVariant prototypes = _variantManager->attributeValue(prop, "prototypes");
        QVariantMap protos = prototypes.toMap();
        if(!protos.contains(prototype))
        {
            qDebug() << "Unexpected: Prototype " << prototype << " not found!";
            return;
        }

        QVariant prototypeAttributes = _variantManager->attributeValue(prop, "prototypeAttributes");
        QVariantMap attrs = prototypeAttributes.toMap();
        QVariantMap at = attrs[prototype].toMap();


        QVariant proto = protos[prototype];

        _ignorePropertyChanges = true;
        QtVariantProperty* item = this->addWidgetsRecursively(prototype, proto, at);
        item->setAttribute("prototype", prototype);
        prop->addSubProperty(item);

        _ignorePropertyChanges = false;

        // send changed value to entity system
        propertyValueChanged(prop);

    }


    void EntityEditor::removeListItem(QtVariantProperty* prop)
    {
        _ignorePropertyChanges = true;
        QtVariantProperty* parent = dynamic_cast<QtVariantProperty*>(
                findParentProperty(_editor, prop));
        Q_ASSERT(parent);
        parent->removeSubProperty(prop);
        delete prop;

        _ignorePropertyChanges = false;

        // send changed value to entity system
        propertyValueChanged(parent);
    }


    QtVariantProperty* EntityEditor::addWidgetsRecursively(const QString& name,
                                                    const QVariant& data,
                                                    const QVariantMap& attributes)
    {
        if(data.type() == QVariant::Map)
        {
            QVariantMap props = data.toMap();

            QtVariantProperty* grp = _variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), name);

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

            QtVariantProperty* lst = _variantManager->addProperty(VariantManager::listId(), name);

            for(auto k = attributes.begin(); k != attributes.end(); ++k)
            {
                lst->setAttribute(k.key(), k.value());
            }

            lst->setAttribute("prototypeAttributes", attributes);

            for(auto j = items.begin(); j != items.end(); ++j)
            {
                QVariantMap entry = j->toMap();
                if(!entry.contains("prototype") || !entry.contains("value"))
                {
                    qDebug("QList properties need entries of format QVariantMap(\"prototype\"=>\"value\"");
                    continue;
                }
                QString prototype = entry["prototype"].toString();
                QVariant val = entry["value"];
                QVariantMap subattribs = attributes[prototype].toMap();

                QtVariantProperty* prop = this->addWidgetsRecursively(prototype, val, subattribs);
                prop->setAttribute("prototype", prototype);
                if(prop)
                {
                    lst->addSubProperty(prop);
                }
            }
            return lst;
        }
        else
        {
            int tid = data.userType();

            // hack to detect enums
            if(attributes.contains("enumNames"))
            {
                tid = VariantManager::enumTypeId();
            }

            QtVariantProperty* propitem = _variantManager->addProperty(tid, name);
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


    void EntityEditor::displayEntity(QtEntity::EntityId id,
                                     const QVariantMap& data,
                                     const QVariantMap& attributes,
                                     const QStringList& availableComponents)
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
        _variantManager->clear();
        _editor->clear();
    }


    void EntityEditor::fetchEntityData(const QtEntity::EntityManager& em,
                                       QtEntity::EntityId eid,
                                       QVariantMap& components,
                                       QVariantMap& attributes,
                                       QStringList& availableComponents)
    {
        for(auto i = em.begin(); i != em.end(); ++i)
        {
            QtEntity::EntitySystem* es = i->second;
            if(!es->component(eid))
            {
                availableComponents.push_back(es->componentName());
            }
            else
            {
                components[es->componentName()] = es->toVariantMap(eid);
                attributes[es->componentName()] = es->editingAttributes();
            }
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
            es->fromVariantMap(eid, values.value(componenttype).toMap());
        }
    }

    void EntityEditor::propertyValueChanged(QtProperty *property)
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
        prop[changedProp->propertyName()] =  _variantManager->value(changedProp);
        QVariantMap components;
        components[changedComponent->propertyName()] = prop;
        emit entityDataChanged(_entityId, components);

    }
}
