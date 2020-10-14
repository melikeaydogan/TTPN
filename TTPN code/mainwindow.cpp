
#include <QtGui>
#include <QLabel>
#include <QTextBrowser>
#include <QFile>
#include <QTextStream>

#include <QVTKWidget.h>

#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkVersion.h>
#include <vtkPoints.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>

#include "mainwindow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "arrow.h"
#include <ctime>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <QtPrintSupport/QtPrintSupport>

#include "chartwindow.h"
#include "PN.h"
#include "unistd.h"
//#include <windows.h>   //Especially for Sleep() function in Windows. Pleasee use commented sleep() in Linux

using namespace std;

MainWindow::MainWindow()
{
    //ui->setupUi(this);
    createActions();
    createToolBox();
    createMenus();

    QFont font("Times", 11, QFont::Bold);
    setFont(font);

    scene = new DiagramScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 2000, 800));
    connect(scene, SIGNAL(itemInserted(DiagramItem*)), this, SLOT(itemInserted(DiagramItem*)));
    connect(scene, SIGNAL(textInserted(QGraphicsTextItem*)), this, SLOT(textInserted(QGraphicsTextItem*)));
    connect(scene, SIGNAL(multiitemsSelected()), this, SLOT(itemsSelected()));
    connect(scene, SIGNAL(arcClicked()), this, SLOT(arcClicked()));
    connect(scene, SIGNAL(curveInserted()), this, SLOT(curveInserted()));

    modeToolBar = addToolBar(tr("Working Mode"));
    modeToolBar->addAction(mode1Action);
    modeToolBar->addAction(mode2Action);
    createToolbars();
    createToolbars2();

    t1 = new NetTable;
    t2 = new ItemTable;

    t1->setColumnWidth(0,122);
    t1->setColumnWidth(1,178);
    t2->setColumnWidth(0,122);
    t2->setColumnWidth(1,178);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    t1->setSizePolicy(sizePolicy);
    t2->setSizePolicy(sizePolicy);


    fnt.setPointSize(9);
    fnt.setFamily("Times");

    QFont fnt2( "Times", 11, QFont::Bold );

    t1->setFont(fnt);
    t2->setFont(fnt);

    connect(scene, SIGNAL(pvalueChange1(int)), t1, SLOT(setItemNumber1(int)));
    connect(scene, SIGNAL(pvalueChange2(int)), t1, SLOT(setItemNumber2(int)));
    connect(scene, SIGNAL(pvalueChange3(int)), t1, SLOT(setItemNumber3(int)));
    connect(scene, SIGNAL(tvalueChange(int)), t1, SLOT(setItemNumber4(int)));
    connect(scene, SIGNAL(avalueChange(int)), t1, SLOT(setItemNumber5(int)));
    connect(scene, SIGNAL(pvalueChange(int)), t1, SLOT(setItemNumber6(int)));

    connect(scene, SIGNAL(sendItemType(int)), t2, SLOT(setTableItem(int)));
    connect(scene, SIGNAL(sendItemIndex(int)), t2, SLOT(setIndex(int)));
    connect(scene, SIGNAL(sendItemLabel(QString)), t2, SLOT(setLabel(QString)));

    connect(scene, SIGNAL(incommingItemsFound(int*)), t2, SLOT(setIncomming(int*)));
    connect(scene, SIGNAL(outgoingItemsFound(int*)), t2, SLOT(setOutgoing(int*)));
    connect(scene, SIGNAL(connectionFound(int*)), t2, SLOT(setConnection(int*)));
    connect(scene, SIGNAL(conditionFound(char*)), t2, SLOT(setCondition(char*)));

    connect(scene, SIGNAL(clearItemTable()), t2, SLOT(clearTable()));
    //connect(scene, SIGNAL(sendChart(int *array)), this, SLOT(openChartWindow(int *array)));

    //QStringList header;
    //header<<" "<<" ";
    //t1->setHorizontalHeaderLabels(header);
    //t2->setHorizontalHeaderLabels(header);

    t1->horizontalHeader()->hide();
    t2->horizontalHeader()->hide();
    t1->verticalHeader()->hide();
    t2->verticalHeader()->hide();

/*
    t1->horizontalHeader()->setFont(fnt2);
    t1->horizontalHeader()->adjustSize();
    t2->horizontalHeader()->setFont(fnt2);
    t2->horizontalHeader()->adjustSize();
*/

    QLabel *label1 = new QLabel("Summary of Current Petri Net");
    label1->setAlignment( Qt::AlignHCenter );
    label1->setFont(fnt2);
    label1->adjustSize();
    QLabel *label2 = new QLabel("Item Information");
    label2->setAlignment( Qt::AlignHCenter );
    label2->setFont(fnt2);
    label2->adjustSize();

    QVBoxLayout *mode1leftLayout = new QVBoxLayout;

    //mode1leftLayout->addStretch();     //可伸缩的空间
    mode1leftLayout->setAlignment(Qt::AlignHCenter);//布局内控件水平居中
    mode1leftLayout->setSpacing(1);   //控件间隔为10
    mode1leftLayout->setMargin(1);   //页边距为10

    mode1leftLayout->addWidget(toolBox);
    mode1leftLayout->addWidget(label1);
    mode1leftLayout->addWidget(t1);
    mode1leftLayout->addWidget(label2);
    mode1leftLayout->addWidget(t2);
    mode1leftLayout->setStretch(0,84);
    mode1leftLayout->setStretch(1,3);
    mode1leftLayout->setStretch(2,30);
    mode1leftLayout->setStretch(3,3);
    mode1leftLayout->setStretch(4,35);

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addLayout(mode1leftLayout);

    view = new QGraphicsView(scene);
    layout1->addWidget(view);

    layout1->setStretch(0,21);
    layout1->setStretch(1,60);

    modewidget1 = new QWidget;
    modewidget1->setLayout(layout1);

    setCentralWidget(modewidget1);
    setWindowTitle(tr("Token Tracking Petri Net (Vizlab, Rutgers University)"));
    setUnifiedTitleAndToolBarOnMac(true);

    isSaved = false;
    workingmode = 1;
    completecircle = false;

    setGeometry(100, 20, 1220, 710);
    setWindowIcon(QIcon("TTPN.png"));
    show();
}



void MainWindow::ModeConfigure1()
{
    workingmode = 1;
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons)
    {
        button->setDisabled(false);
    }
    arcButton->setDisabled(false);
    tokenButton->setDisabled(false);

    removeToolBar(fileToolBar);
    removeToolBar(toolToolBar);
    removeToolBar(editToolBar);
    removeToolBar(colorToolBar);
    removeToolBar(textToolBar);
    removeToolBar(pointerToolbar);
    removeToolBar(dataToolBar);
    removeToolBar(modeToolBar);
    createToolbars();

    QFont font("Times", 11, QFont::Bold);
    setFont(font);

    t1 = new NetTable;
    t2 = new ItemTable;

    t1->setColumnWidth(0,122);
    t1->setColumnWidth(1,178);
    t2->setColumnWidth(0,122);
    t2->setColumnWidth(1,178);

    fnt.setPointSize(9);
    fnt.setFamily("Times");

    QFont fnt2( "Times", 11, QFont::Bold );

    t1->setFont(fnt);
    t2->setFont(fnt);

    connect(scene, SIGNAL(pvalueChange1(int)), t1, SLOT(setItemNumber1(int)));
    connect(scene, SIGNAL(pvalueChange2(int)), t1, SLOT(setItemNumber2(int)));
    connect(scene, SIGNAL(pvalueChange3(int)), t1, SLOT(setItemNumber3(int)));
    connect(scene, SIGNAL(tvalueChange(int)), t1, SLOT(setItemNumber4(int)));
    connect(scene, SIGNAL(avalueChange(int)), t1, SLOT(setItemNumber5(int)));
    connect(scene, SIGNAL(pvalueChange(int)), t1, SLOT(setItemNumber6(int)));

    connect(scene, SIGNAL(sendItemType(int)), t2, SLOT(setTableItem(int)));
    connect(scene, SIGNAL(sendItemIndex(int)), t2, SLOT(setIndex(int)));
    connect(scene, SIGNAL(sendItemLabel(QString)), t2, SLOT(setLabel(QString)));

    connect(scene, SIGNAL(incommingItemsFound(int*)), t2, SLOT(setIncomming(int*)));
    connect(scene, SIGNAL(outgoingItemsFound(int*)), t2, SLOT(setOutgoing(int*)));
    connect(scene, SIGNAL(connectionFound(int*)), t2, SLOT(setConnection(int*)));
    connect(scene, SIGNAL(conditionFound(char*)), t2, SLOT(setCondition(char*)));
    connect(scene, SIGNAL(clearItemTable()), t2, SLOT(clearTable()));

    t1->horizontalHeader()->hide();
    t2->horizontalHeader()->hide();
    t1->verticalHeader()->hide();
    t2->verticalHeader()->hide();

    t1->setItemNumber1(scene->i1);
    t1->setItemNumber2(scene->i2);
    t1->setItemNumber3(scene->i3);
    t1->setItemNumber4(scene->i1 + scene->i2 + scene->i3);
    t1->setItemNumber5(scene->j);
    t1->setItemNumber6(scene->a);

    QLabel *label1 = new QLabel("Summary of Current Petri Net");
    label1->setAlignment( Qt::AlignHCenter );
    label1->setFont(fnt2);
    label1->adjustSize();
    QLabel *label2 = new QLabel("Item Information");
    label2->setAlignment( Qt::AlignHCenter );
    label2->setFont(fnt2);
    label2->adjustSize();

    QVBoxLayout *mode1leftLayout = new QVBoxLayout;
    mode1leftLayout->addWidget(toolBox);
    mode1leftLayout->addWidget(label1);
    mode1leftLayout->addWidget(t1);
    mode1leftLayout->addWidget(label2);
    mode1leftLayout->addWidget(t2);
    mode1leftLayout->setStretch(0,84);
    mode1leftLayout->setStretch(1,3);
    mode1leftLayout->setStretch(2,30);
    mode1leftLayout->setStretch(3,3);
    mode1leftLayout->setStretch(4,35);

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addLayout(mode1leftLayout);

    view = new QGraphicsView(scene);
    layout1->addWidget(view);

    layout1->setStretch(0,21);
    layout1->setStretch(1,60);

    modewidget1 = new QWidget;
    modewidget1->setLayout(layout1);

    setCentralWidget(modewidget1);

    foreach( QGraphicsItem *tablex, scene->items() )
    {
       if( tablex->type() == BarChart::Type || tablex->type() == Token::Type )
          scene->removeItem(tablex);
       else if( tablex->type() == DiagramTextItem::Type )
       {
          DiagramTextItem *three = qgraphicsitem_cast<DiagramTextItem *>(tablex);
          if( three->toPlainText() == "..." )
              scene->removeItem(tablex);
       }
    }

    if(startplay == true)
    {
       scene->removeItem(scene->timestepboard);
       scene->removeItem(scene->ab);
    }

    isSaved = false;
    eventloaded = false;
    scene->firstevent = false;
    scene->firstframe = false;
    scene->firstchart = false;
    startplay = false;

    show();
}




void MainWindow::ModeConfigure2()
{
    workingmode = 2;
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons)
    {
        button->setChecked(false);
        button->setDisabled(true);
    }
    arcButton->setChecked(false);
    arcButton->setDisabled(true);
    tokenButton->setChecked(false);
    tokenButton->setDisabled(true);
    scene->setMode(DiagramScene::MoveItem);

    scene->setSceneRect(0,0,2000,800);

    totalframe = 0;
    currentframe = -1;
    startplay = false;
    eventloaded = false;

    QFont font("Times", 11, QFont::Bold);
    setFont(font);

    removeToolBar(editToolBar);
    removeToolBar(colorToolBar);
    removeToolBar(textToolBar);
    //removeToolBar(pointerToolbar);
    removeToolBar(dataToolBar);
    createToolbars2();
    this->addToolBar(dataToolBar);

    QFont fnt2( "Times", 11, QFont::Bold );

    QLabel *label1 = new QLabel("Summary of Current Petri Net");
    label1->setAlignment( Qt::AlignHCenter );
    label1->setFont(fnt2);
    label1->adjustSize();
    QLabel *label3 = new QLabel("Data Information");
    label3->setAlignment( Qt::AlignHCenter );
    label3->setFont(fnt2);
    label3->adjustSize();

    datatable = new DataTable;
    datatable->setColumnWidth(0,122);
    datatable->setColumnWidth(1,178);
    datatable->setFont(fnt);
    datatable->verticalHeader()->hide();
    datatable->horizontalHeader()->hide();

    QVBoxLayout *mode2leftLayout = new QVBoxLayout;
    mode2leftLayout->addWidget(toolBox);
    mode2leftLayout->addWidget(label1);
    mode2leftLayout->addWidget(t1);
    mode2leftLayout->addWidget(label3);
    mode2leftLayout->addWidget(datatable);
    mode2leftLayout->setStretch(0,84);
    mode2leftLayout->setStretch(1,3);
    mode2leftLayout->setStretch(2,30);
    mode2leftLayout->setStretch(3,3);
    mode2leftLayout->setStretch(4,35);

    polybrowser = new PolyBrowser;

    connect(polybrowser, SIGNAL(sendRow1(int)), datatable, SLOT(setRow1(int)));
    connect(polybrowser, SIGNAL(sendRow2(int)), datatable, SLOT(setRow2(int)));
    connect(polybrowser, SIGNAL(sendRow3(double *)), datatable, SLOT(setRow3(double *)));
    connect(polybrowser, SIGNAL(sendRow4(double *)), datatable, SLOT(setRow4(double *)));
    connect(polybrowser, SIGNAL(sendRow5(double *)), datatable, SLOT(setRow5(double *)));
    //connect(polybrowser, SIGNAL(sendMouseCoordinate(int, int)), this, SLOT(printMouseCoordinate(int, int)));

    connect(scene, SIGNAL(sendChart(int *, int)), this, SLOT(openChartWindow(int *, int)));
    connect(scene, SIGNAL(sendFilter(int *)), polybrowser, SLOT(onFilteringOpen(int *)));
    connect(scene, SIGNAL(sendTokenList(int, int *)), this, SLOT(showTokenIDtable(int, int *)));

    QVBoxLayout *mode2rightLayout = new QVBoxLayout;
    view->scale(0.7, 0.7);

    QGraphicsView *viewpoly = new QGraphicsView(polybrowser);
    viewpoly->scale(1.6, 1.6);
    mode2rightLayout->addWidget(view);
    mode2rightLayout->addWidget(viewpoly);

    mode2rightLayout->setStretch(0,1);
    mode2rightLayout->setStretch(1,2);

    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addLayout(mode2leftLayout);
    layout2->addLayout(mode2rightLayout);

    layout2->setStretch(0,21);
    layout2->setStretch(1,60);

    modewidget2 = new QWidget;
    modewidget2->setLayout(layout2);

    setCentralWidget(modewidget2);
    show();
}


 void MainWindow::buttonGroupClicked(int id)
 {
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons) 
     {
     if (buttonGroup->button(id) != button)
         button->setChecked(false);
     }

     arcButton->setChecked(false);     
     tokenButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);
     rotateButton->setChecked(false);
     reverserotateButton->setChecked(false);

     scene->setItemType(DiagramItem::DiagramType(id));
     scene->setMode(DiagramScene::InsertItem);
 }


 void MainWindow::deleteItem()
 {
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons)
     {
         button->setChecked(false);
     }

     arcButton->setChecked(false);
     tokenButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);

     if(scene->selectedItems().isEmpty())
         return;

     int ip=0;
     int fp=0;
     int np=0;
     int tr=0;
     int arc=0;

     foreach (QGraphicsItem *item, scene->selectedItems())
     {
         if (item->type() == Arrow::Type)
         {
             scene->removeItem(item);
             Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
             arrow->startItem()->removeArrow(arrow);
             arrow->endItem()->removeArrow(arrow);
             delete item;
             arc++;
         }
     }


     foreach (QGraphicsItem *item, scene->selectedItems())
     {
          if (item->type() == DiagramItem::Type)
          {
              DiagramItem *d = qgraphicsitem_cast<DiagramItem *>(item);
              arc = arc + d->arrowNum;
              d->removeArrows();

              if(d->itemtype == 1)
                  ip++;
              if(d->itemtype == 2)
                  fp++;
              if(d->itemtype == 3)
                  np++;
              if(d->itemtype == 4)
                  tr++;

              //scene->removeItem(d->label);
              //delete d->label;
          }

          scene->removeItem(item);
          delete item;
     }

     scene->modified = true;

     scene->resetNetTable(ip, fp, np, tr, arc);
     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);

 }


 void MainWindow::checkItem()
 {
     if(scene->selectedItems().isEmpty())
              return;

     QList<QAbstractButton *> buttons = buttonGroup->buttons();
     foreach (QAbstractButton *button, buttons)
     {
         button->setChecked(false);
     }

     tokenButton->setChecked(false);
     arcButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);

     QGraphicsItem *itemx = scene->selectedItems().first();

     if(itemx->type() == DiagramItem::Type)
     {
         DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
         QList<Arrow *> arcs = item->getArrows();
         int num = arcs.count();
         int *incomming = (int *)malloc(num*sizeof(int));
         int *outgoing = (int *)malloc(num*sizeof(int));
         int i=0;
         int j=0;

         foreach(Arrow *arc, arcs)
         {
             if( arc->startItem()->itemtype == item->itemtype )
             {
                 i++;
                 outgoing[i] = arc->endItem()->index;
             }

             else if( arc->endItem()->itemtype == item->itemtype )
             {
                 j++;
                 incomming[j] = arc->startItem()->index;
             }
         }

         outgoing[0] = i;
         incomming[0] = j;

         int type1 = item->ItemType();
         int index1 = item->ItemIndex();
         QString label1 = item->label->toPlainText();
         char *condition1 = item->condition;

         scene->emitItem(type1, index1, incomming, outgoing, label1, condition1);
     }

     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


 void MainWindow::checkArc()
 {
     if(scene->selectedItems().isEmpty())
              return;

     QList<QAbstractButton *> buttons = buttonGroup->buttons();
     foreach (QAbstractButton *button, buttons)
     {
         button->setChecked(false);
     }
     tokenButton->setChecked(false);
     arcButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);

     QGraphicsItem *itemx = scene->selectedItems().first();

     if(itemx->type() == Arrow::Type)
     {
         Arrow *item = qgraphicsitem_cast<Arrow *>(itemx);
         int type2 = 5;
         int index2 = item->ArcIndex();
         int connection[4];

         connection[0] = item->startItem()->itemtype;
         connection[1] = item->startItem()->index;
         connection[2] = item->endItem()->itemtype;
         connection[3] = item->endItem()->index;

         t2->setTableItem(type2);
         t2->setIndex(index2);
         t2->setConnection(connection);
     }

     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


 void MainWindow::rotateItem()
 {
     if(scene->selectedItems().isEmpty())
              return;

     QList<QAbstractButton *> buttons = buttonGroup->buttons();
     foreach (QAbstractButton *button, buttons)
     {
         button->setChecked(false);
     }
     tokenButton->setChecked(false);
     arcButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);

     QGraphicsItem *itemx = scene->selectedItems().first();

     if(itemx->type() == DiagramItem::Type)
     {
         DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
         //item->rotate(15);

         QPolygonF oldpolygon = item->polygon();
         QTransform t;
         t.rotate(item->rotation()+15);
         QPolygonF newpolygon = t.map(oldpolygon);
         item->setPolygon(newpolygon);
         item->changePolygon(newpolygon);
     }

     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


 void MainWindow::reverserotateItem()
 {
     if(scene->selectedItems().isEmpty())
              return;

     QList<QAbstractButton *> buttons = buttonGroup->buttons();
     foreach (QAbstractButton *button, buttons)
     {
         button->setChecked(false);
     }
     tokenButton->setChecked(false);
     arcButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);

     QGraphicsItem *itemx = scene->selectedItems().first();

     if(itemx->type() == DiagramItem::Type)
     {
         DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);

         QPolygonF oldpolygon = item->polygon();
         QTransform t;
         t.rotate(item->rotation()-15);
         QPolygonF newpolygon = t.map(oldpolygon);
         item->setPolygon(newpolygon);
         item->changePolygon(newpolygon);
     }

     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


 void MainWindow::addCondition()
 {
     if(scene->selectedItems().isEmpty())
              return;

     QList<QAbstractButton *> buttons = buttonGroup->buttons();
     foreach (QAbstractButton *button, buttons)
     {
         button->setChecked(false);
     }
     tokenButton->setChecked(false);
     arcButton->setChecked(false);

     QGraphicsItem *itemx = scene->selectedItems().first();

     if(itemx->type() == DiagramItem::Type)
     {
         DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);

         bool ok;
         QString show = QString(item->condition);
         QString text = QInputDialog::getText(this, tr("Input Condition"),
                                               tr("Condition"), QLineEdit::Normal,
                                               show, &ok);
         if (ok && !text.isEmpty())
         {
             QByteArray ba = text.toLocal8Bit();
             const char *c_str2 = ba.data();
             strcpy(item->condition, c_str2);
         }

          t2->setCondition(item->condition);
     }


     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


 void MainWindow::arcbuttonClicked()
 {     
     tokenButton->setChecked(false);     
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);
     rotateButton->setChecked(false);
     reverserotateButton->setChecked(false);
     
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons) 
     {
         button->setChecked(false);
     }

     scene->setMode(DiagramScene::InsertLine);
 }


