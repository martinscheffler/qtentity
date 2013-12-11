#include "MainWindow"

#include "Game"
#include "Renderer"
#include <QtEntity/DataTypes>
#include "MetaDataSystem"
#include <QtEntityUtils/EntityEditor>
#include <QtEntityUtils/PrefabSystem>
#include <QDebug>

MainWindow::MainWindow()
    : _timer(nullptr)
{
    setupUi(this);

    QtEntity::registerMetaTypes();

    ////////////////// game ///////////////////////////
    _rendererPos->setLayout(new QHBoxLayout());
    _renderer = new Renderer(_rendererPos);
    _rendererPos->layout()->addWidget(_renderer);
    _game = new Game(_renderer);

    // make main window get input events when game area is focused
    _renderer->installRendererEventFilter(this);


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

    ////////////////// entity editor ///////////////////////////
    QtEntityUtils::EntityEditor* editor = new QtEntityUtils::EntityEditor();
    connect(this, &MainWindow::selectedEntityChanged, editor, &QtEntityUtils::EntityEditor::displayEntity);
    connect(editor, &QtEntityUtils::EntityEditor::entityDataChanged, this, &MainWindow::changeEntityData);
    centralWidget()->layout()->addWidget(editor);

    ////////////////// entity list ///////////////////////////
    // connect signals of meta data system to entity list

    connect(_game->metaDataSystem(), &MetaDataSystem::entityAdded,   this, &MainWindow::entityAdded);
    connect(_game->metaDataSystem(), &MetaDataSystem::entityRemoved, this, &MainWindow::entityRemoved);
    connect(_game->metaDataSystem(), &MetaDataSystem::entityChanged, this, &MainWindow::entityChanged);

    connect(_entities, &QTableWidget::itemSelectionChanged, this, &MainWindow::entitySelectionChanged);

    ////////////////// prefab menu ///////////////////////////
    connect(_game->prefabSystem(), &QtEntityUtils::PrefabSystem::prefabAdded, this, &MainWindow::prefabAdded);
    connect(_prefabs, &QListWidget::itemClicked, this, &MainWindow::prefabSelectionChanged);
    connect(_addInstance, &QPushButton::clicked, this, &MainWindow::addPrefabInstance);
    adjustSize();
    setFocusPolicy(Qt::StrongFocus);

    // start the game!
    _game->init();
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


void MainWindow::prefabAdded(const QString& prefab)
{
    _prefabs->addItem(prefab);
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


void MainWindow::prefabSelectionChanged(QListWidgetItem * item)
{

    _addInstance->setEnabled(true);

    QString selected = item->text();
    auto prefab = _game->prefabSystem()->prefab(selected);
    QVariantMap attributes;
    emit selectedEntityChanged(0, prefab->components(), attributes);


}


void MainWindow::addPrefabInstance()
{
    auto items = _prefabs->selectedItems();
    if(!items.empty())
    {
        QString prefab = items.front()->text();
        _game->createPrefabInstance(prefab);
    }
}


void MainWindow::changeEntityData(QtEntity::EntityId id, const QVariantMap& values)
{
    if(id == 0) // edited prefab
    {
        auto items = _prefabs->selectedItems();
        if(!items.empty())
        {
            QString prefab = items.front()->text();
            for(QString key : values.keys())
            {
                _game->prefabSystem()->updateComponentInPrefab(prefab, key, values[key].toMap(), true);
            }

        }
    }
    else
    {
        QtEntityUtils::EntityEditor::applyEntityData(_game->entityManager(), id, values);
    }
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
