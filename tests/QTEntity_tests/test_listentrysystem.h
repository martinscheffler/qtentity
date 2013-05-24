#include <QtTest/QtTest>
#include <QtCore/QObject>

#include <QTEntity/ListEntrySystem>

using namespace qte;

class ListEntrySystemTest: public QObject
{
    Q_OBJECT
private slots:

    void listEntryAddedIsCalled()
    {

        ListEntrySystem ls;
        bool success = false;
        QObject::connect(&ls, &ListEntrySystem::listEntryAdded, [&success] (EntityId id, QString name) {
            success = true;
        });
        ls.createComponent(1);
        QVERIFY(success);

        ListEntry* entry;
        ls.getComponent(1, entry);
        QVERIFY(entry != nullptr);
    }

    void listEntryRemovedIsCalled()
    {
        ListEntrySystem ls;
        bool success = false;
        QObject::connect(&ls, &ListEntrySystem::listEntryRemoved, [&success] (EntityId id, QString name) {
            success = true;
        });
        ls.createComponent(1);
        ls.destructComponent(1);
        QVERIFY(success);

    }

};
