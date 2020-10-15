#include <QtGui>
#include <QApplication>  
#include <QTableWidget>  
#include <QHBoxLayout> 
#include <QHeaderView>
#include "itemtable.h"

ItemTable::ItemTable(QWidget *parent)
     : QTableWidget(parent)
{
     setWindowTitle("QTableWidget");  

     setRowCount(7);
     setColumnCount(2);  

     //verticalHeader()->resizeSection(i, rowHeight);
     verticalHeader()->resizeSection(0,18);
     verticalHeader()->resizeSection(1,18);
     verticalHeader()->resizeSection(2,18);
     verticalHeader()->resizeSection(3,18);
     verticalHeader()->resizeSection(4,18);
     verticalHeader()->resizeSection(5,18);
     verticalHeader()->resizeSection(6,18);

     horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}



void ItemTable::setTableItem(int type)
{
     t=type;
     QTableWidgetItem *item;

     if(t==1)
     {
        setItem(0,0, new QTableWidgetItem("Node ID"));
        setItem(1,0, new QTableWidgetItem("Label"));
        setItem(2,0, new QTableWidgetItem("Condition"));
        setItem(3,0, new QTableWidgetItem("Incoming Transition"));
        setItem(4,0, new QTableWidgetItem("Outgoing Transition"));
        setItem(5,0, new QTableWidgetItem("Initial Place"));
        setItem(6,0, new QTableWidgetItem("Final Place"));

        item = new QTableWidgetItem("1");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(2, 1, item);

        item = new QTableWidgetItem("true");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(5, 1, item);

        item = new QTableWidgetItem("false");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(6, 1, item);
     }

     if(t==2)
     {
        setItem(0,0, new QTableWidgetItem("Node ID"));
        setItem(1,0, new QTableWidgetItem("Label"));
        setItem(2,0, new QTableWidgetItem("Condition"));
        setItem(3,0, new QTableWidgetItem("Incoming Transition"));
        setItem(4,0, new QTableWidgetItem("Outgoing Transition"));
        setItem(5,0, new QTableWidgetItem("Initial Place"));
        setItem(6,0, new QTableWidgetItem("Final Place"));

        item = new QTableWidgetItem("1");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(2, 1, item);

        item = new QTableWidgetItem("false");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(5, 1, item);

        item = new QTableWidgetItem("true");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(6, 1, item);
     }

     if(t==3)
     {
        setItem(0,0, new QTableWidgetItem("Node ID"));
        setItem(1,0, new QTableWidgetItem("Label"));
        setItem(2,0, new QTableWidgetItem("Condition"));
        setItem(3,0, new QTableWidgetItem("Incoming Transition"));
        setItem(4,0, new QTableWidgetItem("Outgoing Transition"));
        setItem(5,0, new QTableWidgetItem("Initial Place"));
        setItem(6,0, new QTableWidgetItem("Final Place"));

        item = new QTableWidgetItem("1");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(2, 1, item);

        item = new QTableWidgetItem("false");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(5, 1, item);

        item = new QTableWidgetItem("false");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(6, 1, item);
     }

     if(t==4)
     {
        setItem(0,0, new QTableWidgetItem("Node ID"));
        setItem(1,0, new QTableWidgetItem("Label"));
        setItem(2,0, new QTableWidgetItem("Condition"));
        setItem(3,0, new QTableWidgetItem("Incoming Place"));
        setItem(4,0, new QTableWidgetItem("Outgoing Place"));
        item = new QTableWidgetItem("/");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(5,0, item);
        item = new QTableWidgetItem("/");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(6,0, item);

        item = new QTableWidgetItem("1");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(2, 1, item);

        item = new QTableWidgetItem("/");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(5, 1, item);

        item = new QTableWidgetItem("/");    
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(6, 1, item);
     }

     if(t==5)
     {
        setItem(0,0, new QTableWidgetItem("Node ID"));
        setItem(1,0, new QTableWidgetItem("Condition"));
        setItem(2,0, new QTableWidgetItem("Start From"));
        setItem(3,0, new QTableWidgetItem("Connect To"));
        item = new QTableWidgetItem("/");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(4,0, item);
        item = new QTableWidgetItem("/");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(5,0, item);
        item = new QTableWidgetItem("/");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(6,0, item);

        item = new QTableWidgetItem("1");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(1, 1, item);

        item = new QTableWidgetItem("/");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(4, 1, item);

        item = new QTableWidgetItem("/");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(5, 1, item);

        item = new QTableWidgetItem("/");
        item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        setItem(6, 1, item);
     }
}


