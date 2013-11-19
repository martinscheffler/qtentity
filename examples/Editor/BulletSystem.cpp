#include "BulletSystem"
#include "ShapeSystem"

#include <QtEntity/EntityManager>

IMPLEMENT_COMPONENT_TYPE(Bullet)

BulletSystem::BulletSystem(QtEntity::EntityManager* em)
   : BaseClass(em)
{
}

QVariantMap BulletSystem::propertyValues(QtEntity::EntityId eid)
{
    QVariantMap m;
    Bullet* b;
    if(component(eid, b))
    {
        m["movement"] = b->movement();
    }
    return m;    
}


void BulletSystem::applyPropertyValues(QtEntity::EntityId eid, const QVariantMap& m)
{
    Bullet* b;
    if(component(eid, b))
    {
        if(m.contains("movement")) b->setMovement(m["movement"].toPoint());
    }
}

void BulletSystem::step(int frameNumber, int totalTime, int delta)
{
   // float t = float(totalTime) / 1000.0f;

    for(auto i = begin(); i != end(); ++i)
    {
        /*Enemy* enemy = qobject_cast<Enemy*>(i.value());
        float lifetime = float(enemy->lifetime()) / 1000.0f;
        enemy->setLifetime(enemy->lifetime() + delta);
        Shape* shape;
        _entityManager->component(i.key(), shape);
        int x = sin(t / 10.0f + lifetime) * 300 + 280;
        int y = float(lifetime * 3.0f) + cos(t / 20.0f) * 10 - 50;
        shape->setPosition(QPoint(x, y));*/
    }
}
