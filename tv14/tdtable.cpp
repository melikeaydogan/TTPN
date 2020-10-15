#include <QtGui>
#include <QApplication>  
#include <QTableWidget>  
#include <QHBoxLayout> 
#include "tdtable.h"

TDtable::TDtable(QWidget *parent)
     : QTableWidget(parent)
{
/*
     QStringList header;
     header<<"Place ID"<<"Token ID";
     setHorizontalHeaderLabels(header);
*/
     //horizontalHeader()->setResizeMode(QHeaderView::Stretch);
     //verticalHeader()->setResizeMode(QHeaderView::Stretch);
}


void TDtable::SendChosenID(int *id)
{
     emit sendID(id);    
}



