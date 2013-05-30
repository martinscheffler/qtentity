#include "Game"

#include "Renderer"
#include <QtEntity/MetaDataSystem>
#include <QCoreApplication>
#include <QTime>
#include <QThread>

Game::Game(Renderer* renderer)
    : _isRunning(false)
    , _renderer(renderer)
{
    _entityManager.addEntitySystem(new QtEntity::MetaDataSystem());

    _renderer->createShape(QPixmap(":/assets/space.jpg"), QPointF(0,0), -10);

    QPixmap pic(":/assets/spaceArt.svg");
    /*QPixmap ship = pic.copy(QRect(374,360,106,90));

    _renderer->createShape(ship, QPointF(0,0), 1);
    _renderer->createShape(ship, QPointF(320,240), 1);*/
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

		QVariantMap m;
		m["name"] = QString("Entity_%1").arg(id);
		m["additionalInfo"] = "LoadedFrom=code;bla=blu";
        _entityManager.createComponent(id, metadata, m);
        _entityManager.getComponent(1, metadata);
        metadata->setAdditionalInfo(QString("LoadedFrom=%1").arg(frameNumber));
    }
}


void Game::keyPressEvent ( QKeyEvent * event )
{
    switch(event->key())
    {
    case Qt::Key_Left: keyLeft(); break;
    case Qt::Key_Right: keyRight(); break;
    }
}


void Game::keyLeft()
{

}


void Game::keyRight()
{

}
