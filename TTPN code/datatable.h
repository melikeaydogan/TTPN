#include <QApplication>
#include <QTableWidget>  
#include <QHBoxLayout>  


class DataTable : public QTableWidget
{   
    Q_OBJECT

    public:
        DataTable(QWidget *parent = 0);
        void setRow0(QString path);

    public slots:
        void setRow1(int pnum);
        void setRow2(int cnum);
        void setRow3(double *Xrange);
        void setRow4(double *Yrange);
        void setRow5(double *Zrange);
        void clearTable();

    //private:
        //int t;
};

 
