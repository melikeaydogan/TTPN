#include <QtGui>
#include <QApplication>  
#include <QTableWidget>  
#include <QHBoxLayout> 
#include <QHeaderView>
#include "datatable.h"

DataTable::DataTable(QWidget *parent)
     : QTableWidget(parent)
{
     setRowCount(6);
     setColumnCount(2);  

     verticalHeader()->resizeSection(0,18);
     verticalHeader()->resizeSection(1,18);
     verticalHeader()->resizeSection(2,18);
     verticalHeader()->resizeSection(3,18);
     verticalHeader()->resizeSection(4,18);
     verticalHeader()->resizeSection(5,18);

     setItem(0,0, new QTableWidgetItem("Current Data"));
     //setItem(1,0, new QTableWidgetItem("Memory"));
     setItem(1,0, new QTableWidgetItem("Points Number"));
     setItem(2,0, new QTableWidgetItem("Cells Number"));
     setItem(3,0, new QTableWidgetItem("X Range"));
     setItem(4,0, new QTableWidgetItem("Y Range"));
     setItem(5,0, new QTableWidgetItem("Z Range"));
}



void DataTable::setRow0(QString path)
{
     QTableWidgetItem *item = new QTableWidgetItem(path);    
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(0, 1, item);      
}



void DataTable::setRow1(int pnum)
{
     QTableWidgetItem *item = new QTableWidgetItem(QString::number(pnum));    
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(1, 1, item);         
}


void DataTable::setRow2(int cnum)
{
     QTableWidgetItem *item = new QTableWidgetItem(QString::number(cnum));    
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(2, 1, item);         
}


void DataTable::setRow3(double *Xrange)
{
     QString str1 = QString::number(Xrange[0]);  
     QString str2 = QString::number(Xrange[1]); 
     QString str = str1 + " ~ " + str2;   
     QTableWidgetItem *item = new QTableWidgetItem(str);   
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(3, 1, item);         
}


void DataTable::setRow4(double *Yrange)
{
     QString str1 = QString::number(Yrange[0]);  
     QString str2 = QString::number(Yrange[1]); 
     QString str = str1 + " ~ " + str2;   
     QTableWidgetItem *item = new QTableWidgetItem(str);   
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(4, 1, item);         
}


void DataTable::setRow5(double *Zrange)
{
     QString str1 = QString::number(Zrange[0]);  
     QString str2 = QString::number(Zrange[1]); 
     QString str = str1 + " ~ " + str2;   
     QTableWidgetItem *item = new QTableWidgetItem(str);   
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(5, 1, item);         
}


void DataTable::clearTable()
{
    setItem(0,0, new QTableWidgetItem(" "));
    setItem(1,0, new QTableWidgetItem(" "));
    setItem(2,0, new QTableWidgetItem(" "));
    setItem(3,0, new QTableWidgetItem(" "));
    setItem(4,0, new QTableWidgetItem(" "));
    setItem(5,0, new QTableWidgetItem(" "));

    setItem(0,1, new QTableWidgetItem(" "));
    setItem(1,1, new QTableWidgetItem(" "));
    setItem(2,1, new QTableWidgetItem(" "));
    setItem(3,1, new QTableWidgetItem(" "));
    setItem(4,1, new QTableWidgetItem(" "));
    setItem(5,1, new QTableWidgetItem(" "));
}

