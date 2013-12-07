#include "Game"

#include <QCoreApplication>
#include <QTime>
#include <QThread>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include "ActorSystem"

Game::Game(QObject* parent)
    : QObject(parent)
    , _leftpressed(false)
    , _rightpressed(false)
    , _spacepressed(false)    
    , _viewer(new osgViewer::Viewer())
    , _isRunning(false)
{
    osg::Group* rootNode = new osg::Group();

    _viewer->setSceneData(rootNode);
    _actorSystem = new ActorSystem(&_entityManager, rootNode);

    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    //ds->setNumMultiSamples(4);

    _viewer->getCamera()->setClearColor(osg::Vec4(0.2, 0.2, 0.2, 1.0));
    _viewer->addEventHandler(new osgViewer::StatsHandler);
    _viewer->setCameraManipulator(new osgGA::TrackballManipulator);
    _viewer->setUpViewInWindow(100,100,800,600);
    _viewer->realize();

}



void Game::run()
{
    _isRunning = true;
    int timeStep = 10;
    int frameNumber = 0;

    QTime startTime;
    while(_isRunning)
    {
        QTime frameTime;
        step(frameNumber++, startTime.elapsed(), timeStep);
        QCoreApplication::processEvents();
        timeStep = frameTime.elapsed();
    }
}

void Game::end()
{
    _isRunning = false;
}


void Game::step(int frameNumber, int totalTime, int delta)
{
    _viewer->frame();
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

