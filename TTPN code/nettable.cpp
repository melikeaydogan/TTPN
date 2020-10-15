#include <QtGui>
#include <QApplication>  
#include <QTableWidget>  
#include <QHBoxLayout> 
#include "nettable.h"
#include <QHeaderView>

NetTable::NetTable(QWidget *parent)
     : QTableWidget(parent)
{
     setWindowTitle("QTableWidget");  

     setRowCount(6);  
     setColumnCount(2);  

     //verticalHeader()->resizeSection(i, rowHeight);
     verticalHeader()->resizeSection(0,18);
     verticalHeader()->resizeSection(1,18);
     verticalHeader()->resizeSection(2,18);
     verticalHeader()->resizeSection(3,18);
     verticalHeader()->resizeSection(4,18);
     verticalHeader()->resizeSection(5,18);

     setItem(0,0, new QTableWidgetItem("# of Initial Place"));
     setItem(1,0, new QTableWidgetItem("# of Final Place"));
     setItem(2,0, new QTableWidgetItem("# of Ordinary Place"));
     setItem(3,0, new QTableWidgetItem("# of Transition"));
     setItem(4,0, new QTableWidgetItem("# of Arc"));
     setItem(5,0, new QTableWidgetItem("# of All Place"));

     n1=0;
     n2=0;
     n3=0;
     n4=0;
     n5=0;
     n=0; 

     horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}



void NetTable::setItemNumber1(int num1)
{
     n1 = num1;
     QTableWidgetItem *item = new QTableWidgetItem(QString::number(n1));    
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(0, 1, item);         
}


void NetTable::setItemNumber2(int num2)
{
     n2 = num2;
     QTableWidgetItem *item = new QTableWidgetItem(QString::number(n2));
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(1, 1, item);         
}


void NetTable::setItemNumber3(int num3)
{
     n3 = num3;
     QTableWidgetItem *item = new QTableWidgetItem(QString::number(n3));
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(2, 1, item);         
}


void NetTable::setItemNumber4(int num4)
{
     n4 = num4;
     QTableWidgetItem *item = new QTableWidgetItem(QString::number(n4));
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(3, 1, item);         
}


void NetTable::setItemNumber5(int num5)
{
     n5 = num5;
     QTableWidgetItem *item = new QTableWidgetItem(QString::number(n5));
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(4, 1, item);         
}


void NetTable::setItemNumber6(int num)
{
     n = num;
     QTableWidgetItem *item = new QTableWidgetItem(QString::number(n));
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(5, 1, item);         
}



