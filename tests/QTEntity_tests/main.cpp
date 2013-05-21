#include <QtTest/QtTest>

#include "test_entitymanager.h"
#include "test_entitysystem.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    EntitySystemTest tc;
    if(0 != QTest::qExec(&tc, argc, argv)) return 1;

    EntityManagerTest mc;
    if(0 != QTest::qExec(&mc, argc, argv)) return 1;
    return 0;
}
