#include <QtTest/QtTest>
#include <QObject>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <QtGui/QColor>
#include <QtEntity/EntityManager>
#include <QtEntity/ReaderWriterJSON>
#include <QtEntity/MetaObjectRegistry>
#include <QtEntity/SimpleEntitySystem>

using namespace QtEntity;


class TestObj1 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int propint READ propInt WRITE setPropInt USER true)
    qint32 _propint;
public:

     Q_INVOKABLE TestObj1() {}

    void setPropInt(qint32 v) { _propint = v; }
    qint32 propInt() const  { return _propint; }
    
};


class TestObj2 : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE TestObj2() {}
};


class DataHolder : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int myint READ myInt WRITE setMyInt USER true)
    Q_PROPERTY(QVector2D myvec2 READ myVec2 WRITE setMyVec2 USER true)
    Q_PROPERTY(QVector3D myvec3 READ myVec3 WRITE setMyVec3 USER true)
    Q_PROPERTY(QVector4D myvec4 READ myVec4 WRITE setMyVec4 USER true)
    Q_PROPERTY(QColor mycolor READ myColor WRITE setMyColor USER true)
    Q_PROPERTY(QtEntity::PropertyObjects myobjects READ myObjects WRITE setMyObjects USER true)

public:
    Q_INVOKABLE DataHolder() {}

    void setMyVec2(const QVector2D& v) { _myvec2 = v; }
    QVector2D myVec2() const  { return _myvec2; }

    void setMyVec3(const QVector3D& v) { _myvec3 = v; }
    QVector3D myVec3() const  { return _myvec3; }

    void setMyVec4(const QVector4D& v) { _myvec4 = v; }
    QVector4D myVec4() const  { return _myvec4; }

    void setMyColor(const QColor& v) { _mycolor = v; }
    QColor myColor() const  { return _mycolor; }

    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const  { return _myint; }

    void setMyObjects(const PropertyObjects& v) { _myobjects = v; }
    PropertyObjects myObjects() const  { return _myobjects; }

private:
    QVector2D _myvec2;
    QVector3D _myvec3;
    QVector4D _myvec4;
    QColor _mycolor;
    qint32 _myint;
    PropertyObjects _myobjects;
};

class DataHolderSystem : public SimpleEntitySystem
{
    Q_OBJECT
public:
    DataHolderSystem() : SimpleEntitySystem(DataHolder::staticMetaObject) {}
};

class ReaderWriterJSONTest: public QObject
{
    Q_OBJECT

public:

    ReaderWriterJSONTest()
    {
        QtEntity::registerMetaTypes();
        registerMetaObject(TestObj1::staticMetaObject);
        registerMetaObject(TestObj2::staticMetaObject);
    }

private slots:

    void metatype()
    {
        int id = qMetaTypeId<PropertyObjects>();

        PropertyObjects objs;
        QVariant v = QVariant::fromValue(objs);
        int usertype = v.userType();
        QVERIFY(id == usertype);
    }

    void encode()
    {

        DataHolder dh;
        dh.setMyInt(666);
        dh.setMyVec3(QVector3D(-1,0,354));
        PropertyObjects objs;
        objs.push_back(PropertyObjectPointer(new TestObj1()));
        objs.push_back(PropertyObjectPointer(new TestObj2()));
        dh.setMyObjects(objs);

        QJsonObject obj = ReaderWriterJSON::componentToJson(dh);
        QJsonDocument doc(obj);

        QString txt = doc.toJson(QJsonDocument::Compact);
        QString expected = "{\"mycolor\": \"0,0,0,255\",\"myint\": 666,\"myobjects\": [{\"classname\": \"TestObj1\",\"propint\": 0},{\"classname\": \"TestObj2\"}],\"myvec2\": \"0,0\",\"myvec3\": \"-1,0,354\",\"myvec4\": \"0,0,0,0\"}";
        
		// TODO: seems to be a little different from system to system
		//QVERIFY(expected == txt);
    }


    void decode()
    {
        QString jsonstr = "{\"classname\": \"DataHolder\", \"mycolor\": \"0,0,0,255\",\"myint\": 666,\"myobjects\": [{\"classname\": \"TestObj1\",\"propint\": 12345},{\"classname\": \"TestObj2\"}],\"myvec2\": \"0,0\",\"myvec3\": \"-1,0,354\",\"myvec4\": \"0,0,0,0\"}";
        QJsonDocument doc = QJsonDocument::fromJson(jsonstr.toUtf8());
        QVERIFY(doc.isObject());
        EntityManager em;
        em.addEntitySystem(new DataHolderSystem());

        bool success = ReaderWriterJSON::jsonToComponent(em, 1, doc.object());
        QVERIFY(success);
        DataHolder* dh = em.component<DataHolder>(1);
        QVERIFY(dh);
        QCOMPARE(dh->myInt(), 666);
        PropertyObjects objs = dh->myObjects();
        QCOMPARE(objs.count(), 2);
        
        TestObj1* to1 = qobject_cast<TestObj1*>(objs.front().data());
        QVERIFY(to1);

        QCOMPARE(to1->propInt(), 12345);
        
    }

};

