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

namespace QtEntityUtils
{
    PropertyObjectEditor::PropertyObjectEditor(QtEntity::PropertyObjects& objects, const QStringList& classnames)
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
            QObject* obj = i->data();
            const QMetaObject* meta = obj->metaObject();

            QString classname = meta->className();
            QtProperty* item = _propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), classname);
            _editor->addProperty(item);
            for(int j = 0; j < meta->propertyCount(); ++j)
            {
                QMetaProperty prop = meta->property((j));
                if(strcmp(prop.name(), "objectName") == 0) continue;
                QtVariantProperty* propitem = _propertyManager->addProperty(prop.userType(), prop.name());
                item->addSubProperty(propitem);
                propitem->setValue(prop.read(obj));
            }
        }
    }


    void PropertyObjectEditor::addObject()
    {
        QString classname = _ui->_classNames->currentText();
        const QMetaObject* mo = QtEntity::metaObjectByClassName(classname);
        if(!mo)
        {
            qDebug() << "Could not instantiate, classname not registered: " << classname;
            return;
        }

        QtProperty* item = _propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), classname);
        _editor->addProperty(item);
        QObject* obj = mo->newInstance();
        if(obj == nullptr)
        {
            qDebug() << "Could not create object of class " << classname;
            return;
        }
        for(int j = 0; j < mo->propertyCount(); ++j)
        {
            QMetaProperty prop = mo->property((j));
            if(strcmp(prop.name(), "objectName") == 0) continue;
            QtVariantProperty* propitem = _propertyManager->addProperty(prop.userType(), prop.name());
            if(propitem == nullptr)
            {
                qDebug() << "Could not create property editor for property named " << prop.name();
            }
            else
            {
                item->addSubProperty(propitem);
                propitem->setValue(prop.read(obj));
            }
        }
        delete obj;
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


    QtEntity::PropertyObjects PropertyObjectEditor::propertyObjects() const
    {
        QtEntity::PropertyObjects ret;
        auto objprops =_editor->properties();
        foreach(QtProperty* p, objprops)
        {
            QtVariantProperty* objprop = static_cast<QtVariantProperty*>(p);
            QString classname = objprop->propertyName();
            const QMetaObject* mo = QtEntity::metaObjectByClassName(classname);
            if(mo == nullptr)
            {
                qDebug() << "Could not create property object, not called registerMetaObject? Classname is " << classname;
                continue;
            }
            QObject* o = mo->newInstance();
            ret.push_back(QtEntity::PropertyObjectPointer(o));

            foreach(QtProperty* subprop, objprop->subProperties())
            {
                bool found = false;
                QString name = subprop->propertyName();
                if(name == "objectName") continue;
                for(int i = 0; i < mo->propertyCount(); ++i)
                {
                    QMetaProperty pr = mo->property(i);
                    if(name == pr.name())
                    {
                        pr.write(o, static_cast<QtVariantProperty*>(subprop)->value());
                        found = true;
                        break;
                    }
                }
                if(!found)
                {
                    qDebug() << "Cannot set Property object parameter " << name;
                }
            }
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
