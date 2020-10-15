#include <QtGui>
#include "diagramitem.h"
#include "diagramscene.h"
#include "arrow.h"
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <string>
#include <stdio.h>

using namespace std;

DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
     : QGraphicsScene(parent)
{
     myItemMenu = itemMenu;
     myMode = MoveItem;
     myItemType = DiagramItem::NormalPlace;
     line = 0;
     rect = 0;
     textItem = 0;
     cvPoint_num = 0;
     i1=0;
     i2=0;
     i3=0;
     j=0;
     a=0;
     collide = false;
     modified = false;

     TrackingFileName = "";
     IsTrackingInfoProvided = "YES";
     InitialFrameNumber = "";
     FinalFrameNumber = "";
     Variables_string = "";
     FilesPath = "";
     TransitionsFileBaseName = "";
     TransitionsFileExtension = "";
     PlacesFileBaseName = "";
     PlacesFileExtension = "";
     DefaultActionsFileBaseName = "";
     DefaultActionsFileExtention = "";
     memset(initialMarkings, 0, 1000);
     precision = "1";
     increment = "1";
     num_inputarcs = 0;
     num_outputarcs = 0;

     num_pcx = 0;
     num_pcy = 0;
     num_tcx = 0;
     num_tcy = 0;

     //myItemColor = QColor(75,125,255,255);
     myItemColor = QColor(160, 160, 160, 255);
     myTextColor = Qt::black;
     myLineColor = Qt::black;

     myFont = QFont("Times", 14, QFont::Bold);
/*
     tag = new QGraphicsTextItem;
     tag->setTextInteractionFlags(Qt::TextEditorInteraction);
     tag->setFlag(QGraphicsItem::ItemIsMovable);
     tag->setFlag(QGraphicsItem::ItemIsSelectable);
     tag->setPos(0,0);
     addItem(tag);
*/
 }


 void DiagramScene::setLineColor(const QColor &color)
 {
     myLineColor = color;
     if (isItemChange(Arrow::Type)) 
     {
         Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
         item->setColor(myLineColor);
         update();
         modified = true;
     }
 }

 void DiagramScene::setTextColor(const QColor &color)
 {
     myTextColor = color;
     if (isItemChange(DiagramTextItem::Type))
     {
         DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
         item->setDefaultTextColor(myTextColor);
         modified = true;
     }
 }

 void DiagramScene::setItemColor(const QColor &color)
 {
     myItemColor = color;
     if (isItemChange(DiagramItem::Type))
     {
         DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
         item->setBrush(myItemColor);
         modified = true;
     }
 }

 void DiagramScene::setFont(const QFont &font)
 {
     myFont = font;

     if (isItemChange(DiagramTextItem::Type))
     {
         QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
         //At this point the selection can change so the first selected item might not be a DiagramTextItem
         if (item)
             item->setFont(myFont);
     }
 }

 void DiagramScene::setMode(Mode mode)
 {
     myMode = mode;
 }

 void DiagramScene::setItemType(DiagramItem::DiagramType type)
 {
     myItemType = type;
 }

 void DiagramScene::editorLostFocus(DiagramTextItem *item)
 {
     QTextCursor cursor = item->textCursor();
     cursor.clearSelection();
     item->setTextCursor(cursor);

     if (item->toPlainText().isEmpty()) 
     {
         removeItem(item);
         item->deleteLater();
     }
 }


 void DiagramScene::resetNetTable(int ip, int fp, int np, int tr, int arc)
 {
     i1 = i1 - ip;
     i2 = i2 - fp;
     i3 = i3 - np;
     j = j - tr;
     a = a - arc;

     emit pvalueChange1(i1);
     emit pvalueChange2(i2);
     emit pvalueChange3(i3);
     emit pvalueChange(i1+i2+i3);
     emit tvalueChange(j);
     emit avalueChange(a);
 }



