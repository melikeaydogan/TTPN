#include <QtGui>
#include <math.h>
#include "token.h"

#define PI 3.14159265

Token::Token(QGraphicsItem *parent, QGraphicsScene *scene)
     : QGraphicsPolygonItem(parent)
 {
     QPainterPath path;
     setPen(QPen(Qt::black, 1));
     path.addEllipse(-9,-9,18,18);
     myPolygon = path.toFillPolygon();
  
     setPolygon(myPolygon);
     setFlag(QGraphicsItem::ItemIsMovable, true);
     setFlag(QGraphicsItem::ItemIsSelectable, true);
     setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
     index = 0;
 }



 QRectF Token::boundingRect() const
 {
     qreal penWidth = 1;
     return QRectF(-10, -10, 20, 20);
 }


 

