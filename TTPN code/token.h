

 #include <QGraphicsPixmapItem>
 #include <QList>
#include "diagramtextitem.h"

 
 class QGraphicsItem;
 class QGraphicsScene;
 class QGraphicsSceneMouseEvent;
 class QPainter;
 class QStyleOptionGraphicsItem;
 class QWidget;
 class QPolygonF;


 class Token : public QGraphicsPolygonItem
 {
 public:
     enum { Type = UserType + 20 };

     Token(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
     QRectF boundingRect() const;
 
     int type() const
     { return Type;}

     void setItemIndex(int i)
     {
         index = i;
     }

     int ItemIndex()
     {
         return index;
     }

     int index;

private:
     QPolygonF myPolygon;

 };