void DiagramScene::checkArc()
{
     if (isItemChange(Arrow::Type)) 
     {
         Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());

         int type = 5;
         int index = item->ArcIndex(); 
         emit sendItemType(type);
         emit sendItemIndex(index); 

         int connection[4];
         connection[0] = item->startItem()->itemtype;
         connection[1] = item->startItem()->index;
         connection[2] = item->endItem()->itemtype;
         connection[3] = item->endItem()->index;
         emit connectionFound(connection);
         emit arcClicked();

         update();
     }
}



 void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
 {
     if (mouseEvent->button() != Qt::LeftButton)
         return;

     QList<QGraphicsItem *> storeditems;
     DiagramItem *item;
     textItem = new DiagramTextItem();
     QString base;
     QString name;
     QFont fnt;
     fnt.setPointSize(8);
     fnt.setFamily("times");
     textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
     textItem->setZValue(1000.0);
     textItem->setFont(fnt);

     QGraphicsLineItem *line1, *line2, *line3, *line4, *line5, *line6, *line7; 

     switch (myMode) 
     {
         case InsertItem:
             item = new DiagramItem(myItemType, myItemMenu);
             item->setBrush(myItemColor);
             item->setPos(mouseEvent->scenePos());
             strcpy(item->condition, "2>1");
             checkCollision(item);

             if(collide == false)
             {
                 addItem(item);

                 if(myItemType==DiagramItem::InitPlace)
                 {
                     item->setItemType(1);
                     IP_pos[i1] = mouseEvent->scenePos();
                     i1++;
                     item->setItemIndex(i1+i2+i3);
                     emit pvalueChange1(i1);
                     emit pvalueChange(i1+i2+i3);

                     base = "P";
                     name = base + QString::number(i1+i2+i3);
                     textItem->setPlainText(name);
                     textItem->setFont(myFont);
                     textItem->setPos(mouseEvent->scenePos().rx()-12, mouseEvent->scenePos().ry()+20);
                     item->addLabel(textItem);
                     addItem(textItem);
                 }

                 if(myItemType==DiagramItem::FinalPlace)
                 {
                     item->setBrush(QColor(255,50,50,255));
                     item->setItemType(2);
                     FP_pos[i2] = mouseEvent->scenePos();
                     i2++;
                     item->setItemIndex(i1+i2+i3);
                     emit pvalueChange2(i2);
                     emit pvalueChange(i1+i2+i3);

                     base = "P";
                     name = base + QString::number(i1+i2+i3);
                     textItem->setPlainText(name);
                     textItem->setFont(myFont);
                     textItem->setPos(mouseEvent->scenePos().rx()-12, mouseEvent->scenePos().ry()+20);
                     item->addLabel(textItem);
                     addItem(textItem);
                 }

                if(myItemType==DiagramItem::NormalPlace)
                {
                     item->setItemType(3);
                     OP_pos[i3] = mouseEvent->scenePos();
                     i3++;
                     item->setItemIndex(i1+i2+i3);
                     emit pvalueChange3(i3);
                     emit pvalueChange(i1+i2+i3);

                     base = "P";
                     name = base + QString::number(i1+i2+i3);
                     textItem->setPlainText(name);
                     textItem->setFont(myFont);
                     textItem->setPos(mouseEvent->scenePos().rx()-12, mouseEvent->scenePos().ry()+20);
                     item->addLabel(textItem);
                     addItem(textItem);
                }

                if(myItemType==DiagramItem::Transition)
                {
                     item->setItemType(4);
                     TR_pos[j] = mouseEvent->scenePos();
                     j++;
                     item->setItemIndex(j);
                     emit tvalueChange(j);

                     base = "T";
                     name = base + QString::number(j);
                     textItem->setPlainText(name);
                     textItem->setFont(myFont);
                     textItem->setPos(mouseEvent->scenePos().rx()-12, mouseEvent->scenePos().ry()+20);
                     item->addLabel(textItem);
                     addItem(textItem);
                }

                emit itemInserted(item);
                modified = true;
             }

             break;

         case InsertLine:
             line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()));
             line->setPen(QPen(myLineColor, 2));
             addItem(line);
             modified = true;
             break;

         case InsertCurve:
             cvPoint_num++;

             if(cvPoint_num==1)
             {
                A = mouseEvent->scenePos();
                QList<QGraphicsItem *> s1s = items(A);
                DiagramItem *s1 = qgraphicsitem_cast<DiagramItem *>(s1s.first());
                A = s1->pos();
                //cout << A.rx() << " " << A.ry() << endl;
             }

             else if(cvPoint_num==2)
             {
                E = mouseEvent->scenePos();
                QList<QGraphicsItem *> s2s = items(E);
                DiagramItem *s2 = qgraphicsitem_cast<DiagramItem *>(s2s.first());
                E = s2->pos();
             }

             else if(cvPoint_num==3)
                L = mouseEvent->scenePos();

             else if(cvPoint_num==4)
             {
                H = mouseEvent->scenePos();
                B.rx() = L.rx();
                B.ry() = E.ry();
                C.rx() = L.rx();
                C.ry() = H.ry();
                D.rx() = A.rx();
                D.ry() = H.ry();
                line1 = new QGraphicsLineItem(QLineF(QPointF(E.rx()+5, E.ry()), B));
                line2 = new QGraphicsLineItem(QLineF(B, C));
                line3 = new QGraphicsLineItem(QLineF(C, D));
                line4 = new QGraphicsLineItem(QLineF(D, QPointF(A.rx(), A.ry()-20)));
            
                line5 = new QGraphicsLineItem(QLineF(QPointF(A.rx()-3.5, A.ry()-20-7), QPointF(A.rx(), A.ry()-20)));
                line6 = new QGraphicsLineItem(QLineF(QPointF(A.rx()+3.5, A.ry()-20-7), QPointF(A.rx(), A.ry()-20)));
                line7 = new QGraphicsLineItem(QLineF(QPointF(A.rx()+3.5, A.ry()-20-7), QPointF(A.rx()-3.5, A.ry()-20-7)));

                line1->setPen(QPen(myLineColor, 1));
                line2->setPen(QPen(myLineColor, 1));
                line3->setPen(QPen(myLineColor, 1));
                line4->setPen(QPen(myLineColor, 1));
                line5->setPen(QPen(myLineColor, 3));
                line6->setPen(QPen(myLineColor, 3));
                line7->setPen(QPen(myLineColor, 3));

                line1->setFlag(QGraphicsItem::ItemIsMovable, true);
                line2->setFlag(QGraphicsItem::ItemIsMovable, true);
                line3->setFlag(QGraphicsItem::ItemIsMovable, true);
                line4->setFlag(QGraphicsItem::ItemIsMovable, true);
                line5->setFlag(QGraphicsItem::ItemIsMovable, true);
                line6->setFlag(QGraphicsItem::ItemIsMovable, true);
                line7->setFlag(QGraphicsItem::ItemIsMovable, true);

                addItem(line1);
                addItem(line2);
                addItem(line3);
                addItem(line4);
                addItem(line5);
                addItem(line6);
                addItem(line7);
                cvPoint_num = 0;
                emit curveInserted();
             }

             break;

         case InsertText:
             textItem = new DiagramTextItem();
             textItem->setFont(myFont);
             textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
             textItem->setZValue(1000.0);
             connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)), this, SLOT(editorLostFocus(DiagramTextItem*)));
             connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(itemSelected(QGraphicsItem*)));
             addItem(textItem);
             textItem->setDefaultTextColor(myTextColor);
             textItem->setPos(mouseEvent->scenePos());
             emit textInserted(textItem);
             modified = true;
             break;

         case ClickItem:
             lastPoint.rx() = mouseEvent->scenePos().rx();
             lastPoint.ry() = mouseEvent->scenePos().ry();
             endPoint.rx() = mouseEvent->scenePos().rx()+1;
             endPoint.ry() = mouseEvent->scenePos().ry()+1;
             checkItem();
             break;

         case MultiSelect:
             lastPoint = mouseEvent->scenePos();
             rect = new QGraphicsRectItem(lastPoint.rx(), lastPoint.ry(), 0, 0);
             rect->setPen(QPen(Qt::red, 1));
             addItem(rect);
             break;

         case RotateRight:
             lastPoint.rx() = mouseEvent->scenePos().rx();
             lastPoint.ry() = mouseEvent->scenePos().ry();
             endPoint.rx() = mouseEvent->scenePos().rx()+1;
             endPoint.ry() = mouseEvent->scenePos().ry()+1;
             rightrotation();
             break;

         case RotateLeft:
             lastPoint.rx() = mouseEvent->scenePos().rx();
             lastPoint.ry() = mouseEvent->scenePos().ry();
             endPoint.rx() = mouseEvent->scenePos().rx()+1;
             endPoint.ry() = mouseEvent->scenePos().ry()+1;
             leftrotation();
             break;
         
         case ClickChart:
             lastPoint.rx() = mouseEvent->scenePos().rx();
             lastPoint.ry() = mouseEvent->scenePos().ry();
             endPoint.rx() = mouseEvent->scenePos().rx()+1;
             endPoint.ry() = mouseEvent->scenePos().ry()+1;
             openHistWindow();
             break;

         case FilterData:
             lastPoint.rx() = mouseEvent->scenePos().rx();
             lastPoint.ry() = mouseEvent->scenePos().ry();
             endPoint.rx() = mouseEvent->scenePos().rx()+1;
             endPoint.ry() = mouseEvent->scenePos().ry()+1;
             filterPolyView();
             break;

         default:
             storeditems = selectedItems();
             if(storeditems.isEmpty()==false)
             {
                clickboth();
             }
         //;
     }

     QGraphicsScene::mousePressEvent(mouseEvent);
 }


 void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
 {
     if (myMode == InsertLine && line != 0) 
     {
         QLineF newLine(line->line().p1(), mouseEvent->scenePos());
         line->setLine(newLine);
     } 

     else if (myMode == MoveItem)
     {
         foreach (QGraphicsItem *item, selectedItems())
         {
             if (item->type() == DiagramItem::Type)
             {
                 DiagramItem *zone = qgraphicsitem_cast<DiagramItem *>(item);
                 zone->label->setSelected(true);
             }
         }

         QGraphicsScene::mouseMoveEvent(mouseEvent);
     }

     else if(myMode == MultiSelect && rect != 0 )
     {
         endPoint = mouseEvent->scenePos();
         QRectF newRect(lastPoint, endPoint);
         rect->setRect(newRect);
     }
 }


 void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
 {
     if (line != 0 && myMode == InsertLine) 
     {
         QList<QGraphicsItem *> startItems = items(line->line().p1());        
         if (startItems.count() && startItems.first() == line)
             startItems.removeFirst();
         QList<QGraphicsItem *> endItems = items(line->line().p2());            
         if (endItems.count() && endItems.first() == line)
             endItems.removeFirst();

         removeItem(line);
         delete line;

         if (startItems.count() > 0 && endItems.count() > 0 &&
             startItems.first() != endItems.first() && 
             startItems.first()->type() == DiagramItem::Type && endItems.first()->type() == DiagramItem::Type) 
         {
             DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(startItems.first());
             DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItems.first());
             
             if( (startItem->ItemType() == 1 && endItem->ItemType() == 4) ||
                 (startItem->ItemType() == 2 && endItem->ItemType() == 4) ||
                 (startItem->ItemType() == 3 && endItem->ItemType() == 4) ||
                 (startItem->ItemType() == 4 && endItem->ItemType() == 1) ||
                 (startItem->ItemType() == 4 && endItem->ItemType() == 2) ||
                 (startItem->ItemType() == 4 && endItem->ItemType() == 3) )
             {
                 Arrow *arrow = new Arrow(startItem, endItem, myItemMenu);
                 arrow->setColor(myLineColor);
                 startItem->addArrow(arrow);
                 endItem->addArrow(arrow);
                 arrow->setZValue(-1000.0);

                 Start_pos[a] = arrow->startItem()->pos();
                 End_pos[a] = arrow->endItem()->pos();

                 a++;
                 arrow->setArcIndex(a);
                 emit avalueChange(a);  

                 addItem(arrow);
                 arrow->updatePosition();
                 modified = true;
             }
         }
     }

     line = 0;

     if(myMode == MultiSelect)
     {
         endPoint = mouseEvent->scenePos();
         QRectF region(lastPoint, endPoint);

         QPainterPath pp;
         pp.addRect(region);
         setSelectionArea(pp);
         removeItem(rect);
         delete rect;
         emit multiitemsSelected();
     }

     rect = 0;

     if(myMode == ClickItem)
     {
         if(selectedItems().isEmpty())
             return;

         foreach (QGraphicsItem *item, selectedItems())
         {
             if (item->type() == DiagramItem::Type)
             {
                 DiagramItem *zone = qgraphicsitem_cast<DiagramItem *>(item);
                 if(zone->ItemType()==1 || zone->ItemType()==3 || zone->ItemType()==4)
                     zone->setBrush(myItemColor);
                 else if(zone->ItemType()==2)
                     zone->setBrush(QColor(255,50,50,255));
             }
         }
     }

     QGraphicsScene::mouseReleaseEvent(mouseEvent);
 }


 bool DiagramScene::isItemChange(int type)
 {
     foreach (QGraphicsItem *item, selectedItems())
     {
         if (item->type() == type)
             return true;
     }
     return false;
 }


 void DiagramScene::clickboth()
 {
     foreach (QGraphicsItem *item, selectedItems())
     {
         if (item->type() == DiagramItem::Type)
         {
             DiagramItem *zone = qgraphicsitem_cast<DiagramItem *>(item);
             zone->label->setSelected(true);
         }
     }
 }



 void DiagramScene::checkItem()
 {
     QRectF region(lastPoint, endPoint);
     QPainterPath pp;
     pp.addRect(region);
     setSelectionArea(pp);

     if(selectedItems().isEmpty()==false)
     {
         if(selectedItems().first()->type() == DiagramItem::Type)
         {
             DiagramItem *zone = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());

             QList<Arrow *> arcs = zone->getArrows();
             int num = arcs.count();
             //qDebug()<<num;
             int *incomming = (int *)malloc(num*sizeof(int));
             int *outgoing = (int *)malloc(num*sizeof(int));

             int i=0;
             int j=0;

             zone->setBrush(Qt::white);

             foreach(Arrow *arc, arcs)
             {
                 if( arc->startItem()->itemtype == zone->itemtype )
                 {
                     i++;
                     outgoing[i] = arc->endItem()->index;
                 }

                 else if( arc->endItem()->itemtype == zone->itemtype )
                 {
                    j++;
                    incomming[j] = arc->startItem()->index;
                 }
             }

             outgoing[0] = i;
             incomming[0] = j;

             int type = zone->ItemType();
             int index = zone->ItemIndex();
             QString label = zone->label->toPlainText();

             emit sendItemType(type);
             emit sendItemIndex(index);
             emit sendItemLabel(label);
             emit incommingItemsFound(incomming);
             emit outgoingItemsFound(outgoing);
             emit conditionFound(zone->condition);
         }
     }

 }



