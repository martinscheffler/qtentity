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
#include <QtEntityUtils/ItemList>
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
#include <QMessageBox>

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

        VariantFactory* variantFactory = new VariantFactory();

        connect(variantFactory, &VariantFactory::listAddButtonClicked, [=](QtVariantProperty* prop) {
            showContextMenu(_editor->mapFromGlobal(QCursor::pos()));
        });

        _editor->setFactoryForManager(_variantManager, variantFactory);
        _editor->setPropertiesWithoutValueMarked(false);
        _editor->setRootIsDecorated(false);
        _editor->setContextMenuPolicy(Qt::CustomContextMenu);
        _editor->setResizeMode(QtTreePropertyBrowser::Interactive);
        connect(_editor, &QWidget::customContextMenuRequested, this, &EntityEditor::showContextMenu);
        
        QHBoxLayout* l = new QHBoxLayout();
        l->setMargin(0);
        l->setSpacing(0);
        l->setContentsMargins(0,0,0,0);
        setLayout(l);
        layout()->addWidget(_editor);

        connect(_variantManager, &VariantManager::valueChanged, this, &EntityEditor::propertyValueChanged);

    }

    QVariant toVariant(const QtProperty* prop)
    {
        const QtVariantProperty* p = dynamic_cast<const QtVariantProperty*>(prop);
        Q_ASSERT(p);
        QVariant value = p->value();
        return value;

    }


    QVariantMap EntityEditor::entityData()
    {
        QVariantMap ret;

        foreach(QtProperty* prop, _editor->properties())
        {
            ret[prop->propertyName()] = toVariant(prop);
        }

        return ret;
    }


    void EntityEditor::showContextMenu(const QPoint& pos)
    {
        QtVariantProperty* prop = dynamic_cast<QtVariantProperty*>(_editor->propertyAt(pos));
        if(!prop)
        {
            return;
        }

        if(prop->propertyType() == VariantManager::listId())
        {
            int maxentries = _variantManager->attributeValue(prop, "maxentries").toInt();
            if(prop->subProperties().size() >= maxentries)
            {
                return;
            }
            QVariant prototypes = _variantManager->attributeValue(prop, "prototypes");
            QVariantList protos = prototypes.toList();
            if(!protos.isEmpty())
            {
                QMenu menu(this);
                foreach(QVariant prototype, protos)
                {
                    QAction* action = menu.addAction(QString("Add %1").arg(prototype.toString()));
                    // connect to lambda
                    connect(action, &QAction::triggered, [this, prototype, prop]() {
                        this->addListItem(prototype.toString(), prop);
                    });
                }
                menu.exec(_editor->mapToGlobal(pos));
            }
        }
        QtVariantProperty* parent = dynamic_cast<QtVariantProperty*>(findParentProperty(_editor, prop));
        if(parent && parent->propertyType() == VariantManager::listId())
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


    void EntityEditor::addListItem(const QString& prototypename, QtVariantProperty* prop)
    {
        QVariant prototypes = _variantManager->attributeValue(prop, "prototypes");
        QStringList protos = prototypes.toStringList();
        if(!protos.contains(prototypename) || !_types.contains(prototypename))
        {
            qDebug() << "Unexpected: Prototype " << prototypename << " not found!";
            return;
        }

        QVariantMap typeentry = _types[prototypename].toMap();

        QVariantMap prototype = typeentry["prototype"].toMap();
        QVariantMap prototypeAttributes = typeentry["attributes"].toMap();        

        _ignorePropertyChanges = true;
        QtVariantProperty* item = this->addWidgetsRecursively(prototypename, prototype, prototypeAttributes);
        item->setAttribute("prototype", prototypename);
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
        QtVariantProperty* createdprop;
        if(data.type() == QVariant::Map)
        {
            QVariantMap props = data.toMap();

            createdprop = _variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), name);

            // set property attributes
            for(auto k = attributes.begin(); k != attributes.end(); ++k)
            {
                createdprop->setAttribute(k.key(), k.value());
            }

            for(auto i = props.begin(); i != props.end(); ++i)
            {
                QVariantMap subattribs = attributes[i.key()].toMap();
                auto prop = this->addWidgetsRecursively(i.key(), i.value(), subattribs);
                if(prop)
                {
                    if(createdprop->subProperties().empty())
                    {
                        createdprop->addSubProperty(prop);
                    } 
                    else
                    {
                        // insert sorted by order property

                        int enterorder = _variantManager->attributeValue(prop, "order").toInt();

                        QtProperty* before = nullptr;
                        for(QtProperty* sub : createdprop->subProperties())
                        {
                            int suborder = _variantManager->attributeValue(sub, "order").toInt();
                            if(suborder >= enterorder)
                            {
                                break;
                            }
                            else
                            before = sub;
                        }                        
                        createdprop->insertSubProperty(prop, before);                       
                    }                    
                }
            }
        }
        else if(data.userType() == VariantManager::listId())
        {
            ItemList items = data.value<ItemList>();
            createdprop = _variantManager->addProperty(VariantManager::listId(), name);
           
            // set property attributes
            for(auto k = attributes.begin(); k != attributes.end(); ++k)
            {
                createdprop->setAttribute(k.key(), k.value());
            }

            foreach(const Item& item, items)
            {               
                QVariantMap entryattrs;

                if(_types.contains(item._prototype))
                {
                    QVariantMap t = _types[item._prototype].toMap();
                    if(t.contains("attributes"))
                    {
                        entryattrs = t["attributes"].toMap();
                    }
                }

                QtVariantProperty* prop = this->addWidgetsRecursively(item._prototype, item._value, entryattrs);
                
                if(prop)
                {
                    prop->setAttribute("prototype", item._prototype);
                    createdprop->addSubProperty(prop);
                }
            }
        }
        else
        {
            int tid = data.userType();

            // hack to detect enums
            if(attributes.contains("enumNames"))
            {
                tid = VariantManager::enumTypeId();
            }
            

            createdprop = _variantManager->addProperty(tid, name);
            // set property attributes
            
            for(auto k = attributes.begin(); k != attributes.end(); ++k)
            {
                createdprop->setAttribute(k.key(), k.value());
            }
           
            if(createdprop)
            {
                createdprop->setValue(data);               
            }
            else
            {
                qDebug() << "Could not create property editor for property " << name;
            }
            
        }

        
        return createdprop;
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

        _types = QVariantMap();

        if(attributes.contains("__types"))
        {
            _types.unite(attributes["__types"].toMap());
        }

        for(auto i = data.begin(); i != data.end(); ++i)
        {
            QVariantMap attrs = attributes.value(i.key(), QVariantMap()).toMap();
            // store prototypes and attributes for list properties
            if(attrs.contains("__types"))
            {
                _types.unite(attrs["__types"].toMap());
            }


            auto item = this->addWidgetsRecursively(i.key(), i.value(), attrs);
            if(item)
            {
                _editor->addProperty(item);

                collapseCollapsedEntries(_editor->topLevelItems());
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
        if(property == changedComponent)
        {
            changedProp = changedComponent;
        }
        else
        {
            foreach(auto prop, changedComponent->subProperties())
            {
                if(findSubProperty(prop, property))
                {
                    changedProp = prop;
                }
            }
        }
        
        Q_ASSERT(changedProp);

        QVariantMap prop;
        prop[changedProp->propertyName()] =  _variantManager->value(changedProp);
        QVariantMap components;
        components[changedComponent->propertyName()] = prop;
        emit entityDataChanged(_entityId, components);

    }

    void EntityEditor::collapseCollapsedEntries(QList<QtBrowserItem *>& entries)
    {
        foreach(auto entry, entries)
        {
            bool expanded = _variantManager->attributeValue(entry->property(), "expanded").toBool();
            if(!expanded)
            {
                _editor->setExpanded(entry, false);
            }
            collapseCollapsedEntries(entry->children());
        }        
    }

    void setAllExpanded(QtTreePropertyBrowser* editor, QtVariantPropertyManager* variantManager, QList<QtBrowserItem *>& entries, bool expanded)
    {
        foreach(auto entry, entries)
        {
            editor->setExpanded(entry, expanded);            
            setAllExpanded(editor, variantManager, entry->children(), expanded);
        }   
    }


    void EntityEditor::expandAll()
    {
        setAllExpanded(_editor, _variantManager, _editor->topLevelItems(), true);
    }


    void EntityEditor::collapseAll()
    {
        setAllExpanded(_editor, _variantManager, _editor->topLevelItems(), false);
    }
}
