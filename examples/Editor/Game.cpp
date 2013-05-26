#include "Game"

#include <QtEntity/MetaDataSystem>
#include <QCoreApplication>
#include <QTime>
#include <QThread>

Game::Game()
    : _isRunning(false)
{
    QtEntity::MetaDataSystem* metasys = new QtEntity::MetaDataSystem();
    _entityManager.addEntitySystem(metasys);

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


void Game::step(int frameNumber, int totalTime, int delta)
{
    if(frameNumber % 60 == 0)
    {
        QtEntity::EntityId id = _entityManager.createEntityId();
        QtEntity::MetaData* metadata;
        _entityManager.createComponent(id, metadata, {{"name", QString("Entity_%1").arg(id)}, {"additionalInfo", "LoadedFrom=code;bla=blu"}});
        _entityManager.getComponent(1, metadata);
        metadata->setAdditionalInfo(QString("LoadedFrom=%1").arg(frameNumber));
    }
}