void DiagramScene::openHistWindow()
{
     QRectF region(lastPoint, endPoint);
     QPainterPath pp;
     pp.addRect(region);
     setSelectionArea(pp);

     int *array = new int [1000];
     memset(array, 0, 1000);
     int i=0;
     if(selectedItems().isEmpty()==false)
     {
         if(selectedItems().first()->type() == BarChart::Type)
         {
             BarChart *bc = qgraphicsitem_cast<BarChart *>(selectedItems().first());
     
             array[0] = bc->frame_num;
             for(i=0; i<bc->frame_num; i++)
                 array[i+1] = bc->count[i];
         }
     }

     emit sendChart(array, i);
}


void DiagramScene::filterPolyView()
{
     QRectF region(lastPoint, endPoint);
     QPainterPath pp;
     pp.addRect(region);
     setSelectionArea(pp);

     int *array = new int [1000];
     memset(array, 0, 1000);
     int PlaceIndex = -1;

     if(selectedItems().isEmpty()==false)
     {
         if(selectedItems().first()->type() == DiagramItem::Type)
         {
             DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
     
             if(item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
             {
                PlaceIndex = item->index;
                array[0] = item->number;
                for(int i=0; i<item->number; i++)
                    array[i+1] = item->tokenList[i];
             }
         }
     }

     //cout << array[0] << endl;

     emit sendFilter(array);
     emit sendTokenList(PlaceIndex, array);
}


/*
void DiagramScene::visualizeActivityState()
{
     QRectF region(lastPoint, endPoint);
     QPainterPath pp;
     pp.addRect(region);
     setSelectionArea(pp);

     int *array = new int [1000];
     memset(array, 0, 1000);
     int PlaceIndex = -1;

     if(selectedItems().isEmpty()==false)
     {
         if(selectedItems().first()->type() == DiagramItem::Type)
         {
             DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
     
             if(item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
             {
                PlaceIndex = item->index;
                array[0] = item->number;
                for(int i=0; i<item->number; i++)
                    array[i+1] = item->tokenList[i];
             }

             item->setBrush(QColor(255, 127, 0, 255));
         }
     }

     emit sendActorsArray(array);
}
*/


void DiagramScene::resetPlaceColor()
{
     foreach(QGraphicsItem *itemx, items())
     {
            if(itemx->type() == DiagramItem::Type)
            {
                DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
                if (item->ItemType()==1 || item->ItemType()==3)
                   item->setBrush(QColor(75,125,255,255));
                else if (item->ItemType()==2)
                   item->setBrush(QColor(255,50,50,255));
            }
     }
}


 void DiagramScene::rightrotation()
 {
     QRectF region(lastPoint, endPoint);
     QPainterPath pp;
     pp.addRect(region);
     setSelectionArea(pp);

     if(selectedItems().isEmpty()==false)
     {
         if(selectedItems().first()->type() == DiagramItem::Type)
         {
             DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());

             QPolygonF oldpolygon = item->polygon();
             QTransform t;
             t.rotate(item->rotation()+15);
             QPolygonF newpolygon = t.map(oldpolygon);
             item->setPolygon(newpolygon);
             item->changePolygon(newpolygon);
         }
     }
 }


 void DiagramScene::leftrotation()
 {
     QRectF region(lastPoint, endPoint);
     QPainterPath pp;
     pp.addRect(region);
     setSelectionArea(pp);

     if(selectedItems().isEmpty()==false)
     {
         if(selectedItems().first()->type() == DiagramItem::Type)
         {
             DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());

             QPolygonF oldpolygon = item->polygon();
             QTransform t;
             t.rotate(item->rotation()-15);
             QPolygonF newpolygon = t.map(oldpolygon);
             item->setPolygon(newpolygon);
             item->changePolygon(newpolygon);
         }
     }
 }



 void DiagramScene::checkCollision(DiagramItem *inspectitem)
 {
     QList<QGraphicsItem *> storeditems = items();

     collide = false;

     foreach(QGraphicsItem *storeditem, storeditems)
     {
         if(inspectitem->collidesWithItem(storeditem) && storeditem->type() == DiagramItem::Type)
             collide = true;
     }

 }



 void DiagramScene::cleareverything()
 {
     clear();

     //myItemMenu = itemMenu;
     myMode = MoveItem;
     myItemType = DiagramItem::NormalPlace;
     line = 0;
     rect = 0;
     textItem = 0;

     i1=0;
     i2=0;
     i3=0;
     j=0;
     a=0;
     collide = false;
     modified = false;

     //myItemColor = Qt::white;
     //myItemColor = QColor(75,125,255,255);  //蓝色
     myItemColor = QColor(160, 160, 160, 255);
     myTextColor = Qt::black;
     myLineColor = Qt::black;

     memset(IP_pos, 0, 1000);
     memset(FP_pos, 0, 1000);
     memset(OP_pos, 0, 1000);
     memset(TR_pos, 0, 1000);
     memset(Start_pos, 0, 1000);
     memset(End_pos, 0, 1000);

     memset(placecordX, 0, 1000);
     memset(placecordY, 0, 1000);
     memset(transitioncordX, 0, 1000);
     memset(transitioncordX, 0, 1000);

     memset(initialMarkings, 0, 1000);

     num_inputarcs = 0;
     num_outputarcs = 0;

     num_pcx = 0;
     num_pcy = 0;
     num_tcx = 0;
     num_tcy = 0;

     TrackingFileName = "";
     InitialFrameNumber = "";
     FinalFrameNumber = "";
     Variables_string = "";
     FilesPath = "";
     TransitionsFileBaseName = "";
     TransitionsFileExtension = "";
     PlacesFileBaseName = "";
     PlacesFileExtension = "";
     DefaultActionsFileBaseName = "";
     DefaultActionsFileExtention = "";
     num_inputarcs = 0;
     num_outputarcs = 0;

     placescondition.clear();
     transitionscondition.clear();
     Variables.clear();
     Places.clear();
     Transitions.clear();

     emit pvalueChange1(i1);
     emit pvalueChange2(i2);
     emit pvalueChange3(i3);
     emit pvalueChange(i1+i2+i3);
     emit tvalueChange(j);
     emit avalueChange(a);
     emit clearItemTable();
 }



 /************************ load whole file line by line to string vector *************************/
 void DiagramScene::loadConf()
 {
      ifstream infile;
      //vector<string> FullContent;
      FullContent.clear();
      infile.open(f);

      int pos, pound_pos;
      string delimiter = "=";
      string pound_sign = "#";
      string right, str;
      int num_line = 0;

      while(!infile.eof())
      {
           getline(infile, str);

           if(str.find(pound_sign)==string::npos && !str.empty() && str.size()>1) 
           {
              num_line++;
              pos = str.find(delimiter);
              right = str.substr(pos+delimiter.length(), str.length());
              FullContent.push_back(right);
           }
      }

      infile.close();

      int i;
      char *pch;

      //line 0 ~ 3
      TrackingFileName = FullContent[0];
      IsTrackingInfoProvided = FullContent[1];
      InitialFrameNumber = FullContent[2];
      FinalFrameNumber = FullContent[3];

      //line 4: Variables
      char *line4 = new char [3000];
      Variables.clear();
      strcpy(line4, FullContent[4].c_str());
      pch = strtok(line4, " ;");
      Variables.push_back(pch);
      num_Variables = 0;
      while(pch != NULL)
      {
           pch = strtok (NULL, " ;");
           Variables.push_back(pch);
           num_Variables++;
      }
      Variables_string = FullContent[4];
      delete [] line4;

      //line 5: Places
      char *line5 = new char [3000];
      Places.clear();
      strcpy(line5, FullContent[5].c_str());
      pch = strtok(line5, " ;");
      Places.push_back(pch);
      num_Places = 0;
      while(pch != NULL)
      {
           pch = strtok (NULL, " ;");
           Places.push_back(pch);
           num_Places++;
      }

      //line 6: Transitions
      char *line6 = new char [3000];
      Transitions.clear();
      strcpy(line6, FullContent[6].c_str());
      pch = strtok(line6, " ;");
      Transitions.push_back(pch);
      while(pch != NULL)
      {
           pch = strtok (NULL, " ;");
           Transitions.push_back(pch);
           j++;
      }
      delete [] line6;

      //line 7 ~ 13
      FilesPath = FullContent[7];
      TransitionsFileBaseName = FullContent[8];
      TransitionsFileExtension = FullContent[9];
      PlacesFileBaseName = FullContent[10];
      PlacesFileExtension = FullContent[11];
      DefaultActionsFileBaseName = FullContent[12];
      DefaultActionsFileExtention = FullContent[13];

      //line 14: inputarcs
      char *line14 = new char [3000];
      vector<char*> input;
      input.clear();
      strcpy(line14, FullContent[14].c_str());
      pch = strtok(line14, " ;");
      input.push_back(pch);
      num_inputarcs = 0;
      while(pch != NULL)
      {
           pch = strtok (NULL, " ;");
           input.push_back(pch);
           num_inputarcs++;
      }
        
      for (i=0;i<num_inputarcs;i++)   
          inputarcs[i]=atoi(input[i]); 
 
      delete [] line14;     

      //line 15: outputarcs
      char *line15 = new char [3000];
      vector<char*> output;
      output.clear();
      strcpy(line15, FullContent[15].c_str());
      pch = strtok(line15, " ;");
      output.push_back(pch);
      num_outputarcs = 0;
      while(pch != NULL)
      {
           pch = strtok (NULL, " ;");
           output.push_back(pch);
           num_outputarcs++;
      }
        
      for (i=0;i<num_outputarcs;i++)   
          outputarcs[i]=atoi(output[i]);

      delete [] line15;
        
      //line 16: initialMarkings
      char *line16 = new char [3000];
      vector<char*> initMarks;
      initMarks.clear();
      strcpy(line16, FullContent[16].c_str());
      pch = strtok(line16, " ;");
      initMarks.push_back(pch);
      num_initialMarkings = 0;
      while(pch != NULL)
      {
           pch = strtok (NULL, " ;");
           initMarks.push_back(pch);
           num_initialMarkings++;
      }

      for (i=0;i<num_initialMarkings;i++)      
          initialMarkings[i]=atoi(initMarks[i]);
       
      //line 17: FinalPlaceIDs
      char *line17 = new char [3000];
      vector<char*> FPID;
      FPID.clear();
      strcpy(line17, FullContent[17].c_str());
      pch = strtok(line17, " ;");
      FPID.push_back(pch);
      while(pch != NULL)
      {
           pch = strtok (NULL, " ;");
           FPID.push_back(pch);
           i2++;
      }
        
      for (i=0;i<num_FinalPlaceIDs;i++)
          FinalPlaceIDs[i]=atoi(FPID[i]);

      delete [] line16;
      
      //line 18: InitialPlaceIDs
      char *line18 = new char [3000];
      vector<char*> INID;
      INID.clear();
      strcpy(line18, FullContent[18].c_str());
      pch = strtok(line18, " ;");
      INID.push_back(pch);
      while(pch != NULL)
      {
           pch = strtok (NULL, " ;");
           INID.push_back(pch);
           i1++;
      }

      for (i=0;i<num_InitialPlaceIDs;i++)
          InitialPlaceIDs[i]=atoi(INID[i]);

      delete [] line18;
     
      i3 = num_Places - i1 - i2;
      a = (int)num_inputarcs/3 + (int)num_outputarcs/3;

      if (num_line>19)        //line 19: Precision
         precision = FullContent[19];

      if (num_line>20)       //line 20: Increment
         increment = FullContent[20];

      if (num_line>21)       //line 21: PlaceCordXs
      {
         char *line21 = new char [3000];
         vector<char*> pcordx;
         pcordx.clear();
         strcpy(line21, FullContent[21].c_str());
         pch = strtok(line21, " ;");
         pcordx.push_back(pch);
         while(pch != NULL)
         {
              pch = strtok (NULL, " ;");
              pcordx.push_back(pch);
              num_pcx++;
         }

         for(i=0; i<num_pcx; i++)          
            placecordX[i]=atoi(pcordx[i]);  
  
         delete [] line21;       
      }

      if (num_line>22)      //line 22: PlaceCordYs
      {
         char *line22 = new char [3000];
         vector<char*> pcordy;
         pcordy.clear();
         strcpy(line22, FullContent[22].c_str());
         pch = strtok(line22, " ;");
         pcordy.push_back(pch);

         while(pch != NULL)
         {
              pch = strtok (NULL, " ;");
              pcordy.push_back(pch);
              num_pcy++;
         }

         for (i=0; i<num_pcy; i++)
             placecordY[i]=atoi(pcordy[i]);
          
         delete [] line22;  
      }

      if (num_line>23)     //line 23: TransitionCordXs
      {
         char *line23 = new char [3000];
         vector<char*> tcordx;
         tcordx.clear();
         strcpy(line23, FullContent[23].c_str());
         pch = strtok(line23, " ;");
         tcordx.push_back(pch);

         while(pch != NULL)
         {
              pch = strtok (NULL, " ;");
              tcordx.push_back(pch);
              num_tcx++;
         }

         for (i=0; i<num_tcx; i++)        
             transitioncordX[i]=atoi(tcordx[i]);
          
         delete [] line23; 
      }

      if (num_line>24)      //line 24: TransitionCordYs
      {
         char *line24 = new char [3000];
         vector<char*> tcordy;
         tcordy.clear();
         strcpy(line24, FullContent[24].c_str());
         pch = strtok(line24, " ;");
         tcordy.push_back(pch);

         while(pch != NULL)
         {
              pch = strtok (NULL, " ;");
              tcordy.push_back(pch);
              num_tcy++;
         }

         for (i=0; i<num_tcy; i++)        
             transitioncordY[i]=atoi(tcordy[i]);
           
         delete [] line24;
       }

 }


