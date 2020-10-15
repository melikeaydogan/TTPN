
 #ifndef ARROW_H
 #define ARROW_H

 #include <QGraphicsLineItem>
 #include <QtWidgets>
 #include "diagramitem.h"

 class QGraphicsPolygonItem;
 class QGraphicsLineItem;
 class QGraphicsScene;
 class QRectF;
 class QGraphicsSceneMouseEvent;
 class QPainterPath;

 class Arrow : public QGraphicsLineItem
 {
 public:
     enum { Type = UserType + 4 };

     Arrow(DiagramItem *startItem, DiagramItem *endItem, QMenu *contextMenu,
       QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

     int type() const
         { return Type; }
     QRectF boundingRect() const;
     QPainterPath shape() const;
     void setColor(const QColor &color)
         { myColor = color; }
     DiagramItem *startItem() const
         { return myStartItem; }
     DiagramItem *endItem() const
         { return myEndItem; }

     void updatePosition();

     void setArcIndex(int i)
     {
         index = i;
     }

     int ArcIndex()
     {
         return index;
     }

     int index;  

 protected:
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0);
     void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

     //void mousePressEvent(QGraphicsSceneMouseEvent *event);
     //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

 private:
     DiagramItem *myStartItem;
     DiagramItem *myEndItem;
     QColor myColor;
     QPolygonF arrowHead;
     QMenu *myContextMenu;
 };

 #endif
