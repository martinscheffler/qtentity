#include "EnemySystem"
#include "ShapeSystem"

#include <QtEntity/EntityManager>
#include <QtEntity/MetaObjectRegistry>

EnemySystem::EnemySystem()
    :QtEntity::SimpleEntitySystem(Enemy::staticMetaObject)
{
    QtEntity::registerMetaObject(TestObj1::staticMetaObject);
    QtEntity::registerMetaObject(TestObj2::staticMetaObject);

    QTE_ADD_PROPERTY("lifetime", int, Enemy, lifetime, setLifetime);
    QTE_ADD_PROPERTY("myobjects", QtEntity::PropertyObjects, Enemy, myObjects, setMyObjects);

}


void EnemySystem::step(int frameNumber, int totalTime, int delta)
{
    float t = float(totalTime) / 1000.0f;

    for(auto i = begin(); i != end(); ++i)
    {
        Enemy* enemy = qobject_cast<Enemy*>(i.value());
        float lifetime = float(enemy->lifetime()) / 1000.0f;
        enemy->setLifetime(enemy->lifetime() + delta);
        Shape* shape;
        _entityManager->component(i.key(), shape);
        int x = sin(t / 10.0f + lifetime) * 300 + 280;
        int y = float(lifetime * 3.0f) + cos(t / 20.0f) * 10 - 50;
        shape->setPosition(QPoint(x, y));
    }
}



const QVariantMap EnemySystem::attributesForProperty(const QString& name) const
{
    QVariantMap r;
    if(name == "myobjects")
    {
        QStringList sl;
        sl.push_back(TestObj1::staticMetaObject.className());
        sl.push_back(TestObj2::staticMetaObject.className());
        r["classnames"] = sl;
    }
    return r;
}
