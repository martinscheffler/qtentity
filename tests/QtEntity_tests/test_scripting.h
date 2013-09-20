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
        /*QtEntity::EntityId count = _em.createEntityId();        
        QScriptValue ret = _engine.evaluate("EM.system(\"Testing\");");
        if(_engine.hasUncaughtException())
        {
            qDebug() << "Script error: " << _engine.uncaughtException().toString();
        }
        QtEntity::EntitySystem* es = qscriptvalue_cast<QtEntity::EntitySystem*>(ret);
        QCOMPARE(es, _ts);*/
    }
};

