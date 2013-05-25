#include <QtTest/QtTest>
#include <QtCore/QObject>

#include <QtEntity/EntityManager>

using namespace qte;

class MocComponent : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE MocComponent() {}
};

class MocEntitySystem : public EntitySystem
{
    Q_OBJECT
public:
    MocEntitySystem() : EntitySystem(MocComponent::staticMetaObject) {}
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
