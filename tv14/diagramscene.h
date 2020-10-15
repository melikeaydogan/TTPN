#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include "diagramitem.h"
#include "diagramtextitem.h"
#include "token.h"
#include "barchart.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class Token;

class DiagramScene : public QGraphicsScene
{
     Q_OBJECT

 public:
     enum Mode { InsertItem, InsertLine, InsertCurve, InsertText, ClickItem, MultiSelect, RotateRight, RotateLeft, ClickChart, FilterData, MoveItem };

     DiagramScene(QMenu *itemMenu, QObject *parent = 0);
     QFont font() const
         { return myFont; }
     QColor textColor() const
         { return myTextColor; }
     QColor itemColor() const
         { return myItemColor; }
     QColor lineColor() const
         { return myLineColor; }
     void setLineColor(const QColor &color);
     void setTextColor(const QColor &color);
     void setItemColor(const QColor &color);
     void setFont(const QFont &font);

     void checkArc();                         //check the information of the selected arc
     void checkItem();                        //check the information of the selected place/transition
     void clickboth();                        //this function gurantees both item and its label be chosen when the item is clicked
     void checkCollision(DiagramItem *inspectitem);   //check if the bounding boxs of the two items collide with each other
     void rightrotation();                    //rotate the chosen item to the right
     void leftrotation();                     //rotate the chosen item to the left
     void openHistWindow();                   //open a histogram window for the selected bar chart
     void filterPolyView();                   //send the chosen place ID and its token list to poly browser 
     //void visualizeActivityState();

     bool isModified() const { return modified; }
     bool isItemChange(int type);
     void saveConf(const QString &fileName, const char *fileFormat);
     void loadConf();
     void cleareverything();
     void refresh();
     void resetPlaceColor();
    
     void resetNetTable(int ip, int fp, int np, int tr, int arc);       //reset net table according to the input
     void emitItem(int type1, int index1, int *incomming1, int *outgoing1, QString label1, char *condition1); //send all item information   
     void clearToken();       //clear all tokens in the diagram scene
     void addTokensToPlace(int* rgb, int eventObject, int eventPlace, int index, int FrameID);   //add tokens to the specified place
     void setFrameBoard(int FrameID);
     void setBarChart(int eventPlace, int FrameID, int maxtokennum_bynow);
     void setActivityBoard(int totalactivity);

     char *f;
     int i1, i2, i3, j, a;              //(i1, i2, i3), j, and a represent the number of place, transition, and arc respectively
     int cvPoint_num;                   //the number of points in a curve arc
     QPointF A, B, C, D, E, H, L;       //These points are used for drawing curve arc
     bool modified;                     //modified is used for determining if the content has been modified without saving
     bool firstevent, firstframe, firstchart;  //they are used for the function addTokensToPlace(), setFrameBoard(), and setBarChart()
     QGraphicsTextItem *tag, *ab, *timestepboard;
     QPointF lastPoint, endPoint;       //these two points are used for drawing a rectangle selection region                  

     
 /************** Below are Config file related variables  ****************/
     vector<string> FullContent;
     string TrackingFileName;
     string IsTrackingInfoProvided;
     string InitialFrameNumber;
     string FinalFrameNumber;
     vector<char*> Variables;
     vector<char*> Places;
     vector<char*> Transitions;
     vector<string> placescondition;
     vector<string> transitionscondition;
     string FilesPath;
     string TransitionsFileBaseName;
     string TransitionsFileExtension;
     string PlacesFileBaseName;
     string PlacesFileExtension;
     string DefaultActionsFileBaseName;
     string DefaultActionsFileExtention;
     int inputarcs[1000];
     int outputarcs[1000];
     int initialMarkings[1000];
     int FinalPlaceIDs[1000];
     int InitialPlaceIDs[1000];
     string precision;
     string increment;
     string Variables_string;
     string Places_string;
     string Transitons_string;
 /***********************************************************************/

    

 public slots:
     void setMode(Mode mode);                            //set working mode on the diagram scene
     void setItemType(DiagramItem::DiagramType type);    //set item type when adding new items to the diagram scene        
     void editorLostFocus(DiagramTextItem *item);

 signals:
     void itemInserted(DiagramItem *item);               //send a signal to main window after an item is inserted
     void textInserted(QGraphicsTextItem *item);         //send a signal to main window after a text is inserted        
     void itemSelected(QGraphicsItem *item);             //send a signal to main window after an item is selected
     void sendItemType(int itemType);                    //send type of the chosen item 
     void sendItemIndex(int index);                      //send index of the chosen item 
     void sendItemLabel(QString label);                  //send label of the chosen item 
     void pvalueChange(int pValue);                      //send a signal to main window when total place number changes
     void pvalueChange1(int pValue1);                    //send a signal to main window when initial place number changes
     void pvalueChange2(int pValue2);                    //send a signal to main window when ordinary place number changes
     void pvalueChange3(int pValue3);                    //send a signal to main window when final place number changes
     void tvalueChange(int tValue);                      //send a signal to main window when transition number changes
     void avalueChange(int aValue);                      //send a signal to main window when arc number changes
     void multiitemsSelected();                          //send a signal to main window when multiple items are selected
     void arcClicked();                                  //send a signal to main window when an arc is selected      
     void clearItemTable();                              //send a signal to main window to clear item table

     void incommingItemsFound(int *incomming);           //send all incomming items of a chosen item      
     void outgoingItemsFound(int *outgoing);             //send all outgoing items of a chosen item 
     void connectionFound(int *connection);
     void conditionFound(char *condition);
     void sendChart(int *count, int size);
     void sendFilter(int *list);
     void sendActorsArray(int *list);
     void sendTokenList(int PlaceIndex, int *list);
     void curveInserted();

 protected:
     void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
     void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

 private:
     DiagramItem::DiagramType myItemType;
     QMenu *myItemMenu;
     Mode myMode;
     QFont myFont;
     QColor myTextColor, myItemColor, myLineColor;
     bool leftButtonDown;
     QGraphicsLineItem *line;
     QGraphicsRectItem *rect;     
     DiagramTextItem *textItem;   
     bool collide;

     QPointF IP_pos[1000], FP_pos[1000], OP_pos[1000], TR_pos[1000], Start_pos[1000], End_pos[1000];
     int placecordX[1000], placecordY[1000], transitioncordX[1000], transitioncordY[1000];
     int num_Variables, num_Places, num_Transitions, num_inputarcs, num_outputarcs, num_initialMarkings, num_FinalPlaceIDs, num_InitialPlaceIDs;
     int num_pcx, num_pcy, num_tcx, num_tcy;
     int tokenlist[10000];
 };

 #endif
