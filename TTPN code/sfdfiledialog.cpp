    #include "sfdfiledialog.h"
    #include <QListView>
    #include <QTreeView>
     
     
    SFDFileDialog::SFDFileDialog(QWidget *parent, const QString &caption, const QString &directory, const QString &filter)
        : QFileDialog(parent, caption, directory, filter)
    {
        // Try to select multiple files and directories at the same time in QFileDialog
        QListView *l = this->findChild<QListView*>("listView");
        if (l)
            l->setSelectionMode(QAbstractItemView::ExtendedSelection);
     
        QTreeView *t = this->findChild<QTreeView*>();
        if (t)
            t->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
     
     
    SFDFileDialog::SFDFileDialog(QWidget *parent, Qt::WindowFlags flags)
        : QFileDialog(parent,flags)
    {
    }
     
    SFDFileDialog::~SFDFileDialog()
    {
    }
     
     
    void SFDFileDialog::accept()
    {
        QStringList files = selectedFiles();
        if (files.isEmpty())
            return;
        emit filesSelected(files);
        QDialog::accept();
    }

