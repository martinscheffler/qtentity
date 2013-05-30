#include "Game"

#include "BulletSystem"
#include "DamageSystem"
#include "Renderer"
#include "EnemySystem"
#include "ShapeSystem"
#include <QtEntity/MetaDataSystem>
#include <QCoreApplication>
#include <QTime>
#include <QThread>

Game::Game(Renderer* renderer)
    : _isRunning(false)
    , _renderer(renderer)
    , _bulletsys(new BulletSystem())
    , _damagesys(new DamageSystem())
    , _metasys(new QtEntity::MetaDataSystem())
    , _enemysys(new EnemySystem())
    , _shapesys(new ShapeSystem(renderer))
    , _leftpressed(false)
    , _rightpressed(false)
    , _spacepressed(false)
{
    _entityManager.addEntitySystem(_bulletsys);
    _entityManager.addEntitySystem(_damagesys);
    _entityManager.addEntitySystem(_metasys);
    _entityManager.addEntitySystem(_enemysys);
    _entityManager.addEntitySystem(_shapesys);

    _renderer->createShape(QPixmap(":/assets/space.jpg"), QPointF(0,0), -10);

    createPlayer();
}


void Game::run()
{
    _isRunning = true;

    int wantedTimeStep = 1000.0 / 60.0;
    int timeStep = 10;
    int frameNumber = 0;

    QTime startTime;
    while(_isRunning)
    {
        QTime frameTime;
        step(frameNumber++, startTime.elapsed(), timeStep);
        QCoreApplication::processEvents();
        timeStep = frameTime.elapsed();
        int sleepTime = wantedTimeStep - timeStep;
        if(sleepTime > 0)
        {
            QThread::msleep(sleepTime);
        }
    }    
}


void Game::end()
{
    _isRunning = false;
}


void Game::createPlayer()
{
    _playerid = _entityManager.createEntityId();

    {
        Shape* shape;
        QVariantMap m;
        m["position"] = QPoint(300, 380);
        m["path"] = ":/assets/spaceArt.svg";
        m["subtex"] = QRect(374,360,106,90);
        m["zIndex"] = 10;
        _entityManager.createComponent(_playerid, shape, m);
    }
    {
        QtEntity::MetaData* metadata;
        QVariantMap m;
        m["name"] = QString("Player");
        m["additionalInfo"] = "prefab=player";
        _entityManager.createComponent(_playerid, metadata, m);
    }
    {
        Damage* damage;
        QVariantMap m;
        m["energy"] = 1000;
        _entityManager.createComponent(_playerid, damage, m);
    }
}


void Game::createEnemy()
{
    QtEntity::EntityId id = _entityManager.createEntityId();

    {
        Shape* shape;
        QVariantMap m;
        m["position"] = QPoint(0, 0);
        m["path"] = ":/assets/spaceArt.svg";
        m["subtex"] = QRect(590,148,100,55);
        m["zIndex"] = 10;
        _entityManager.createComponent(id, shape, m);
    }
    {
        QtEntity::MetaData* metadata;
        QVariantMap m;
        m["name"] = QString("Enemy").arg(id);
        m["additionalInfo"] = "prefab=enemy";
        _entityManager.createComponent(id, metadata, m);
    }
    {
        Damage* damage;
        QVariantMap m;
        m["energy"] = 100;
        _entityManager.createComponent(id, damage, m);
    }
    {
        Enemy* enemy;
        _entityManager.createComponent(id, enemy);
    }
}


void Game::step(int frameNumber, int totalTime, int delta)
{
    if(frameNumber % 120 == 0)
    {
        createEnemy();
    }

    _enemysys->step(frameNumber, totalTime, delta);

    Shape* player;
    _entityManager.getComponent(_playerid, player);
    QPoint pos = player->position();
    if(_leftpressed)
    {
        int newpos = int(pos.x() - 5);
        pos.setX(std::max(-40, newpos));
        player->setPosition(pos);
    }
    if(_rightpressed)
    {
        int newpos = int(pos.x() + 5);
        pos.setX(std::min(590, newpos));
        player->setPosition(pos);
    }
}


void Game::keyPressEvent ( QKeyEvent * event )
{
    switch(event->key())
    {
    case Qt::Key_Left:  _leftpressed  = true;break;
    case Qt::Key_Right: _rightpressed = true; break;
    case Qt::Key_Space: _spacepressed = true; break;
    }
}

void Game::keyReleaseEvent ( QKeyEvent * event )
{
    switch(event->key())
    {
    case Qt::Key_Left:  _leftpressed  = false;break;
    case Qt::Key_Right: _rightpressed = false; break;
    case Qt::Key_Space: _spacepressed = false; break;
    }
}