void DiagramScene::saveConf(const QString &fileName, const char *fileFormat)              //文件存储
{
     int i;
     //QString format(fileFormat);
     //QString fileName2;
     //fileName2 = fileName + "." + format;
     //QByteArray ba = fileName2.toLatin1();

     QByteArray ba = fileName.toLatin1();
     char *name = ba.data();

     QFile file(name);
     file.open(QIODevice::WriteOnly | QIODevice::Text);
     QTextStream out(&file);


     //ofstream out;
     //out.open(name);

     DiagramItem *node;
     Arrow *arc;

     int* p_seq = new int[i1+i2+i3];
     int* t_seq = new int[j];
     int pnum = 0;
     int tnum = 0;
     num_inputarcs = 0;
     num_outputarcs = 0;
     placescondition.clear();
     transitionscondition.clear();
     //memset(inputarcs, 0, 1000);
     //memset(outputarcs, 0, 1000);

     foreach( QGraphicsItem *item, items() )
     {
         if(item->type() == DiagramItem::Type)
         {
            node = qgraphicsitem_cast<DiagramItem *>(item);
            if(node->ItemType() == 1 || node->ItemType() == 2 || node->ItemType() == 3)
            {
                string tempstring(node->condition);
                placescondition.push_back(tempstring);
                p_seq[pnum] = (node->index)-1;
                placecordX[pnum] = node->pos().rx();
                placecordY[pnum] = node->pos().ry();
                pnum++;
            }

            if(node->ItemType() == 4)
            {
                string tempstring(node->condition);
                //qDebug() << node->condition << endl;
                transitionscondition.push_back(tempstring);
                t_seq[tnum]=(node->index)-1;
                transitioncordX[tnum] = node->pos().rx();
                transitioncordY[tnum] = node->pos().ry();
                tnum++;
            }
         }

         if(item->type() == Arrow::Type)
         {
             arc = qgraphicsitem_cast<Arrow *>(item);
             if(arc->startItem()->ItemType()<4 && arc->startItem()->ItemType()>0)
             {
                 inputarcs[num_inputarcs] = arc->startItem()->index;
                 inputarcs[num_inputarcs+1] = arc->endItem()->index;
                 inputarcs[num_inputarcs+2] = 1;
                 num_inputarcs=num_inputarcs+3;
             }

             if(arc->startItem()->ItemType()==4)
             {
                 outputarcs[num_outputarcs] = arc->endItem()->index;
                 outputarcs[num_outputarcs+1] = arc->startItem()->index;
                 outputarcs[num_outputarcs+2] = 1;
                 num_outputarcs=num_outputarcs+3;
             }
         }
     }

     vector<string> placescondition_copy = placescondition;
     int *placecordX_copy = new int [1000];
     int *placecordY_copy = new int [1000];
     memcpy(placecordX_copy, placecordX, 1000*sizeof(int));
     memcpy(placecordY_copy, placecordY, 1000*sizeof(int));

     for(i=0; i<pnum; i++)
     {
         if(i!=p_seq[i])
         {
             placescondition[i] = placescondition_copy[p_seq[i]];
             placecordX[i] = placecordX_copy[p_seq[i]];
             placecordY[i] = placecordY_copy[p_seq[i]];
         }
     }

     delete [] placecordX_copy;
     delete [] placecordY_copy;

     vector<string> transitionscondition_copy = transitionscondition;
     int *transitioncordX_copy = new int [1000];
     int *transitioncordY_copy = new int [1000];
     memcpy(transitioncordX_copy, transitioncordX, 1000*sizeof(int));
     memcpy(transitioncordY_copy, transitioncordY, 1000*sizeof(int));

     for (i=0; i<tnum; i++)
     {
         if(i!=t_seq[i])
         {
             transitionscondition[i] = transitionscondition_copy[t_seq[i]];
             transitioncordX[i] = transitioncordX_copy[t_seq[i]];
             transitioncordY[i] = transitioncordY_copy[t_seq[i]];
         }
     }

     delete [] transitioncordX_copy;
     delete [] transitioncordY_copy;

     for (i=0; i<i1; i++)
         InitialPlaceIDs[i] = i+1; 

     for (i=0; i<i2; i++)    
         FinalPlaceIDs[i] = i1+i3+i+1;    

     out << "TrackingFileName = " << TrackingFileName.c_str() << endl;
     out << "IsTrackingInfoProvided = " << IsTrackingInfoProvided.c_str() << endl;
     out << "InitialFrameNumber = " << InitialFrameNumber.c_str() << endl;
     out << "FinalFrameNumber = " << FinalFrameNumber.c_str() << endl;
     out << "Variables = " << Variables_string.c_str() << endl;
     out << "Places = ";

     for (i=0; i<pnum; i++)
     {
         out << placescondition[i].c_str() << "; ";
         //qDebug() << placescondition[i].c_str() << "; ";
     }
     out<<"\n";

     //qDebug() << "\n";

     out << "Transitions = ";
     for(i=0; i<tnum; i++)
     {
         out << transitionscondition[i].c_str() << "; ";
         //qDebug() << transitionscondition[i].c_str() << "; ";
     }
     out<<"\n";
     //qDebug() << "\n";

     out << "FilesPath = " << FilesPath.c_str() << endl;
     out << "TransitionsFileBaseName = " << TransitionsFileBaseName.c_str() << endl;
     out << "TransitionsFileExtension = " << TransitionsFileExtension.c_str() << endl;
     out << "PlacesFileBaseName = " << PlacesFileBaseName.c_str() << endl;
     out << "PlacesFileExtension = " << PlacesFileExtension.c_str() << endl;
     out << "DefaultActionsFileBaseName = " << DefaultActionsFileBaseName.c_str() << endl;
     out << "DefaultActionsFileExtention = " << DefaultActionsFileExtention.c_str() << endl;

     out << "inputarcs = ";
     for(i=0; i<num_inputarcs; i++)
     {
         out << inputarcs[i] << " ";
     }
     out<<"\n";

     out << "outputarcs = ";
     for(i=0; i<num_outputarcs; i++)
     {
         out << outputarcs[i] << " ";
     }
     out<<"\n";

     out << "initialMarkings = ";
     for(i=0; i<pnum; i++)
     {
         out << initialMarkings[i] << " ";
     }
     out<<"\n";

     out << "FinalPlaceIDs = ";
     for(i=0; i<i2; i++)
     {
         out << FinalPlaceIDs[i] << " ";
     }
     out<<"\n";

     out << "InitialPlaceIDs = ";
     for(i=0; i<i1; i++)
     {
         out << InitialPlaceIDs[i] << " ";
     }
     out<<"\n";

     out << "Precision = " << precision.c_str() << endl;
     out << "Increment = " << increment.c_str() << endl;

     out << "PlaceCordXs = ";
     for(i=0; i<pnum; i++)
     {
         out << placecordX[i] << "; ";
     }
     out<<"\n";

     out << "PlaceCordYs = ";
     for(i=0; i<pnum; i++)
     {
         out << placecordY[i] << "; ";
     }
     out<<"\n";

     out << "TransitionCordXs = ";
     for(i=0; i<tnum; i++)
     {
         out << transitioncordX[i] << "; ";
     }
     out<<"\n";

     out << "TransitionCordYs = ";
     for(i=0; i<tnum; i++)
     {
         out << transitioncordY[i] << "; ";
     }
     out<<"\n";

     //qDebug()<<Variables_string.c_str()<<endl;
     //return true;

     modified = false;
     delete p_seq;
     delete t_seq;
}




