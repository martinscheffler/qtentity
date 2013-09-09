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

#include <QtEntityUtils/FileEdit>

#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>
#include <QFocusEvent>

namespace QtEntityUtils
{
   
    
    FileEdit::FileEdit(QWidget *parent)
        : QWidget(parent)
    {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setMargin(0);
        layout->setSpacing(0);
        theLineEdit = new QLineEdit(this);
        theLineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
        QToolButton *button = new QToolButton(this);
        button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
        button->setText(QLatin1String("..."));
        layout->addWidget(theLineEdit);
        layout->addWidget(button);
        setFocusProxy(theLineEdit);
        setFocusPolicy(Qt::StrongFocus);
        setAttribute(Qt::WA_InputMethodEnabled);
        connect(theLineEdit, SIGNAL(textEdited(const QString &)),
                    this, SIGNAL(filePathChanged(const QString &)));
        connect(button, SIGNAL(clicked()),
                    this, SLOT(buttonClicked()));
    }

    void FileEdit::buttonClicked()
    {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Choose a file"), theLineEdit->text(), theFilter);
        if (filePath.isNull())
            return;
        theLineEdit->setText(filePath);
        emit filePathChanged(filePath);
    }

    void FileEdit::focusInEvent(QFocusEvent *e)
    {
        theLineEdit->event(e);
        if (e->reason() == Qt::TabFocusReason || e->reason() == Qt::BacktabFocusReason) {
            theLineEdit->selectAll();
        }
        QWidget::focusInEvent(e);
    }

    void FileEdit::focusOutEvent(QFocusEvent *e)
    {
        theLineEdit->event(e);
        QWidget::focusOutEvent(e);
    }

    void FileEdit::keyPressEvent(QKeyEvent *e)
    {
        theLineEdit->event(e);
    }

    void FileEdit::keyReleaseEvent(QKeyEvent *e)
    {
        theLineEdit->event(e);
    }
}
