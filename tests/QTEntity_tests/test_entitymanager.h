#include <QtTest/QtTest>
#include <QtCore/QObject>

#include <QTEntity/EntityManager>

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
        QVERIFY(es == es2);
    }

    void getComponent()
    {
        EntityManager em;
        auto es = new MocEntitySystem();
        em.addEntitySystem(es);
        es->createComponent(1);
        MocComponent* comp;
        bool success = em.getComponent(1, comp);
        QVERIFY(success);
        QVERIFY(comp == es->getComponent(1));
    }

    void createEntity()
    {
        EntityManager em;
        EntityId eid = em.createEntity();
        QVERIFY(1 == eid);
        EntityId eid2 = em.createEntity();
        QVERIFY(2 == eid2);
    }


};