void MainWindow::pointerbuttonClicked()
 {
     arcButton->setChecked(false);
     tokenButton->setChecked(false);     
     clickButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);
     rotateButton->setChecked(false);
     reverserotateButton->setChecked(false);
     
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons) 
     {
         button->setChecked(false);
     }
   
     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


void MainWindow::curveInserted()
{
     tokenButton->setChecked(false);
     pointerButton->setChecked(true);
     scene->setMode(DiagramScene::MoveItem);
}


void MainWindow::arcinfobuttonClicked()
 {
     arcButton->setChecked(false);
     tokenButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);
     rotateButton->setChecked(false);
     reverserotateButton->setChecked(false);
     
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons) 
     {
         button->setChecked(false);
     }

     scene->checkArc();
 }


void MainWindow::tokenbuttonClicked()
 {
     arcButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);
     rotateButton->setChecked(false);
     reverserotateButton->setChecked(false);
     
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons) 
     {
         button->setChecked(false);
     }

     //setGeometry(100, 25, 800, 600);
     //show();     

     //scene->setMode(DiagramScene::MoveItem);
     //pointerButton->setChecked(true);

     scene->setMode(DiagramScene::InsertCurve);
 }


void MainWindow::clickbuttonClicked()
{
     arcButton->setChecked(false);
     tokenButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);
     rotateButton->setChecked(false);
     reverserotateButton->setChecked(false);
     
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons) 
     {
         button->setChecked(false);
     }
   
     scene->setMode(DiagramScene::ClickItem);
 }


void MainWindow::textbuttonClicked()
{
     arcButton->setChecked(false);
     tokenButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     multiselectButton->setChecked(false);
     rotateButton->setChecked(false);
     reverserotateButton->setChecked(false);
     
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons) 
     {
         button->setChecked(false);
     }

     scene->setMode(DiagramScene::InsertText);
}


void MainWindow::multiselectbuttonClicked()
{
     arcButton->setChecked(false);   
     tokenButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     rotateButton->setChecked(false);
     reverserotateButton->setChecked(false);
     
     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons) 
     {
         button->setChecked(false);
     }
     
     scene->setMode(DiagramScene::MultiSelect);
 }


void MainWindow::rotatebuttonClicked()
{
     arcButton->setChecked(false);
     tokenButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);
     reverserotateButton->setChecked(false);

     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons)
     {
         button->setChecked(false);
     }

     scene->setMode(DiagramScene::RotateRight);
 }


void MainWindow::reverserotatebuttonClicked()
{
     arcButton->setChecked(false);
     tokenButton->setChecked(false);
     clickButton->setChecked(false);
     pointerButton->setChecked(false);
     arcInfoButton->setChecked(false);
     textButton->setChecked(false);
     multiselectButton->setChecked(false);
     rotateButton->setChecked(false);

     QList<QAbstractButton *> buttons = buttonGroup->buttons();

     foreach (QAbstractButton *button, buttons)
     {
         button->setChecked(false);
     }

     scene->setMode(DiagramScene::RotateLeft);
 }


 void MainWindow::openChartWindow(int *array, int size)
 {
     QMainWindow *myNewWindow = new QMainWindow; 
     ChartWindow *chart = new ChartWindow(array);

     FILE *fp;
     fp = fopen("hist.txt", "a");
     if (fp != NULL)
     {
         int t1 = sizeof(array);
         int t2 = sizeof(array[0]);
         for(int i = 0; i <= size;i++)
         {
             fprintf(fp, "%d ",array[i]);
         }
         fprintf(fp, "\n");
     }
     fclose(fp);
     connect( chart, SIGNAL(sendBarIndex(int)), this, SLOT(setChosenDataScene(int)) );
     myNewWindow->setCentralWidget(chart);
     myNewWindow->setWindowTitle(tr("Histogram of Token Numbers in Current Place"));

     myNewWindow->show();
     scene->setMode(DiagramScene::MoveItem);
     barchartButton->setChecked(false);
 }


