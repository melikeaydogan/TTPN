    #ifndef SFDFILEDIALOG_HP
    #define SFDFILEDIALOG_HP
     
    #include <QFileDialog>
     
    // SelectFileDirDialog
    class SFDFileDialog : public QFileDialog
    {
        Q_OBJECT
    public:
        SFDFileDialog(QWidget * parent, Qt::WindowFlags flags );
        SFDFileDialog(QWidget * parent = 0, const QString & caption = QString(),
                      const QString & directory = QString(),
                      const QString & filter = QString() );
        ~SFDFileDialog();

    public slots:
        void accept ();
    };
     
    #endif // SFDFILEDIALOG_HP


