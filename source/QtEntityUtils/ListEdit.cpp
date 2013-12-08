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

#include <QtEntityUtils/ListEdit>
#include <QPushButton>
#include <QHBoxLayout>

namespace QtEntityUtils
{
     
    ListEdit::ListEdit(QWidget *parent)
        : QWidget(parent)
    {
        /*QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setMargin(0);
        layout->setSpacing(0);
        QPushButton* b1 = new QPushButton(this);
        b1->setText("+");
        b1->setMaximumWidth(20);
        layout->addWidget(b1);
        QPushButton* b2 = new QPushButton(this);
        b2->setText("-");
        b2->setMaximumWidth(20);
        layout->addWidget(b2);
        layout->addStretch();*/
    }

}
