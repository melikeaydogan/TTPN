#include <QtGui>
#include <QtWidgets>
#include <math.h>
#include "diagramitem.h"
#include "arrow.h"

#define PI 3.14159265

DiagramItem::DiagramItem(DiagramType diagramType, QMenu *contextMenu,
             QGraphicsItem *parent, QGraphicsScene *scene)
     : QGraphicsPolygonItem(parent)
 {
     myDiagramType = diagramType;
     myContextMenu = contextMenu;
     arrowNum = 0;
     itemtype = 0;
     index = 0;
     number = 0;

     QPainterPath path;
     switch (myDiagramType) 
     {
         case NormalPlace:
             setPen(QPen(Qt::black, 1));
             path.addEllipse(-20,-20,40,40);
             myPolygon = path.toFillPolygon();
             break;

         case InitPlace:
             setPen(QPen(Qt::black, 1));
             path.addEllipse(-20,-20,40,40);
             path.addEllipse(-15,-15,30,30);
             myPolygon = path.toFillPolygon();
             break;

         case FinalPlace: 
             setPen(QPen(Qt::black, 1, Qt::DashLine));
             path.addEllipse(-20,-20,40,40);
             myPolygon = path.toFillPolygon();
             break;

         case Transition:
             setPen(QPen(Qt::black, 1));
             path.addRect(-5,-24,10,48);
             myPolygon = path.toFillPolygon();
             break;
     }

     setPolygon(myPolygon);
     setFlag(QGraphicsItem::ItemIsMovable, true);
     setFlag(QGraphicsItem::ItemIsSelectable, true);
     setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

     tokenNum = new int [5000];
     memset(tokenNum, 0, 5000);
     tokenList = new int [5000];
     memset(tokenList, -1, 5000);
 }



 QRectF DiagramItem::boundingRect() const
 {
     qreal penWidth = 1;

     //return QRectF(-30, -30, 60, 60);
     return QRectF(-25, -25, 50, 50);
 }


 void DiagramItem::changePolygon(QPolygonF polygon)
 {
     myPolygon = polygon;
 }


 void DiagramItem::removeArrow(Arrow *arrow)
 {
     int index = arrows.indexOf(arrow);

     if (index != -1)
         arrows.removeAt(index);

     arrowNum--;
 }

 void DiagramItem::removeArrows()
 {
     foreach (Arrow *arrow, arrows) 
     {
         arrow->startItem()->removeArrow(arrow);
         arrow->endItem()->removeArrow(arrow);
         scene()->removeItem(arrow);
         delete arrow;
     }

     arrowNum=0;
 }

 void DiagramItem::addArrow(Arrow *arrow)
 {
     arrows.append(arrow);
     arrowNum++;
 }


 void DiagramItem::addLabel(DiagramTextItem *text1)
 {
     label = text1;
 }



 QPixmap DiagramItem::image() const
 {
     QPixmap pixmap(48, 48);
     pixmap.fill(Qt::transparent);
     QPainter painter(&pixmap);
     if(myDiagramType==FinalPlace)
        painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
     else
        painter.setPen(QPen(Qt::black, 2));
     painter.translate(24, 24);
     painter.drawPolyline(myPolygon);

     return pixmap;
 }


 void DiagramItem::setTextlabelSelected()
 {
     label->setSelected(true);
 }


 void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
 {
     scene()->clearSelection();
     setSelected(true);
     myContextMenu->exec(event->screenPos());
 }


 QVariant DiagramItem::itemChange(GraphicsItemChange change,const QVariant &value)
 {
     if (change == QGraphicsItem::ItemPositionChange)
     {
         foreach (Arrow *arrow, arrows)
         {
             arrow->updatePosition();
         }
     }

     return value;
 }

/*
 void DiagramItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
 {
     if( event->button() == Qt::RightButton)
         setSelected(true);
 }
 */