void MainWindow::setChosenDataScene(int index)
{
     //cout << "I receive your message: " << index << endl;
     //currentEvent = index;
     previousframebutton->setDisabled(false);
     fastbackwardbutton->setDisabled(false);
     nextframebutton->setDisabled(false);
     fastforwardbutton->setDisabled(false);

     dataName = dataNames.at(index);
     QByteArray ba = dataName.toLatin1();
     char *name = ba.data();
  
     polybrowser->camera = polybrowser->renderer->GetActiveCamera();      
     polybrowser->onAnimation(name, table[index].object_num, table[index].objects);
     showEvent = index;
  
     datatable->setRow0(dataName);

     int i,j;
     bool maybeend = false;
     for(i=0; i<table[index].object_num; i++)
     {
         int *rgb = new int [3];
         rgb = colormap_reader(dataName.toUtf8().constData(), table[index].objects[i]);
         scene->addTokensToPlace(rgb, table[index].objects[i], table[index].places[i], i, index);
         //scene->setBarChart(table[index].places[i], index);
 
         if( table[index].places[i] == maxPlaceID )
         //if( table[index].places[i] == 4 )
             maybeend = true;
     }


    int maxtokennum_bythen = 0;
    foreach(QGraphicsItem *itemx, scene->items())
    {
        if(itemx->type() == DiagramItem::Type)
        {
           DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
           if(item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
           {
              for(i=0; i<=showEvent; i++)
              {
                  if(item->tokenNum[i]>maxtokennum_bythen)
                     maxtokennum_bythen = item->tokenNum[i];
              }
           }
         }
    }
    //qDebug() << "max token number: " << maxtokennum_bythen << endl;

     for(i=0; i<table[index].object_num; i++)
         scene->setBarChart(table[index].places[i], index, maxtokennum_bythen);


     if(table[index].object_num==0)
        scene->clearToken();

     scene->setFrameBoard(index);
}


void MainWindow::barchartbuttonClicked()
{
     scene->setMode(DiagramScene::ClickChart);
     filterButton->setChecked(false);
     activitystateButton->setChecked(false);
}


void MainWindow::filterbuttonClicked()
{
     scene->setMode(DiagramScene::FilterData);
     backButton->setDisabled(false);
}


void MainWindow::activitystatebuttonClicked()
{
     
     QColor *color = new QColor [15];
     
     color[0] = QColor(255, 255, 0, 255);         //yellow  
     color[1] = QColor(255, 128, 0, 255);         //orange 
     color[2] = QColor(0, 128, 0, 255);           //light green            
     color[3] = QColor(255, 0, 127, 255);         //pink
     color[4] = QColor(127, 0, 255, 255);         //purple
     color[5] = QColor(0, 255, 0, 255);           //green    
     color[6] = QColor(160, 160, 160, 255);       //grey
     color[7] = QColor(102, 102, 255, 255);       //light blue
     color[8] = QColor(0, 204, 204, 255); 
     color[9] = QColor(153, 0, 153, 255);
     color[10] = QColor(229, 204, 255, 255);
     color[11] = QColor(0, 0, 255, 255);           //blue
     color[12] = QColor(255, 0, 0, 255);           //red
     color[13] = QColor(32, 32, 32, 255);         //black
     color[14] = QColor(102, 0, 0, 255);          //dark red

     vector<vtkSmartPointer<vtkPolyData> >polydatagroup;
     polydatagroup.clear();
     int number = 0;
     foreach (QGraphicsItem *itemx, scene->items())
     {
          if (itemx->type() == DiagramItem::Type)
          {
              DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
              if (item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
              {
                 if (item->number>0)
                 {
                    int itemindex = item->index;
                    item->setBrush(color[itemindex]);
                    //cout << "Type: " << item->ItemType() << "   " << "Number: " << number << endl;
                    vtkPolyData *polydata = vtkPolyData::New();
                    polydata = object_reader(string(polybrowser->currentFileName), item->number, item->tokenList, color[itemindex], true);

                    polydatagroup.push_back(polydata);
                    number++;
                 }

                 else if (item->number==0)
                    item->setBrush(QColor(255, 255, 255, 255));
              }             
          }
      
          else if (itemx->type() == Token::Type)
          {
               Token *token = qgraphicsitem_cast<Token *>(itemx);
               token->setVisible(false);
          }
/*
          else if (itemx->type() == BarChart::Type)
          {   
               BarChart *chart = qgraphicsitem_cast<BarChart *>(itemx);
               chart->setVisible(false);
          }
*/
          else if (itemx->type() == BarChart::Type)
          {   
               BarChart *chart = qgraphicsitem_cast<BarChart *>(itemx);
               int placeID = chart->placeID;
               chart->setColor(color[placeID]);
          }
          
     }

     polybrowser->visualizeActivityActors(polydatagroup, number);

     scene->modified = true;
     activitystateButton->setChecked(false);
     backButton->setDisabled(false);
}


 void MainWindow::backbuttonClicked()
 {
     scene->setMode(DiagramScene::MoveItem);
     scene->resetPlaceColor();
     
     foreach (QGraphicsItem *itemx, scene->items())
     {
             if (itemx->type() == Token::Type)
             {
                Token *token = qgraphicsitem_cast<Token *>(itemx);
                token->setVisible(true);
             }

             else if (itemx->type() == BarChart::Type)
             {   
                BarChart *chart = qgraphicsitem_cast<BarChart *>(itemx);
                //chart->setVisible(true);
                chart->setColor(Qt::yellow);
             }
     }

     dataName = dataNames.at(showEvent);     
     QByteArray ba = dataName.toLatin1();
     char *name = ba.data(); 
     polybrowser->camera = polybrowser->renderer->GetActiveCamera();      
     //polybrowser->onAnimation(name, table[currentEvent].object_num, table[currentEvent].objects);
     polybrowser->onAnimation(name, table[showEvent].object_num, table[showEvent].objects);
     backButton->setChecked(false);
     backButton->setDisabled(true);
     filterButton->setChecked(false);
     activitystateButton->setChecked(false);
 }


 void MainWindow::multiviewbuttonClicked()
 {
     activitytable = new TDtable;
     QFont fnt( "Times", 12, QFont::Bold );
     activitytable->setFont(fnt);

     activitytable->setRowCount(activity_num);
     activitytable->setColumnCount(3);

     activitytable->setMaximumWidth(800);
     activitytable->setMinimumWidth(600);
     activitytable->setMaximumHeight(800);
     activitytable->setMinimumHeight(600); 

     activitytable->setColumnWidth(0,100);
     activitytable->setColumnWidth(1,150);
     activitytable->setColumnWidth(2,150);
     activitytable->setColumnWidth(3,200);

     QString eventIDs;
     QString comma = ", ";
/*
     for (int i=0; i<96; i++)
     {
         QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i+1));
         item1->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(i, 0, item1);

         QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(365));
         item2->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(i, 1, item2);

         QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(369));
         item3->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(i, 2, item3);
     }

     QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(97));
         item1->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(96, 0, item1);

         QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(370));
         item2->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(96, 1, item2);

         QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(373));
         item3->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(96, 2, item3);

      QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(98));
         item4->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(97, 0, item4);

         QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(371));
         item5->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(97, 1, item5);

         QTableWidgetItem *item6 = new QTableWidgetItem(QString::number(375));
         item6->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(97, 2, item6);


        QTableWidgetItem *item7 = new QTableWidgetItem(QString::number(99));
         item7->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(98, 0, item7);

         QTableWidgetItem *item8 = new QTableWidgetItem(QString::number(375));
         item8->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(98, 1, item8);

         QTableWidgetItem *item9 = new QTableWidgetItem(QString::number(381));
         item9->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(98, 2, item9);

QTableWidgetItem *item10 = new QTableWidgetItem(QString::number(100));
         item10->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(99, 0, item10);

         QTableWidgetItem *item11 = new QTableWidgetItem(QString::number(395));
         item11->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(99, 1, item11);

         QTableWidgetItem *item12 = new QTableWidgetItem(QString::number(400));
         item12->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(99, 2, item12);
*/

     for (int i=0; i<activity_num; i++)
     {
         QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i+1));
         item1->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         activitytable->setItem(i, 0, item1);

         if (activity[i].singleactivity_num==1)
         {
            int k = activity[i].singleactivitylist[0];
            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(singleactivity[k].startframe+1));
            item2->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
            activitytable->setItem(i, 1, item2);

            QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(singleactivity[k].endframe+1));
            item3->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
            activitytable->setItem(i, 2, item3);

            QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(k+1));
            item4->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
            activitytable->setItem(i, 3, item4);
         }

         else if (activity[i].singleactivity_num>1)
         {
            for (int j=0; j<activity[i].singleactivity_num; j++)
            {
                int k = activity[i].singleactivitylist[j];
                if (j==0)
                   eventIDs = QString::number(k+1) + comma;
                else if(j>0 && j<activity[i].singleactivity_num-1)
                   eventIDs = eventIDs + QString::number(k+1) + comma;
                else if(j==activity[i].singleactivity_num-1)
                   eventIDs = eventIDs + QString::number(k+1);
            }                     

            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(activity[i].startframe+1));
            item2->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
            activitytable->setItem(i, 1, item2);

            QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(activity[i].endframe+1));
            item3->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
            activitytable->setItem(i, 2, item3);

            QTableWidgetItem *item4 = new QTableWidgetItem(eventIDs);
            item4->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
            activitytable->setItem(i, 3, item4);
         }          
     }

     connect(activitytable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(connectActivitytableToMultiwindow(int, int)));
     activitytable->setWindowTitle("Activity State Summary Table");

     QStringList header;
     header << "Activity" << "Beginning Timestep" << "Completed Timestep" << "Events";
     activitytable->setHorizontalHeaderLabels(header);
     activitytable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
     activitytable->show();

     multiviewButton->setChecked(false);
 }


 void MainWindow::bringToFront()
 {
     if (scene->selectedItems().isEmpty())
         return;

     QGraphicsItem *selectedItem = scene->selectedItems().first();
     QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

     qreal zValue = 0;
     foreach (QGraphicsItem *item, overlapItems)
     {
         if (item->zValue() >= zValue &&
             item->type() == DiagramItem::Type)
             zValue = item->zValue() + 0.1;
     }
     selectedItem->setZValue(zValue);
 }


 void MainWindow::sendToBack()
 {
     if (scene->selectedItems().isEmpty())
         return;

     QGraphicsItem *selectedItem = scene->selectedItems().first();
     QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

     qreal zValue = 0;
     foreach (QGraphicsItem *item, overlapItems)
     {
         if (item->zValue() <= zValue && item->type() == DiagramItem::Type)
             zValue = item->zValue() - 0.1;
     }
     selectedItem->setZValue(zValue);
 }


 void MainWindow::itemInserted(DiagramItem *item)
 {
     //pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
     //scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
     //buttonGroup->button(int(item->diagramType()))->setChecked(false);
 }


 void MainWindow::textInserted(QGraphicsTextItem *)
 {
     textButton->setChecked(false);
     pointerButton->setChecked(true);
     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


 void MainWindow::itemsSelected()
 {
     multiselectButton->setChecked(false);
     //pointerButton->setChecked(true);
     
     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


 void MainWindow::arcClicked()
 {
     //arcInfoButton->setChecked(false);
     //pointerButton->setChecked(true);
     scene->setMode(DiagramScene::MoveItem);
     pointerButton->setChecked(true);
 }


 void MainWindow::currentFontChanged(const QFont &)
 {
     handleFontChange();
 }


 void MainWindow::fontSizeChanged(const QString &)
 {
     handleFontChange();
 }


 void MainWindow::sceneScaleChanged(const QString &scale)
 {
     double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
     QMatrix oldMatrix = view->matrix();
     view->resetMatrix();
     view->translate(oldMatrix.dx(), oldMatrix.dy());
     view->scale(newScale, newScale);
 }


 void MainWindow::textColorChanged()
 {
     textAction = qobject_cast<QAction *>(sender());
     fontColorToolButton->setIcon(createColorToolButtonIcon(
                 "/waytottpncode/tv14/images/textpointer.png",
                 qvariant_cast<QColor>(textAction->data())));
     textButtonTriggered();
 }


 void MainWindow::itemColorChanged()
 {
     fillAction = qobject_cast<QAction *>(sender());
     fillColorToolButton->setIcon(createColorToolButtonIcon(
                  "/waytottpncode/tv14/images/floodfill.png",
                  qvariant_cast<QColor>(fillAction->data())));
     fillButtonTriggered();
 }


 void MainWindow::lineColorChanged()
 {
     lineAction = qobject_cast<QAction *>(sender());
     lineColorToolButton->setIcon(createColorToolButtonIcon(
                  "/waytottpncode/tv14/images/linecolor.png",
                  qvariant_cast<QColor>(lineAction->data())));
     lineButtonTriggered();
 }


 void MainWindow::textButtonTriggered()
 {
     scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
 }


 void MainWindow::fillButtonTriggered()
 {
     scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
 }


 void MainWindow::lineButtonTriggered()
 {
     scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
 }


 void MainWindow::handleFontChange()
 {
     QFont font = fontCombo->currentFont();
     font.setPointSize(fontSizeCombo->currentText().toInt());
     font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
     font.setItalic(italicAction->isChecked());
     font.setUnderline(underlineAction->isChecked());

     scene->setFont(font);
 }


 void MainWindow::itemSelected(QGraphicsItem *item)
 {
     DiagramTextItem *textItem =
     qgraphicsitem_cast<DiagramTextItem *>(item);

     QFont font = textItem->font();
     QColor color = textItem->defaultTextColor();
     fontCombo->setCurrentFont(font);
     fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
     boldAction->setChecked(font.weight() == QFont::Bold);
     italicAction->setChecked(font.italic());
     underlineAction->setChecked(font.underline());
 }


 void MainWindow::about()
 {
     QMessageBox::about(this, tr("About TTPN"),
                 tr("<h1>TTPN version 1.0</h1>\n"
                 "<p>Copyright 2013 @ <a href=\"http://coewww.rutgers.edu/www2/vizlab\">Vizlab</a>, Rutgers University. TTPN is a Qt app to simulate Token Tracking Petri Net model."
                         " For more information, updates, and user guide about this software, please visit the <a href=\"http://coewww.rutgers.edu/www2/vizlab/liliu/research\">Main Page of TTPN</a>.</p>\n"
                 "<h2>Authors</h2>\n"
                 "<dl>\n"
                 "<dt><a href=\"http://coewww.rutgers.edu/www2/vizlab/liliu\">Li Liu</a></dt>\n"
                 "<dt><a href=\"mailto:l.liu6819@gmail.com\">l.liu6819@gmail.com</a><br>\n"
                 "Original author, design and implement the software. </dd>\n"
                 "<dl>\n"
                 "<dt> <a href=\"http://eceweb1.rutgers.edu/~sedat\">Dr. Sedat Ozer</a></dt>\n"
                 "<dt><a href=\"mailto:sedat@eden.rutgers.edu\">sedat@eden.rutgers.edu</a><br>\n"
                 "Design the software. </dd>\n"
                 "<dl>\n"
                 "<dt><a href=\"http://www.ece.rutgers.edu/faculty/silver\">Prof. Deborah Silver</a></dt>\n"
                 "<dt><a href=\"mailto:silver@jove.rutgers.edu\">silver@jove.rutgers.edu</a><br>\n"
                 "Supervisor. </dd>\n"
                 "</dl>\n"
                 "<h2>Acknowledgement</h2>\n"
                 "<dl>\n"
                 "<dt><a href=\"http://geology.rutgers.edu/people/23-people/annuals-and-adjuncts/258-karen-bemis\">Dr. Karen Bemis</a></dt>\n"
                 "<dt><a href=\"mailto:bemis@rci.rutgers.edu\">bemis@rci.rutgers.edu</a><br>\n"
                 "Feedback and data support.</dd>\n"
                 "</dl>"
                 ));

 }


 void MainWindow::createToolBox()
  {
      buttonGroup = new QButtonGroup(this);
      buttonGroup->setExclusive(false);
      connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupClicked(int)));
      QGridLayout *layout = new QGridLayout;

      layout->addWidget(createCellWidget(tr("Initial Place"), DiagramItem::InitPlace), 0, 0);
      layout->addWidget(createCellWidget(tr("Final Place"),DiagramItem::FinalPlace),0, 1);
      layout->addWidget(createCellWidget(tr("Ordinary Place"),DiagramItem::NormalPlace), 0, 2);
      layout->addWidget(createCellWidget(tr("Transition"), DiagramItem::Transition), 1, 0);

      arcButton = new QToolButton;
      arcButton->setCheckable(true);
 /*
      arcButton->setIcon(QIcon(QPixmap(":/images/arc.png")
                          .scaled(100, 100)));
 */

      QPixmap pixmap(100, 100);
      pixmap.fill(Qt::transparent);
      QPainter painter(&pixmap);
      painter.setPen(QPen(Qt::black, 4));
      painter.translate(50, 50);
      painter.drawLine(-42,-42, 42, 42);
      painter.drawLine(-42,-42, -42, -32);
      painter.drawLine(-42,-42, -32, -42);
      painter.drawLine(-42,-32, -32, -42);
      QIcon icon(pixmap);
      arcButton->setIcon(icon);

      arcButton->setIconSize(QSize(40, 40));
      connect( arcButton, SIGNAL(clicked()), this, SLOT(arcbuttonClicked()) );

      QGridLayout *arcLayout = new QGridLayout;
      arcLayout->addWidget(arcButton, 0, 0, Qt::AlignHCenter);

      QFont font("Times", 10, QFont::Bold);
      QLabel *label0 = new QLabel;
      label0->setText("Arc");
      label0->setFont(font);

      arcLayout->addWidget(label0, 1, 0, Qt::AlignCenter);
      QWidget *arcWidget = new QWidget;
      arcWidget->setLayout(arcLayout);
      layout->addWidget(arcWidget, 1, 1);


      tokenButton = new QToolButton;
      tokenButton->setCheckable(true);

      QPixmap pixmap2(100, 100);
      pixmap2.fill(Qt::transparent);
      QPainter painter2(&pixmap2);
      painter2.setPen(QPen(Qt::black, 2));
      painter2.setBrush(QBrush(Qt::black, Qt::SolidPattern));
      painter2.translate(50, 50);
      painter2.drawEllipse(-16, -40, 32, 32);
      painter2.drawEllipse(-42, 6, 32, 32);
      painter2.drawEllipse(10, 6, 32, 32);

      QIcon icon2(pixmap2);
      tokenButton->setIcon(icon2);

      tokenButton->setIconSize(QSize(40, 40));
      connect( tokenButton, SIGNAL(clicked()), this, SLOT(tokenbuttonClicked()) );

      QGridLayout *tokenLayout = new QGridLayout;
      tokenLayout->addWidget(tokenButton, 0, 0, Qt::AlignHCenter);

      label0 = new QLabel;
      label0->setText("Token");
      label0->setFont(font);

      tokenLayout->addWidget(label0, 1, 0, Qt::AlignCenter);
      QWidget *tokenWidget = new QWidget;
      tokenWidget->setLayout(tokenLayout);

      layout->addWidget(tokenWidget, 1, 2);

      layout->setHorizontalSpacing(0);
      layout->setVerticalSpacing(0);
      layout->setColumnStretch(0,10);
      layout->setColumnStretch(1,10);
      layout->setColumnStretch(2,10);

      QWidget *itemWidget = new QWidget;
      itemWidget->setLayout(layout);


      QGridLayout *saveLayout = new QGridLayout;

      QLabel *label3 = new QLabel("Tracking File:");
      //label3->setAlignment( Qt::AlignHCenter );
      label3->setFont(font);
      label3->adjustSize();

      QLabel *label4 = new QLabel("Starting Data File:");
      //label4->setAlignment( Qt::AlignHCenter );
      label4->setFont(font);
      label4->adjustSize();

      QLabel *label5 = new QLabel("Ending Data File:");
      //label5->setAlignment( Qt::AlignHCenter );
      label5->setFont(font);
      label5->adjustSize();

      QLabel *label6 = new QLabel("Precision:");
      //label6->setAlignment( Qt::AlignHCenter );
      label6->setFont(font);
      label6->adjustSize();

      QLabel *label7 = new QLabel("Increment:");
      //label7->setAlignment( Qt::AlignHCenter );
      label7->setFont(font);
      label7->adjustSize();

      QLabel *label8 = new QLabel("PN Variables:");
      label8->setFont(font);
      label8->adjustSize();

      QString temp = "";

      TrackingFile = new QLineEdit;
      TrackingFile->resize(10,20);
      TrackingFile->setText(temp);
      connect( TrackingFile, SIGNAL(textChanged(QString)), this, SLOT(TrackingFileSelected(QString)) );

      FirstDataFile = new QLineEdit;
      FirstDataFile->resize(10,20);
      FirstDataFile->setText(temp);
      connect( FirstDataFile, SIGNAL(textChanged(QString)), this, SLOT(FirstDataFileSelected(QString)) );

      LastDataFile = new QLineEdit;
      LastDataFile->resize(10,20);
      LastDataFile->setText(temp);
      connect( LastDataFile, SIGNAL(textChanged(QString)), this, SLOT(LastDataFileSelected(QString)) );

      precision = new QLineEdit;
      precision->resize(10,20);
      precision->setText(temp);
      connect( precision, SIGNAL(textChanged(QString)), this, SLOT(precisionInputed(QString)) );

      increment = new QLineEdit;
      increment->resize(10,20);
      increment->setText(temp);
      connect( increment, SIGNAL(textChanged(QString)), this, SLOT(incrementInputed(QString)) );

      PNvariables = new QLineEdit;
      PNvariables->resize(10,20);
      PNvariables->setText(temp);
      connect( PNvariables, SIGNAL(textChanged(QString)), this, SLOT(PNvariablesInputed(QString)) );


      browse1Button = new QToolButton;
      browse1Button->setCheckable(true);
      browse1Button->setIcon(QIcon(QPixmap("/waytottpncode/tv14/images/browse.png")
                          .scaled(200, 100)));
      browse1Button->setIconSize(QSize(20, 10));
      connect( browse1Button, SIGNAL(clicked()), this, SLOT(browseTrackingFile()) );


      browse2Button = new QToolButton;
      browse2Button->setCheckable(true);
      browse2Button->setIcon(QIcon(QPixmap("/waytottpncode/tv14/images/browse.png")
                          .scaled(200, 100)));
      browse2Button->setIconSize(QSize(20, 10));
      connect( browse2Button, SIGNAL(clicked()), this, SLOT(browseFirstDataFile()) );


      browse3Button = new QToolButton;
      browse3Button->setCheckable(true);
      browse3Button->setIcon(QIcon(QPixmap("/waytottpncode/tv14/images/browse.png")
                          .scaled(200, 100)));
      browse3Button->setIconSize(QSize(20, 10));
      connect( browse3Button, SIGNAL(clicked()), this, SLOT(browseLastDataFile()) );

      QHBoxLayout *hlayout = new QHBoxLayout;
      hlayout->addWidget(label6);
      hlayout->addWidget(precision);
      hlayout->addWidget(label7);
      hlayout->addWidget(increment);

      QHBoxLayout *hlayout2 = new QHBoxLayout;
      hlayout2->addWidget(label8);
      hlayout2->addWidget(PNvariables);

      saveLayout->addWidget(label3, 0, 0);
      saveLayout->addWidget(label4, 1, 0);
      saveLayout->addWidget(label5, 2, 0);

      saveLayout->addWidget(TrackingFile, 0, 1);
      saveLayout->addWidget(FirstDataFile, 1, 1);
      saveLayout->addWidget(LastDataFile, 2, 1);

      saveLayout->addWidget(browse1Button, 0, 2);
      saveLayout->addWidget(browse2Button, 1, 2);
      saveLayout->addWidget(browse3Button, 2, 2);


      saveLayout->addLayout(hlayout, 3, 0, 1, 2);
      saveLayout->addLayout(hlayout2, 4, 0, 1, 2);

      QWidget *saveWidget = new QWidget;
      saveWidget->setLayout(saveLayout);

      toolBox = new QToolBox;
      toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
      //toolBox->adjustSize();
      toolBox->setMinimumWidth(200);
      toolBox->setMinimumHeight(200);

      toolBox->addItem(itemWidget, tr("Primary Items"));
      //toolBox->addItem(operationWidget, tr("Basic Operations"));
      toolBox->addItem(saveWidget, tr("Configure File Information"));
  }


 void MainWindow::createActions()
 {
     toFrontAction = new QAction(QIcon("/waytottpncode/tv14/images/bringtofront.png"),
                                 tr("Bring to &Front"), this);
     toFrontAction->setShortcut(tr("Ctrl+F"));
     toFrontAction->setStatusTip(tr("Bring item to front"));
     connect(toFrontAction, SIGNAL(triggered()),
             this, SLOT(bringToFront()));

     sendBackAction = new QAction(QIcon("/waytottpncode/tv14/images/sendtoback.png"),
                                  tr("Send to &Back"), this);
     sendBackAction->setShortcut(tr("Ctrl+B"));
     sendBackAction->setStatusTip(tr("Send item to back"));
     connect(sendBackAction, SIGNAL(triggered()),
         this, SLOT(sendToBack()));

     deleteAction = new QAction(QIcon("/waytottpncode/tv14/images/delete.png"),
                                tr("&Delete"), this);
     deleteAction->setShortcut(tr("Delete"));
     deleteAction->setStatusTip(tr("Delete item from diagram"));
     connect(deleteAction, SIGNAL(triggered()),
         this, SLOT(deleteItem()));    

     boldAction = new QAction(tr("Bold"), this);
     boldAction->setCheckable(true);
     QPixmap pixmap("/waytottpncode/tv14/images/bold.png");
     boldAction->setIcon(QIcon(pixmap));
     boldAction->setShortcut(tr("Ctrl+B"));
     connect(boldAction, SIGNAL(triggered()),
             this, SLOT(handleFontChange()));

     italicAction = new QAction(QIcon("/waytottpncode/tv14/images/italic.png"),
                                tr("Italic"), this);
     italicAction->setCheckable(true);
     italicAction->setShortcut(tr("Ctrl+I"));
     connect(italicAction, SIGNAL(triggered()),
             this, SLOT(handleFontChange()));

     underlineAction = new QAction(QIcon("/waytottpncode/tv14/images/underline.png"),
                                   tr("Underline"), this);
     underlineAction->setCheckable(true);
     underlineAction->setShortcut(tr("Ctrl+U"));
     connect(underlineAction, SIGNAL(triggered()),
             this, SLOT(handleFontChange()));

     aboutAction = new QAction(tr("A&bout TTPN"), this);
     aboutAction->setShortcut(tr("Ctrl+B"));
     connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

     aboutQtAction = new QAction(tr("About Qt"), this);
     //aboutQtAction->setShortcut(tr("Ctrl+Q"));
     connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


     openConfAction = new QAction(QIcon("/waytottpncode/tv14/images/openText.png"), tr("&Open &Conf"), this);
     openConfAction->setShortcut(tr("Ctrl+C"));
     openConfAction->setStatusTip(tr("Open Saved *.Conf file"));
     connect(openConfAction, SIGNAL(triggered()), this, SLOT(openConfAction_triggered()));

     /*
     openTextAction = new QAction(QIcon("/waytottpncode/tv14/images/openText.png"), tr("&Open &Text"), this);
     openTextAction->setShortcut(tr("Ctrl+O"));
     openTextAction->setStatusTip(tr("Open Saved *.txt file"));
     connect(openTextAction, SIGNAL(triggered()), this, SLOT(openTextAction_triggered()));
     */

     saveAction = new QAction(QIcon("/waytottpncode/tv14/images/save.png"), tr("&Save"), this);
     saveAction->setShortcut(tr("Ctrl+S"));
     saveAction->setStatusTip(tr("Save *.txt file to current path"));
     connect(saveAction, SIGNAL(triggered()), this, SLOT(saveAction_triggered()));

     saveasAction = new QAction(QIcon("/waytottpncode/tv14/images/save.png"), tr("&Save &As"), this);
     saveasAction->setShortcut(tr("Ctrl+A"));
     saveasAction->setStatusTip(tr("Save *.txt file to chosen path"));
     connect(saveasAction, SIGNAL(triggered()), this, SLOT(saveasAction_triggered()));

     printAction = new QAction(QIcon("/waytottpncode/tv14/images/printer.png"), tr("&Print"), this);
     printAction->setShortcut(tr("Ctrl+P"));
     printAction->setStatusTip(tr("Print"));
     connect(printAction, SIGNAL(triggered()), this, SLOT(printAction_triggered()));

     exitAction = new QAction(QIcon("/waytottpncode/tv14/images/exit.png"), tr("&Exit"), this);
     exitAction->setShortcuts(QKeySequence::Quit);
     exitAction->setStatusTip(tr("Quit PNS"));
     connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

     grabAction = new QAction(QIcon("/waytottpncode/tv14/images/camera.png"), tr("Snapshot"), this);
     grabAction->setShortcut(tr("Ctrl+G"));
     grabAction->setStatusTip(tr("Take a snapshot of current view"));
     connect(grabAction, SIGNAL(triggered()), this, SLOT(grabAction_triggered()));

     clearAction = new QAction(QIcon("/waytottpncode/tv14/images/clear.png"), tr("Clear"), this);
     clearAction->setShortcut(tr("Ctrl+L"));
     clearAction->setStatusTip(tr("Clear current view"));
     connect(clearAction, SIGNAL(triggered()), this, SLOT(clearAction_triggered()));

     undoAction = new QAction(QIcon("/waytottpncode/tv14/images/undo.png"), tr("&Undo"), this);
     undoAction->setShortcut(tr("Ctrl+U"));
     undoAction->setStatusTip(tr("undo last operation"));

     redoAction = new QAction(QIcon("/waytottpncode/tv14/images/redo.png"), tr("&Redo"), this);
     redoAction->setShortcut(tr("Ctrl+R"));
     redoAction->setStatusTip(tr("redo last operation"));


     pointerAction = new QAction(QIcon("/waytottpncode/tv14/images/pointer2.png"), tr("&Move Item"), this);
     pointerAction->setStatusTip(tr("Move Item by mouse"));
     connect( pointerAction, SIGNAL(triggered()),this, SLOT(pointerbuttonClicked()) );

     clickAction = new QAction(QIcon("/waytottpncode/tv14/images/click2.png"), tr("Click P/T"), this);
     clickAction->setStatusTip(tr("Display information of the clicked place or transition"));
     connect( clickAction, SIGNAL(triggered()), this, SLOT(checkItem()) );

     clickArcAction = new QAction(QIcon("/waytottpncode/tv14/images/click.png"), tr("Click Arc"), this);
     clickArcAction->setStatusTip(tr("Display information of the clicked arc"));
     connect( clickArcAction, SIGNAL(triggered()), this, SLOT(checkArc()) );

     multiselectAction = new QAction(QIcon("/waytottpncode/tv14/images/multiselect.png"), tr("Multiple Selection"), this);
     multiselectAction->setStatusTip(tr("Select multiple items in the scene by mouse dragging"));
     connect( multiselectAction, SIGNAL(triggered()), this, SLOT(multiselectbuttonClicked()) );

     addTextAction = new QAction(QIcon("/waytottpncode/tv14/images/text.png"), tr("Add Text"), this);
     addTextAction->setStatusTip(tr("Add text label to the scene"));
     connect( addTextAction, SIGNAL(triggered()), this, SLOT(textbuttonClicked()) );

     addConditionAction = new QAction(QIcon("/waytottpncode/tv14/images/condition.png"), tr("Add Condition To Item"), this);
     addConditionAction->setStatusTip(tr("Add condition to the selected item"));
     connect( addConditionAction, SIGNAL(triggered()), this, SLOT(addCondition()) );

     rotateAction = new QAction(QIcon("/waytottpncode/tv14/images/rotate.png"), tr("Rotate Item Clockwise"), this);
     rotateAction->setStatusTip(tr("Rotate the selected item clockwise"));
     connect( rotateAction, SIGNAL(triggered()), this, SLOT(rotateItem()) );

     reverserotateAction = new QAction(QIcon("/waytottpncode/tv14/images/rotate2.png"), tr("Rotate Item Anti-clockwise"), this);
     reverserotateAction->setStatusTip(tr("Rotate the selected item anti-clockwise"));
     connect( reverserotateAction, SIGNAL(triggered()), this, SLOT(reverserotateItem()) );

     mode1Action = new QAction(QIcon("/waytottpncode/tv14/images/pnt.png"), tr("Switch to Mode1"), this);
     mode1Action->setStatusTip(tr("Switch the working mode to mode1"));
     connect( mode1Action, SIGNAL(triggered()), this, SLOT(ModeConfigure1()) );

     mode2Action = new QAction(QIcon("/waytottpncode/tv14/images/data.png"), tr("Switch to Mode2"), this);
     mode2Action->setStatusTip(tr("Switch the working mode to mode2"));
     connect( mode2Action, SIGNAL(triggered()), this, SLOT(ModeConfigure2()) );

 }


 void MainWindow::createMenus()
 {
     fileMenu = menuBar()->addMenu(tr("&File"));
     fileMenu->addAction(openConfAction);
     //fileMenu->addAction(openTextAction);
     fileMenu->addAction(saveAction);
     fileMenu->addAction(saveasAction);
     fileMenu->addAction(printAction);
     fileMenu->addAction(exitAction);

     toolMenu = menuBar()->addMenu(tr("&Tool"));
     toolMenu->addAction(undoAction);
     toolMenu->addAction(redoAction);
     toolMenu->addSeparator();
     toolMenu->addAction(grabAction);
     toolMenu->addAction(clearAction);

     itemMenu = menuBar()->addMenu(tr("&Item"));
     itemMenu->addAction(deleteAction);
     itemMenu->addAction(clickAction);
     itemMenu->addAction(clickArcAction);
     itemMenu->addAction(addConditionAction);
     itemMenu->addAction(rotateAction);
     itemMenu->addAction(reverserotateAction);
     //itemMenu->addSeparator();
     //itemMenu->addAction(toFrontAction);
     //itemMenu->addAction(sendBackAction);

     aboutMenu = menuBar()->addMenu(tr("&Help"));
     aboutMenu->addAction(aboutAction);
     aboutMenu->addAction(aboutQtAction);
 }


 void MainWindow::createToolbars()
 {
     fileToolBar = addToolBar(tr("File"));
     fileToolBar->addAction(openConfAction);
     //fileToolBar->addAction(openTextAction);
     fileToolBar->addAction(saveasAction);
     fileToolBar->addAction(printAction);
     fileToolBar->addAction(exitAction);

     toolToolBar = addToolBar(tr("Tool"));
     toolToolBar->addAction(undoAction);
     toolToolBar->addAction(redoAction);
     toolToolBar->addAction(grabAction);
     toolToolBar->addAction(clearAction);

     editToolBar = addToolBar(tr("Edit"));
     editToolBar->addAction(deleteAction);

     pointerButton = new QToolButton;
     pointerButton->setCheckable(true);
     pointerButton->setChecked(true);
     pointerButton->setIcon(QIcon("/waytottpncode/tv14/images/pointer.png"));
     pointerButton->setToolTip("Select/Move Item");
     connect( pointerButton, SIGNAL(clicked()),this, SLOT(pointerbuttonClicked()) );

     clickButton = new QToolButton;
     clickButton->setCheckable(true);
     clickButton->setIcon(QIcon("/waytottpncode/tv14/images/click2.png"));
     clickButton->setToolTip("Check Clicked Place/Transition");
     connect( clickButton, SIGNAL(clicked()), this, SLOT(clickbuttonClicked()) );

     arcInfoButton = new QToolButton;
     arcInfoButton->setCheckable(true);
     arcInfoButton->setIcon(QIcon("/waytottpncode/tv14/images/click.png"));
     arcInfoButton->setToolTip("Check Clicked Arc");
     connect( arcInfoButton, SIGNAL(clicked()),this, SLOT(arcinfobuttonClicked()) );

     textButton = new QToolButton;
     textButton->setCheckable(true);
     textButton->setIcon(QIcon("/waytottpncode/tv14/images/text.png"));
     textButton->setToolTip("Add Text Label");
     connect( textButton, SIGNAL(clicked()), this, SLOT(textbuttonClicked()) );

     multiselectButton = new QToolButton;
     multiselectButton->setCheckable(true);
     multiselectButton->setIcon(QIcon("/waytottpncode/tv14/images/multiselect.png"));
     multiselectButton->setToolTip("Multiple Selection");
     connect( multiselectButton, SIGNAL(clicked()), this, SLOT(multiselectbuttonClicked()) );

     rotateButton = new QToolButton;
     rotateButton->setCheckable(true);
     rotateButton->setIcon(QIcon("/waytottpncode/tv14/images/rotate.png"));
     rotateButton->setToolTip("Rotate Item Clockwise");
     connect( rotateButton, SIGNAL(clicked()), this, SLOT(rotatebuttonClicked()) );

     reverserotateButton = new QToolButton;
     reverserotateButton->setCheckable(true);
     reverserotateButton->setIcon(QIcon("/waytottpncode/tv14/images/rotate2.png"));
     reverserotateButton->setToolTip("Rotate Item Anticlockwise");
     connect( reverserotateButton, SIGNAL(clicked()), this, SLOT(reverserotatebuttonClicked()) );

     editToolBar->addWidget(pointerButton);
     editToolBar->addWidget(clickButton);
     editToolBar->addWidget(arcInfoButton);
     editToolBar->addWidget(textButton);
     editToolBar->addWidget(multiselectButton);
     editToolBar->addWidget(rotateButton);
     editToolBar->addWidget(reverserotateButton);

     fontCombo = new QFontComboBox();
     connect(fontCombo, SIGNAL(currentFontChanged(QFont)), this, SLOT(currentFontChanged(QFont)));

     fontSizeCombo = new QComboBox;
     fontSizeCombo->setEditable(true);
     for (int i = 8; i < 30; i = i + 2)
         fontSizeCombo->addItem(QString().setNum(i));
     QIntValidator *validator = new QIntValidator(2, 64, this);
     fontSizeCombo->setValidator(validator);
     connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(fontSizeChanged(QString)));

     fontColorToolButton = new QToolButton;
     fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
     fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
     textAction = fontColorToolButton->menu()->defaultAction();
     fontColorToolButton->setIcon(createColorToolButtonIcon("/waytottpncode/tv14/images/textpointer.png", Qt::black));
     fontColorToolButton->setAutoFillBackground(true);
     fontColorToolButton->setToolTip("Set Color to Text");
     connect(fontColorToolButton, SIGNAL(clicked()), this, SLOT(textButtonTriggered()));

     fillColorToolButton = new QToolButton;
     fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
     fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
     fillAction = fillColorToolButton->menu()->defaultAction();
     fillColorToolButton->setIcon(createColorToolButtonIcon("/waytottpncode/tv14/images/floodfill.png", QColor(125,125,255,255)));
     fillColorToolButton->setToolTip("Set Color to Place/Transition");
     connect(fillColorToolButton, SIGNAL(clicked()), this, SLOT(fillButtonTriggered()));

     lineColorToolButton = new QToolButton;
     lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
     lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
     lineAction = lineColorToolButton->menu()->defaultAction();
     lineColorToolButton->setIcon(createColorToolButtonIcon("/waytottpncode/tv14/images/linecolor.png", Qt::black));
     lineColorToolButton->setToolTip("Set Color to Arc");
     connect(lineColorToolButton, SIGNAL(clicked()), this, SLOT(lineButtonTriggered()));


     textToolBar = addToolBar(tr("Font"));
     //textToolBar = new QToolBar(tr("Font"));
     textToolBar->addWidget(fontCombo);
     textToolBar->addWidget(fontSizeCombo);
     textToolBar->addAction(boldAction);
     textToolBar->addAction(italicAction);
     textToolBar->addAction(underlineAction);
     //textToolBar->setAllowedAreas(Qt::RightToolBarArea | Qt::LeftToolBarArea);
     addToolBar(textToolBar);
     //textToolBar->setMovable(true);
     //textToolBar->setFloatable(true);

     colorToolBar = addToolBar(tr("Color"));
     colorToolBar->addWidget(fontColorToolButton);
     colorToolBar->addWidget(fillColorToolButton);
     colorToolBar->addWidget(lineColorToolButton);


     sceneScaleCombo = new QComboBox;
     QStringList scales;
     scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
     sceneScaleCombo->addItems(scales);
     sceneScaleCombo->setCurrentIndex(2);
     sceneScaleCombo->setToolTip("Set Screen Scale");
     connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),this, SLOT(sceneScaleChanged(QString)));

     pointerToolbar = addToolBar(tr("Pointer type"));
     pointerToolbar->addWidget(sceneScaleCombo);
 /*  setOpen()
     modeToolBar = addToolBar(tr("Working Mode"));
     modeToolBar->addAction(mode1Action);
     modeToolBar->addAction(mode2Action);
*/
 }


