#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtEntity/PrefabSystem>
#include <QtEntity/EntityManager>

using namespace QtEntity;

class MyComponent : public QObject
{
    Q_OBJECT

public:

    Q_INVOKABLE MyComponent() {}
    Q_PROPERTY(int myint READ myInt WRITE setMyInt USER true)
    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const  { return _myint; }
    qint32 _myint;
};


class PrefabSystemTest: public QObject
{
    Q_OBJECT

private slots:

    void create()
    {
        EntityManager em;
        PrefabSystem* ps = new PrefabSystem();
        em.addEntitySystem(ps);

        SimpleEntitySystem* es = new SimpleEntitySystem(MyComponent::staticMetaObject);
        em.addEntitySystem(es);

        QVariantMap mycomponent;
        mycomponent["myint"] = 12345;

        QVariantMap components;
        QString cn = MyComponent::staticMetaObject.className();
        components[cn] = mycomponent;
        ps->addPrefab("bla.prefab", components);

        PrefabInstance* instance;

        QVariantMap props;
        props["path"] = "bla.prefab";
        em.createComponent(1, instance, props);

        auto test = em.component<MyComponent>(1);
        QVERIFY(test != nullptr);
        QCOMPARE(12345, test->myInt());

    }



    void update()
    {
        EntityManager em;
        PrefabSystem* ps = new PrefabSystem();
        em.addEntitySystem(ps);

        SimpleEntitySystem* es = new SimpleEntitySystem(MyComponent::staticMetaObject);
        em.addEntitySystem(es);

        QVariantMap mycomponent;
        mycomponent["myint"] = 12345;

        QVariantMap components;
        QString cn = MyComponent::staticMetaObject.className();
        components[cn] = mycomponent;
        ps->addPrefab("bla.prefab", components);

        PrefabInstance* instance;

        QVariantMap props;
        props["path"] = "bla.prefab";
        em.createComponent(1, instance, props);

        auto test = em.component<MyComponent>(1);
        QVERIFY(test != nullptr);
        QCOMPARE(12345, test->myInt());

        mycomponent["myint"] = 6789;
        components[cn] = mycomponent;
        ps->updatePrefab("bla.prefab", components, true);
        QCOMPARE(6789, test->myInt());
    }

    void destroyComponent()
    {
        EntityManager em;
        PrefabSystem* ps = new PrefabSystem();
        em.addEntitySystem(ps);

        SimpleEntitySystem* es = new SimpleEntitySystem(MyComponent::staticMetaObject);
        em.addEntitySystem(es);

        QVariantMap mycomponent;
        mycomponent["myint"] = 12345;

        QVariantMap components;
        QString cn = MyComponent::staticMetaObject.className();
        components[cn] = mycomponent;
        ps->addPrefab("bla.prefab", components);

        PrefabInstance* instance;

        QVariantMap props;
        props["path"] = "bla.prefab";
        em.createComponent(1, instance, props);

        auto test = em.component<MyComponent>(1);
        QVERIFY(test != nullptr);
        QCOMPARE(12345, test->myInt());
        auto x = em.component<MyComponent>(1);
        QVERIFY(x);
        components.clear();
        ps->updatePrefab("bla.prefab", components, true);

        auto y = em.component<MyComponent>(1);
        QVERIFY(y == nullptr);
    }


    void add()
    {
        EntityManager em;
        PrefabSystem* ps = new PrefabSystem();
        em.addEntitySystem(ps);

        SimpleEntitySystem* es = new SimpleEntitySystem(MyComponent::staticMetaObject);
        em.addEntitySystem(es);

        QVariantMap mycomponent;
        mycomponent["myint"] = 12345;

        QVariantMap components;
        QString cn = MyComponent::staticMetaObject.className();

        ps->addPrefab("bla.prefab", components);

        PrefabInstance* instance;

        QVariantMap props;
        props["path"] = "bla.prefab";
        em.createComponent(1, instance, props);

        auto test = em.component<MyComponent>(1);
        QVERIFY(test == nullptr);

        mycomponent["myint"] = 6789;
        components[cn] = mycomponent;
        components[cn] = mycomponent;
        ps->updatePrefab("bla.prefab", components, true);
        test = em.component<MyComponent>(1);
        QVERIFY(test != nullptr);
        QCOMPARE(6789, test->myInt());
    }

};
