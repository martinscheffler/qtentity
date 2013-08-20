#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtGui/QVector2D>
#include <QtEntity/EntityManager>
#include <QtEntity/SimpleEntitySystem>
#include <QtEntity/PooledEntitySystem>
#include <QElapsedTimer>
#include "common.h"

using namespace QtEntity;

class TransformSystemPooled : public PooledEntitySystem<Transform>
{
public:
    TransformSystemPooled(size_t capacity = 0, size_t chunkSize = 4)
        : PooledEntitySystem<Transform>(capacity, chunkSize)
    {
        QTE_ADD_PROPERTY("myint", int, Transform, myInt, setMyInt);
        QTE_ADD_PROPERTY("myvec2", QVector2D, Transform, myVec2, setMyVec2);
    }
};


class PooledEntitySystemTest: public QObject
{
    Q_OBJECT


#define NUM_COMPONENTS 500
#define NUM_ITERATIONS 10000

private:
    template <typename T>
    int speed1(T& es)
    {
        QElapsedTimer timer;
        timer.start();
        QtEntity::EntityId id = 1;
        for(int i = 1; i <= NUM_COMPONENTS; ++i)
        {
            es.createComponent(id++);
        }

        for(int i = 1; i <= NUM_COMPONENTS; i += 4)
        {
            es.destroyComponent(i);
        }

        for(int i = 1; i <= NUM_COMPONENTS; ++i)
        {
            es.createComponent(id++);
        }

        for(int i = 0; i <= NUM_ITERATIONS; ++i)
        {
            auto end = es.end();
            for(auto j = es.begin(); j != end; ++j)
            {
                QObject* c = *j;
                Transform* t = qobject_cast<Transform*>(c);
                t->setMyInt(t->myInt() + 1);
            }
        }

        return timer.elapsed();
    }

    int speed2(EntitySystem& es)
    {

        QElapsedTimer timer;
        timer.start();

        QtEntity::EntityId id = 1;
        for(int i = 1; i <= NUM_COMPONENTS; ++i)
        {
            es.createComponent(id++);
        }

        for(int i = 1; i <= NUM_COMPONENTS; i += 4)
        {
            es.destroyComponent(i);
        }

        for(int i = 1; i <= NUM_COMPONENTS; ++i)
        {
            es.createComponent(id++);
        }

        for(int i = 0; i <= NUM_ITERATIONS; ++i)
        {
            auto end = es.pend();
            for(auto j = es.pbegin(); j != end; ++j)
            {
                QObject* c = *j;
                Transform* t = qobject_cast<Transform*>(c);
                t->setMyInt(t->myInt() + 1);
            }
        }

        return timer.elapsed();
    }

private slots:

    void createAndFetch()
    {
        TransformSystemPooled ts;

		QVariantMap m;
		m["myint"] = 666;
		m["myvec2"] =  QVector2D(77.0,88.0);
        QObject* c = ts.createComponent(1, m);

        QObject* c2 = ts.component(1);
        QVERIFY(c == c2);
        QVERIFY(ts.component(1) != nullptr);

        QObject* c3 = ts.component(2);
        QVERIFY(c3 == nullptr);

        Transform* tr = qobject_cast<Transform*>(c);
        QVERIFY(tr != nullptr);
        QCOMPARE(tr->myInt(), 666);
        QCOMPARE(tr->myVec2().x(), 77.0);
        QCOMPARE(tr->myVec2().y(), 88.0);
     

    }

    void reserve()
    {
        TransformSystemPooled ts(0, 2);
		for(int i = 0; i < 3; ++i) ts.createComponent(i + 1);
        QCOMPARE(ts.count(), (size_t)3);
        QCOMPARE(ts.capacity(), (size_t)4);
    }

    void destroyOne()
    {
        TransformSystemPooled ts(0, 2);
		ts.createComponent(1);
        ts.destroyComponent(1);
        QCOMPARE(ts.count(), (size_t)0);
        QCOMPARE(ts.capacity(), (size_t)2);
    }


     void destroyMore()
    {
        TransformSystemPooled ts(0, 2);
        QVariantMap m;
		m["myint"] = 1;		
        ts.createComponent(1, m);
        m["myint"] = 2;
        ts.createComponent(2, m);
        m["myint"] = 3;
        ts.createComponent(3, m);
        
        QCOMPARE(ts.count(), (size_t)3);
        ts.destroyComponent(2);
        QCOMPARE(ts.count(), (size_t)2);

        QCOMPARE(static_cast<Transform*>(ts.component(1))->myInt(), 1);
        QCOMPARE(static_cast<Transform*>(ts.component(3))->myInt(), 3);
        
    }

    
    void speedTest()
    {
        {
            TransformSystemPooled pooled(0, 8);
            SimpleEntitySystem simple(Transform::staticMetaObject);

            float timepooled = float(speed1(pooled)) / 1000.0f;
            float timesimple = float(speed1(simple)) / 1000.0f;
            qDebug() << "Time pooled1: " << timepooled;
            qDebug() << "Time simple1: " << timesimple;
        }

        {
            TransformSystemPooled pooled(0, 8);
            SimpleEntitySystem simple(Transform::staticMetaObject);

            float timepooled = float(speed2(pooled)) / 1000.0f;
            float timesimple = float(speed2(simple)) / 1000.0f;
            qDebug() << "Time pooled1: " << timepooled;
             qDebug() << "Time simple1: " << timesimple;
        }
    }

    void iteratorTest1()
    {
        TransformSystemPooled pooled(0, 8);
        QVERIFY(pooled.begin() == pooled.end());

        QVariantMap m;
		m["myint"] = 1; pooled.createComponent(1, m);
        m["myint"] = 2; pooled.createComponent(2, m);
        m["myint"] = 3; pooled.createComponent(3, m);
        TransformSystemPooled::Iterator i = pooled.begin();
        Transform* t = *i;
        QCOMPARE(t, static_cast<Transform*>(pooled.component(1)));
        QCOMPARE(*++i, static_cast<Transform*>(pooled.component(2)));
        QCOMPARE(*++i, static_cast<Transform*>(pooled.component(3)));
        TransformSystemPooled::Iterator j = ++i;
        TransformSystemPooled::Iterator end = pooled.end();
        QCOMPARE(j, end);
    }

    void iteratorTest2()
    {
        TransformSystemPooled pooled(0, 8);
        QVariantMap m;
        for(int i = 1; i <= 10; ++i)
        {
            m["myint"] = i;
            pooled.createComponent(i, m);
        }
        int count = 1;
        for(TransformSystemPooled::Iterator i = pooled.begin(); i != pooled.end(); ++i)
        {
            Transform* o = *i;
            QCOMPARE(o, static_cast<Transform*>(pooled.component(count++)));
        }
    }

    void iteratorTest3()
    {
        TransformSystemPooled pooled(0, 8);
        QVariantMap m;
        for(int i = 1; i <= 10; ++i)
        {
            m["myint"] = i;
            pooled.createComponent(i, m);
        }
        int count = 1;
        for(EntitySystem::Iterator i = pooled.pbegin(); i != pooled.pend(); ++i)
        {
            Transform* o = static_cast<Transform*>(*i);
            QCOMPARE(o, static_cast<Transform*>(pooled.component(count++)));
        }
    }
};
