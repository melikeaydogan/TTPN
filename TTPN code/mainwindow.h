#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QVTKWidget.h>
#include <QtWidgets>
#include "diagramitem.h"
#include "itemtable.h"
#include "nettable.h"
#include "polybrowser.h"
#include "sfdfiledialog.h"
#include "datatable.h"
#include "tdtable.h"
#include "multiviewwindow.h"


#include <iostream>
#include <vector>
#include <cstring>

class DiagramScene;
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;

class MainWindow : public QMainWindow
{
     Q_OBJECT

 public:
     MainWindow();
//     void sleep(unsigned msec);
     int getObjectList(int targetFrame);
     int getObjectFromPacket(int targetFrame, int *packetlist, int packetcount);        //used for reconbt
     int* colormap_reader(string filename, int targetID);
     void buildFrameTables();
     void buildPacketTables();
     void collectActivities();
     void collectActivities2();
     void collectActivities3();
     int workingmode;                  //The GUI has two working modes, 1 for model design, 2 for activity detection and visualization

 private slots:  
     void connectTokenIDtableToBrowser(int r, int c); 
     void connectActivitytableToMultiwindow(int r, int c); 
     void showTokenIDtable(int PlaceIndex, int *tokenlist);

     void ModeConfigure1();
     void ModeConfigure2();
     void buttonGroupClicked(int id);

     void deleteItem();
     //void pointerGroupClicked(int id);
     void checkItem();
     void checkArc();
     void rotateItem();
     void reverserotateItem();
     void addCondition();
     void curveInserted();

     void arcbuttonClicked();
     void tokenbuttonClicked();
     void clickbuttonClicked();
     void pointerbuttonClicked();
     void arcinfobuttonClicked();
     void textbuttonClicked();
     //void firebuttonClicked();
     void multiselectbuttonClicked();
     void rotatebuttonClicked();
     void reverserotatebuttonClicked();
     void barchartbuttonClicked();
     void filterbuttonClicked();
     void activitystatebuttonClicked();
     void backbuttonClicked();
     void multiviewbuttonClicked();

     void bringToFront();
     void sendToBack();
     void itemInserted(DiagramItem *item);
     void textInserted(QGraphicsTextItem *item);
     void itemsSelected();
     void arcClicked();

     void currentFontChanged(const QFont &font);
     void fontSizeChanged(const QString &size);
     void sceneScaleChanged(const QString &scale);
     void textColorChanged();
     void itemColorChanged();
     void lineColorChanged();
     void textButtonTriggered();
     void fillButtonTriggered();
     void lineButtonTriggered();

     void TrackingFileSelected(QString);
     void FirstDataFileSelected(QString);
     void LastDataFileSelected(QString);
     void precisionInputed(QString);
     void incrementInputed(QString);
     void PNvariablesInputed(QString);

     void handleFontChange();
     void itemSelected(QGraphicsItem *item);
     void about();

     void doOpenConf();      //open config file
     bool doFileSave();      //save model into a config file at current path
     bool doFileSaveAs();    //save model into a config file at customerized path

     void closeEvent(QCloseEvent *);

     void openConfAction_triggered();
     //void openTextAction_triggered();
     void saveAction_triggered();
     void saveasAction_triggered();
     void printAction_triggered();

     void grabAction_triggered();
     void clearAction_triggered();

     void browseTrackingFile();
     void browseFirstDataFile();
     void browseLastDataFile();

     void setOpen();
     void setEvent();
     void showNext();
     void showLast();
     void Playforward();
     void Playbackward();
     void playandstop();
     void openChartWindow(int *array, int size);
     void setChosenDataScene(int index);

     void runPN();          //run Petri Net code (do activity detection)
     //void printMouseCoordinate(int x, int y);

 private:
     //Ui::MainWindow *ui;
     //QDockWidget *dockwidget;

