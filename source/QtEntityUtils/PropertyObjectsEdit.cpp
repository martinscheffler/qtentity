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

namespace QtEntityUtils
{
    PropertyObjectEditor::PropertyObjectEditor(QtEntity::PropertyObjects& objects, const QStringList& classnames)
        : _classNames(classnames)
        , _ui(new Ui_PropertyObjectsEdit())
        , _propertyManager(new VariantManager(this))
        , _editor(new QtTreePropertyBrowser())
    {
        _ui->setupUi(this);
        _editor->setFactoryForManager(_propertyManager, new VariantFactory());
        _editor->setPropertiesWithoutValueMarked(true);
        _editor->setRootIsDecorated(false);
        _ui->_editorPlaceholder->setLayout(new QHBoxLayout());
        _ui->_editorPlaceholder->layout()->addWidget(_editor);

        connect(_propertyManager, &VariantManager::valueChanged, this, &PropertyObjectEditor::propertyValueChanged);

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


    void PropertyObjectEditor::propertyValueChanged(QtProperty *property, const QVariant &val)
    {

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
        if(result != QDialog::Accepted) return;
        Q_ASSERT(_editor != nullptr);

        QtEntity::PropertyObjects obs = _editor->propertyObjects();
        emit objectsChanged(obs);
        delete _editor;
        _editor = nullptr;
    }

}
