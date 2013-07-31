#include <QtEntityUtils/PropertyObjectsEdit>

#include <QtEntity/MetaObjectRegistry>
#include <QtEntityUtils/VariantManager>
#include <QtEntityUtils/VariantFactory>

#include <QtPropertyBrowser/QtTreePropertyBrowser>
#include <QtPropertyBrowser/QtVariantProperty>
#include "ui_PropertyObjectsEdit.h"
#include <QHBoxLayout>
#include <QMetaProperty>
#include <QDebug>
#include <QComboBox>
#include <QUuid>

namespace QtEntityUtils
{
    PropertyObjectEditor::PropertyObjectEditor(QVariantList& objects, const QStringList& classnames)
        : _classNames(classnames)
        , _ui(new Ui_PropertyObjectsEdit())
        , _propertyManager(new VariantManager(this))
        , _editor(new QtTreePropertyBrowser())
        , _selectedItem(nullptr)
    {
        _ui->setupUi(this);
        foreach(QString cn, classnames)
        {
            _ui->_classNames->addItem(cn);
        }
        _editor->setFactoryForManager(_propertyManager, new VariantFactory());
        _editor->setPropertiesWithoutValueMarked(true);
        _editor->setRootIsDecorated(false);
        _ui->_editorPlaceholder->setLayout(new QHBoxLayout());
        _ui->_editorPlaceholder->layout()->addWidget(_editor);

        connect(_ui->_addButton, &QPushButton::clicked, this, &PropertyObjectEditor::addObject);
        connect(_editor, &QtAbstractPropertyBrowser::currentItemChanged, this, &PropertyObjectEditor::currentItemChanged);
        connect(_ui->_upButton, &QPushButton::clicked, this, &PropertyObjectEditor::upPressed);
        connect(_ui->_downButton, &QPushButton::clicked, this, &PropertyObjectEditor::downPressed);
        connect(_ui->_removeButton, &QPushButton::clicked, this, &PropertyObjectEditor::removePressed);
        for(auto i = objects.begin(); i != objects.end(); ++i)
        {
            QVariantMap map = i->value<QVariantMap>();
            QString classname = map["classname"].toString();
            addObjectProperties(classname, map);
        }
    }


    QVariantMap PropertyObjectEditor::componentToVariantMap(const QObject& object)
    {
        QVariantMap ret;
       
        const QMetaObject& meta = *object.metaObject();
        
        for(int j = 0; j < meta.propertyCount(); ++j)
        {
            QMetaProperty prop = meta.property(j);
			const char* name = prop.name();            

            if(prop.userType() == VariantManager::propertyObjectsId())
            {
                QVariantList vl;

                QtEntity::PropertyObjects po = prop.read(&object).value<QtEntity::PropertyObjects>();
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
                ret[name] = vl;
            }
            else
            {
                ret[name] = prop.read(&object);
            } 
        }        
    
        return ret;
    }


    void applyPropertiesToObject(const QVariantMap& map, QObject* object)
    {
        const QMetaObject* meta = object->metaObject();

        for(int i = 0; i < meta->propertyCount(); ++i)
        {
            QMetaProperty prop = meta->property(i);  
            QString propname = prop.name();
            if(!map.contains(propname)) continue;

            QVariant val = map[propname];

            if(prop.userType() != VariantManager::propertyObjectsId())
            {
                prop.write(object, val);
            }
            else
            {
                QVariantList vars = val.value<QVariantList>();
                QtEntity::PropertyObjects subobjs;
              
                // Create new objects from vars
                foreach(auto j, vars)
                {
                    QVariantMap objmap = j.value<QVariantMap>();
                    Q_ASSERT(objmap.contains("classname"));
                    
                    // create new UUID
                    objmap["objectName"] = QUuid::createUuid().toString();

                    QString classname = objmap["classname"].toString();

                    const QMetaObject* mo = QtEntity::metaObjectByClassName(classname);
                    if(!mo)
                    {
                        qDebug() << "Could not instantiate, classname not registered: " << classname;
                        continue;
                    }

                    QObject* subobj = mo->newInstance();

                    if(subobj == nullptr)
                    {
                        qDebug() << "Could not create object of class " << classname;
                        continue;
                    }
                    
                    for(int k = 0; k < mo->propertyCount(); ++k)
                    {
                        QMetaProperty prp = mo->property(k);
                        if(objmap.contains(prp.name()))
                        {
                            prp.write(subobj, map[prp.name()]);
                        }
                    }
                    subobjs.push_back(QtEntity::PropertyObjectPointer(subobj));  
                }

                prop.write(object, QVariant::fromValue(subobjs));
            }            
        }
    }


