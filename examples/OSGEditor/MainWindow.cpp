#include "MainWindow"

#include "Game"
#include "ActorSystem"
#include <QtEntity/DataTypes>
#include <QtEntityUtils/EntityEditor>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , _game(new Game())
    , _gameThread(new QThread(this))
{
    setupUi(this);
    QtEntity::registerMetaTypes();

    _rendererPos->setLayout(new QHBoxLayout());

    // connect signals of meta data system to entity list
    ActorSystem* as = static_cast<ActorSystem*>(_game->entityManager().system(qMetaTypeId<Actor>()));
    connect(as, &ActorSystem::entityAdded,   this, &MainWindow::entityAdded);
    connect(as, &ActorSystem::entityRemoved, this, &MainWindow::entityRemoved);
    connect(as, &ActorSystem::entityChanged, this, &MainWindow::entityChanged);

    connect(_entities, &QTableWidget::itemSelectionChanged, this, &MainWindow::entitySelectionChanged);
    connect(_addActorButton, &QPushButton::clicked, this, &MainWindow::addActor);

    QtEntityUtils::EntityEditor* editor = new QtEntityUtils::EntityEditor();
    connect(this, &MainWindow::selectedEntityChanged, editor, &QtEntityUtils::EntityEditor::displayEntity);
    connect(editor, &QtEntityUtils::EntityEditor::entityDataChanged, this, &MainWindow::changeEntityData);
    centralWidget()->layout()->addWidget(editor);

    adjustSize();

    setFocusPolicy(Qt::StrongFocus);

    _game->moveToThread(_gameThread);
    connect(_gameThread, &QThread::started, _game, &Game::run);
    _gameThread->start();

}


MainWindow::~MainWindow()
{
    delete _game;
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    QMetaObject::invokeMethod(_game, "end");
    QApplication::quit();
}


int frameNumber = 0;
void MainWindow::stepGame()
{
    ++frameNumber;
    _game->step(frameNumber, frameNumber * 60, 60);
}


void MainWindow::addActor()
{
    QtEntity::EntityId id = _game->entityManager().createEntityId();
    QVariantMap m;
    m["name"] = QString("actor_%1").arg(id);
    _game->entityManager().createComponent<Actor>(id, m);
}


// insert entry into entity list
void MainWindow::entityAdded(QtEntity::EntityId id, QString name)
{
    _entities->setSortingEnabled(false);

    int row = _entities->rowCount();
    _entities->insertRow(row);
    auto item = new QTableWidgetItem(QString("%1").arg(id));
    item->setData(Qt::UserRole, id);
    _entities->setItem(row, 0, item);
    _entities->setItem(row, 1, new QTableWidgetItem(name));

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
        emit selectedEntityChanged(0, QVariantMap(), QVariantMap(), QStringList());
    }
    else
    {
        QtEntity::EntityId selected = items.front()->data(Qt::UserRole).toUInt();
        QVariantMap props, attributes;
        QStringList available;
        QtEntityUtils::EntityEditor::fetchEntityData(_game->entityManager(), selected, props, attributes, available);
        emit selectedEntityChanged(selected, props, attributes, available);
    }
}


void MainWindow::changeEntityData(QtEntity::EntityId id, const QVariantMap& values)
{
    QtEntityUtils::EntityEditor::applyEntityData(_game->entityManager(), id, values);

    //re-send changed component properties to editor
    entitySelectionChanged();
}


// update entry in entity list
void MainWindow::entityChanged(QtEntity::EntityId id, QString name)
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
            break;
        }
    }
    _entities->setSortingEnabled(true);
    qCritical() << "could not update entity in entity list, not found: " << id;
}
