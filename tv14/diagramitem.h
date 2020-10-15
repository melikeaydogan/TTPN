#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPixmapItem>
#include <QList>
#include "diagramtextitem.h"

class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
class Arrow;

class DiagramItem : public QGraphicsPolygonItem
{
 public:
     enum { Type = UserType + 15 };
     //enum DiagramType { Step, Conditional, StartEnd, Io };
     enum DiagramType { InitPlace, FinalPlace, NormalPlace, Transition };

     DiagramItem(DiagramType diagramType, QMenu *contextMenu, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

     QRectF boundingRect() const;

     void removeArrow(Arrow *arrow);
     void removeArrows();
     void setTextlabelSelected();

     DiagramTextItem *label;
     char condition[500];

     int arrowNum;

     QList<Arrow *> getArrows()
     {
         return arrows;
     }

     DiagramType diagramType() const
         { return myDiagramType; }
     QPolygonF polygon() const
         { return myPolygon; }

     void changePolygon(QPolygonF polygon);

     void addArrow(Arrow *arrow);
     void addLabel(DiagramTextItem *text);
     QPixmap image() const;
     int type() const
         { return Type;}

     void setItemType(int i)
     {
         itemtype = i;
     }

     int ItemType()
     {
         return itemtype;
     }

     void setItemIndex(int i)
     {
         index = i;
     }

     int ItemIndex()
     {
         return index;
     }

     int itemtype, index, *tokenNum, *tokenList, number;

signals:
     //void showPlaceTable();
     //void showTransitionTable();

 protected:
     void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);

     //void mousePressEvent(QGraphicsSceneMouseEvent *event);
     //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

 private:
     DiagramType myDiagramType;
     QPolygonF myPolygon;
     QMenu *myContextMenu;
     QList<Arrow *> arrows;

 };

 #endif