void MainWindow::createToolbars2()
{      
     dataloadbutton = new QPushButton;
     dataloadbutton->setText("Data");
     connect(this->dataloadbutton, SIGNAL(clicked()), this, SLOT(setOpen()));

     eventbutton = new QPushButton;
     eventbutton->setText("Event");
     connect(this->eventbutton, SIGNAL(clicked()), this, SLOT(setEvent()));

     nextframebutton = new QPushButton;
//     nextframebutton->setText("Next");
     nextframebutton->setIcon(QIcon("/waytottpncode/tv14/images/nextframe.png"));
     connect(this->nextframebutton, SIGNAL(clicked()), this, SLOT(showNext()));

     previousframebutton = new QPushButton; 
//     previousframebutton->setText("Last");
     previousframebutton->setIcon(QIcon("/waytottpncode/tv14/images/lastframe.png"));
     connect(this->previousframebutton, SIGNAL(clicked()), this, SLOT(showLast()));

     fastforwardbutton = new QPushButton;
     fastforwardbutton->setIcon(QIcon("/waytottpncode/tv14/images/fastforward.png"));
     connect(this->fastforwardbutton, SIGNAL(clicked()), this, SLOT(Playforward()));

     fastbackwardbutton = new QPushButton;
     fastbackwardbutton->setIcon(QIcon("/waytottpncode/tv14/images/fastbackward.png"));
     connect(this->fastbackwardbutton, SIGNAL(clicked()), this, SLOT(Playbackward()));

     playbutton = new QPushButton;
     playbutton->setIcon(QIcon("/waytottpncode/tv14/images/playandstop.png"));
     connect(this->playbutton, SIGNAL(clicked()), this, SLOT(playandstop()));

     barchartButton = new QToolButton;
     barchartButton->setCheckable(true);
     barchartButton->setIcon(QIcon("/waytottpncode/tv14/images/barchart.png"));
     barchartButton->setToolTip("Display Bar Chart In A Window");
     connect( barchartButton, SIGNAL(clicked()), this, SLOT(barchartbuttonClicked()) );

     filterButton = new QToolButton;
     filterButton->setCheckable(true);
     filterButton->setIcon(QIcon("/waytottpncode/tv14/images/filter.png"));
     filterButton->setToolTip("Visualize Activity Actors of Current State");
     connect( filterButton, SIGNAL(clicked()), this, SLOT(filterbuttonClicked()) );



     activitystateButton = new QToolButton;
     activitystateButton->setCheckable(true);
     activitystateButton->setIcon(QIcon("/waytottpncode/tv14/images/states.png"));
     activitystateButton->setToolTip("Visualize Current Activity State");
     connect( activitystateButton, SIGNAL(clicked()), this, SLOT(activitystatebuttonClicked()) );




     backButton = new QToolButton;
     backButton->setCheckable(true);
     backButton->setIcon(QIcon("/waytottpncode/tv14/images/goback.png"));
     backButton->setToolTip("Go Back To Original Visualization");
     connect( backButton, SIGNAL(clicked()), this, SLOT(backbuttonClicked()) );     

     multiviewButton = new QToolButton;
     multiviewButton->setCheckable(true);
     multiviewButton->setIcon(QIcon("/waytottpncode/tv14/images/grid.png"));
     multiviewButton->setToolTip("Show All Frames Of Current Event");
     connect( multiviewButton, SIGNAL(clicked()), this, SLOT(multiviewbuttonClicked()) );  

     pnButton = new QToolButton;
     pnButton->setCheckable(true);
     pnButton->setText("RunPN");
     pnButton->setIcon(QIcon("/waytottpncode/tv14/images/pn.png"));
     pnButton->setToolTip("Run Sedat's Peti Net code");
     connect( pnButton, SIGNAL(clicked()), this, SLOT(runPN()) );  

     nextframebutton->setDisabled(true);
     previousframebutton->setDisabled(true);
     fastforwardbutton->setDisabled(true);
     fastbackwardbutton->setDisabled(true);
     //playbutton->setDisabled(true);
     eventbutton->setDisabled(true);
     barchartButton->setDisabled(true);
     filterButton->setDisabled(true);
     activitystateButton->setDisabled(true);
     backButton->setDisabled(true);
     multiviewButton->setDisabled(true);

     dataToolBar = new QToolBar(tr("Data Display")); 
     dataToolBar->addWidget(dataloadbutton);  
     dataToolBar->addWidget(eventbutton); 
     dataToolBar->addWidget(fastbackwardbutton);  
     dataToolBar->addWidget(previousframebutton);
     dataToolBar->addWidget(playbutton);
     dataToolBar->addWidget(nextframebutton);
     dataToolBar->addWidget(fastforwardbutton);
     dataToolBar->addWidget(barchartButton);
     dataToolBar->addWidget(filterButton);
     dataToolBar->addWidget(activitystateButton);
     dataToolBar->addWidget(backButton);
     dataToolBar->addWidget(multiviewButton);
     dataToolBar->addWidget(pnButton);

     modeToolBar = new QToolBar(tr("Working Mode"));
     modeToolBar->addAction(mode1Action);
     modeToolBar->addAction(mode2Action);
 }


 QWidget *MainWindow::createCellWidget(const QString &text, DiagramItem::DiagramType type)
 {

     DiagramItem item(type, itemMenu);
     QIcon icon(item.image());

     QToolButton *button = new QToolButton;
     button->setIcon(icon);
     button->setIconSize(QSize(40, 40));
     button->setCheckable(true);
     buttonGroup->addButton(button, int(type));

     QGridLayout *layout = new QGridLayout;
     layout->addWidget(button, 0, 0, Qt::AlignHCenter);

     QFont font("Times", 10, QFont::Bold);
     QLabel *label = new QLabel;
     label->setText(text);
     label->setFont(font);

     layout->addWidget(label, 1, 0, Qt::AlignCenter);

     QWidget *widget = new QWidget;
     widget->setLayout(layout);

     return widget;
 }


 QMenu *MainWindow::createColorMenu(const char *slot, QColor defaultColor)
 {
     QList<QColor> colors;
     colors << Qt::white << Qt::yellow << QColor("orange") << Qt::green << QColor("pink") << QColor(255,50,50,255) << Qt::red << QColor(75,125,255,255) << Qt::blue << QColor("purple") << Qt::gray << Qt::black;
     QStringList names;
     names << tr("white") << tr("yellow") << tr("orange") << tr("green") << tr("pink") << tr("light red") << tr("red") << tr("light blue") << tr("blue") << tr("purple") << tr("gray") << tr("black");

     QMenu *colorMenu = new QMenu(this);
     for (int i = 0; i < colors.count(); ++i)
     {
         QAction *action = new QAction(names.at(i), this);
         action->setData(colors.at(i));
         action->setIcon(createColorIcon(colors.at(i)));
         connect(action, SIGNAL(triggered()), this, slot);
         colorMenu->addAction(action);
         if (colors.at(i) == defaultColor)
         {
             colorMenu->setDefaultAction(action);
         }
     }
     return colorMenu;
 }


 QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile, QColor color)
 {
     QPixmap pixmap(50, 80);
     pixmap.fill(Qt::transparent);
     QPainter painter(&pixmap);
     QPixmap image(imageFile);
     QRect target(0, 0, 50, 60);
     QRect source(0, 0, 42, 42);
     painter.fillRect(QRect(0, 60, 50, 80), color);
     painter.drawPixmap(target, image, source);

     return QIcon(pixmap);
 }


 QIcon MainWindow::createColorIcon(QColor color)
 {
     QPixmap pixmap(20, 20);
     QPainter painter(&pixmap);
     painter.setPen(Qt::NoPen);
     painter.fillRect(QRect(0, 0, 20, 20), color);

     return QIcon(pixmap);
 }


 bool MainWindow::maybeSave()
 {
     if(scene->isModified())
     {
         QMessageBox::StandardButton box;
         box = QMessageBox::warning(this,tr("Save File"),tr("The file has been changed, do you want to save?"), QMessageBox::Yes|    QMessageBox::No|QMessageBox::Cancel);

         if(box == QMessageBox::Yes)
         {
             return doFileSave();
         }
         else if(box == QMessageBox::Cancel)
         {
             return false;
         }
     }

     return true;
 }


 bool MainWindow::doFileSave()
 {
     if(isSaved)
     {
        return saveFile(currentFilepath);
     }
     else
        return doFileSaveAs();
 }


 bool MainWindow::saveFile(QString fileName)
 {
     /*
     if(scene->saveConf(fileName,"Conf"))
     {
        isSaved = true;
        return true;
     }
     else
        return false;
        */
     scene->saveConf(fileName,"Conf");
     isSaved = true;
     return true;
 }


 bool MainWindow::doFileSaveAs()
 {
     QString caption = tr("Save As");
     QString currentDirectory = QDir::current().path();
     QString filter = tr("Conf files (*.Conf)");
     QString fileName = QFileDialog::getSaveFileName (this, caption, currentDirectory, filter);

     if(fileName.isEmpty())
     {
        return false;
     }
     else
        return saveFile(fileName);
 }


 void MainWindow::closeEvent(QCloseEvent *event)   //关闭事件
 {
     if(maybeSave())
         qApp->quit();
     else event->ignore();
 }


