#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtEntity/EntityManager>
#include <QtEntity/SimpleEntitySystem>

using namespace QtEntity;

class MocComponent : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE MocComponent() {}
};

class MocEntitySystem : public SimpleEntitySystem
{
    Q_OBJECT
public:
    MocEntitySystem() : SimpleEntitySystem(MocComponent::staticMetaObject) {}
};


class EntityManagerTest: public QObject
{
    Q_OBJECT
private slots:

    void getEntitySystemByComponent()
    {
        EntityManager em;

        EntitySystem* esnothing = em.getSystemByComponentType(MocComponent::staticMetaObject);
        QVERIFY(esnothing == nullptr);

        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        EntitySystem* es2 = em.getSystemByComponentType(MocComponent::staticMetaObject);
        QVERIFY(es == es2);
    }

    void getEntitySystemBySystem()
    {
        EntityManager em;

        EntitySystem* esnothing = em.getSystemBySystemType(MocEntitySystem::staticMetaObject);
        QVERIFY(esnothing == nullptr);

        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        EntitySystem* es2 = em.getSystemBySystemType(MocEntitySystem::staticMetaObject);
        QVERIFY(es == es2);
    }

    void removeEntitySystem()
    {
        EntityManager em;

        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        em.removeEntitySystem(es);
        EntitySystem* es2 = em.getSystemByComponentType(MocComponent::staticMetaObject);
        QVERIFY(nullptr == es2);
        delete es;
    }

    void getEntitySystem()
    {
        EntityManager em;
        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        MocEntitySystem* es2;
        bool success = em.getEntitySystem(es2);
        QVERIFY(success);
        QVERIFY(em.hasEntitySystem(es));
        QVERIFY(es == es2);
    }

    void getComponent()
    {
        EntityManager em;
        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        MocComponent *comp, *comp2;
        em.createComponent(1, comp);

        bool success = em.getComponent(1, comp2);
        QVERIFY(success);
        QVERIFY(comp == comp2);
    }

    void getComponent2()
    {
        EntityManager em;
        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        MocComponent *comp, *comp2;
        em.createComponent(1, comp);

        comp2 = em.getComponent<MocComponent>(1);
        QVERIFY(comp == comp2);
    }

    void createComponent2()
    {
        EntityManager em;
        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        auto c1 = em.createComponent<MocComponent>(1);

        MocComponent* c2;
        bool success = em.getComponent(1, c2);
        QVERIFY(success);
        QVERIFY(c1 == c2);
    }

    void destroyComponent()
    {
        EntityManager em;
        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        MocComponent *comp, *comp2;
        em.createComponent(1, comp);
        em.createComponent(2, comp2);
        em.destroyComponent<MocComponent>(1);

        bool success = em.getComponent(1, comp);
        QVERIFY(!success);
        success = em.getComponent(2, comp2);
        QVERIFY(success);
    }


    void destroyEntity()
    {
        EntityManager em;
        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        MocComponent *comp;
        em.createComponent(1, comp);
        em.destroyEntity(1);
        bool success = em.getComponent(1, comp);
        QVERIFY(!success);
        
    }

    void getOrCreateComponent()
    {
        EntityManager em;
        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        MocComponent *comp, *comp2;
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
