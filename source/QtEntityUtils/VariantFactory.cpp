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

#include <QtEntityUtils/VariantFactory>

#include <QtEntityUtils/FileEdit>
#include <QtEntityUtils/PropertyObjectsEdit>
#include <QtEntityUtils/VariantManager>
#include <QMetaProperty>

namespace QtEntityUtils
{
    
    VariantFactory::~VariantFactory()
    {
        {
            QList<FileEdit *> editors = _fileEditorToProperty.keys();
            QListIterator<FileEdit *> it(editors);
            while (it.hasNext())
                delete it.next();
        }
        {
            QList<PropertyObjectsEdit *> editors = _propertyObjectsEditorToProperty.keys();
            QListIterator<PropertyObjectsEdit *> it(editors);
            while (it.hasNext())
                delete it.next();
        }
    }


    void VariantFactory::connectPropertyManager(QtVariantPropertyManager *manager)
    {
        connect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                    this, SLOT(slotPropertyChanged(QtProperty *, const QVariant &)));
        connect(manager, SIGNAL(attributeChanged(QtProperty *, const QString &, const QVariant &)),
                    this, SLOT(slotPropertyAttributeChanged(QtProperty *, const QString &, const QVariant &)));
        QtVariantEditorFactory::connectPropertyManager(manager);
    }


    QWidget *VariantFactory::createEditor(QtVariantPropertyManager *manager,
            QtProperty *property, QWidget *parent)
    {
        if (manager->propertyType(property) == VariantManager::filePathTypeId())
        {
            FileEdit *editor = new FileEdit(parent);
            editor->setFilePath(manager->value(property).toString());
            editor->setFilter(manager->attributeValue(property, QLatin1String("filter")).toString());
            _createdFileEditors[property].append(editor);
            _fileEditorToProperty[editor] = property;

            connect(editor, &FileEdit::filePathChanged, this, &VariantFactory::slotSetValue);
            connect(editor, &PropertyObjectsEdit::destroyed, this, &VariantFactory::slotEditorDestroyed);
            return editor;
        }
        if (manager->propertyType(property) == VariantManager::variantListId())
        {
            PropertyObjectsEdit *editor = new PropertyObjectsEdit(parent);

            editor->setClasses(manager->attributeValue(property, QLatin1String("classes")).toMap());
            editor->setValue( manager->value(property).value<QVariantList>());
            
            _createdPropertyObjectsEditors[property].append(editor);
            _propertyObjectsEditorToProperty[editor] = property;

            connect(editor, &PropertyObjectsEdit::objectsChanged, this, &VariantFactory::slotSetObjectsValue);
            connect(editor, &PropertyObjectsEdit::destroyed, this, &VariantFactory::slotEditorDestroyed);
            return editor;
        }
        return QtVariantEditorFactory::createEditor(manager, property, parent);
    }


    void VariantFactory::disconnectPropertyManager(QtVariantPropertyManager *manager)
    {
        disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                    this, SLOT(slotPropertyChanged(QtProperty *, const QVariant &)));
        disconnect(manager, SIGNAL(attributeChanged(QtProperty *, const QString &, const QVariant &)),
                    this, SLOT(slotPropertyAttributeChanged(QtProperty *, const QString &, const QVariant &)));
        QtVariantEditorFactory::disconnectPropertyManager(manager);
    }


    void VariantFactory::slotPropertyChanged(QtProperty *property,
                    const QVariant &value)
    {
        if (!_createdFileEditors.contains(property))
            return;

        {
            QList<FileEdit *> editors = _createdFileEditors[property];
            QListIterator<FileEdit *> itEditor(editors);
            while (itEditor.hasNext())
                itEditor.next()->setFilePath(value.toString());
        }

        {
            QList<PropertyObjectsEdit *> editors = _createdPropertyObjectsEditors[property];
            QListIterator<PropertyObjectsEdit *> itEditor(editors);
            while (itEditor.hasNext())
                itEditor.next()->setValue(value.value<QVariantList>());
        }
    }


    void VariantFactory::slotPropertyAttributeChanged(QtProperty *property,
                const QString &attribute, const QVariant &value)
    {
        if (_createdFileEditors.contains(property))
        {

            if (attribute != QLatin1String("filter"))
                return;

            QList<FileEdit *> editors = _createdFileEditors[property];
            QListIterator<FileEdit *> itEditor(editors);
            while (itEditor.hasNext())
                itEditor.next()->setFilter(value.toString());
        }
        if (_createdPropertyObjectsEditors.contains(property))
        {
            if (attribute != QLatin1String("classes"))
                return;

            QList<PropertyObjectsEdit *> editors = _createdPropertyObjectsEditors[property];
            QListIterator<PropertyObjectsEdit *> itEditor(editors);
            while (itEditor.hasNext())
                itEditor.next()->setClasses(value.toMap());

        }
    }

    void VariantFactory::slotSetValue(const QString &value)
    {
        QObject *object = sender();

        QMap<FileEdit *, QtProperty *>::ConstIterator itEditor =
                    _fileEditorToProperty.constBegin();
        while (itEditor != _fileEditorToProperty.constEnd()) {
            if (itEditor.key() == object) {
                QtProperty *property = itEditor.value();
                QtVariantPropertyManager *manager = propertyManager(property);
                if (!manager)
                    return;
                manager->setValue(property, value);
                return;
            }
            itEditor++;
        }
    }


    void VariantFactory::slotSetObjectsValue(const QVariantList& value)
    {
        QObject *object = sender();

        QMap<PropertyObjectsEdit *, QtProperty *>::ConstIterator itEditor =
                    _propertyObjectsEditorToProperty.constBegin();
        while (itEditor != _propertyObjectsEditorToProperty.constEnd()) {
            if (itEditor.key() == object) {
                QtProperty *property = itEditor.value();
                QtVariantPropertyManager *manager = propertyManager(property);
                if (!manager)
                    return;
                manager->setValue(property, QVariant::fromValue(value));
                return;
            }
            itEditor++;
        }
    }


    void VariantFactory::slotEditorDestroyed(QObject *object)
    {
        {
            QMap<FileEdit *, QtProperty *>::ConstIterator itEditor =
                        _fileEditorToProperty.constBegin();
            while (itEditor != _fileEditorToProperty.constEnd()) {
                if (itEditor.key() == object) {
                    FileEdit *editor = itEditor.key();
                    QtProperty *property = itEditor.value();
                    _fileEditorToProperty.remove(editor);
                    _createdFileEditors[property].removeAll(editor);
                    if (_createdFileEditors[property].isEmpty())
                        _createdFileEditors.remove(property);
                    return;
                }
                itEditor++;
            }
        }

        {
            QMap<PropertyObjectsEdit *, QtProperty *>::ConstIterator itEditor =
                        _propertyObjectsEditorToProperty.constBegin();
            while (itEditor != _propertyObjectsEditorToProperty.constEnd()) {
                if (itEditor.key() == object) {
                    PropertyObjectsEdit *editor = itEditor.key();
                    QtProperty *property = itEditor.value();
                    _propertyObjectsEditorToProperty.remove(editor);
                    _createdPropertyObjectsEditors[property].removeAll(editor);
                    if (_createdPropertyObjectsEditors[property].isEmpty())
                        _createdPropertyObjectsEditors.remove(property);
                    return;
                }
                itEditor++;
            }
        }
    }


}
