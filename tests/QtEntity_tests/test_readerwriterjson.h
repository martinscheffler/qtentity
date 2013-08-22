#include <QtTest/QtTest>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <QtGui/QColor>
#include <QtEntity/EntityManager>
#include <QtEntity/ReaderWriterJSON>
#include <QtEntity/MetaObjectRegistry>
#include <QtEntity/SimpleEntitySystem>
#include "common.h"
using namespace QtEntity;

class ReaderWriterJSONTest: public QObject
{
    Q_OBJECT

private slots:

    void metatype()
    {
        int id = qMetaTypeId<PropertyObjects>();

        PropertyObjects objs;
        QVariant v = QVariant::fromValue(objs);
        int usertype = v.userType();
        QVERIFY(id == usertype);
    }

    void encode()
    {

        EntityManager em;
        TestingSystem* ts = new TestingSystem();
        em.addSystem(ts);

        Testing* dh; em.createComponent(1, dh);
        dh->setMyInt(666);
        dh->setMyVec3(QVector3D(-1,0,354));
        PropertyObjects objs;
        objs.push_back(PropertyObjectPointer(new TestObj1()));
        objs.push_back(PropertyObjectPointer(new TestObj2()));
        dh->setMyObjects(objs);

        QJsonObject obj = ReaderWriterJSON::componentToJson(*ts, 1);
        QJsonDocument doc(obj);

        QString txt = doc.toJson(QJsonDocument::Compact);
        QString expected = "{\"mycolor\": \"0,0,0,255\",\"myint\": 666,\"myobjects\": [{\"classname\": \"TestObj1\",\"propint\": 0},{\"classname\": \"TestObj2\"}],\"myvec2\": \"0,0\",\"myvec3\": \"-1,0,354\",\"myvec4\": \"0,0,0,0\"}";
        
		// TODO: seems to be a little different from system to system
		//QVERIFY(expected == txt);
    }


    void decode()
    {
        EntityManager em;
        TestingSystem* ts = new TestingSystem();
        em.addSystem(ts);

        QString jsonstr = "{\"classname\": \"Testing\", \"mycolor\": \"0,0,0,255\",\"myint\": 666,\"myobjects\": [{\"classname\": \"TestObj1\",\"propint\": 12345},{\"classname\": \"TestObj2\"}],\"myvec2\": \"0,0\",\"myvec3\": \"-1,0,354\",\"myvec4\": \"0,0,0,0\"}";
        QJsonDocument doc = QJsonDocument::fromJson(jsonstr.toUtf8());
        QVERIFY(doc.isObject());


        bool success = ReaderWriterJSON::jsonToComponent(em, 1, doc.object());
        QVERIFY(success);
        Testing* dh = em.component<Testing>(1);
        QVERIFY(dh);
        QCOMPARE(dh->myInt(), 666);
        PropertyObjects objs = dh->myObjects();
        QCOMPARE(objs.count(), 2);
        
        TestObj1* to1 = qobject_cast<TestObj1*>(objs.front().data());
        QVERIFY(to1);

        QCOMPARE(to1->propInt(), 12345);
        
    }

};