void ItemTable::setIndex(int index)
{
     int i = index;

     QString str;

     if(t==1||t==2||t==3)
         str="P";
     if(t==4)
         str="T";
     if(t==5)
         str="A";


    QString name = str + QString::number(i);
    QTableWidgetItem *item = new QTableWidgetItem(name);

     //QTableWidgetItem *item = new QTableWidgetItem(QString::number(i));
     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(0,1, item);    
}


void ItemTable::setLabel(QString label)
{
    QTableWidgetItem *item = new QTableWidgetItem(label);

     item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
     setItem(1,1, item);
}


void ItemTable::setIncomming(int *group)
{
    QString str;
    int f;

    if(t==1||t==2||t==3)
        str="T";
    if(t==4)
        str="P";

    QString base = str;
    QString comma = ", ";
    QString name;

    if(group[0]==0)
        name = "none";

    else if(group[0]==1)
    {
        f = group[1];
        name = base + QString::number(f);
    }

    else if(group[0]>1)
    {
        for(int i=1; i<=group[0]; i++)
        {
            f = group[i];
            if(i==1)
                name = base + QString::number(f) + comma;
            else if(i>1 && i<group[0])
                name = name + base + QString::number(f) + comma;
            else
                name = name + base + QString::number(f);
        }
    }

    QTableWidgetItem *item = new QTableWidgetItem(name);

    item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
    setItem(3,1, item);
}



void ItemTable::setOutgoing(int *group)
{
    QString str;
    int f;

    if(t==1||t==2||t==3)
        str="T";
    if(t==4)
        str="P";

    QString base = str;
    QString comma = ", ";
    QString name;

    if(group[0]==0)
        name = "none";

    else if(group[0]==1)
    {
        f = group[1];
        name = base + QString::number(f);
    }

    else if(group[0]>1)
    {
        for(int i=1; i<=group[0]; i++)
        {
            f = group[i];
            if(i==1)
                name = base + QString::number(f) + comma;
            else if(i>1 && i<group[0])
                name = name + base + QString::number(f) + comma;
            else
                name = name + base + QString::number(f);
        }
    }

    QTableWidgetItem *item = new QTableWidgetItem(name);

    item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
    setItem(4,1, item);
}



void ItemTable::setConnection(int *group)
{
    int incommingType = group[0];
    int incommingIndex = group[1];
    int outgoingType = group[2];
    int outgoingIndex = group[3];

    QString base1, base2;

    if(incommingType==1||incommingType==2||incommingType==3)
        base1 = "P";
    if(incommingType==4)
        base1 = "T";

    if(outgoingType==1||outgoingType==2||outgoingType==3)
        base2 = "P";
    if(outgoingType==4)
        base2 = "T";

    QString name1 = base1 + QString::number(incommingIndex);
    QString name2 = base2 + QString::number(outgoingIndex);

    QTableWidgetItem *item1 = new QTableWidgetItem(name1);
    item1->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
    setItem(2,1, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem(name2);
    item2->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
    setItem(3,1, item2);
}


void ItemTable::setCondition(char *cond)
{
    //char *cond1 = "abs(F0A1-F-1A1)<100";
    QString condition = QString(QLatin1String(cond));
    QTableWidgetItem *item = new QTableWidgetItem(condition);
    item->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
    setItem(2,1, item);
}


void ItemTable::clearTable()
{
    setItem(0,0, new QTableWidgetItem(" "));
    setItem(1,0, new QTableWidgetItem(" "));
    setItem(2,0, new QTableWidgetItem(" "));
    setItem(3,0, new QTableWidgetItem(" "));
    setItem(4,0, new QTableWidgetItem(" "));
    setItem(5,0, new QTableWidgetItem(" "));
    setItem(6,0, new QTableWidgetItem(" "));

    setItem(0,1, new QTableWidgetItem(" "));
    setItem(1,1, new QTableWidgetItem(" "));
    setItem(2,1, new QTableWidgetItem(" "));
    setItem(3,1, new QTableWidgetItem(" "));
    setItem(4,1, new QTableWidgetItem(" "));
    setItem(5,1, new QTableWidgetItem(" "));
    setItem(6,1, new QTableWidgetItem(" "));
}
