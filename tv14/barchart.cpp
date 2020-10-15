#include <QtGui>
#include <stdlib.h>
#include <string.h>
#include "barchart.h"


BarChart::BarChart(int a, int b, int mh, int *c)
{  
        index = a;
        frame_num = b;
        maxh = mh;
        count = new int [frame_num];
        count = c;
        placeID = -1;
        color = Qt::yellow;
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}


QRectF BarChart::boundingRect() const
{
       qreal penWidth = 1;
      //return QRectF(-100 - penWidth / 2, -100 - penWidth / 2, 200 + penWidth, 200 + penWidth);
       return QRectF(-15, -1, 190 + penWidth, 170 + penWidth);
}


void BarChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
     painter->setPen(Qt::black); 
     painter->drawLine(0,120,170,120);
     painter->drawLine(0,0,0,120);
     painter->drawText(165,133, "t");
     painter->drawText(-15, 10, "n");

     int i;
     float w = 1.0;

     if (frame_num<=5)
        w = 24;
     else if(frame_num>5 && frame_num<=10)
        w = 15;
     else if(frame_num>10 && frame_num<=20)
        w = 8;
     else if(frame_num>20 && frame_num<=30)
        w = 5;
     else if(frame_num>30 && frame_num<=40)
        w = 4;
     else if(frame_num>40 && frame_num<=60)
        w = 2.5;
     else if(frame_num>60 && frame_num<=100)
        w = 1.5;
     else if(frame_num>100 && frame_num<=150)
        w = 1.0;
     else if(frame_num>150 && frame_num<=200)
        w = 0.75;
     else if(frame_num>200 && frame_num<=300)
        w = 0.5;

     painter->drawText((frame_num-1)*w+w/3, 8, QString::number(count[frame_num-1]));

     float h;
     float hscale = 100/maxh;
     for (i=0; i<frame_num; i++)
     {
         h = count[i]*hscale;
         painter->setBrush(Qt::NoBrush); 
         if (i == index)
            painter->setBrush(color); 

         painter->drawRect(i*w, 120-h, w, h);
     }
}



void BarChart::setColor(QColor newcolor)
{
     color = newcolor;
     update();
}
   
