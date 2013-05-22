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

    void createTest()
    {
        qte::EntityManager em;
        TransformSystem* ts = new TransformSystem();
        em.addEntitySystem(ts);
        ts->createComponent(1);

        Transform* test;
        bool success = em.getComponent(1, test);
        QVERIFY(success);

    }


};