void MainWindow::saveAction_triggered()
{
    doFileSave();
}


void MainWindow::saveasAction_triggered()
{
    doFileSaveAs();
}



void MainWindow::clearAction_triggered()
{
    scene->cleareverything();

    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons)
    {
        button->setChecked(false);
    }

    arcButton->setChecked(false);
    tokenButton->setChecked(false);
    clickButton->setChecked(false);
    pointerButton->setChecked(false);
    arcInfoButton->setChecked(false);
    textButton->setChecked(false);
    multiselectButton->setChecked(false);
    rotateButton->setChecked(false);
    reverserotateButton->setChecked(false);

    scene->setMode(DiagramScene::MoveItem);
    pointerButton->setChecked(true);
}



void MainWindow::doOpenConf()
{
    if(maybeSave())
    {
        SFDFileDialog *dialog = new SFDFileDialog(this, "Open Conf files:", QDir::currentPath(), tr("Conf files (*.Conf)"));
        dialog->setFileMode(QFileDialog::ExistingFiles);

        if( !dialog->exec() )
        {
            qDebug() << "You select nothing.";
            return;
        }

        QStringList fileNames = dialog->selectedFiles();
        QString fileName = fileNames.at(0);
        QByteArray ba = fileName.toLatin1();
        char *name = ba.data();

        scene->f = name;
        scene->refresh();   
    }
}


void MainWindow::openConfAction_triggered()
{
    doOpenConf();

    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons)
    {
        button->setChecked(false);
    }

    arcButton->setChecked(false);
    tokenButton->setChecked(false);
    clickButton->setChecked(false);
    pointerButton->setChecked(false);
    arcInfoButton->setChecked(false);
    textButton->setChecked(false);
    multiselectButton->setChecked(false);
    rotateButton->setChecked(false);
    reverserotateButton->setChecked(false);

    scene->setMode(DiagramScene::MoveItem);
    pointerButton->setChecked(true);
}


void MainWindow::printAction_triggered()
{
    //scene->addRect(QRectF(2000, 2000, 2200, 2200), QPen(Qt::black), QBrush(Qt::green));

     //QPrinter printer;
     QPrinter printer(QPrinter::HighResolution);
     //printer.setPaperSize(QPrinter::A4);

     if (QPrintDialog(&printer).exec() == QDialog::Accepted)
     {
         QPainter painter(&printer);
         painter.setRenderHint(QPainter::Antialiasing);

         scene->setMode(DiagramScene::MultiSelect);

         scene->setSceneRect(500,0,1000,800);
         scene->render(&painter);
     }

     scene->setSceneRect(0,0,2000,800);
}


void MainWindow::grabAction_triggered()
{
/*
    QString fileName = QFileDialog::getSaveFileName(this, "Save Scene", "", "Image (*.png)");
    string name = fileName.toLatin1().data();
    string format = ".png";
    QString fn = QString::fromStdString(name+format);
*/

    QString caption = tr("Save Snapshot");
    QString currentDirectory = QDir::current().path();
    QString filter = tr("PNG files (*.png)");
    QString fileName = QFileDialog::getSaveFileName (this, caption, currentDirectory, filter);

    QPixmap pixMap;
    //pixMap = QPixmap::grabWidget(this);


    if(workingmode==1)
//        pixMap = polybrowser->grab();
       pixMap = QPixmap::grabWidget(view);
    else if(workingmode==2)
    {
//        pixMap = polybrowser->grab();
       pixMap = QPixmap::grabWidget(modewidget2, 350, 20, 830, 620);
       //pixMap = QPixmap::grabWidget(modewidget2, 340, 160, 855, 450);
    }


    pixMap.save(fileName);

}



void MainWindow::browseTrackingFile()
{
    QFileDialog *dialog = new QFileDialog;
    QString fileName = dialog->getOpenFileName(this, tr("Open Tracking File"), QDir::currentPath());

    QDesktopServices::openUrl(QUrl(fileName));

    browse1Button->setChecked(false);
    TrackingFile->setText(fileName);
}



void MainWindow::browseFirstDataFile()
{
    QFileDialog *dialog = new QFileDialog;
    QString fileName = dialog->getOpenFileName(this, tr("Open First Data File"), QDir::currentPath());

    QDesktopServices::openUrl(QUrl(fileName));

    browse2Button->setChecked(false);
    FirstDataFile->setText(fileName);
}


void MainWindow::browseLastDataFile()
{

    QFileDialog *dialog = new QFileDialog;
    QString fileName = dialog->getOpenFileName(this, tr("Open Last Data File"), QDir::currentPath());

    QDesktopServices::openUrl(QUrl(fileName));

    browse3Button->setChecked(false);
    LastDataFile->setText(fileName);
}


void MainWindow::TrackingFileSelected(QString)
{
    QString t = TrackingFile->text();
    scene->TrackingFileName = t.toUtf8().constData();
}


void MainWindow::FirstDataFileSelected(QString)
{
    QString t = FirstDataFile->text();
    string str = t.toUtf8().constData();

    int str_size = str.size();
    unsigned int pos1 = str.find_last_of(".");
    unsigned int pos2 = str.find_last_of("/\\");

    string extention = str.substr(pos1, str_size-pos1);
    string filename = str.substr(pos2+1, pos1-pos2-1);
    //string filepath = str.substr(0, str_size-pos2-1);
    string filepath = str.substr(0, pos2+1);

    int pos = filename.find_first_of("0123456789");
    string number;

    while (pos!= string::npos)
    {
        number = number + filename[pos];
        pos = filename.find_first_of("0123456789", pos+1);
    }

    int len1 = filename.length();
    int len2 = number.length();
    string basename = filename.substr(0, len1-len2);

    scene->FilesPath = filepath;
    scene->TransitionsFileBaseName = basename;
    scene->TransitionsFileExtension = extention;
    scene->PlacesFileBaseName = basename;
    scene->PlacesFileExtension = extention;
    scene->DefaultActionsFileBaseName = basename;
    scene->DefaultActionsFileExtention = extention;

    scene->InitialFrameNumber = number;
}


void MainWindow::LastDataFileSelected(QString)
{
    QString t = LastDataFile->text();
    string str = t.toUtf8().constData();

    int str_size = str.size();
    unsigned int pos1 = str.find_last_of(".");
    unsigned int pos2 = str.find_last_of("/\\");

    string extention = str.substr(pos1, str_size-pos1);
    string filename = str.substr(pos2+1, pos1-pos2-1);
    //string filepath = str.substr(0, str_size-pos2-1);
    string filepath = str.substr(0, pos2+1);

    int pos = filename.find_first_of("0123456789");
    string number;

    while (pos!= string::npos)
    {
        number = number + filename[pos];
        pos = filename.find_first_of("0123456789", pos+1);
    }

    int len1 = filename.length();
    int len2 = number.length();
    string basename = filename.substr(0, len1-len2);

    scene->FilesPath = filepath;
    scene->TransitionsFileBaseName = basename;
    scene->TransitionsFileExtension = extention;
    scene->PlacesFileBaseName = basename;
    scene->PlacesFileExtension = extention;
    scene->DefaultActionsFileBaseName = basename;
    scene->DefaultActionsFileExtention = extention;
    scene->FinalFrameNumber = number;
}


void MainWindow::precisionInputed(QString)
{
    QString t = precision->text();
    scene->precision = t.toUtf8().constData();
}


void MainWindow::incrementInputed(QString)
{
    QString t = increment->text();
    scene->increment = t.toUtf8().constData();
}


void MainWindow::PNvariablesInputed(QString)
{
    QString t = PNvariables->text();
    scene->Variables_string = t.toUtf8().constData();
}



void MainWindow::setOpen()
{ 
    file_dlg = new SFDFileDialog(this, "Select *.poly data files:", QDir::currentPath(), tr("Poly Data Files (*.poly)"));
    file_dlg->setFileMode(QFileDialog::ExistingFiles);

    scene->firstframe = true; 

    if( !file_dlg->exec() )
    {
        qDebug() << "You select nothing.";
        return;
    }

    dataNames = file_dlg->selectedFiles();
    totalframe = dataNames.size();
    currentframe = 0;
    dataName = dataNames.at(currentframe);
    QByteArray ba = dataName.toLatin1();
    char *name = ba.data();
    
    polybrowser->onOpen(name);
    datatable->setRow0(dataName);
    scene->setFrameBoard(currentframe);

    nextframebutton->setDisabled(false);
    //previousframebutton->setDisabled(false);
    fastforwardbutton->setDisabled(false);
    //fastbackwardbutton->setDisabled(false);
    //playbutton->setDisabled(false);
    eventbutton->setDisabled(false);

    startplay = true;    
}