    QObject* PropertyObjectEditor::variantMapToObject(const QVariantMap& map)
    {
        
        if(!map.contains("classname")) 
        {
            qDebug() << "Cannot convert variant map to object, no classname set!";
            return nullptr;
        }

        QString classname = map["classname"].toString();
        const QMetaObject* meta = QtEntity::metaObjectByClassName(classname);
        if(!meta) 
        {
            qDebug() << "Cannot convert variant map to object, class not registered: " << classname;
            return nullptr;
        }

        QObject* retobj = meta->newInstance();
        applyPropertiesToObject(map, retobj);
        return retobj;    
    }


    void PropertyObjectEditor::addObjectProperties(const QString& classname, const QVariantMap& obj)
    {        
        const QMetaObject* meta = QtEntity::metaObjectByClassName(classname);
        if(!meta)
        {
            qDebug() << "Could not instantiate, classname not registered: " << classname;
            return;
        }

        QtProperty* item = _propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), classname);
        _editor->addProperty(item);
        for(int j = 0; j < meta->propertyCount(); ++j)
        {
            QMetaProperty prop = meta->property((j));
            //if(strcmp(prop.name(), "objectName") == 0) continue;
            QtVariantProperty* propitem = _propertyManager->addProperty(prop.userType(), prop.name());
            item->addSubProperty(propitem);
            if(obj.find(prop.name()) != obj.end())
            {
                propitem->setValue(obj[prop.name()]);
            }
        }
    }


    QtEntity::PropertyObjects PropertyObjectEditor::updatePropertyObjectsFromVariantList(const QtEntity::PropertyObjects& in, const QVariantList& varlist)
    {
        QtEntity::PropertyObjects ret;
        
        foreach(auto i, varlist)
        {
            QVariantMap map = i.value<QVariantMap>();
            if(!map.contains("classname"))
            {
                qDebug() << "Error: Property Objects Variant Map contains no classname value! Skipping.";
                continue;
            }

            QString classname = map["classname"].toString();

            // when no object name is set then this is a newly created object. Create new object instance and
            // add to ret value
            if(!map.contains("objectName") || map["objectName"].toString() == "")
            {
                // create a new object name from random string
                map["objectName"] = QUuid::createUuid().toString();

                const QMetaObject* mo = QtEntity::metaObjectByClassName(classname);
                if(!mo)
                {
                    qDebug() << "Could not instantiate, classname not registered: " << classname;
                    continue;
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

                ret.push_back(QtEntity::PropertyObjectPointer(obj));
            }
            else
            {
                QtEntity::PropertyObjectPointer obj;
                QString objname = map["objectName"].toString();
                foreach(auto j, in)
                {                    
                    if(j->objectName() == objname)
                    {
                        obj = j;
                        break;
                    }
                }
                if(obj.isNull())
                {
                    qDebug() << "Could not update existing object, not found! ObjectName: " << objname;
                    continue;
                }
                const QMetaObject* mo = obj->metaObject();
                for(int k = 0; k < mo->propertyCount(); ++k)
                {
                    QMetaProperty prp = mo->property(k);
                    QString prpname = prp.name();
                    if(map.contains(prpname) && map[prpname] != prp.read(obj.data()))
                    {
                        prp.write(obj.data(), map[prpname]);
                    }
                }
                ret.push_back(obj);
            }
        }

        return ret;
    }

    void PropertyObjectEditor::addObject()
    {
        QString classname = _ui->_classNames->currentText();
        const QMetaObject* mo = QtEntity::metaObjectByClassName(classname);
        QObject* instance = mo->newInstance();
        QVariantMap map = componentToVariantMap(*instance);
        delete instance;
        addObjectProperties(classname, map);
    }


    void PropertyObjectEditor::currentItemChanged(QtBrowserItem* item)
    {
        bool objselected = (item != nullptr && item->parent() == nullptr) ;
        
        // only for top level properties (object classnames)
        _selectedItem = (objselected) ? item : nullptr;
        
        _ui->_removeButton->setEnabled(objselected);
        _ui->_upButton->setEnabled(objselected);
        _ui->_downButton->setEnabled(objselected);        
    }


    void PropertyObjectEditor::upPressed()
    {
        if(_selectedItem == nullptr || _editor->topLevelItems().first() == _selectedItem) return;
        QtProperty* selected = _selectedItem->property();
        QtProperty* before = nullptr;
        int size = _editor->topLevelItems().size();
        for(int i = 1; i < size; ++i)
        {
            if(_editor->topLevelItems()[i] == _selectedItem) 
            {
                before =  (i == 1) ? nullptr : _editor->topLevelItems()[i - 2]->property();
                break;
            }
        }
        _editor->removeProperty(selected);            
        QtBrowserItem* newbrowser = _editor->insertProperty(selected, before);
        _editor->setCurrentItem(newbrowser);
        
    }


    void PropertyObjectEditor::downPressed()
    {
       
        if(_selectedItem == nullptr || _editor->topLevelItems().last() == _selectedItem) return;
        QtProperty* selected = _selectedItem->property();
        QtProperty* before = nullptr;
        int size = _editor->topLevelItems().size();
        for(int i = 0; i < size - 1; ++i)
        {
            if(_editor->topLevelItems()[i] == _selectedItem) 
            {
                before =  _editor->topLevelItems()[i + 1]->property();
                break;
            }
        }
        _editor->removeProperty(selected);            
        QtBrowserItem* newbrowser = _editor->insertProperty(selected, before);
        _editor->setCurrentItem(newbrowser);
    }


    void PropertyObjectEditor::removePressed()
    {
        if(_selectedItem == nullptr) return;
         QtProperty* selected = _selectedItem->property();
         _editor->removeProperty(selected);     
         delete selected;
    }


    QVariantList PropertyObjectEditor::propertyObjects() const
    {
        QVariantList ret;
        auto objprops =_editor->properties();
        foreach(QtProperty* p, objprops)
        {
            QtVariantProperty* objprop = static_cast<QtVariantProperty*>(p);
            QString classname = objprop->propertyName();
            QVariantMap retobj;
            retobj["classname"] = classname;
            foreach(QtProperty* subprop, objprop->subProperties())
            {
                QString name = subprop->propertyName();
                retobj[name] = static_cast<QtVariantProperty*>(subprop)->value();
            }
            ret.push_back(retobj);
        }
        return ret;
    }

    
    PropertyObjectsEdit::PropertyObjectsEdit(QWidget *parent)
        : QWidget(parent)
        , _editor(nullptr)
    {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setMargin(0);
        layout->setSpacing(0);
        _editButton = new QPushButton();
        _editButton->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
        _editButton->setText(tr("Edit"));
        layout->addWidget(_editButton);
        setFocusProxy(_editButton);
        setFocusPolicy(Qt::StrongFocus);
        setAttribute(Qt::WA_InputMethodEnabled);
        connect(_editButton, &QPushButton::clicked, this, &PropertyObjectsEdit::openEditor);

    }


    void PropertyObjectsEdit::openEditor()
    {
        Q_ASSERT(_editor == nullptr);
        _editor = new PropertyObjectEditor(_value, _classnames);
        connect(_editor, &PropertyObjectEditor::finished, this, &PropertyObjectsEdit::editorClosed);
        _editor->exec();
    }


    void PropertyObjectsEdit::editorClosed(int result)
    {        
        Q_ASSERT(_editor != nullptr);
        if(result == QDialog::Accepted) 
        {
            _value = _editor->propertyObjects();
            emit objectsChanged(_value);
        }
        _editor->deleteLater();
        _editor = nullptr;        
    }

}
