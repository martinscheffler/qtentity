/*
Copyright (c) 2013 Martin Scheffler
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QtEntity/EntitySystem>
#include <unordered_map>

namespace QtEntity
{
    typedef std::unordered_map<std::string, ClassTypeId> ClassTypeIds;
    typedef std::unordered_map<ClassTypeId, std::string> ClassNames;
    
    inline ClassTypeIds& classTypeIds() 
    {
        static ClassTypeIds ids;
        return ids;
    }

    inline ClassNames& classNames() 
    {
        static ClassNames names;
        return names;
    }

    ClassTypeId ComponentRegistry::registerComponent(const QString& classname)
    {
        int count = classTypeIds().size();
        Q_ASSERT(classTypeIds().find(classname.toStdString()) == classTypeIds().end());
        ClassTypeId s = (ClassTypeId) classTypeIds().size();
        classTypeIds()[classname.toStdString()] = s;
        classNames()[s] = classname.toStdString();
        return s;
    }


    ClassTypeId ComponentRegistry::classTypeId(const QString& classname)
    {
        auto i = classTypeIds().find(classname.toStdString());
        if(i == classTypeIds().end()) return -1;
        return i->second;
    }


    QString ComponentRegistry::className(ClassTypeId typeId)
    {
        auto i = classNames().find(typeId);
        if(i == classNames().end()) return "<Class not registered>";
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
