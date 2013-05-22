#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtGui/QVector3D>
#include <QTEntity/EntityManager>

using namespace qte;

class Transform : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVector3D position READ position WRITE setPosition USER true)
public:
    Q_INVOKABLE Transform() {}
    void setPosition(const QVector3D& v) { _position = v; }
    QVector3D position() const  { return _position; }
private:
    QVector3D _position;
};


class TransformSystem : public qte::EntitySystem
{
    Q_OBJECT
public:
    TransformSystem()
        : qte::EntitySystem(Transform::staticMetaObject)
    {
    }

};



class EntitySystemTest: public QObject
{
    Q_OBJECT
private slots:

    void createAndFetch()
    {
        TransformSystem ts;
        QObject* c = ts.createComponent(1);
        QObject* c2 = ts.getComponent(1);
        QVERIFY(c == c2);
        QVERIFY(ts.hasComponent(1));

        QObject* c3 = ts.getComponent(2);
        QVERIFY(c3 == nullptr);
    }

    void destruct()
    {
        TransformSystem ts;
        QObject* c = ts.createComponent(1);
        QObject* c2 = ts.getComponent(1);
        QVERIFY(c == c2);
        QVERIFY(ts.hasComponent(1));
        ts.destructComponent(1);
        QVERIFY(!ts.hasComponent(1));
        QObject* c3 = ts.getComponent(1);
        QVERIFY(c3 == nullptr);
    }

};
