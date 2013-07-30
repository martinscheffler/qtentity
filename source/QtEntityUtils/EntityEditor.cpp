#include <QtEntityUtils/EntityEditor>
#include <QtEntityUtils/VariantFactory>
#include <QtEntityUtils/VariantManager>
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
    }


    void EntityEditor::clear()
    {
        _propertyManager->clear();
        _editor->clear();
    }


    QVariant EntityEditor::fetchEntityData(const QtEntity::EntityManager& em, QtEntity::EntityId eid)
    {
        QVariantMap components;
        for(auto i = em.begin(); i != em.end(); ++i)
        {
            QtEntity::EntitySystem* es = *i;
            QObject* component = es->getComponent(eid);
            if(component == nullptr) continue;

            const QMetaObject& meta = es->componentMetaObject();

            // store property name-value pairs
            QVariantMap componentvals;
            for(int j = 0; j < meta.propertyCount(); ++j)
            {
                QMetaProperty prop = meta.property(j);
				const char* name = prop.name();
                if(strcmp(name, "objectName") == 0) continue;

                if(prop.userType() == VariantManager::propertyObjectsId())
                {
                    QVariantList vl;

                    QtEntity::PropertyObjects po = prop.read(component).value<QtEntity::PropertyObjects>();
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
                    componentvals[name] = vl;
                }
                else
                {
                    componentvals[name] = prop.read(component);
                }

                // store property constraints. Prepend a "#|" to identify
                QVariantMap constraints = es->attributesForProperty(name);
                if(!constraints.empty())
                {
                    componentvals[QString("#|%1").arg(name)] = constraints;
                }

            }
            components[es->name()] = componentvals;

        }
        return components;
    }


    void EntityEditor::applyEntityData(QtEntity::EntityManager& em, QtEntity::EntityId eid, const QString& componenttype, const QString& propertyname, const QVariant& value)
    {
        for(auto i = em.begin(); i != em.end(); ++i)
        {
            QtEntity::EntitySystem* es = *i;
            if(es->name() != componenttype) continue;

            QObject* component = es->getComponent(eid);
            if(!component) continue;

            const QMetaObject& meta = es->componentMetaObject();
            for(int j = 0; j < meta.propertyCount(); ++j)
            {
                QMetaProperty prop = meta.property(j);
                if(prop.name() != propertyname) continue;

                if(prop.userType() == VariantManager::propertyObjectsId())
                {
                    QVariantList vars = value.value<QVariantList>();
                    QtEntity::PropertyObjects objs = prop.read(component).value<QtEntity::PropertyObjects>();

                    // delete objects not in vars list or update existing.
                    for(auto i = objs.begin(); i != objs.end(); ++i)
                    {
                        QString objname = i->data()->objectName();
                        bool found = false;
                        foreach(auto j, vars)
                        {
                            QVariantMap map = j.value<QVariantMap>();
                            if(map.contains("objectName") && map["objectName"].toString() == objname)
                            {
                                found = true;

                                // object still exists. Update from vars
                                const QMetaObject* mo = i->data()->metaObject();
                                for(int k = 0; k < mo->propertyCount(); ++k)
                                {
                                    QMetaProperty prp = mo->property(k);
                                    QString todo_remove =prp.name();
                                    if(map.contains(prp.name()))
                                    {
                                        prp.write(i->data(), map[prp.name()]);
                                    }
                                }
                            }
                        }

                        // delete object if not found in property list
                        if(!found)
                        {
                            i = objs.erase(i);
                        }
                    }

                    // Create new objects from vars
                    foreach(auto i, vars)
                    {
                        QVariantMap map = i.value<QVariantMap>();

                        if(!map.contains("objectName") || map["objectName"].toString() == "")
                        {
                            map["objectName"] = QUuid::createUuid().toString();

                            Q_ASSERT(map.contains("classname"));
                            QString classname = map["classname"].toString();

                            const QMetaObject* mo = QtEntity::metaObjectByClassName(classname);
                            if(!mo)
                            {
                                qDebug() << "Could not instantiate, classname not registered: " << classname;
                                return;
                            }

                            QObject* obj = mo->newInstance();

                            if(obj == nullptr)
                            {
                                qDebug() << "Could not create object of class " << classname;
                                continue;
                            }



                            for(int j = 0; j < mo->propertyCount(); ++j)
                            {
                                QMetaProperty prp = mo->property((j));
                                if(map.contains(prp.name()))
                                {
                                    prp.write(obj, map[prp.name()]);
                                }
                            }

                            objs.push_back(QtEntity::PropertyObjectPointer(obj));
                            prop.write(component, QVariant::fromValue(objs));

                        }
                    }
                }
                else
                {
                    prop.write(component, value);
                }
                return;
            }

        }
    }


    QString getComponentNameForProperty(QtAbstractPropertyManager* manager, QtProperty *property)
    {
        QSet<QtProperty*> comps = manager->properties();
        foreach(auto comp, comps)
        {
            
			QString tmpname= comp->propertyName();
            foreach(auto prop, comp->subProperties())
            {
				Q_ASSERT(prop != NULL);
				QString tmpname2 = prop->propertyName();
                if(prop == property)
                {
                     return comp->propertyName();
                }
            }
        }
        return "";
    }


    void EntityEditor::propertyValueChanged(QtProperty *property, const QVariant &val)
    {
        QString componentName = getComponentNameForProperty(_propertyManager, property);
        if(!componentName.isEmpty())
        {
            emit entityDataChanged(_entityId, componentName, property->propertyName(), val);
        }
    }
}
