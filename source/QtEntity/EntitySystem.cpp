#include <QtEntity/EntitySystem>
#include <unordered_map>

namespace QtEntity
{

    std::unordered_map<std::string, ClassTypeId> s_classTypeIds;
    std::unordered_map<ClassTypeId, std::string> s_classNames;

    ClassTypeId ComponentRegistry::registerComponent(const QString& classname)
    {
        Q_ASSERT(s_classTypeIds.find(classname.toStdString()) == s_classTypeIds.end());
        ClassTypeId s = (ClassTypeId) s_classTypeIds.size();
        s_classTypeIds[classname.toStdString()] = s;
        s_classNames[s] = classname.toStdString();
        return s;
    }


    ClassTypeId ComponentRegistry::classTypeId(const QString& classname)
    {
        auto i = s_classTypeIds.find(classname.toStdString());
        if(i == s_classTypeIds.end()) return -1;
        return i->second;
    }


    QString ComponentRegistry::className(ClassTypeId typeId)
    {
        auto i = s_classNames.find(typeId);
        if(i == s_classNames.end()) return "<Class not registered>";
        return QString::fromStdString(i->second);
    }


    EntitySystem::EntitySystem()
        : _entityManager(nullptr)
    {
    }


    EntitySystem::~EntitySystem()
    {
    }


    QString EntitySystem::componentName() const
    { 
        return ComponentRegistry::className(componentType()); 
    }


    int EntitySystem::propertyCount() const 
    { 
        return _properties.size(); 
    }
    

    PropertyAccessor EntitySystem::property(int idx) 
    { 
        return _properties.at(idx); 
    }


    const PropertyAccessor EntitySystem::property(int idx) const 
    { 
        return _properties.at(idx); 
    }


    void EntitySystem::addProperty(const PropertyAccessor& a) 
    { 
        _properties.push_back(a); 
    }

}
