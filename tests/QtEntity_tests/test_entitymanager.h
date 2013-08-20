#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtEntity/EntityManager>
#include <QtEntity/SimpleEntitySystem>
#include "common.h"

using namespace QtEntity;


class EntityManagerTest: public QObject
{
    Q_OBJECT
private slots:

    void getEntitySystemByComponent()
    {
        EntityManager em;

        EntitySystem* esnothing = em.systemByComponentType(Testing::staticMetaObject);
        QVERIFY(esnothing == nullptr);

        auto es = new TestingSystem();
        em.addSystem(es);
        EntitySystem* es2 = em.systemByComponentType(Testing::staticMetaObject);
        QVERIFY(es == es2);
    }

    void getEntitySystemBySystem()
    {
        EntityManager em;

        EntitySystem* esnothing = em.systemBySystemType(TestingSystem::staticMetaObject);
        QVERIFY(esnothing == nullptr);

        auto es = new TestingSystem();
        em.addSystem(es);
        EntitySystem* es2 = em.systemBySystemType(TestingSystem::staticMetaObject);
        QVERIFY(es == es2);
    }

    void removeSystem()
    {
        EntityManager em;

        auto es = new TestingSystem();
        em.addSystem(es);
        em.removeSystem(es);
        EntitySystem* es2 = em.systemByComponentType(Testing::staticMetaObject);
        QVERIFY(nullptr == es2);
        delete es;
    }

    void getEntitySystem()
    {
        EntityManager em;
        auto es = new TestingSystem();
        em.addSystem(es);
        TestingSystem* es2;
        bool success = em.system(es2);
        QVERIFY(success);
        QVERIFY(em.hasSystem(es));
        QVERIFY(es == es2);
    }

    void getComponent()
    {
        EntityManager em;
        auto es = new TestingSystem();
        em.addSystem(es);
        Testing *comp, *comp2;
        em.createComponent(1, comp);

        bool success = em.component(1, comp2);
        QVERIFY(success);
        QVERIFY(comp == comp2);
    }

    void getComponent2()
    {
        EntityManager em;
        auto es = new TestingSystem();
        em.addSystem(es);
        Testing *comp, *comp2;
        em.createComponent(1, comp);

        comp2 = em.component<Testing>(1);
        QVERIFY(comp == comp2);
    }

    void createComponent2()
    {
        EntityManager em;
        auto es = new TestingSystem();
        em.addSystem(es);
        auto c1 = em.createComponent<Testing>(1);

        Testing* c2;
        bool success = em.component(1, c2);
        QVERIFY(success);
        QVERIFY(c1 == c2);
    }

    void destroyComponent()
    {
        EntityManager em;
        auto es = new TestingSystem();
        em.addSystem(es);
        Testing *comp, *comp2;
        em.createComponent(1, comp);
        em.createComponent(2, comp2);
        em.destroyComponent<Testing>(1);

        bool success = em.component(1, comp);
        QVERIFY(!success);
        success = em.component(2, comp2);
        QVERIFY(success);
    }


    void destroyEntity()
    {
        EntityManager em;
        auto es = new TestingSystem();
        em.addSystem(es);
        Testing *comp;
        em.createComponent(1, comp);
        em.destroyEntity(1);
        bool success = em.component(1, comp);
        QVERIFY(!success);
        
    }

    void getOrCreateComponent()
    {
        EntityManager em;
        auto es = new TestingSystem();
        em.addSystem(es);
        Testing *comp, *comp2;
        em.getOrCreateComponent(1, comp);

        bool success = em.getOrCreateComponent(1, comp2);
        QVERIFY(success);
        QVERIFY(comp == comp2);
    }

    void createEntityId()
    {
        EntityManager em;
        EntityId eid = em.createEntityId();
        QVERIFY(1 == eid);
        EntityId eid2 = em.createEntityId();
        QVERIFY(2 == eid2);
    }


};