     void createToolBox();
     void createActions();
     void createMenus();
     void createToolbars();
     void createToolbars2();
     QWidget *createCellWidget(const QString &text, DiagramItem::DiagramType type);
     QMenu *createColorMenu(const char *slot, QColor defaultColor);
     QIcon createColorToolButtonIcon(const QString &image, QColor color);
     QIcon createColorIcon(QColor color);

     DiagramScene *scene;
     QGraphicsView *view;

     bool isSaved;           // Is file saved
     QString currentFilepath;        //Current path of file
     bool maybeSave();       //Save or not
     bool saveFile(QString fileName);     //Save function

     QAction *openConfAction;
     QAction *saveAction;
     QAction *saveasAction;
     QAction *printAction;
     QAction *exitAction;

     QAction *undoAction;
     QAction *redoAction;
     QAction *grabAction;
     QAction *clearAction;

     QAction *addAction;
     QAction *deleteAction;
     QAction *clickAction;
     QAction *clickArcAction;
     QAction *pointerAction;
     QAction *multiselectAction;
     QAction *addTextAction;
     QAction *addConditionAction;
     QAction *rotateAction;
     QAction *reverserotateAction;

     QAction *toFrontAction;
     QAction *sendBackAction;
     QAction *aboutAction;
     QAction *aboutQtAction;

     QAction *boldAction;
     QAction *underlineAction;
     QAction *italicAction;
     QAction *textAction;
     QAction *fillAction;
     QAction *lineAction;
     QAction *mode1Action, *mode2Action;

     QMenu *fileMenu;
     QMenu *toolMenu;
     QMenu *itemMenu;
     QMenu *aboutMenu;

     QToolBar *fileToolBar, *toolToolBar, *textToolBar, *editToolBar, *colorToolBar, *pointerToolbar, *modeToolBar, *dataToolBar;
     QComboBox *sceneScaleCombo, *itemColorCombo, *textColorCombo, *fontSizeCombo;
     QFontComboBox *fontCombo;

     QToolBox *toolBox;

     NetTable *t1;
     ItemTable *t2;
     TDtable *tokenIDtable, *activitytable;
     QLineEdit *TrackingFile, *FirstDataFile, *LastDataFile, *precision, *increment, *PNvariables;

     QButtonGroup *buttonGroup;
     //QButtonGroup *pointerTypeGroup;
     //QButtonGroup *backgroundButtonGroup;
     QButtonGroup *operationButtonGroup;

     QToolButton *fontColorToolButton, *fillColorToolButton, *lineColorToolButton, *arcButton, *tokenButton, *clickButton, *pointerButton, *arcInfoButton, *textButton, *multiselectButton, *rotateButton, *reverserotateButton, *browse1Button, *browse2Button, *browse3Button, *barchartButton, *filterButton, *activitystateButton, *backButton, *multiviewButton, *pnButton;    

     //QVBoxLayout *mode1leftLayout, *mode2leftLayout, *mode2rightLayout;
     QWidget *modewidget1, *modewidget2;
     PolyBrowser *polybrowser;
     SFDFileDialog *file_dlg;
     DataTable *datatable; 
     QStringList dataNames;   //All chosen *.poly file
     QString dataName; 
     int totalframe, currentframe, currentEvent, showEvent, num_eventFrame, num_eventObject, num_eventPlace;
     int *eventFrame, *eventObject, *eventPlace;   //Save all frame, object and place in event.list as array
     QPushButton *dataloadbutton, *eventbutton, *nextframebutton, *previousframebutton, *fastforwardbutton, *fastbackwardbutton, *playbutton;
     QFont fnt;
     bool startplay, eventloaded;
     int *ObjectList, *PlaceList, *PacketObjectList, *PlaceList2, *colorlist;   //New
     FrameTable *table;
     PacketTable *packettable;
     SingleActivity *singleactivity;
     Activity *activity;
     int singleactivity_num, activity_num, ce;
     bool completecircle, usegrouptracking;

     int *packetformation, *packetformation2;
     int formationcount;
     int maxPlaceID;
 };

 #endif