void MainWindow::setEvent()
{ 
     SFDFileDialog *dialog = new SFDFileDialog(this, "Open Event List", QDir::currentPath(), tr("Event List (*.list)"));
     dialog->setFileMode(QFileDialog::ExistingFiles);

     if( !dialog->exec() )
     {
         qDebug() << "You select nothing.";
         return;
     }

     QMessageBox::StandardButton box;
     box = QMessageBox::warning(this,tr("Use Group Tracking"),tr("Does the event list use group ID as token ID?"), QMessageBox::Yes|   QMessageBox::No);

     if(box == QMessageBox::Yes)
        usegrouptracking = true;
     else if(box == QMessageBox::No)
        usegrouptracking = false;

     QStringList eventFiles = dialog->selectedFiles();
     QString eventFile = eventFiles.at(0);
     QByteArray ba = eventFile.toLatin1();
     char *name = ba.data();

     eventloaded = true;
     scene->firstevent = true; 
     scene->firstchart = true; 

     ifstream infile;
     vector<string> events;
     events.clear();
     infile.open(name);

     string event, temp;

     //Rewrite this part of code to use vector instead of "new memory" in heap.
     //The memory mgiht be much larger than we thought.
     eventFrame = new int [60000];
     eventObject = new int [60000];
     eventPlace = new int [60000];

     memset(eventFrame, 0, 60000);
     memset(eventObject, 0, 60000);
     memset(eventPlace, 0, 60000);
     num_eventFrame = 0;
     num_eventObject = 0;
     num_eventPlace = 0;

     int *eventFrame_eachline = new int [1000];
     int num_eventFrame_eachline, num_eventPlace_eachline, num_eventObject_eachline;

     int ff = -1;
     int line = 0;
     packetformation = new int [10000];
     memset(packetformation, -1, 10000);
     maxPlaceID = 0;

     singleactivity = new SingleActivity [10000];
     while(infile.peek()!=EOF)
     {

         getline(infile, event);
         stringstream os(event);

         events.push_back(event);
         memset(eventFrame_eachline, 0, 1000);
         num_eventFrame_eachline = 0;
         num_eventPlace_eachline = 0;
         num_eventObject_eachline = 0;

         singleactivity[line].count = 0;
         singleactivity[line].framelist = new int [1000];
         singleactivity[line].placelist = new int [1000];
         singleactivity[line].objectlist = new int [1000];
         singleactivity[line].filename.clear();
     
         int num = 0;
         while(os >> temp)
         {
             if(num%3==0)
             {
                eventObject[num_eventObject] = atoi(temp.c_str())-1;
                singleactivity[line].objectlist[num_eventObject_eachline] = eventObject[num_eventObject];
                num_eventObject++;
                num_eventObject_eachline++;
             }

             else if(num%3==1)
             {
                eventFrame[num_eventFrame] = atoi(temp.c_str())-1;
                singleactivity[line].framelist[num_eventFrame_eachline] = eventFrame[num_eventFrame];
                eventFrame_eachline[num_eventFrame_eachline] = eventFrame[num_eventFrame];
                dataName = dataNames.at(eventFrame[num_eventFrame]);
                singleactivity[line].filename.push_back(dataName.toUtf8().constData());
                num_eventFrame++;
                num_eventFrame_eachline++;

                if(atoi(temp.c_str())>totalframe)
                {
                   delete [] singleactivity;
                   eventloaded = false;
                   memset(eventFrame, 0, 60000);
                   memset(eventObject, 0, 60000);
                   memset(eventPlace, 0, 60000);
                   num_eventFrame = 0;
                   num_eventObject = 0;
                   num_eventPlace = 0;
                   cout<<"There are not enough data files loaded!"<<endl;
                   return;
                }
                ff = eventFrame[num_eventFrame-1];
             }           

             else if(num%3==2)            
             {
                eventPlace[num_eventPlace] = atoi(temp.c_str());            //Place是从1开始计数的
                singleactivity[line].placelist[num_eventPlace_eachline] = eventPlace[num_eventPlace];
                if(maxPlaceID < eventPlace[num_eventPlace])
                   maxPlaceID = eventPlace[num_eventPlace];
                num_eventPlace++;
                num_eventPlace_eachline++;
                singleactivity[line].count++;
             }

             num++;
          }  // end of while()

          int count_temp = singleactivity[line].count-1;

          if (singleactivity[line].placelist[0] < singleactivity[line].placelist[count_temp])
          {
             singleactivity[line].startframe = singleactivity[line].framelist[0];
             singleactivity[line].startplace = singleactivity[line].placelist[0];
             singleactivity[line].startobject = singleactivity[line].objectlist[0];
             singleactivity[line].endframe = singleactivity[line].framelist[count_temp];
             singleactivity[line].endplace = singleactivity[line].placelist[count_temp];
             singleactivity[line].endobject = singleactivity[line].objectlist[count_temp];
          }

          else if (singleactivity[line].placelist[0] > singleactivity[line].placelist[count_temp])
          {
             singleactivity[line].startframe = singleactivity[line].framelist[count_temp];
             singleactivity[line].startplace = singleactivity[line].placelist[count_temp];
             singleactivity[line].startobject = singleactivity[line].objectlist[count_temp];
             singleactivity[line].endframe = singleactivity[line].framelist[0];
             singleactivity[line].endplace = singleactivity[line].placelist[0];
             singleactivity[line].endobject = singleactivity[line].objectlist[0];
          }

          for (int j=0; j<num_eventFrame_eachline; j++)
          {
              if (ff < eventFrame_eachline[j])
                 ff = eventFrame_eachline[j];
          }

          packetformation[line] = ff;
          line++;
     } // end of while()

     singleactivity_num = line-1;
     multiviewButton->setDisabled(false);

     infile.close();
     buildFrameTables();

     if (usegrouptracking == true)
        buildPacketTables();

     collectActivities();

     packetformation2 = new int [1000];
     memset(packetformation2, 0, 1000);

     int i,j, frame;
     for (i=0; i<line-1; i++)
     {
         frame = packetformation[i];
         packetformation2[frame]++;
     }

     currentEvent = 0;
     showEvent = currentEvent;
     dataName = dataNames.at(currentEvent);
     ba = dataName.toLatin1();
     name = ba.data();

     if(usegrouptracking == true)
     {
        int packet_num = getObjectList(currentEvent);                                                  //用于reconbt数据
        int object_num = getObjectFromPacket(currentEvent, ObjectList, packet_num);                    //用于reconbt数据
        polybrowser->camera = polybrowser->renderer->GetActiveCamera();  
        polybrowser->onAnimation(name, object_num, PacketObjectList);                                  //用于reconbt数据      
        //for(int ii=0; ii<object_num; ii++)
            //cout << PacketObjectList[ii] << endl;
     }

     else if(usegrouptracking == false)
     {
        polybrowser->camera = polybrowser->renderer->GetActiveCamera();  
        polybrowser->onAnimation(name, table[currentEvent].object_num, table[currentEvent].objects);
     }

     datatable->setRow0(dataName);  
     int index;

     //cout << table[currentEvent].object_num << endl;

     for(index=0; index<table[currentEvent].object_num; index++)
     {
         int *rgb = new int [3];
         if(usegrouptracking == true)
            rgb = colormap_reader(dataName.toUtf8().constData(), colorlist[index]);                    //用于reconbt数据
         else if(usegrouptracking == false)
            rgb = colormap_reader(dataName.toUtf8().constData(), table[currentEvent].objects[index]);
        
         //cout << rgb[0] << " " << rgb[1] << " " << rgb[2] << endl;
         scene->addTokensToPlace(rgb, table[currentEvent].objects[index], table[currentEvent].places[index], index, currentEvent);
     }

     int maxtokennum_bynow = 0;
     foreach(QGraphicsItem *itemx, scene->items())
     {
        if(itemx->type() == DiagramItem::Type)
        {
           DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
           if(item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
           {
              for(int i=0; i<=currentEvent; i++)
              {
                  if(item->tokenNum[i]>maxtokennum_bynow)
                     maxtokennum_bynow = item->tokenNum[i];
              }
           }
         }
     }
    //qDebug() << "max token number: " << maxtokennum_bynow << endl;
    
    for(index=0; index<table[currentEvent].object_num; index++)
        scene->setBarChart(table[currentEvent].places[index], currentEvent, maxtokennum_bynow);    

    scene->setFrameBoard(currentEvent);   

    barchartButton->setDisabled(false);
    filterButton->setDisabled(false);
    activitystateButton->setDisabled(false);

    formationcount = packetformation2[currentEvent];
    scene->setActivityBoard(formationcount);

/*
    QString currentDirectory = QDir::current().path();
    QString figname = currentDirectory + "/fig" + QString::number(currentEvent) + ".png";
    QPixmap pixMap = QPixmap::grabWidget(modewidget2, 320, 20, 855, 640);
    pixMap.save(figname);
*/
}



void MainWindow::showNext()
{
    if(eventloaded == 0)
    {
       currentframe++;
       previousframebutton->setDisabled(false);
       fastbackwardbutton->setDisabled(false);

       if(currentframe<totalframe)
       {
           fastforwardbutton->setDisabled(false);
           nextframebutton->setDisabled(false);
           dataName = dataNames.at(currentframe);
           QByteArray ba = dataName.toLatin1();
           char *name = ba.data();
  
           polybrowser->camera = polybrowser->renderer->GetActiveCamera();     
           polybrowser->onAnimation(name, -1, ObjectList);
           datatable->setRow0(dataName);
           scene->setFrameBoard(currentframe);
       }

       else if(currentframe>=totalframe)
       {
           fastforwardbutton->setDisabled(true);
           nextframebutton->setDisabled(true);
           return;
       }
    }

    if(eventloaded == 1)
    {
        currentEvent++;
        showEvent = currentEvent;
        previousframebutton->setDisabled(false);
        fastbackwardbutton->setDisabled(false);

        if(currentEvent<totalframe)
        {

           fastforwardbutton->setDisabled(false);
           nextframebutton->setDisabled(false);
           dataName = dataNames.at(currentEvent);
           QByteArray ba = dataName.toLatin1();
           char *name = ba.data();
  
/*
           polybrowser->camera = polybrowser->renderer->GetActiveCamera();   
           polybrowser->onAnimation(name, table[currentEvent].object_num, table[currentEvent].objects);
           datatable->setRow0(dataName);
*/

           if(usegrouptracking == true)
           {
              int packet_num = getObjectList(currentEvent);                                                  //用于reconbt数据
              int object_num = getObjectFromPacket(currentEvent, ObjectList, packet_num);                    //用于reconbt数据
              polybrowser->camera = polybrowser->renderer->GetActiveCamera();  
              polybrowser->onAnimation(name, object_num, PacketObjectList);                                  //用于reconbt数据      
           }
           else if(usegrouptracking == false)
           {
              polybrowser->camera = polybrowser->renderer->GetActiveCamera();  
              polybrowser->onAnimation(name, table[currentEvent].object_num, table[currentEvent].objects);
           }

           datatable->setRow0(dataName);

           int index;
           bool maybeend = false;

           //cout << table[currentEvent].object_num << endl;
           for(index=0; index<table[currentEvent].object_num; index++)
           {
               
               int *rgb = new int [3];
               if(usegrouptracking == true)
                  rgb = colormap_reader(dataName.toUtf8().constData(), colorlist[index]);                    //用于reconbt数据
               else if(usegrouptracking == false)
                  rgb = colormap_reader(dataName.toUtf8().constData(), table[currentEvent].objects[index]);
               scene->addTokensToPlace(rgb, table[currentEvent].objects[index], table[currentEvent].places[index], index, currentEvent);
               //scene->setBarChart(table[currentEvent].places[index], currentEvent); 
      
               if( table[currentEvent].places[index] == maxPlaceID )
                   maybeend = true;
           }

           int maxtokennum_bynow = 0;
           foreach(QGraphicsItem *itemx, scene->items())
           {
                  if(itemx->type() == DiagramItem::Type)
                  {
                     DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
                     if(item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
                     {
                        for(int i=0; i<=currentEvent; i++)
                        {
                            if(item->tokenNum[i]>maxtokennum_bynow)
                               maxtokennum_bynow = item->tokenNum[i];
                        }
                     }
                   }
            }
            //qDebug() << "max token number: " << maxtokennum_bynow << endl;
    
            for(index=0; index<table[currentEvent].object_num; index++)
                scene->setBarChart(table[currentEvent].places[index], currentEvent, maxtokennum_bynow);    


            if(table[currentEvent].object_num==0)
               scene->clearToken();
            scene->setFrameBoard(currentEvent);     

            formationcount = formationcount + packetformation2[currentEvent];
            scene->setActivityBoard(formationcount);

/*
            QString currentDirectory = QDir::current().path();
            QString figname = currentDirectory + "/fig" + QString::number(currentEvent) + ".png";
            QPixmap pixMap = QPixmap::grabWidget(modewidget2, 320, 20, 855, 640);
            pixMap.save(figname);
*/
        } 

        else if(currentEvent>=totalframe)
        {
            fastforwardbutton->setDisabled(true);
            nextframebutton->setDisabled(true);
            return;
        }  
    }  
}


void MainWindow::showLast()
{
    if(eventloaded == 0)
    {
       currentframe--;
       nextframebutton->setDisabled(false);
       fastforwardbutton->setDisabled(false);

       if(currentframe>=0)
       {
          previousframebutton->setDisabled(false);
          dataName = dataNames.at(currentframe);
          QByteArray ba = dataName.toLatin1();
          char *name = ba.data();
        
          polybrowser->camera = polybrowser->renderer->GetActiveCamera();  
          polybrowser->onAnimation(name, -1, ObjectList);
          datatable->setRow0(dataName);
          scene->setFrameBoard(currentframe);
       }
    
       else if(currentframe<0)
       {
          fastbackwardbutton->setDisabled(true);
          previousframebutton->setDisabled(true);      
          return;
       }
    }

    if(eventloaded == 1)
    {
       currentEvent--;
       showEvent = currentEvent;
       nextframebutton->setDisabled(false);
       fastforwardbutton->setDisabled(false);

       if(currentEvent>=0)
       {
          previousframebutton->setDisabled(false);
          dataName = dataNames.at(currentEvent);
          QByteArray ba = dataName.toLatin1();
          char *name = ba.data();
/*
          polybrowser->camera = polybrowser->renderer->GetActiveCamera();  
          polybrowser->onAnimation(name, table[currentEvent].object_num, table[currentEvent].objects);      
          datatable->setRow0(dataName);
*/

          if(usegrouptracking == true)
          {
             int packet_num = getObjectList(currentEvent);                                                  //用于reconbt数据
             int object_num = getObjectFromPacket(currentEvent, ObjectList, packet_num);                    //用于reconbt数据
             polybrowser->camera = polybrowser->renderer->GetActiveCamera();  
             polybrowser->onAnimation(name, object_num, PacketObjectList);                                  //用于reconbt数据      
          }
          else if(usegrouptracking == false)
          {
             polybrowser->camera = polybrowser->renderer->GetActiveCamera();  
             polybrowser->onAnimation(name, table[currentEvent].object_num, table[currentEvent].objects);
          }
          datatable->setRow0(dataName);

          int index;
          bool maybeend = false;
          for(index=0; index<table[currentEvent].object_num; index++)
          {
              int *rgb = new int [3];
              if(usegrouptracking == true)
                 rgb = colormap_reader(dataName.toUtf8().constData(), colorlist[index]);                    //用于reconbt数据
              else if(usegrouptracking == false)
                 rgb = colormap_reader(dataName.toUtf8().constData(), table[currentEvent].objects[index]);
              scene->addTokensToPlace(rgb, table[currentEvent].objects[index], table[currentEvent].places[index], index, currentEvent);
              //scene->setBarChart(table[currentEvent].places[index], currentEvent);
               
              if( table[currentEvent].places[index] == maxPlaceID )
                  maybeend = true;
          }

          int maxtokennum_bynow = 0;
          foreach(QGraphicsItem *itemx, scene->items())
          {
                  if(itemx->type() == DiagramItem::Type)
                  {
                     DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemx);
                     if(item->ItemType()==1 || item->ItemType()==2 || item->ItemType()==3)
                     {
                        for(int i=0; i<=currentEvent; i++)
                        {
                            if(item->tokenNum[i]>maxtokennum_bynow)
                               maxtokennum_bynow = item->tokenNum[i];
                        }
                     }
                  }
          }
          //qDebug() << "max token number: " << maxtokennum_bynow << endl;
    
          for(index=0; index<table[currentEvent].object_num; index++)
              scene->setBarChart(table[currentEvent].places[index], currentEvent, maxtokennum_bynow);    


          if(table[currentEvent].object_num==0)
              scene->clearToken();
          scene->setFrameBoard(currentEvent);

          formationcount = formationcount - packetformation2[currentEvent];
          scene->setActivityBoard(formationcount);
       }
    
       else if(currentEvent<0)
       {
          fastbackwardbutton->setDisabled(true);
          previousframebutton->setDisabled(true);      
          return;
       }
    }
}


void MainWindow::Playforward()
{
    if(eventloaded == 0)
    {
       currentframe++;
       previousframebutton->setDisabled(false);
       fastbackwardbutton->setDisabled(false);

       while(currentframe<totalframe)
       {
             nextframebutton->setDisabled(false);
             dataName = dataNames.at(currentframe);
             QByteArray ba = dataName.toLatin1();
             char *name = ba.data();

             QFileInfo fi(name);
             QString PN_folder = fi.absolutePath()+"/screenshot/PN";
             mkdir(PN_folder.toStdString().c_str(), S_IRWXU);
             QString wf_folder = fi.absolutePath()+"/screenshot/with filter";
             mkdir(wf_folder.toStdString().c_str(), S_IRWXU);
             QString wo_folder = fi.absolutePath()+"/screenshot/without filter";
             mkdir(wo_folder.toStdString().c_str(), S_IRWXU);

  
             polybrowser->camera = polybrowser->renderer->GetActiveCamera(); 

             if(startplay == true)
             {    
                polybrowser->onAnimation(name, -1, ObjectList);
                datatable->setRow0(dataName);
                scene->setFrameBoard(currentframe);
                currentframe++;
                sleep(0.2);
             }
        }
    
        if(currentframe>=totalframe)
        {
           fastforwardbutton->setDisabled(true);
           nextframebutton->setDisabled(true);
           return;
        }
     }

     if(eventloaded == 1)
     {
        currentEvent++;
        previousframebutton->setDisabled(false);
        fastbackwardbutton->setDisabled(false);

        while(currentEvent<totalframe)
        {
           nextframebutton->setDisabled(false);
           dataName = dataNames.at(currentEvent);
           QByteArray ba = dataName.toLatin1();
           char *name = ba.data();
  
           polybrowser->camera = polybrowser->renderer->GetActiveCamera();     
           polybrowser->onAnimation(name, table[currentEvent].object_num, table[currentEvent].objects);
           datatable->setRow0(dataName);



           sleep(0.2);

           int index;
           for(index=0; index<table[currentEvent].object_num; index++)
           {
               int *rgb = new int [3];
               rgb = colormap_reader(dataName.toUtf8().constData(), table[currentEvent].objects[index]);
               scene->addTokensToPlace(rgb, table[currentEvent].objects[index], table[currentEvent].places[index], index, currentEvent);
           }
           scene->setFrameBoard(currentEvent);

           QPixmap pixMap;
           pixMap = modewidget2->grab();

           QFileInfo fi(name);
           QString PN_folder = fi.absolutePath()+"/screenshot/PN";
           mkdir(PN_folder.toStdString().c_str(), S_IRWXU);
           QString saveFileName = fi.fileName();
           QString save_PNname = PN_folder + "/" + saveFileName.left(saveFileName.length()-5) + "_PN.png";
           pixMap.save(save_PNname);


           currentEvent++;
        } 

        if(currentEvent>=totalframe)
        {
           fastforwardbutton->setDisabled(true);
           nextframebutton->setDisabled(true);
           return;
        }  
     }

}



void MainWindow::Playbackward()
{
    if(eventloaded == 0)
    {
       currentframe--;
       nextframebutton->setDisabled(false);
       fastforwardbutton->setDisabled(false);

       while(currentframe>=0)  
       {
             previousframebutton->setDisabled(false);
             dataName = dataNames.at(currentframe);
             QByteArray ba = dataName.toLatin1();
             char *name = ba.data();
  
             polybrowser->camera = polybrowser->renderer->GetActiveCamera();     
             polybrowser->onAnimation(name, -1, ObjectList);
             scene->setFrameBoard(currentframe);

             datatable->setRow0(dataName);

             sleep(0.2);
             currentframe--;
             if(startplay == false)
                return;

        }
    
        if(currentframe<0)
        {
           fastbackwardbutton->setDisabled(true);
           previousframebutton->setDisabled(true);
           return;
        }
     }


     if(eventloaded == 1)
     {
        currentEvent--;
        nextframebutton->setDisabled(false);
        fastforwardbutton->setDisabled(false);

        while(currentEvent>=0)
        {
           previousframebutton->setDisabled(false);
           dataName = dataNames.at(currentEvent);
           QByteArray ba = dataName.toLatin1();
           char *name = ba.data();
  
           polybrowser->camera = polybrowser->renderer->GetActiveCamera();       
           polybrowser->onAnimation(name, table[currentEvent].object_num, table[currentEvent].objects);
           datatable->setRow0(dataName);
           

           sleep(0.2);

           int index;
           for(index=0; index<table[currentEvent].object_num; index++)
           {
               int *rgb = new int [3];
               rgb = colormap_reader(dataName.toUtf8().constData(), table[currentEvent].objects[index]);
               scene->addTokensToPlace(rgb, table[currentEvent].objects[index], table[currentEvent].places[index], index, currentEvent);
           }

           scene->setFrameBoard(currentEvent);
           currentEvent--;
        } 

        if(currentEvent<0)
        {
           fastbackwardbutton->setDisabled(true);
           previousframebutton->setDisabled(true);
           return;
        }  
     }
}


void MainWindow::playandstop()
{
     if(startplay == true)
        startplay = false;
     else if(startplay == false)
        startplay = true;
}


//void MainWindow::sleep(unsigned msec)
//{

//    struct timespec req, rem;
//    int err;
//    req.tv_sec = msec / 1000;
//    req.tv_nsec = (msec % 1000) * 1000000;

//    while ((req.tv_sec != 0) || (req.tv_nsec != 0))
//    {
//        if (nanosleep(&req, &rem) == 0)
//            break;
//        err = errno;
 
//        if (err == EINTR)
//        {
//            req.tv_sec = rem.tv_sec;
//            req.tv_nsec = rem.tv_nsec;
//        }

//        break;
//    }

//}


/*
void MainWindow::printMouseCoordinate(int x, int y)
{
     cout << "Coordinate X = " << x << endl;
     cout << "Coordinate Y = " << y << endl;
}
*/


int* MainWindow::colormap_reader(string filename, int targetID)
{

    int pos = filename.find_first_of("0123456789");
    string number;
    string extension = ".txt";  

    while (pos!= string::npos)
    {
        number = number + filename[pos];
        pos = filename.find_first_of("0123456789", pos+1);
    }

    int filename_size = filename.size();
    int pos2 = filename.find_last_of("/");
    string filepath = filename.substr(0, pos2+1);
    string colormap = filepath + "ColorMap" + number + extension;

    //cout << colormap << endl;

    int r, g, b, o;
    int *color = new int [3];
    memset(color, 0, 3);

    int num = 0;

    string  buf;
    fstream pfile(colormap.c_str(),ios_base::in);  

    while(!pfile.eof())
    {
        getline(pfile,buf,'\n');
        istringstream  bufstr2(buf,ios_base::out);
        bufstr2 >> r >> g >> b >> o;

        if(num==targetID)
        {
           color[0] = r;
           color[1] = g;
           color[2] = b;
        }
        num++;
    }  

    pfile.close();

    return color;   
}



int MainWindow::getObjectList(int targetFrame)
{
     int i, temp;
     bool maybesame;
     int number = 0;
     ObjectList = new int [10000];
     memset(ObjectList, -1, 10000);
     PlaceList = new int [10000];
     memset(PlaceList, -1, 10000);
    
     for(i = 0; i < num_eventFrame; i++) 
     {
         if( eventFrame[i] == targetFrame )
         {
             if( number == 0 )
             {
                 ObjectList[number] = eventObject[i];
                 PlaceList[number] = eventPlace[i];
                 number++; 
             }

             else if( number > 0 )
             {
                  temp = number;
                  maybesame = 0;
                  while(temp>=0)
                  {
                      if(ObjectList[temp] == eventObject[i])
                         maybesame=1;
                      temp--;
                  }

                  if(maybesame == 0)
                  {
                     ObjectList[number] = eventObject[i];
                     PlaceList[number] = eventPlace[i];
                     number++;
                  }
             }
          }
     }  

     return number;  
}



int MainWindow::getObjectFromPacket(int targetFrame, int *packetlist, int packetcount)
{
    int i, j, temp;
    for(i=1; i<packetcount; i++)
        for(j=i; j>0; j--)
            if(packetlist[j] < packetlist[j-1])
            {
               temp = packetlist[j];
               packetlist[j] = packetlist[j-1];
               packetlist[j-1] = temp;
            }

    string fullname = dataNames.at(targetFrame).toUtf8().constData();

    int fullname_size = fullname.size();
    unsigned int pos1 = fullname.find_last_of(".");
    unsigned int pos2 = fullname.find_last_of("/\\");

    string extension = ".packet";
    string filename = fullname.substr(pos2+1, pos1-pos2-1);
    string filepath = fullname.substr(0, pos2+1);

    int pos = filename.find_first_of("0123456789");
    string number;

    while (pos!= string::npos)
    {
        number = number + filename[pos];
        pos = filename.find_first_of("0123456789", pos+1);
    }

    int len1 = filename.length();
    int len2 = number.length();
    string basename = filename.substr(0, len1-len2);
    string packet = filepath + basename + number + extension;

    float t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12;
    int t13;
    int num = 0;
    int line = 0;
    int index = 0;
    string  buf;
    fstream pfile(packet.c_str(),ios_base::in);  
    PacketObjectList = new int [5000];

    //PlaceList2 = new int [10000];
    //memset(PlaceList2, -1, 10000);

    colorlist = new int [1000];
    memset(colorlist, -1, 1000);  

    while(!pfile.eof())
    {
        getline(pfile,buf,'\n');
        if(line == packetlist[index])
        {
           istringstream bufstr2(buf,ios_base::out);
           bufstr2 >> t1 >> t2 >> t3 >> t4 >> t5 >> t6 >> t7 >> t8 >> t9 >> t10 >> t11 >> t12 >> t13;
           for(int i=0; i<t13; i++)
               bufstr2 >> PacketObjectList[num+i];
 
           colorlist[index] = PacketObjectList[num];
           num = num + t13;
           index++;
        }
        line++;       
    }

    return num;
}


void MainWindow::buildFrameTables()
{
     int i, j;
     int number = 0;

     table = new FrameTable [totalframe];
     
     for(i=0; i < totalframe; i++) 
     {
         table[i].objects = new int [10000];
         memset(table[i].objects, -1, 10000);
         table[i].places = new int [10000];
         memset(table[i].places, -1, 10000);
         table[i].object_num = 0;
     } 

     int current = 0;

     for(i=0; i < num_eventFrame; i++) 
     {
         current = eventFrame[i];

         number = getObjectList(current);
         table[current].object_num = number;
         table[current].objects = ObjectList;
         table[current].places = PlaceList;    
     }
}


void MainWindow::buildPacketTables()
{
     packettable = new PacketTable [totalframe];

     string fullname = dataNames.at(0).toUtf8().constData();
     int fullname_size = fullname.size();
     unsigned int pos1 = fullname.find_last_of(".");
     unsigned int pos2 = fullname.find_last_of("/\\");

     string extension = ".packet";
     string filename = fullname.substr(pos2+1, pos1-pos2-1);
     string filepath = fullname.substr(0, pos2+1);

     int pos = filename.find_first_of("0123456789");
     string number;

     while (pos!= string::npos)
     {
        number = number + filename[pos];
        pos = filename.find_first_of("0123456789", pos+1);
     }

     int len1 = filename.length();
     int len2 = number.length();
     string basename = filename.substr(0, len1-len2);

     float t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12;
     int t13;
    
     for (int i=0; i<totalframe; i++)
     {
         stringstream ss;  
         ss << (i+1);          
         number = ss.str();  

         string packetname = filepath + basename + number + extension;

         string  buf;
         fstream pfile(packetname.c_str(),ios_base::in);  
   
         packettable[i].packet_num = 0;
         packettable[i].packetlist.clear();

         while(!pfile.eof())
         {
            getline(pfile,buf,'\n');
            istringstream bufstr2(buf,ios_base::out);
            bufstr2 >> t1 >> t2 >> t3 >> t4 >> t5 >> t6 >> t7 >> t8 >> t9 >> t10 >> t11 >> t12 >> t13;
            Packet packet;
            packet.objectlist = new int [t13];
            packet.object_num = t13;
            for (int j=0; j<t13; j++)
            {
                int tempID;
                bufstr2 >> tempID;
                packet.objectlist[j] = tempID;
            }
            packettable[i].packetlist.push_back(packet);
            packettable[i].packet_num++;
         }
              
     }

}


void MainWindow::collectActivities()
{
     int i,j,k;
     int *visited_singleactivity = new int [singleactivity_num];

     for (i=0; i<singleactivity_num; i++)
         visited_singleactivity[i] = 0; 

     activity = new Activity [singleactivity_num];
     int number1 = 0;
     int number2;

     for (i=0; i<singleactivity_num; i++)
     { 
         number2 = 0;     
         if (visited_singleactivity[i]==0)
         {
            activity[number1].singleactivitylist = new int [500];
            activity[number1].singleactivitylist[number2] = i;            
            for (j=i+1; j<singleactivity_num; j++)
            {
                if ( (singleactivity[i].startobject == singleactivity[j].startobject) && (singleactivity[i].startframe == singleactivity[j].startframe) && (singleactivity[i].startplace == singleactivity[j].startplace) )
                {
                   number2++;
                   activity[number1].singleactivitylist[number2] = j;
                   visited_singleactivity[j] = 1;       
                }
            }
            
            activity[number1].singleactivity_num = number2+1;
            number1++;
          }
      }

      activity_num = number1;

      for (i=0; i<activity_num; i++)
      {
          int starting = 9999;
          int ending = -9999;
          int max_timestep = -9999;
          int chosen_singleactivity = -1;
          for (j=0; j<activity[i].singleactivity_num; j++)
          {
              k = activity[i].singleactivitylist[j];
              if (singleactivity[k].startframe<starting)
                 starting = singleactivity[k].startframe;
              if (singleactivity[k].endframe>ending)
                 ending = singleactivity[k].endframe;

              if (singleactivity[k].count>max_timestep)
              {
                 max_timestep = singleactivity[k].count;
                 chosen_singleactivity = k;
              }
          }
         
          activity[i].startframe = starting;
          activity[i].endframe = ending;
          activity[i].timestep = new TimeStep [max_timestep];
          activity[i].framelist = new int [max_timestep];
          activity[i].placelist = new int [max_timestep];
          activity[i].filename.clear();
          

          for (j=0; j<max_timestep; j++)
          {
              int c = singleactivity[chosen_singleactivity].count-1;
              if (singleactivity[chosen_singleactivity].placelist[c]<singleactivity[chosen_singleactivity].placelist[0])
              {
                 activity[i].framelist[max_timestep-1-j] = singleactivity[chosen_singleactivity].framelist[j];
                 activity[i].placelist[max_timestep-1-j] = singleactivity[chosen_singleactivity].placelist[j];
              }

              else
              {
                 activity[i].framelist[j] = singleactivity[chosen_singleactivity].framelist[j];
                 activity[i].placelist[j] = singleactivity[chosen_singleactivity].placelist[j];
              }
          }


          int oldframeID = -1;
          int oldplaceID = -1;
          int index = 0;
          for (j=0; j<max_timestep; j++)  
          {
              activity[i].timestep[j].objectlist.clear();
              activity[i].timestep[j].object_num = 0; 
              int currentframeID = activity[i].framelist[j];
              int currentplaceID = activity[i].placelist[j];

              if (currentframeID!=oldframeID || currentplaceID!=oldplaceID)
              {
                 dataName = dataNames.at(currentframeID);
                 activity[i].filename.push_back(dataName.toUtf8().constData());
                 for (int step=0; step<table[currentframeID].object_num; step++)
                 {
                     if (currentplaceID==table[currentframeID].places[step])
                     {
                        int currentobjectID = table[currentframeID].objects[step];
                        activity[i].timestep[index].objectlist.push_back(currentobjectID); 
                        activity[i].timestep[index].object_num++;
                        activity[i].timestep[index].PlaceID = currentplaceID;
                     }
                 }
                 index++;
                 oldframeID = currentframeID;
                 oldplaceID = currentplaceID;
              }
           }
              
      }

}


void MainWindow::collectActivities2()
{
     int i,j,k;
     activity = new Activity [singleactivity_num];
 
     for (i=0; i<singleactivity_num; i++)
     {      
         activity[i].singleactivitylist = new int [1];           
         activity[i].singleactivitylist[0] = i;                      
         activity[i].singleactivity_num = 1;
         activity[i].startframe = singleactivity[i].startframe;
         activity[i].endframe = singleactivity[i].endframe;
         int c = singleactivity[i].count-1;
         activity[i].framelist = new int [c+1];
         activity[i].placelist = new int [c+1];

         if (singleactivity[i].placelist[c]<singleactivity[i].placelist[0])
         {
            for (j=0; j<=c; j++)
            {
                activity[i].framelist[c-j] = singleactivity[i].framelist[j];
                activity[i].placelist[c-j] = singleactivity[i].placelist[j];
            }
         }

         else
         {
            for (j=0; j<=c; j++)
            {
                activity[i].framelist[j] = singleactivity[i].framelist[j];
                activity[i].placelist[j] = singleactivity[i].placelist[j];
            }
         }

         activity[i].timestep = new TimeStep [c+1];        
         activity[i].filename.clear();
         int oldframeID = -1;
         int oldplaceID = -1;
         int index = 0;

         for (j=0; j<=c; j++)  
         {
             activity[i].timestep[j].objectlist.clear();
             activity[i].timestep[j].object_num = 0; 
             int currentframeID = activity[i].framelist[j];
             int currentplaceID = activity[i].placelist[j];

             if (currentframeID!=oldframeID || currentplaceID!=oldplaceID)
             {
                dataName = dataNames.at(currentframeID);
                activity[i].filename.push_back(dataName.toUtf8().constData());
                for (int step=0; step<table[currentframeID].object_num; step++)
                {
                    if (currentplaceID==table[currentframeID].places[step])
                    { 
                       activity[i].timestep[index].PlaceID = currentplaceID;                    
                       if (usegrouptracking == false)
                       {
                          int currentobjectID = table[currentframeID].objects[step];
                          activity[i].timestep[index].objectlist.push_back(currentobjectID); 
                          activity[i].timestep[index].object_num++;
                       }
                       else if (usegrouptracking == true)
                       {
                          int currentpacketID = table[currentframeID].objects[step];
                          int object_contained_num = packettable[currentframeID].packetlist[currentpacketID].object_num;

                          for (int step2=0; step2<object_contained_num; step2++)
                          {
                              activity[i].timestep[index].objectlist.push_back(packettable[currentframeID].packetlist[currentpacketID].objectlist[step2]); 
                              activity[i].timestep[index].object_num++;
                          }
                       }
                    }
                }

                index++;
                oldframeID = currentframeID;
                oldplaceID = currentplaceID;
              }
          }
     }

     activity_num = singleactivity_num;
}



void MainWindow::collectActivities3()
{
     int i,j,k;
     activity = new Activity [singleactivity_num];
 
     for (i=0; i<singleactivity_num; i++)
     {      
         activity[i].singleactivitylist = new int [1];           
         activity[i].singleactivitylist[0] = i;                      
         activity[i].singleactivity_num = 1;
         activity[i].startframe = singleactivity[i].startframe;
         activity[i].endframe = singleactivity[i].endframe;
         int c = singleactivity[i].count-1;
         activity[i].framelist = new int [c+1];
         activity[i].placelist = new int [c+1];

         if (singleactivity[i].placelist[c]<singleactivity[i].placelist[0])
         {
            for (j=0; j<=c; j++)
            {
                activity[i].framelist[c-j] = singleactivity[i].framelist[j];
                activity[i].placelist[c-j] = singleactivity[i].placelist[j];
            }
         }

         else
         {
            for (j=0; j<=c; j++)
            {
                activity[i].framelist[j] = singleactivity[i].framelist[j];
                activity[i].placelist[j] = singleactivity[i].placelist[j];
            }
         }

         activity[i].timestep = new TimeStep [c+1];        
         activity[i].filename.clear();
         int oldframeID = -1;
         int oldplaceID = -1;
         int index = -1;

         for (j=0; j<=c; j++)  
         {
             activity[i].timestep[j].objectlist.clear();
             activity[i].timestep[j].object_num = 0; 
             int currentframeID = activity[i].framelist[j];
             int currentplaceID = activity[i].placelist[j];

             if (currentframeID!=oldframeID || currentplaceID!=oldplaceID)
             {
                index++;
                activity[i].timestep[index].PlaceID = currentplaceID;
                dataName = dataNames.at(currentframeID);
                activity[i].filename.push_back(dataName.toUtf8().constData());
             }
             
             if (usegrouptracking == false)
             {
                int currentobjectID;
                if (singleactivity[i].placelist[c]>singleactivity[i].placelist[0])
                   currentobjectID = singleactivity[i].objectlist[j];
                else if (singleactivity[i].placelist[c]<singleactivity[i].placelist[0])
                   currentobjectID = singleactivity[i].objectlist[c-j];
                activity[i].timestep[index].objectlist.push_back(currentobjectID); 
                activity[i].timestep[index].object_num++;
             }

             else if (usegrouptracking == true)
             {
                int currentpacketID;
                if (singleactivity[i].placelist[c]>singleactivity[i].placelist[0])
                   currentpacketID = singleactivity[i].objectlist[j];
                else if (singleactivity[i].placelist[c]<singleactivity[i].placelist[0])
                   currentpacketID = singleactivity[i].objectlist[c-j];
                int object_contained_num = packettable[currentframeID].packetlist[currentpacketID].object_num;

                for (int s=0; s<object_contained_num; s++)
                {
                    activity[i].timestep[index].objectlist.push_back(packettable[currentframeID].packetlist[currentpacketID].objectlist[s]); 
                    activity[i].timestep[index].object_num++;
                }
             }

             oldframeID = currentframeID;
             oldplaceID = currentplaceID;
         }
     }

     activity_num = singleactivity_num;      
/*
     for (i=0; i<=activity[0].endframe-activity[0].startframe; i++)
     {
         cout << activity[0].filename[i] << endl;
         int object_num = activity[0].timestep[i].object_num;
         for (j=0; j<object_num; j++)
             cout << activity[0].timestep[i].objectlist[j] << " ";
         cout << "\n";
     }
*/    
}



void MainWindow::connectTokenIDtableToBrowser(int r, int c)
{
     /*
     qDebug() << r << " " << c << endl;
     QTableWidgetItem *item = new QTableWidgetItem;
     item = tokenIDtable->item(r, c);
     QString qs = item->text();
     cout << qs.toUtf8().constData() << endl;
     */

     QTableWidgetItem *item = new QTableWidgetItem;
     item = tokenIDtable->item(r, 2);
     QString idstr = item->text();
     bool ok;
     int *id = new int [2];
     id[0] = 1;
     id[1] = idstr.toInt(&ok, 10);

     tokenIDtable->SendChosenID(id);
     connect(tokenIDtable, SIGNAL(sendID(int *)), polybrowser, SLOT(onFilteringOpen(int *)));
}


void MainWindow::connectActivitytableToMultiwindow(int r, int c)
{
     //cout << "r: " << r << " " << " c: " << c << endl;
     QMainWindow *myNewWindow = new QMainWindow;
     MultiviewWindow *mwin = new MultiviewWindow(activity[r], r);

     myNewWindow->setCentralWidget(mwin);
     myNewWindow->setWindowTitle(tr("Isolated Activity Visualization"));
     myNewWindow->show();

     multiviewButton->setChecked(false);
}


void MainWindow::showTokenIDtable(int PlaceIndex, int *tokenlist)
{
     dataName = dataNames.at(currentEvent);

     tokenIDtable = new TDtable;
     tokenIDtable->setRowCount(tokenlist[0]);
     tokenIDtable->setColumnCount(3);
     
     QFont fnt( "Times", 10, QFont::Bold );
     tokenIDtable->setFont(fnt);
     tokenIDtable->setMaximumWidth(600);
     tokenIDtable->setMaximumHeight(600);

     if (tokenlist[0]<=9)
        tokenIDtable->setMinimumWidth(250);
     else if (tokenlist[0]>9)
        tokenIDtable->setMinimumWidth(265);

     if (tokenlist[0]<=15)
        tokenIDtable->setMinimumHeight(40+tokenlist[0]*30);
     else if (tokenlist[0]>15)
        tokenIDtable->setMinimumHeight(500);

     tokenIDtable->setColumnWidth(0,25);
     tokenIDtable->setColumnWidth(1,105);
     tokenIDtable->setColumnWidth(2,105);

     for (int i=0; i<tokenlist[0]; i++)
     {
         int *rgb = new int [3];
         int objectID = tokenlist[i+1];
         rgb = colormap_reader(dataName.toUtf8().constData(), objectID);

         QPixmap pixmap2(80, 80);
         pixmap2.fill(Qt::transparent);
         QPainter painter2(&pixmap2);
         painter2.setPen(QPen(Qt::black, 1));
         painter2.setBrush(QBrush(QColor(rgb[0], rgb[1], rgb[2], 255), Qt::SolidPattern));
         painter2.translate(40, 40);
         painter2.drawEllipse(-40, -40, 80, 80);
         QIcon icon2(pixmap2);

         QTableWidgetItem *item1 = new QTableWidgetItem;
         item1->setIcon(icon2);
         item1->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         tokenIDtable->setItem(i, 0, item1);

         QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(PlaceIndex));
         item2->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         tokenIDtable->setItem(i, 1, item2);

         QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(objectID));
         item3->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
         tokenIDtable->setItem(i, 2, item3);
     }
 
     connect(tokenIDtable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(connectTokenIDtableToBrowser(int, int)));
     tokenIDtable->setWindowTitle("Token ID Table");
     QStringList header;
     header << " " << "Place ID" << "Token ID";
     tokenIDtable->setHorizontalHeaderLabels(header);
     //tokenIDtable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
     tokenIDtable->show();
     filterButton->setChecked(false);
}



