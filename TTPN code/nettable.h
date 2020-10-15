#include <QApplication>  
#include <QTableWidget>  
#include <QHBoxLayout>  


class NetTable : public QTableWidget
{   
    Q_OBJECT

    public:
        NetTable(QWidget *parent = 0);

    public slots:
        void setItemNumber1(int num1);
        void setItemNumber2(int num2);
        void setItemNumber3(int num3);
        void setItemNumber4(int num4);
        void setItemNumber5(int num5);
        void setItemNumber6(int num);

        //void haha(int r, int c);

    private:
        int n1, n2, n3, n4, n5, n;        
};

 
