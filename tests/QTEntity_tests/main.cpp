#include <QtTest/QtTest>

#include "test_entitymanager.h"
#include "test_entitysystem.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

    { EntitySystemTest t; if(0 != QTest::qExec(&t, argc, argv)) return 1; }
    { EntityManagerTest t; if(0 != QTest::qExec(&t, argc, argv)) return 1; }

    return 0;
}
