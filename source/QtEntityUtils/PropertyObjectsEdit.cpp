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

#include <QtEntityUtils/PropertyObjectsEdit>

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
    PropertyObjectEditor::PropertyObjectEditor(QVariantList& objects, const QVariantMap& classes)
        : _classes(classes)
        , _ui(new Ui_PropertyObjectsEdit())
        , _propertyManager(new VariantManager(this))
        , _editor(new QtTreePropertyBrowser())
        , _selectedItem(nullptr)
    {
        _ui->setupUi(this);
        for(auto i = classes.begin(); i != classes.end(); ++i)
        {
            _ui->_classNames->addItem(i.key());
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
            QVariantMap map = i->toMap();
            QString classname = map["classname"].toString();
            addObjectProperties(classname, classes[classname].toMap(), map);
        }
    }

 
    void PropertyObjectEditor::addObjectProperties(const QString& classname, const QVariantMap& clss, const QVariantMap& values)
    { 
        QtProperty* item = _propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), classname);
        _editor->addProperty(item);
        for(auto i = clss.begin(); i != clss.end(); ++i)
        {
            QString name = i.key();
            QVariantMap propvals = i.value().toMap();
            int usertype = propvals["__type__"].toInt();
            QVariant val = propvals["__value__"];
            if(values.contains(name)) 
            {
                val = values[name];
            }
            
            QtVariantProperty* propitem = _propertyManager->addProperty(usertype, name);
            item->addSubProperty(propitem);
            propitem->setValue(val);

            if(propvals.contains("__attributes__"))
            {
                QVariantMap attrs = propvals["__attributes__"].toMap();
                for(auto j = attrs.begin(); j != attrs.end(); ++j)
                {
                    propitem->setAttribute(j.key(), j.value());
                }     
            }
        }
    }
    

   /* QtEntity::PropertyObjects PropertyObjectEditor::updatePropertyObjectsFromVariantList(const QtEntity::PropertyObjects& in, const QVariantList& varlist)
    {
        QtEntity::PropertyObjects ret;
        
        foreach(auto i, varlist)
        {
            QVariantMap map = i.toMap();
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
    }*/

    void PropertyObjectEditor::addObject()
    {
        QString classname = _ui->_classNames->currentText();
        if(!_classes.contains(classname))
        {
            qDebug() << "Classname not in class structure!";
            return;
        }
        QVariantMap clss = _classes[classname].toMap();        
        addObjectProperties(classname, clss);
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
        _editor = new PropertyObjectEditor(_value, _classes);
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
