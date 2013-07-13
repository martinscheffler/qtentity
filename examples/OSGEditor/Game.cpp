#include "Game"

#include <QCoreApplication>
#include <QTime>
#include <QThread>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include "ActorSystem"
#include <QVector3D>

Game::Game()
    : _leftpressed(false)
    , _rightpressed(false)
    , _spacepressed(false)    
    , _viewer(new osgViewer::Viewer())
    , _isRunning(false)
{
    osg::Group* rootNode = new osg::Group();

    _viewer->setSceneData(rootNode);
    _actorSystem = new ActorSystem(rootNode);
    _entityManager.addEntitySystem(_actorSystem);

    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowDecoration = false;
    traits->x = 100;
    traits->y = 100;
    traits->width = 800;
    traits->height = 600;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = 4;

    _viewer->getCamera()->setClearColor( osg::Vec4(0.2, 0.2, 0.2, 1.0) );

    _viewer->addEventHandler(new osgViewer::StatsHandler);
    _viewer->setCameraManipulator(new osgGA::TrackballManipulator);
    _viewer->setUpViewInWindow(100,100,800,600);
    _viewer->realize();

}


QGLWidget* Game::glwidget()
{
    return static_cast<osgQt::GraphicsWindowQt*>(_viewer->getCamera()->getGraphicsContext())->getGLWidget();
}


void Game::end()
{
    _isRunning = false;
}


void Game::step(int frameNumber, int totalTime, int delta)
{
    _viewer->frame();
    /*if(frameNumber % 100 == 0)
    {
        QVariantMap m;
        m["position"] = QVector3D(0, frameNumber / 50, 0);
        m["name"] = QString("actor_%1").arg(frameNumber / 100);
        _actorSystem->createComponent(frameNumber / 100 + 1, m);
    }*/
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