void MainWindow::runPN()
{ 
     SFDFileDialog *dialog = new SFDFileDialog(this, "Please Choose Your Config File For Activity Detection", QDir::currentPath(), tr("Config File (*.Conf)"));
     dialog->setFileMode(QFileDialog::ExistingFiles);

     if( !dialog->exec() )
     {
         qDebug() << "You select nothing.";
         return;
     }

     QStringList eventFiles = dialog->selectedFiles();
     QString eventFile = eventFiles.at(0);
     QByteArray ba = eventFile.toLatin1();
     char *name = ba.data();

     string PNlabel = string(name);
     int str_size = PNlabel.size();
     int dashpos = PNlabel.find_last_of("/");
     string filepath = PNlabel.substr(0, dashpos+1);
     string EventFName = filepath + "event.list";
     //cout << EventFName.c_str() << endl;

     cout<<" ======== The Beginning of the Code ======== "<<endl;
     float thresh1, thresh2, deltaxval, deltayval, deltazval;
     int   InitialtimeStep, FinaltimeStep, SmallestObjVol, TimePrecision, TimeIncrement, currentTime;
     vector<string> allvariableNames;
     vector<stObject> totalObjects;
     vector<stObject> currentObjects;
     string file_name,datapath,FileBaseName,fileextension,trakTableFile,label;
     vector<string> trakTable;
     vector<string> dummytrakTable;
     vector<string> dummytokensforTrakTable;  
     vector<string> tokensforTrakTable;
     vector<int> objectNumbersInEachFrame;
     vector<int> currentActions;
     vector<int> ObjectIDinNextFrame;
     vector<float> totalActions;  // <================================ this can be removed in the future......... 
     //vector<StEvent> EventList;
     ThePetriNet CurrentPetriNet;
     vector<pnPlace> CurrentPlaces;
     vector<pnTransition> CurrentTransitions;
     vector<int> finalPlaces;
     vector<int> initialPlaces;
     int totalobjNumber = 0;
     bool a;
  
     vector<stObject> CurrentMarkings;
     vector<Arc_self> inputArcs;
     int PNumber, transitionNumber;
     vector<Arc_self> outputArcs;
     vector<string> transitionVector;
     vector<string> placeVector;
     vector<int> AttributeNumbers;
     placeVector.clear();  
     transitionVector.clear();
     int frame, attributenumber;
     PNAllVaribles ConfigFile;
     // read the petri net configuration...
     // string PNlabel = "/home/liliu/Conf/Vorts.Conf";
     readPT_ConfFile(PNlabel, ConfigFile,inputArcs,outputArcs);

     // ConfigFile.TrackingFileName = ConfigFile.FilePath+ConfigFile.TransitionFileBaseName+TtoS <int> ( (int) 1 ) +".trakTable";  
     CurrentPetriNet.ConfigFileInfo = ConfigFile;  

     cout<<" ConfigFile.TrackingFileName is: "<<CurrentPetriNet.ConfigFileInfo.TrackingFileName   <<endl;
     string file = string(CurrentPetriNet.ConfigFileInfo.FilePath + CurrentPetriNet.ConfigFileInfo.TransitionFileBaseName).c_str();
  
     string fileExtention = CurrentPetriNet.ConfigFileInfo.TransitionFileExtension;
  
  //-------------------- Create the petri net and test it --------------------
  createPT(CurrentPetriNet,inputArcs, outputArcs); // AttributeNumbers is obsolete... Can be removed in the future...

  testCreatedPT(CurrentPetriNet); // FOR DEBUGGING.. Prints all the values in the petriNet...

  //-------------------- Create the petri net and test it --------------------
  
  InitialtimeStep = ConfigFile.initialFrameNumber;
  
  FinaltimeStep = ConfigFile.FinalFrameNumber;  
  string DefaultActionsFileExtention = CurrentPetriNet.ConfigFileInfo.DefaultActionsFileExtention.c_str();
  string placefilename =  string(CurrentPetriNet.ConfigFileInfo.FilePath + CurrentPetriNet.ConfigFileInfo.PlaceFileBaseName).c_str();//CurrentPetriNet.ConfigFileInfo.FilePath + CurrentPetriNet.ConfigFileInfo.TransitionFileBaseName;
  string placefileextension = string(CurrentPetriNet.ConfigFileInfo.PlaceFileExtension).c_str(); 
  string transitionfilename = string(CurrentPetriNet.ConfigFileInfo.FilePath + CurrentPetriNet.ConfigFileInfo.TransitionFileBaseName).c_str();//CurrentPetriNet.ConfigFileInfo.FilePath + CurrentPetriNet.ConfigFileInfo.TransitionFileBaseName;
  string transitionfileextension = string(CurrentPetriNet.ConfigFileInfo.TransitionFileExtension).c_str();
  string fullplaceFileName;
  string fulltransitionFileName;
 vector<string> CurrentFramefullTransitionAttributeFile;
 vector<string> PreviousFramefullTransitionAttributeFile;
 vector<string> CurrentFramefullPlaceAttributeFile;
 vector<string> PreviousFramefullPlaceAttributeFile;   
  
 
  //remember trakTable starts from the second frame... for each frame read the trak and event files and update the objectIDs in the places and then re-evaluate the PetriNet
  for (int currentFrame = InitialtimeStep ; currentFrame < FinaltimeStep+1; currentFrame++)
  {
    cout<<"Inside the main and the for loop, the currentFrame is: "<< currentFrame <<endl;
    fullplaceFileName = string(placefilename + TtoS <int> ( (int) currentFrame ) + placefileextension).c_str();
    fulltransitionFileName = string(transitionfilename+ TtoS <int> ( (int) currentFrame ) +transitionfileextension).c_str();
    FileBaseName = transitionfilename + TtoS <int> ( (int) currentFrame ); //<=============== use the eventfilename inthe future instead of transitionfilename
    string EventFileName = FileBaseName+DefaultActionsFileExtention;
    currentObjects.clear();
    currentActions.clear();
    CurrentPetriNet.PreviousFramefullTransitionAttributeFile = CurrentPetriNet.CurrentFramefullTransitionAttributeFile;
    CurrentPetriNet.PreviousFramefullPlaceAttributeFile = CurrentPetriNet.CurrentFramefullPlaceAttributeFile;
  
    readFile(fulltransitionFileName, CurrentFramefullTransitionAttributeFile);

    CurrentPetriNet.CurrentFramefullTransitionAttributeFile = CurrentFramefullTransitionAttributeFile;
    if (fullplaceFileName == fulltransitionFileName)
      CurrentPetriNet.CurrentFramefullPlaceAttributeFile = CurrentPetriNet.CurrentFramefullTransitionAttributeFile;
    else
    {
      readFile(fulltransitionFileName, CurrentFramefullPlaceAttributeFile);
      CurrentPetriNet.CurrentFramefullPlaceAttributeFile = CurrentFramefullPlaceAttributeFile;
    }
 
    CreateObjectsFortheCurrentFrame( fullplaceFileName, currentFrame, currentObjects); //get the objects and their attributes for the current 
    ReadCurrentFrameTrackInfo(CurrentPetriNet.ConfigFileInfo.TrackingFileName, currentFrame, trakTable); //returns current frame's trakTable 
    updateObjects2(CurrentPetriNet, currentObjects, currentFrame,initialPlaces,finalPlaces, InitialtimeStep, trakTable,trakTableFile );
    
    evaluatePT6(CurrentPetriNet,inputArcs, outputArcs,finalPlaces,currentFrame,InitialtimeStep,FinaltimeStep );  
 
    for(int dummyplaceID = 0; dummyplaceID < CurrentPetriNet.CurrentPlaces.size(); dummyplaceID++ )
    {
      cout<<"FrameNumber is["<<currentFrame <<"] Current PlaceID is["<< dummyplaceID+1<<"] and total Object number in this frame is: [" <<CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace.size() <<"] !!"<< endl;

      for(int dummyObj = 0;dummyObj < CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace.size(); dummyObj++ )
      {
        if (CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyObj].activeObject == 1 )
        {
          int realObId = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyObj].RealObjectID;
          int summariz = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyObj].ObjectSummary.size();
          cout<<"FrameNumber is["<<currentFrame <<"] Current PlaceID is["<< dummyplaceID+1<<"] and active ObjectID in this frame is: [" <<realObId <<"] and its ObjectSummary size is: ["<<summariz   <<"] !!"<< endl;
        }
      }      
    }    
  }

  cout<<"Before Generate_EventHistory!!! " <<endl; 
  Generate_EventHistory(EventFName, CurrentPetriNet);
  cout<<" ========= The End of the Code! ========= "<<endl;
}

