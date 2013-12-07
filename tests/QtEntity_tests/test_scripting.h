#include <QtTest/QtTest>
#include <QtEntity/EntityManager>
#include <QScriptEngine>
#include "common.h"
using namespace QtEntity;

inline QScriptValue entityIdToScriptValue(QScriptEngine *engine, const QtEntity::EntityId &s)
{
  return engine->newVariant(s);
}

inline void entityIdFromScriptValue(const QScriptValue &obj, QtEntity::EntityId &s)
{
    s = obj.toUInt32();
}


class ScriptingTest: public QObject
{
    Q_OBJECT

public:

    EntityManager _em;
    TestingSystem* _ts;
    QScriptEngine _engine;

    ScriptingTest()
    {
        _ts = new TestingSystem(&_em);
        QScriptValue objectValue = _engine.newQObject(&_em);
        _engine.globalObject().setProperty("EM", objectValue);
        qScriptRegisterMetaType(&_engine, entityIdToScriptValue, entityIdFromScriptValue);
        
        EntitySystemPrototype* esProto = new EntitySystemPrototype();
        _engine.setDefaultPrototype(qMetaTypeId<QtEntity::EntitySystem*>(), _engine.newQObject(esProto));
    }


private slots:
    
    void testCreateEntity()
    {
        QtEntity::EntityId count = _em.createEntityId();       
        QScriptValue ret = _engine.evaluate("EM.createEntityId();");
        if(_engine.hasUncaughtException())
        {
            qDebug() << "Script error: " << _engine.uncaughtException().toString();
        }
        QtEntity::EntityId next = (QtEntity::EntityId)ret.toInt32();
        QVERIFY(next == count + 1);
    }
        
    void testCreateComponent()
    {        
        size_t count = _ts->count();       
        QScriptValue ret = _engine.evaluate("EM.Testing.createComponent(667, {myint:999});");
        if(_engine.hasUncaughtException())
        {
            qDebug() << "Script error: " << _engine.uncaughtException().toString();
        }
        QCOMPARE(_ts->count(), count + 1);
    }

    void testDestroyComponent()
    {        
        size_t count = _ts->count();       
        _engine.evaluate("EM.Testing.createComponent(668, {myint:999});");
        if(_engine.hasUncaughtException())
        {
            qDebug() << "Script error: " << _engine.uncaughtException().toString();
        }
        _engine.evaluate("EM.Testing.destroyComponent(668, {myint:999});");
        if(_engine.hasUncaughtException())
        {
            qDebug() << "Script error: " << _engine.uncaughtException().toString();
        }
        QCOMPARE(_ts->count(), count);
    }

    void testGetProperty()
    {        
        QVariantMap m; m["myint"] = 12345;
        _ts->createComponent(999, m);
        QScriptValue ret = _engine.evaluate("var props = EM.Testing.properties(999); props.myint;");
        if(_engine.hasUncaughtException())
        {
            qDebug() << "Script error: " << _engine.uncaughtException().toString();
        }
        QCOMPARE(ret.toInt32(), 12345);
    }

    void testSetProperty()
    {        
        QVariantMap m; m["myint"] = 12345;
        _ts->createComponent(998, m);
        QScriptValue ret = _engine.evaluate("var props = {myint:6789}; EM.Testing.fromVariantMap(998, props);");
        if(_engine.hasUncaughtException())
        {
            qDebug() << "Script error: " << _engine.uncaughtException().toString();
        }

        QScriptValue ret2 = _engine.evaluate("var props = EM.Testing.properties(998); props.myint;");
        if(_engine.hasUncaughtException())
        {
            qDebug() << "Script error: " << _engine.uncaughtException().toString();
        }
        QCOMPARE(ret2.toInt32(), 6789);
    }
};

