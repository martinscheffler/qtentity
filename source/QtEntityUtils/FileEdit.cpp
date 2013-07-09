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
