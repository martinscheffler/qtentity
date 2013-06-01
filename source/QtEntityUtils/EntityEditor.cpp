#include <QtEntityUtils/EntityEditor>
#include <QtPropertyBrowser/QtVariantPropertyManager>
#include <QtPropertyBrowser/QtTreePropertyBrowser>
#include <QtEntity/EntityManager>
#include <QDate>
#include <QLocale>
#include <QHBoxLayout>
#include <QMetaProperty>

namespace QtEntityUtils
{
    EntityEditor::EntityEditor()
        : _entityId(0)
        , _propertyManager(new QtVariantPropertyManager(this))
        , _editor(new QtTreePropertyBrowser())
    {

        QtVariantEditorFactory* variantFactory = new QtVariantEditorFactory();

        _editor->setFactoryForManager(_propertyManager, variantFactory);
        _editor->setPropertiesWithoutValueMarked(true);
        _editor->setRootIsDecorated(false);
        setLayout(new QHBoxLayout());
        layout()->addWidget(_editor);

/*
 * QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                    QString::number(i++) + QLatin1String(" Group Property"));
        QtVariantProperty *item = variantManager->addProperty(QVariant::Bool, QString::number(i++) + QLatin1String(" Bool Property"));
        item->setValue(true);
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Int, QString::number(i++) + QLatin1String(" Int Property"));
        item->setValue(20);
        item->setAttribute(QLatin1String("minimum"), 0);
        item->setAttribute(QLatin1String("maximum"), 100);
        item->setAttribute(QLatin1String("singleStep"), 10);
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Int, QString::number(i++) + QLatin1String(" Int Property (ReadOnly)"));
        item->setValue(20);
        item->setAttribute(QLatin1String("minimum"), 0);
        item->setAttribute(QLatin1String("maximum"), 100);
        item->setAttribute(QLatin1String("singleStep"), 10);
        item->setAttribute(QLatin1String("readOnly"), true);
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Double, QString::number(i++) + QLatin1String(" Double Property"));
        item->setValue(1.2345);
        item->setAttribute(QLatin1String("singleStep"), 0.1);
        item->setAttribute(QLatin1String("decimals"), 3);
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Double, QString::number(i++) + QLatin1String(" Double Property (ReadOnly)"));
        item->setValue(1.23456);
        item->setAttribute(QLatin1String("singleStep"), 0.1);
        item->setAttribute(QLatin1String("decimals"), 5);
        item->setAttribute(QLatin1String("readOnly"), true);
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::String, QString::number(i++) + QLatin1String(" String Property"));
        item->setValue("Value");
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::String, QString::number(i++) + QLatin1String(" String Property (Password)"));
        item->setAttribute(QLatin1String("echoMode"), QLineEdit::Password);
        item->setValue("Password");
        topItem->addSubProperty(item);

        // Readonly String Property
        item = variantManager->addProperty(QVariant::String, QString::number(i++) + QLatin1String(" String Property (ReadOnly)"));
        item->setAttribute(QLatin1String("readOnly"), true);
        item->setValue("readonly text");
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Date, QString::number(i++) + QLatin1String(" Date Property"));
        item->setValue(QDate::currentDate().addDays(2));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Time, QString::number(i++) + QLatin1String(" Time Property"));
        item->setValue(QTime::currentTime());
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::DateTime, QString::number(i++) + QLatin1String(" DateTime Property"));
        item->setValue(QDateTime::currentDateTime());
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::KeySequence, QString::number(i++) + QLatin1String(" KeySequence Property"));
        item->setValue(QKeySequence(Qt::ControlModifier | Qt::Key_Q));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Char, QString::number(i++) + QLatin1String(" Char Property"));
        item->setValue(QChar(386));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Locale, QString::number(i++) + QLatin1String(" Locale Property"));
        item->setValue(QLocale(QLocale::Polish, QLocale::Poland));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Point, QString::number(i++) + QLatin1String(" Point Property"));
        item->setValue(QPoint(10, 10));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::PointF, QString::number(i++) + QLatin1String(" PointF Property"));
        item->setValue(QPointF(1.2345, -1.23451));
        item->setAttribute(QLatin1String("decimals"), 3);
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Size, QString::number(i++) + QLatin1String(" Size Property"));
        item->setValue(QSize(20, 20));
        item->setAttribute(QLatin1String("minimum"), QSize(10, 10));
        item->setAttribute(QLatin1String("maximum"), QSize(30, 30));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::SizeF, QString::number(i++) + QLatin1String(" SizeF Property"));
        item->setValue(QSizeF(1.2345, 1.2345));
        item->setAttribute(QLatin1String("decimals"), 3);
        item->setAttribute(QLatin1String("minimum"), QSizeF(0.12, 0.34));
        item->setAttribute(QLatin1String("maximum"), QSizeF(20.56, 20.78));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Rect, QString::number(i++) + QLatin1String(" Rect Property"));
        item->setValue(QRect(10, 10, 20, 20));
        topItem->addSubProperty(item);
        item->setAttribute(QLatin1String("constraint"), QRect(0, 0, 50, 50));

        item = variantManager->addProperty(QVariant::RectF, QString::number(i++) + QLatin1String(" RectF Property"));
        item->setValue(QRectF(1.2345, 1.2345, 1.2345, 1.2345));
        topItem->addSubProperty(item);
        item->setAttribute(QLatin1String("constraint"), QRectF(0, 0, 50, 50));
        item->setAttribute(QLatin1String("decimals"), 3);

        item = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),
                        QString::number(i++) + QLatin1String(" Enum Property"));
        QStringList enumNames;
        enumNames << "Enum0" << "Enum1" << "Enum2";
        item->setAttribute(QLatin1String("enumNames"), enumNames);
        item->setValue(1);
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QtVariantPropertyManager::flagTypeId(),
                        QString::number(i++) + QLatin1String(" Flag Property"));
        QStringList flagNames;
        flagNames << "Flag0" << "Flag1" << "Flag2";
        item->setAttribute(QLatin1String("flagNames"), flagNames);
        item->setValue(5);
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::SizePolicy, QString::number(i++) + QLatin1String(" SizePolicy Property"));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Font, QString::number(i++) + QLatin1String(" Font Property"));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Cursor, QString::number(i++) + QLatin1String(" Cursor Property"));
        topItem->addSubProperty(item);

        item = variantManager->addProperty(QVariant::Color, QString::number(i++) + QLatin1String(" Color Property"));
        topItem->addSubProperty(item);
*/


    }

    void EntityEditor::displayEntity(QtEntity::EntityId id, const QVariant& data)
    {
        if(!data.canConvert<QVariantMap>()) return;

        clear();

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
                QVariant propval = j.value();
                QtVariantProperty* propitem = _propertyManager->addProperty(QVariant::String, propname);
                propitem->setValue(propval.toString());
                item->addSubProperty(propitem);
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
            if(es->hasComponent(eid))
            {
                QObject* component = es->getComponent(eid);
                const QMetaObject& meta = es->componentMetaObject();

                QVariantMap componentvals;
                for(int j = 0; j < meta.propertyCount(); ++j)
                {
                    QMetaProperty prop = meta.property(j);
                    componentvals[prop.name()] = prop.read(component);

                }
                components[es->name()] = componentvals;
            }
        }
        return components;
    }


    void EntityEditor::applyEntityData(QtEntity::EntityManager& em, QtEntity::EntityId eid, const QString& componenttype, const QString& propertyname, const QVariant& value)
    {

    }

}
