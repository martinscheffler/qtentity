#include "MainWindow"

#include "Game"
#include "Renderer"
#include <QtEntity/DataTypes>
#include <QtEntity/EntityManager>
#include "ShapeSystem"
#include <QtEntityUtils/EntityEditor>
#include <QtEntityUtils/PrefabSystem>
#include <QDebug>

MainWindow::MainWindow()    
    : _selectedEntity(0)
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


    startTimer(20, Qt::PreciseTimer);

    ////////////////// entity editor ///////////////////////////
    QtEntityUtils::EntityEditor* editor = new QtEntityUtils::EntityEditor();
    connect(this, &MainWindow::selectedEntityChanged, editor, &QtEntityUtils::EntityEditor::displayEntity);
    connect(this, &MainWindow::selectedEntityChanged, this, &MainWindow::entityChanged);
    connect(editor, &QtEntityUtils::EntityEditor::entityDataChanged, this, &MainWindow::changeEntityData);
    _editorPos->setLayout(new QHBoxLayout);
    _editorPos->layout()->addWidget(editor);

    ////////////////// entity list ///////////////////////////
    // connect signals of meta data system to entity list

    connect(_game->shapeSystem(), &ShapeSystem::entityAdded,   this, &MainWindow::entityAdded);
    connect(_game->shapeSystem(), &ShapeSystem::entityRemoved, this, &MainWindow::entityRemoved);
    connect(_game->shapeSystem(), &ShapeSystem::entityNameChanged, this, &MainWindow::entityNameChanged);

    connect(_entities, &QTableWidget::itemSelectionChanged, this, &MainWindow::entitySelectionChanged);

    ////////////////// prefab menu ///////////////////////////
    connect(_game->prefabSystem(), &QtEntityUtils::PrefabSystem::prefabAdded, this, &MainWindow::prefabAdded);
    connect(_prefabs, &QListWidget::itemClicked, this, &MainWindow::prefabSelectionChanged);
    connect(_addInstance, &QPushButton::clicked, this, &MainWindow::addPrefabInstance);

    ////////////////// component buttons ///////////////////////////
    connect(_addComponentButton, &QPushButton::clicked, this, &MainWindow::addComponentButtonClicked);
    connect(_removeComponentButton, &QPushButton::clicked, this, &MainWindow::removeComponentButtonClicked);
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


void MainWindow::timerEvent(QTimerEvent * event)
{
    stepGame();
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


void MainWindow::addComponentButtonClicked()
{
    QMenu menu(this);
    foreach(QString component, _availableComponents)
    {
        QAction* action = menu.addAction(component);
        connect(action, &QAction::triggered, [this, component]() {
            this->addComponentToCurrent(component);
        });
    }

    //QAction* action = menu.addAction(QString("BLABLA"));
    //    connect(action, &QAction::triggered, [this, prototype, prop]() {
    //        this->addListItem(prototype, prop);
    //    });

    menu.exec(QCursor::pos());

}


void MainWindow::removeComponentButtonClicked()
{

}


void MainWindow::addComponentToCurrent(const QString& component)
{
    if(_selectedEntity == 0) return;
    QtEntity::EntitySystem* es = _game->entityManager()->system(component);
    if(es)
    {
        es->createComponent(_selectedEntity);
    }
    updateEditorWithCurrent();

}

int frameNumber = 0;
void MainWindow::stepGame()
{
    ++frameNumber;
    _game->step(frameNumber, frameNumber * 20, 0.02);
}


void MainWindow::prefabAdded(const QString& prefab)
{
    _prefabs->addItem(prefab);
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


void MainWindow::entityChanged(QtEntity::EntityId id, const QVariantMap& data,
                   const QVariantMap& attributes, const QStringList& availableComponents)
{
    Q_UNUSED(id)
    Q_UNUSED(data)
    Q_UNUSED(attributes)
    _availableComponents = availableComponents;
}


void MainWindow::updateEditorWithCurrent()
{
    if(_selectedEntity == 0)
    {
        emit selectedEntityChanged(0, QVariantMap(), QVariantMap(), QStringList());
    }
    else
    {
        QVariantMap props, attributes;
        QStringList availableComponents;
        QtEntityUtils::EntityEditor::fetchEntityData(*_game->entityManager(), _selectedEntity, props, attributes, availableComponents);
        emit selectedEntityChanged(_selectedEntity, props, attributes, availableComponents);
    }
}


void MainWindow::entitySelectionChanged()
{
    auto items = _entities->selectedItems();
    if(items.empty())
    {
        _selectedEntity = 0;
    }
    else
    {
        _selectedEntity = items.front()->data(Qt::UserRole).toUInt();
    }
    updateEditorWithCurrent();
}


void MainWindow::prefabSelectionChanged(QListWidgetItem * item)
{
    _addInstance->setEnabled(true);
    _selectedEntity = 0;
    QString selected = item->text();
    auto prefab = _game->prefabSystem()->prefab(selected);
    QVariantMap attributes;
    emit selectedEntityChanged(0, prefab->components(), QVariantMap(), QStringList());
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
        QtEntityUtils::EntityEditor::applyEntityData(*_game->entityManager(), id, values);
    }
}


// update entry in entity list
void MainWindow::entityNameChanged(QtEntity::EntityId id, QString name)
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