void DiagramScene::refresh()
{
    cleareverything();
    loadConf();

    int i,ii;
/*
    int space = 100;
    int startp_x = 2200;
    int startp_y = 2500;
    int startt_x = 2260;
    int startt_y1 = 2400;
    int startt_y2 = 2600;
*/
    
    int space = 100;
    int startp_x = 800;
    int startp_y = 400;
    int startt_x = 860;
    int startt_y1 = 300;
    int startt_y2 = 500;

    vector<string> pp;
    for(i=0; i<num_Places; i++)
    {
        string str;
        str.assign(Places[i], strlen(Places[i]));
        pp.push_back(str);
    }

    vector<string> tt;
    for(i=0; i<j; i++)
    {
        string str;
        str.assign(Transitions[i], strlen(Transitions[i]));
        tt.push_back(str);
    }


    //qDebug()<<pp[2].c_str()<<endl;
    //qDebug()<<pp[3].c_str()<<endl;

    QFont fnt;
    fnt.setPointSize(8);
    fnt.setFamily("times");

    QString base, name;

    DiagramItem *newItem, *startItem, *endItem;
    QPointF nPos;

    for(i=0;i<i1;i++)
    {
        nPos.rx() = startp_x + i*space;
        nPos.ry() = startp_y;
/*
        if(placecordX[i]>0 && placecordY[i]>0)
        {
            nPos.rx() = placecordX[i];
            nPos.ry() = placecordY[i];
        }
*/
        newItem = new DiagramItem(DiagramItem::InitPlace, myItemMenu);
        newItem->setBrush(myItemColor);
        newItem->setPos(nPos);
        strcpy(newItem->condition, pp[i].c_str());

        //qDebug() << Places[i] << endl;

        newItem->setItemType(1);
        newItem->setItemIndex(i+1);
        addItem(newItem);

        base = "P";
        name = base + QString::number(i+1);
        textItem = new DiagramTextItem();
        textItem->setFont(fnt);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setZValue(1000.0);
        textItem->setPlainText(name);
        textItem->setFont(myFont);
        textItem->setPos( nPos.rx()-12, nPos.ry()+20);
        newItem->addLabel(textItem);
        addItem(textItem);
    }


    for(i=0;i<i3;i++)
    {
        nPos.rx() = startp_x + (i+i1)*space;
        nPos.ry() = startp_y;
/*
        if(placecordX[i+i1]>0 && placecordY[i+i1]>0)
        {
            nPos.rx() = placecordX[i+i1];
            nPos.ry() = placecordY[i+i1];
        }
*/
        newItem = new DiagramItem(DiagramItem::NormalPlace, myItemMenu);
        newItem->setBrush(myItemColor);
        newItem->setPos(nPos);
        strcpy(newItem->condition, pp[i+i1].c_str());

        newItem->setItemType(3);
        newItem->setItemIndex(i+1+i1);
        addItem(newItem);

        base = "P";
        name = base + QString::number(i+1+i1);
        textItem = new DiagramTextItem();
        textItem->setFont(fnt);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setZValue(1000.0);
        textItem->setPlainText(name);
        textItem->setFont(myFont);
        textItem->setPos( nPos.rx()-12, nPos.ry()+20);
        newItem->addLabel(textItem);
        addItem(textItem);
    }


    for(i=0;i<i2;i++)
    {
        nPos.rx() = startp_x + (i+i1+i3)*space;
        nPos.ry() = startp_y;
  /*
        if(placecordX[i+i1+i3]>0 && placecordY[i+i1+i3]>0)
        {
            nPos.rx() = placecordX[i+i1+i3];
            nPos.ry() = placecordY[i+i1+i3];
        }
*/
        newItem = new DiagramItem(DiagramItem::FinalPlace, myItemMenu);
        newItem->setBrush(QColor(255,50,50,255));
        newItem->setPos(nPos);
        //newItem->condition = Places[i+i1+i3];
        //newItem->condition = "Sufan";
        //strcpy(newItem->condition, Places[i+i1+i3]);
        strcpy(newItem->condition, pp[i+i1+i3].c_str());

        newItem->setItemType(2);
        newItem->setItemIndex(i+1+i1+i3);
        addItem(newItem);

        base = "P";
        name = base + QString::number(i+1+i1+i3);
        textItem = new DiagramTextItem();
        textItem->setFont(fnt);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setZValue(1000.0);
        textItem->setPlainText(name);
        textItem->setFont(myFont);
        textItem->setPos( nPos.rx()-12, nPos.ry()+20);
        newItem->addLabel(textItem);
        addItem(textItem);
    }


    for(i=0; i<j; i++)
    {
        if(i%2==0)
        {
            nPos.rx() = startt_x + i*space*0.5;
            nPos.ry() = startt_y1;
        }
        else if(i%2==1)
        {
            nPos.rx() = startt_x + (i-1)*space*0.5;
            nPos.ry() = startt_y2;
        }

        newItem = new DiagramItem(DiagramItem::Transition, myItemMenu);
        newItem->setBrush(myItemColor);
        newItem->setPos(nPos);
        //newItem->condition = Transitions[i];
        //newItem->condition = "Sufan";

        //strcpy(newItem->condition, Transitions[i]);
        strcpy(newItem->condition, tt[i].c_str());

        //qDebug() << tt[i].c_str() << endl;

        newItem->setItemType(4);
        newItem->setItemIndex(i+1);
        addItem(newItem);

        base = "T";
        name = base + QString::number(i+1);
        textItem = new DiagramTextItem();
        textItem->setFont(fnt);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setZValue(1000.0);
        textItem->setPlainText(name);
        textItem->setFont(myFont);
        textItem->setPos( nPos.rx()-12, nPos.ry()+20 );
        newItem->addLabel(textItem);
        addItem(textItem);
    }


    for(i=0; i<num_inputarcs-1; i=i+3)
    {
        ii = inputarcs[i]-1;
        foreach( QGraphicsItem *item, items(QPointF(startp_x+ii*space, startp_y)) )
        {
            if(item->type() == DiagramItem::Type)
            {
                startItem = qgraphicsitem_cast<DiagramItem *>(item);
            }
        }


        ii = inputarcs[i+1]-1;
        if(ii%2==0)
        {
            foreach( QGraphicsItem *item, items(QPointF(startt_x+ii*space*0.5, startt_y1)) )
            {
                if(item->type() == DiagramItem::Type)
                {
                    endItem = qgraphicsitem_cast<DiagramItem *>(item);

                }
            }
        }

        if(ii%2==1)
        {
            foreach( QGraphicsItem *item, items(QPointF(startt_x+(ii-1)*space*0.5, startt_y2)) )
            {
                if(item->type() == DiagramItem::Type)
                {
                    endItem = qgraphicsitem_cast<DiagramItem *>(item);
                }
            }
        }

        Arrow *arrow = new Arrow(startItem, endItem, myItemMenu);
        arrow->setColor(myLineColor);
        startItem->addArrow(arrow);
        endItem->addArrow(arrow);
        arrow->setZValue(-1000.0);
        arrow->setArcIndex(i+1);
        addItem(arrow);
        arrow->updatePosition();
    }


    for(i=0; i<num_outputarcs-1; i=i+3)
    {
        ii = outputarcs[i]-1;
        foreach( QGraphicsItem *item, items(QPointF(startp_x+ii*space, startp_y)) )
        {
            if(item->type() == DiagramItem::Type)
            {
               endItem = qgraphicsitem_cast<DiagramItem *>(item);
            }
        }

        ii = outputarcs[i+1]-1;
        if(ii%2==0)
        {
            foreach( QGraphicsItem *item, items(QPointF(startt_x+ii*space*0.5, startt_y1)) )
            {
                if(item->type() == DiagramItem::Type)
                {
                    startItem = qgraphicsitem_cast<DiagramItem *>(item);

                }
            }
        }

        if(ii%2==1)
        {
            foreach( QGraphicsItem *item, items(QPointF(startt_x+(ii-1)*space*0.5, startt_y2)) )
            {
                if(item->type() == DiagramItem::Type)
                {
                    startItem = qgraphicsitem_cast<DiagramItem *>(item);
                }
            }
        }

        Arrow *arrow = new Arrow(startItem, endItem, myItemMenu);
        arrow->setColor(myLineColor);
        startItem->addArrow(arrow);
        endItem->addArrow(arrow);
        arrow->setZValue(-1000.0);
        arrow->setArcIndex(i+1);
        addItem(arrow);
        arrow->updatePosition();
    }

    emit pvalueChange1(i1);
    emit pvalueChange2(i2);
    emit pvalueChange3(i3);
    emit pvalueChange(i1+i2+i3);
    emit tvalueChange(j);
    emit avalueChange(a);

    if(placecordX[0]>0 && placecordY[0]>0)
    {
    for(i=0; i<i1+i2+i3; i++)
    {
        foreach( QGraphicsItem *itemx, items(QPointF(startp_x+i*space, startp_y)) )
        {
            if(itemx->type() == DiagramItem::Type)
            {
                DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
                if( item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3 && item->index==i+1)
                {
                    item->setPos(placecordX[i], placecordY[i]);
                    item->label->setPos(placecordX[i]-12, placecordY[i]+20);
                }
            }
        }
    }
    }


    if(transitioncordX[0]>0 && transitioncordY[0]>0)
    {
    for(i=0; i<j; i++)
    {
        if(i%2==0)
        {
            foreach( QGraphicsItem *itemx, items(QPointF(startt_x+i*space*0.5, startt_y1)) )
            {
                if(itemx->type() == DiagramItem::Type)
                {
                    DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
                    if( item->ItemType()==4 && item->index==i+1)
                    {
                        item->setPos(transitioncordX[i], transitioncordY[i]);
                        item->label->setPos(transitioncordX[i]-12, transitioncordY[i]+20);
                    }
                }
            }
        }

        if(i%2==1)
        {
            foreach( QGraphicsItem *itemx, items(QPointF(startt_x+(i-1)*space*0.5, startt_y2)) )
            {
                if(itemx->type() == DiagramItem::Type)
                {
                    DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
                    if( item->ItemType()==4 && item->index==i+1)
                    {
                        item->setPos(transitioncordX[i], transitioncordY[i]);
                        item->label->setPos(transitioncordX[i]-12, transitioncordY[i]+20);
                    }
                }
            }
        }

    }

    }


}


