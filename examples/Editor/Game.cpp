#include "Game"

#include "BulletSystem"
#include "DamageSystem"
#include "Renderer"
#include "EnemySystem"
#include "ShapeSystem"
#include "MetaDataSystem"
#include <QCoreApplication>
#include <QTime>
#include <QThread>

#define PLAYERWIDTH 110
#define PLAYERHEIGHT 90
#define GAMEHEIGHT 600
#define GAMEWIDTH 480
#define PLAYERSPEED 10

void setBit(unsigned int& mask, Game::KeyBit bit, bool val)
{
    if(val)
    {
        mask |= bit;
    }
    else
    {
        mask &= ~bit;
    }
}

bool bitIsSet(unsigned int mask, Game::KeyBit bit)
{
    return (mask & bit) != 0;
}

Game::Game(Renderer* renderer)
    : _keyBits(0)
    , _renderer(renderer)
    , _isRunning(false)
    , _bulletsys(new BulletSystem(&_entityManager))
    , _damagesys(new DamageSystem(&_entityManager))
    , _metasys(new MetaDataSystem(&_entityManager))
    , _enemysys(new EnemySystem(&_entityManager))
    , _shapesys(new ShapeSystem(&_entityManager, renderer))
    , _playerid(0)
{

    //_background = _renderer->createShape(":/assets/GalaxyUno.jpg", QPoint(0,0), QRect(0, 0, 480,600), -10);

}


void Game::init()
{
    createPlayer();
}


void Game::run()
{
    init();
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
        m["position"] = QPoint(GAMEWIDTH / 2 - PLAYERWIDTH / 2, 600 - PLAYERHEIGHT);
        m["path"] = ":/assets/spaceArt.svg";
        m["subTex"] = QRect(374,360,PLAYERWIDTH,PLAYERHEIGHT);
        m["zIndex"] = 10;
        _entityManager.createComponent(_playerid, shape, m);
    }
    {
        MetaData* metadata;
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
        m["subTex"] = QRect(590,148,100,55);
        m["zIndex"] = 10;
        _entityManager.createComponent(id, shape, m);
    }
    {
        MetaData* metadata;
        QVariantMap m;
        m["name"] = QString("Enemy_%1").arg(id);
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
    if(frameNumber % 12000 == 0)
    {
        createEnemy();
    }

    _enemysys->step(frameNumber, totalTime, delta);

    Shape* player;
    _entityManager.component(_playerid, player);
    QPoint pos = player->position();
    if(bitIsSet(_keyBits, LEFT_PRESSED))
    {
        pos.setX(std::max(-PLAYERWIDTH / 2, pos.x() - PLAYERSPEED));
        player->setPosition(pos);
    }
    if(bitIsSet(_keyBits, RIGHT_PRESSED))
    {
        pos.setX(std::min(GAMEWIDTH - PLAYERWIDTH / 2, pos.x() + PLAYERSPEED));
        player->setPosition(pos);
    }
    if(bitIsSet(_keyBits, DOWN_PRESSED))
    {
        pos.setY(std::min(GAMEHEIGHT - PLAYERHEIGHT / 2, pos.y() + PLAYERSPEED));
        player->setPosition(pos);
    }
    if(bitIsSet(_keyBits, UP_PRESSED))
    {
        pos.setY(std::max(-PLAYERHEIGHT / 2, pos.y() - PLAYERSPEED));
        player->setPosition(pos);
    }
}


void Game::keyPressEvent ( QKeyEvent * event )
{
    switch(event->key())
    {
    case Qt::Key_Left:  setBit(_keyBits, KeyBit::LEFT_PRESSED, true);  break;
    case Qt::Key_Right: setBit(_keyBits, KeyBit::RIGHT_PRESSED, true); break;
    case Qt::Key_Up:    setBit(_keyBits, KeyBit::UP_PRESSED, true);    break;
    case Qt::Key_Down:  setBit(_keyBits, KeyBit::DOWN_PRESSED, true);  break;
    case Qt::Key_Space: setBit(_keyBits, KeyBit::SPACE_PRESSED, true); break;
    }
}

void Game::keyReleaseEvent ( QKeyEvent * event )
{
    switch(event->key())
    {
    case Qt::Key_Left:  setBit(_keyBits, KeyBit::LEFT_PRESSED, false);  break;
    case Qt::Key_Right: setBit(_keyBits, KeyBit::RIGHT_PRESSED, false); break;
    case Qt::Key_Up:    setBit(_keyBits, KeyBit::UP_PRESSED, false);    break;
    case Qt::Key_Down:  setBit(_keyBits, KeyBit::DOWN_PRESSED, false);  break;
    case Qt::Key_Space: setBit(_keyBits, KeyBit::SPACE_PRESSED, false); break;
    }
}

