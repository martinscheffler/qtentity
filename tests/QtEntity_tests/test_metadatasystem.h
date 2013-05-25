#include <QtTest/QtTest>
#include <QtCore/QObject>

#include <QtEntity/MetaDataSystem>

using namespace qte;

class MetaDataSystemTest: public QObject
{
    Q_OBJECT
private slots:

    void entityAddedIsCalled()
    {

        MetaDataSystem ls;
        bool success = false;
        QObject::connect(&ls, &MetaDataSystem::entityAdded, [&success] (EntityId id, QString name, QString info) {
            success = true;
        });
        ls.createComponent(1);
        QVERIFY(success);

        MetaData* entry;
        ls.getComponent(1, entry);
        QVERIFY(entry != nullptr);
    }

    void entityRemovedIsCalled()
    {
        MetaDataSystem ls;
        bool success = false;
        QObject::connect(&ls, &MetaDataSystem::entityRemoved, [&success] (EntityId id) {
            success = true;
        });
        ls.createComponent(1);
        ls.destructComponent(1);
        QVERIFY(success);

    }

};
