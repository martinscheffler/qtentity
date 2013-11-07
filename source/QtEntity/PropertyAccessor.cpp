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

#include <QtEntity/PropertyAccessor>

namespace QtEntity
{
    PropertyAccessor::PropertyAccessor(QObject* parent, const QString& name, int variantType, Getter getter, Setter setter, const QVariantMap& attributes)
        : QObject(parent)
        , _name(name)
        , _variantType(variantType)
        , _getter(getter)
        , _setter(setter)
        , _attributes(attributes)
    {
    }


    QString PropertyAccessor::name() const 
    { 
        return _name; 
    }


    QVariant PropertyAccessor::read(EntityId id) const
    { 
        assert(_getter); 
        return _getter(id); 
    }


    bool PropertyAccessor::write(EntityId id, const QVariant& val) const
    {
        if(!_setter) 
        {
            return false; 
        }
        _setter(id, val); 
        return true; 
    }

    const QVariantMap& PropertyAccessor::attributes() const
    { 
        return _attributes; 
    }
        

    bool PropertyAccessor::hasGetter() const 
    { 
        return _getter != nullptr; 
    }
        

    bool PropertyAccessor::hasSetter() const 
    {
        return _setter != nullptr; 
    }


    int PropertyAccessor::variantType() const 
    {
        return _variantType; 
    }
}
