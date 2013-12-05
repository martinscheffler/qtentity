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
#include <QtEntityUtils/ListEdit>
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
            QString path = manager->value(property).value<QtEntityUtils::FilePath>();

            editor->setFilePath(path);
            editor->setFilter(manager->attributeValue(property, QLatin1String("filter")).toString());
            _createdFileEditors[property].append(editor);
            _fileEditorToProperty[editor] = property;

            connect(editor, &FileEdit::filePathChanged, this, &VariantFactory::slotSetFilePathValue);
            connect(editor, &FileEdit::destroyed, this, &VariantFactory::slotEditorDestroyed);
            return editor;
        }
        if (manager->propertyType(property) == VariantManager::listId())
        {
            ListEdit *editor = new ListEdit(parent);

            _createdListEditors[property].append(editor);
            _listEditorToProperty[editor] = property;

            connect(editor, &ListEdit::listChanged, this, &VariantFactory::slotSetListValue);
            connect(editor, &ListEdit::destroyed, this, &VariantFactory::slotEditorDestroyed);
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
    }

    void VariantFactory::slotSetFilePathValue(const QString &value)
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


    void VariantFactory::slotSetListValue(const QVariantList &value)
    {
        QObject *object = sender();

        QMap<ListEdit *, QtProperty *>::ConstIterator itEditor =
                    _listEditorToProperty.constBegin();
        while (itEditor != _listEditorToProperty.constEnd()) {
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
    }
}
