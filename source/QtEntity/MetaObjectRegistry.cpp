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

#include <QtEntity/MetaObjectRegistry>

#include <QMutex>
#include <QMutexLocker>
#include <unordered_map>

namespace QtEntity
{
    QMutex s_metaObjectRegistryMutex;
    std::unordered_map<std::string,const QMetaObject*> s_metaObjects;
	

    void registerMetaObject(const QMetaObject& mo)
    {
        QMutexLocker m(&s_metaObjectRegistryMutex);
        s_metaObjects[mo.className()] = &mo;
    }


    const QMetaObject* metaObjectByClassName(const QString& classname)
    {
        QMutexLocker m(&s_metaObjectRegistryMutex);
        auto i = s_metaObjects.find(classname.toStdString());
        return (i == s_metaObjects.end()) ? nullptr : i->second;
    }

}
