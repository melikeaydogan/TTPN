#include <QApplication>
#include <QTableWidget>  
#include <QHBoxLayout>  


class TDtable : public QTableWidget
{   
    Q_OBJECT

    public:
        TDtable(QWidget *parent = 0);
        void SendChosenID(int *id);

   signals:
        void sendID(int *id);
};

 
