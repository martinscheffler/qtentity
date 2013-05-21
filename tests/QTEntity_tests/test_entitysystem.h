#include <QtTest/QtTest>
#include <QtCore/QObject>

#include <QTEntity/EntitySystem>

class EntitySystemTest: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void myFirstTest()
    { QVERIFY(1 == 1); }

    void mySecondTest()
    { QVERIFY(1 != 2); }

};