void DiagramScene::emitItem(int type1, int index1, int *incomming1, int *outgoing1, QString label1, char *condition)
{
    emit sendItemType(type1);
    emit sendItemLabel(label1);
    emit sendItemIndex(index1);
    emit incommingItemsFound(incomming1);
    emit outgoingItemsFound(outgoing1);
    emit conditionFound(condition);
}




void DiagramScene::addTokensToPlace(int* rgb, int eventObject, int eventPlace, int index, int FrameID)
{
     if( index == 0 )
     {
         foreach( QGraphicsItem *itemx, items() )
         {
            if( itemx->type() == Token::Type )   
               removeItem(itemx);

            else if( itemx->type() == DiagramTextItem::Type )
            {
               DiagramTextItem *three = qgraphicsitem_cast<DiagramTextItem *>(itemx);
               if( three->toPlainText() == "..." )
                  removeItem(itemx);
            }
         }

         memset(tokenlist, 0, 10000);

         foreach( QGraphicsItem *itemx, items() )
         {
            if(itemx->type() == DiagramItem::Type)
            {
               DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
               memset(item->tokenList, -1, 10000);
               item->number = 0;
            }
         }
     }      
         
     Token *token = new Token;
     addItem(token);
     token->setBrush(QColor(rgb[0], rgb[1], rgb[2]));

     DiagramTextItem *textItem3 = new DiagramTextItem();
     addItem(textItem3);

     foreach(QGraphicsItem *itemx, items())
     {
        if(itemx->type() == DiagramItem::Type)
        {
           DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
           if(item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
           {
              if(item->index==eventPlace)
              {
                 QPointF position = item->pos();
                 int line = (tokenlist[eventPlace] - tokenlist[eventPlace]%3)/3;

                 if(tokenlist[eventPlace]<6)    
                 {
                    if( tokenlist[eventPlace]%3 == 0 )
                        token->setPos(position.rx(), position.ry()+60+20*line);
 
                    else if( tokenlist[eventPlace]%3 == 1 )
                    {
                        foreach( QGraphicsItem *t1, items(QPointF(position.rx(), position.ry()+60+20*line)) )
                        {
                           Token *token1 = qgraphicsitem_cast<Token *>( t1 );
                           token1->setPos(position.rx()-10, position.ry()+60+20*line);                               
                        }
                        token->setPos(position.rx()+10, position.ry()+60+20*line);
                    }
                          
                    else if( tokenlist[eventPlace]%3 == 2 )
                    {
                        foreach( QGraphicsItem *t1, items(QPointF(position.rx()-10, position.ry()+60+20*line)) )
                        {
                           Token *token1 = qgraphicsitem_cast<Token *>( t1 );
                           token1->setPos(position.rx()-20, position.ry()+60+20*line);                               
                        }
                        foreach( QGraphicsItem *t2, items(QPointF(position.rx()+10, position.ry()+60+20*line)) )
                        {
                           Token *token2 = qgraphicsitem_cast<Token *>( t2 );
                           token2->setPos(position.rx(), position.ry()+60+20*line);                               
                        }
                        token->setPos(position.rx()+20, position.ry()+60+20*line);
                    }
                       
                 } 

                 else if(tokenlist[eventPlace]>=6)
                 {
                    QFont fnt;
                    fnt.setPointSize(20);
                    textItem3->setTextInteractionFlags(Qt::TextEditorInteraction);
                    textItem3->setZValue(1000.0);
                    textItem3->setFont(fnt);
                    textItem3->setPlainText("...");
                    textItem3->setPos(position.rx()-12, position.ry()+80);
                 }

                 item->tokenNum[FrameID] = tokenlist[eventPlace]+1;
                 //item->tokenList[index] = eventObject;
                 //item->number = index+1;
                 item->tokenList[item->number] = eventObject;
                 item->number++;
              }
           }
         }
      }

      tokenlist[eventPlace]++;  
}



void DiagramScene::clearToken()
{
     foreach( QGraphicsItem *itemx, items() )
     {
        if(itemx->type() == Token::Type)
        {
           removeItem(itemx);
        }
     }
}


void DiagramScene::setFrameBoard(int FrameID)
{
     if(firstframe == 1)
     {
        QFont fnt2 = QFont("Times", 18, QFont::Bold);

        timestepboard = new QGraphicsTextItem;
        timestepboard->setTextInteractionFlags(Qt::TextEditorInteraction);
        timestepboard->setFlag(QGraphicsItem::ItemIsMovable);
        timestepboard->setFlag(QGraphicsItem::ItemIsSelectable);
        timestepboard->setFont(fnt2);
        timestepboard->setDefaultTextColor(Qt::black);
        timestepboard->setPlainText("Time Step: "+QString::number(FrameID+1));
        timestepboard->setPos(1300, 200);    
        addItem(timestepboard);
     }

     timestepboard->setPlainText("Time Step: "+QString::number(FrameID+1)); 
     firstframe = 0;   
}


void DiagramScene::setActivityBoard(int totalactivity)
{
     if(firstevent == 1)
     {
        QFont fnt1("Times", 16, QFont::Bold);

        ab = new QGraphicsTextItem;
        ab->setTextInteractionFlags(Qt::TextEditorInteraction);
        ab->setFlag(QGraphicsItem::ItemIsMovable);
        ab->setFlag(QGraphicsItem::ItemIsSelectable);
        ab->setFont(fnt1);
        ab->setDefaultTextColor(Qt::black);
        ab->setPlainText("Detected Activity: "+QString::number(totalactivity));
        ab->setPos(1250, 550);    
        addItem(ab);
     }

     ab->setPlainText("Detected Activity: "+QString::number(totalactivity)); 
     firstevent = 0; 
}


void DiagramScene::setBarChart(int eventPlace, int FrameID, int maxtokennum_bynow)
{
     foreach(QGraphicsItem *itemx, items())
     {
            if(itemx->type() == DiagramItem::Type)
            {
                DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
                if(item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
                {
                    QPointF position = item->pos();
                    QPointF position2 = QPointF(position.rx()-50, position.ry()+120);
                    if(item->index==eventPlace)
                    {
                       foreach( QGraphicsItem *tablex, items() )
                       {
                           if(tablex->type() == BarChart::Type)
                           {
                              BarChart *table = qgraphicsitem_cast<BarChart *>(tablex);
                              if(table->placeID == item->index)
                              {
                                 position2 = table->pos();
                                 removeItem(tablex);
                              }
                           }
                       }

                       BarChart *chart = new BarChart(FrameID, FrameID+1, maxtokennum_bynow, item->tokenNum);
                       addItem(chart);
                       chart->placeID = eventPlace; 
                       if( firstchart == true )
                          chart->setPos(QPointF(position.rx()-50, position.ry()+120));  
                       else
                          chart->setPos(position2);
                    } 

                    else if(item->index!=eventPlace) 
                    {
                       if(item->tokenNum[FrameID]==0)
                       {
                          foreach( QGraphicsItem *tablex, items() )
                          {
                              if(tablex->type() == BarChart::Type)
                              {
                                 BarChart *table = qgraphicsitem_cast<BarChart *>(tablex);
                                 if(table->placeID==item->index)
                                 {
                                    position2 = table->pos();
                                    removeItem(tablex);
                                 }
                              }
                          }

                          BarChart *chart = new BarChart(FrameID, FrameID+1, maxtokennum_bynow, item->tokenNum);
                          addItem(chart);
                          chart->placeID = item->index;  
                          if( firstchart == true )
                              chart->setPos(QPointF(position.rx()-50, position.ry()+120));  
                          else
                              chart->setPos(position2);
                        }
                    }               
                 }                
             }
      } 
      
      firstchart = 0;
}

