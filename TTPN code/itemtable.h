#include <QApplication>
#include <QTableWidget>  
#include <QHBoxLayout>  


class ItemTable : public QTableWidget
{   
    Q_OBJECT

    public:
        ItemTable(QWidget *parent = 0);

    public slots:
        void setTableItem(int type);
        void setIndex(int index);
        void setLabel(QString label);
        void setIncomming(int *group);
        void setOutgoing(int *group);
        void setConnection(int *group);
        void setCondition(char *cond);
        void clearTable();

    private:
        int t;
};

 
