#include "common.h"
#include <QtScript/QScriptEngine>

IMPLEMENT_COMPONENT_TYPE(Testing)
Q_DECLARE_METATYPE(QtEntity::EntitySystem*)

EntitySystemPrototype::EntitySystemPrototype(QObject *parent)
    : QObject(parent)
{
}


bool EntitySystemPrototype::createComponent(QtEntity::EntityId id, const QVariantMap& params)
{
    QtEntity::EntitySystem *es = qscriptvalue_cast<QtEntity::EntitySystem*>(thisObject());
    return (es->createComponent(id, params) != nullptr);
}


Q_INVOKABLE quint32 EntitySystemPrototype::count() const
{
    QtEntity::EntitySystem *es = qscriptvalue_cast<QtEntity::EntitySystem*>(thisObject());
    return (quint32)es->count();
}


QtEntity::PropertyAccessor EntitySystemPrototype::property(const QString& name) const
{
    QtEntity::EntitySystem *es = qscriptvalue_cast<QtEntity::EntitySystem*>(thisObject());
    for(int i = 0; i < es->propertyCount(); ++i)
    {
        QtEntity::PropertyAccessor a = es->property(i);
        if(a.name() == name)
        {
            return a;
        }
    }
    QtEntity::PropertyAccessor p;
    return p;
}