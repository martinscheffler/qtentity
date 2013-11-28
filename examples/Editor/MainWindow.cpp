#include "MainWindow"

#include "Game"
#include "Renderer"
#include <QtEntity/DataTypes>
#include "MetaDataSystem"
#include <QtEntityUtils/EntityEditor>

MainWindow::MainWindow()
    : _timer(nullptr)
{
    setupUi(this);

    QtEntity::registerMetaTypes();

    _rendererPos->setLayout(new QHBoxLayout());
    _renderer = new Renderer(_rendererPos);
    _rendererPos->layout()->addWidget(_renderer);


    QtEntityUtils::EntityEditor* editor = new QtEntityUtils::EntityEditor();
    connect(this, &MainWindow::selectedEntityChanged, editor, &QtEntityUtils::EntityEditor::displayEntity);
    connect(editor, &QtEntityUtils::EntityEditor::entityDataChanged, this, &MainWindow::changeEntityData);
    centralWidget()->layout()->addWidget(editor);

    _game = new Game(_renderer);

    // make main window get input events when game area is focused
    _renderer->installRendererEventFilter(this);

    // connect signals of meta data system to entity list
    MetaDataSystem* ms = static_cast<MetaDataSystem*>(_game->entityManager().system(MetaData::classTypeId()));
    connect(ms, &MetaDataSystem::entityAdded,   this, &MainWindow::entityAdded);
    connect(ms, &MetaDataSystem::entityRemoved, this, &MainWindow::entityRemoved);
    connect(ms, &MetaDataSystem::entityChanged, this, &MainWindow::entityChanged);

    connect(_entities, &QTableWidget::itemSelectionChanged, this, &MainWindow::entitySelectionChanged);

    _game->init();
    // setup game tick
#ifdef RUN_GAME_IN_THREAD
    _game->moveToThread(&_gameThread);
    connect(&_gameThread, &QThread::started, _game, &Game::run);
    _gameThread.start();
#else
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &MainWindow::stepGame);
    _timer->start((int)(1000.0 / 60.0));
#endif

    adjustSize();

    setFocusPolicy(Qt::StrongFocus);

}


void MainWindow::keyPressEvent ( QKeyEvent * event )
{
    _game->keyPressEvent(event);
}


void MainWindow::keyReleaseEvent ( QKeyEvent * event )
{
    _game->keyReleaseEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        keyPressEvent(static_cast<QKeyEvent*>(event));
        return true;
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        keyReleaseEvent(static_cast<QKeyEvent*>(event));
        return true;
    }
    return false;
}

MainWindow::~MainWindow()
{
}


int frameNumber = 0;
void MainWindow::stepGame()
{
    ++frameNumber;
    _game->step(frameNumber, frameNumber * 60, 60);
}


// insert entry into entity list
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
                auto item = new QTableWidgetItem(pair.last());
                item->setData(Qt::UserRole, id);
                _entities->setItem(row, i, item);
            }
        }
    }
    _entities->setSortingEnabled(true);
}


// remove entry from entity list
void MainWindow::entityRemoved(QtEntity::EntityId id)
{
    for(int i = 0; i < _entities->rowCount(); ++i)
    {
        QTableWidgetItem* item = _entities->item(i, 0);
        if(item && item->data(Qt::UserRole).toUInt() == id)
        {
            _entities->removeRow(i);
            return;
        }
    }
    qCritical() << "could not remove entity from entity list, not found: " << id;
}


void MainWindow::entitySelectionChanged()
{
    auto items = _entities->selectedItems();
    if(items.empty())
    {
        QVariantMap empty;
        emit selectedEntityChanged(0, empty, empty);
    }
    else
    {
        QtEntity::EntityId selected = items.front()->data(Qt::UserRole).toUInt();
        QVariantMap props, attributes;
        QtEntityUtils::EntityEditor::fetchEntityData(_game->entityManager(), selected, props, attributes);
        emit selectedEntityChanged(selected, props, attributes);
    }
}


void MainWindow::changeEntityData(QtEntity::EntityId id, const QVariantMap& values)
{
     QtEntityUtils::EntityEditor::applyEntityData(_game->entityManager(), id, values);
}


// update entry in entity list
void MainWindow::entityChanged(QtEntity::EntityId id, QString name, QString additionalInfo)
{
    _entities->setSortingEnabled(false);

    for(int i = 0; i < _entities->rowCount(); ++i)
    {
        QTableWidgetItem* item = _entities->item(i, 0);
        if(item && item->data(Qt::UserRole).toUInt() == id)
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
