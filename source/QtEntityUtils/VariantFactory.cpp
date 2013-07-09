#include <QtEntityUtils/VariantFactory>

#include <QtEntityUtils/FileEdit>
#include <QtEntityUtils/VariantManager>

namespace QtEntityUtils
{
    
    VariantFactory::~VariantFactory()
    {
        QList<FileEdit *> editors = theEditorToProperty.keys();
        QListIterator<FileEdit *> it(editors);
        while (it.hasNext())
            delete it.next();
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
        if (manager->propertyType(property) == VariantManager::filePathTypeId()) {
            FileEdit *editor = new FileEdit(parent);
            editor->setFilePath(manager->value(property).toString());
            editor->setFilter(manager->attributeValue(property, QLatin1String("filter")).toString());
            theCreatedEditors[property].append(editor);
            theEditorToProperty[editor] = property;

            connect(editor, SIGNAL(filePathChanged(const QString &)),
                    this, SLOT(slotSetValue(const QString &)));
            connect(editor, SIGNAL(destroyed(QObject *)),
                    this, SLOT(slotEditorDestroyed(QObject *)));
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
        if (!theCreatedEditors.contains(property))
            return;

        QList<FileEdit *> editors = theCreatedEditors[property];
        QListIterator<FileEdit *> itEditor(editors);
        while (itEditor.hasNext())
            itEditor.next()->setFilePath(value.toString());
    }

    void VariantFactory::slotPropertyAttributeChanged(QtProperty *property,
                const QString &attribute, const QVariant &value)
    {
        if (!theCreatedEditors.contains(property))
            return;

        if (attribute != QLatin1String("filter"))
            return;

        QList<FileEdit *> editors = theCreatedEditors[property];
        QListIterator<FileEdit *> itEditor(editors);
        while (itEditor.hasNext())
            itEditor.next()->setFilter(value.toString());
    }

    void VariantFactory::slotSetValue(const QString &value)
    {
        QObject *object = sender();
        QMap<FileEdit *, QtProperty *>::ConstIterator itEditor =
                    theEditorToProperty.constBegin();
        while (itEditor != theEditorToProperty.constEnd()) {
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
        QMap<FileEdit *, QtProperty *>::ConstIterator itEditor =
                    theEditorToProperty.constBegin();
        while (itEditor != theEditorToProperty.constEnd()) {
            if (itEditor.key() == object) {
                FileEdit *editor = itEditor.key();
                QtProperty *property = itEditor.value();
                theEditorToProperty.remove(editor);
                theCreatedEditors[property].removeAll(editor);
                if (theCreatedEditors[property].isEmpty())
                    theCreatedEditors.remove(property);
                return;
            }
            itEditor++;
        }
    }


}
