#include "MainWindow"

#include "Game"
#include "Renderer"
#include <QtEntity/DataTypes>
#include <QtEntity/MetaDataSystem>

MainWindow::MainWindow()
{
    setupUi(this);
    resize(800, 600);

    QtEntity::registerMetaTypes();

    _game = new Game();

    QtEntity::MetaDataSystem* ms;
    _game->entityManager().getEntitySystem(ms);
    connect(ms, &QtEntity::MetaDataSystem::entityAdded, this, &MainWindow::entityAdded);
    connect(ms, &QtEntity::MetaDataSystem::entityRemoved, this, &MainWindow::entityRemoved);
    connect(ms, &QtEntity::MetaDataSystem::entityChanged, this, &MainWindow::entityChanged);

    // game can run in separate thread, but QGraphicsView can't
    /*_game->moveToThread(&_gameThread);
    connect(&_gameThread, &QThread::started, _game, &Game::run);
    _gameThread.start();*/

    QHBoxLayout* layout = new QHBoxLayout();
    _rendererPos->setLayout(layout);
    _renderer = new Renderer(_rendererPos);
    layout->addWidget(_renderer);

    _renderer->createShape(QPixmap(":/assets/space.jpg"), QPointF(0,0), -10);

    QPixmap pic(":/assets/spaceArt.svg");
    QPixmap ship = pic.copy(QRect(374,360,106,90));

    _renderer->createShape(ship, QPointF(0,0), 1);
    _renderer->createShape(ship, QPointF(320,240), 1);

}


void MainWindow::entityAdded(QtEntity::EntityId id, QString name, QString additionalInfo)
{
    _entities->setSortingEnabled(false);

    int row = _entities->rowCount();
    _entities->insertRow(row);
    auto item = new QTableWidgetItem(QString("%1").arg(id));
    item->setData(Qt::UserRole, id);
    _entities->setItem(row, 0, item);
    _entities->setItem(row, 1, new QTableWidgetItem(name));
    QStringList additional = additionalInfo.split(";");
    foreach(QString val, additional)
    {
        QStringList pair = val.split("=");
        if(pair.count() < 2) continue;
        for(int i = 0; i < _entities->columnCount(); ++i)
        {
            QString colname = _entities->horizontalHeaderItem(i)->text();
            if(colname == pair.first())
            {
                _entities->setItem(row, i, new QTableWidgetItem(pair.last()));
            }
        }
    }
    _entities->setSortingEnabled(true);
}


void MainWindow::entityRemoved(QtEntity::EntityId id)
{
    for(int i = 0; i < _entities->rowCount(); ++i)
    {
        QTableWidgetItem* item = _entities->item(i, 0);
        if(item && item->data(Qt::UserRole).toInt() == id)
        {
            _entities->removeRow(i);
            return;
        }
    }
    qCritical() << "could not remove entity from entity list, not found: " << id;
}


void MainWindow::entityChanged(QtEntity::EntityId id, QString name, QString additionalInfo)
{
    _entities->setSortingEnabled(false);

    for(int i = 0; i < _entities->rowCount(); ++i)
    {
        QTableWidgetItem* item = _entities->item(i, 0);
        if(item && item->data(Qt::UserRole).toInt() == id)
        {
            _entities->item(i, 1)->setText(name);

            // delete all additional entries before recreating them
            for(int j = 2; j < _entities->columnCount(); ++j)
            {
                QTableWidgetItem* item = _entities->item(i, j);
                if(item)
                {
                    delete _entities->takeItem(i, j);
                }
            }

            QStringList additional = additionalInfo.split(";");
            foreach(QString val, additional)
            {
                QStringList pair = val.split("=");
                if(pair.count() < 2) continue;
                for(int j = 0; j < _entities->columnCount(); ++j)
                {
                    QString colname = _entities->horizontalHeaderItem(j)->text();
                    if(colname == pair.first())
                    {
                        _entities->setItem(i, j, new QTableWidgetItem(pair.last()));
                    }
                }
            }
            break;
        }
    }
    _entities->setSortingEnabled(true);
    qCritical() << "could not update entity in entity list, not found: " << id;
}
