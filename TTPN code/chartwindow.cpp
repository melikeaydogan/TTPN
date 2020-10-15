#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

//#include <vtkChartXY.h>
//#include "myvtkChartXY.h"

#include <vtkPlot.h>
#include <vtkTable.h>
#include <vtkIntArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVersion.h>
#include <vtkAxis.h>

#include "chartwindow.h"
#include <vtkEventQtSlotConnect.h>
#include <QtGui>

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()


ChartWindow::ChartWindow(int *array)
{
     // Set up a 2D scene, add an XY chart to it
  VTK_CREATE(vtkContextView, view);
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

  vtkwidget = new QVTKWidget;
  vtkwidget->setFixedSize(480,360);
  view->SetRenderWindow(vtkwidget->GetRenderWindow());

  //VTK_CREATE(vtkChartXY, chart);
  //VTK_CREATE(myvtkChartXY, chart);

  chart = new myvtkChartXY;
  view->GetScene()->AddItem(chart);


  Connections = vtkEventQtSlotConnect::New();

  // get right mouse pressed with high priority
  Connections->Connect( vtkwidget->GetRenderWindow()->GetInteractor(), vtkCommand::RightButtonPressEvent, this, SLOT( displayindex() ) );

  // Create a table with some points in it...
  VTK_CREATE(vtkTable, table);

  VTK_CREATE(vtkIntArray, time);
  time->SetName("Frame");
  table->AddColumn(time);

  VTK_CREATE(vtkIntArray, token);
  token->SetName("Tokens");
  table->AddColumn(token);




/*
int *p1 = new int [600];
int *p2 = new int [600];
int *p3 = new int [600];

for (int i=0; i<600; i++)
{
    p1[i] = 0;
    p2[i] = 0;
    p3[i] = 0;
}


p1[0] = 2;
p1[200] = 2;
p1[400] = 2;

p2[1] = 1;
p2[201] = 1;
p2[401] = 1;

p1[2] = 2;
p1[202] = 2;
p1[402] = 2;

p3[2] = 2;
p3[202] = 2;
p3[402] = 2;

p2[3] = 1;
p2[203] = 1;
p2[403] = 1;

p2[4] = 1;
p2[204] = 1;
p2[404] = 1;

p2[5] = 1;
p2[205] = 1;
p2[405] = 1;

p2[6] = 1;
p2[206] = 1;
p2[406] = 1;

p3[7] = 2;
p3[207] = 2;
p3[407] = 2;

p1[30] = 2;
p1[230] = 2;
p1[430] = 2;

p2[31] = 1;
p2[231] = 1;
p2[431] = 1;

p1[32] = 3;
p1[232] = 3;
p1[432] = 3;

p2[33] = 1;
p2[233] = 1;
p2[433] = 1;

p3[34] = 3;
p3[234] = 3;
p3[434] = 3;

p1[36] = 2;
p1[236] = 2;
p1[436] = 2;

p2[37] = 1;
p2[237] = 1;
p2[437] = 1;

p2[38] = 1;
p2[238] = 1;
p2[438] = 1;

p2[39] = 1;
p2[239] = 1;
p2[439] = 1;

p3[40] = 2;
p3[240] = 2;
p3[440] = 2;

p1[51] = 2;
p1[251] = 2;
p1[451] = 2;

p1[52] = 2;
p1[252] = 2;
p1[452] = 2;

p2[52] = 1;
p2[252] = 1;
p2[452] = 1;

p2[53] = 2;
p2[253] = 2;
p2[453] = 2;

p1[54] = 2;
p1[254] = 2;
p1[454] = 2;

p3[54] = 4;
p3[254] = 4;
p3[454] = 4;

p2[55] = 1;
p2[255] = 1;
p2[455] = 1;

p2[56] = 1;
p2[256] = 1;
p2[456] = 1;

p2[57] = 1;
p2[257] = 1;
p2[457] = 1;

p2[58] = 1;
p2[258] = 1;
p2[458] = 1;

p3[59] = 2;
p3[259] = 2;
p3[459] = 2;

p1[63] = 2;
p1[263] = 2;
p1[463] = 2;

p1[64] = 2;
p1[264] = 2;
p1[464] = 2;

p2[64] = 1;
p2[264] = 1;
p2[464] = 1;

p2[65] = 1;
p2[265] = 1;
p2[465] = 1;

p3[65] = 2;
p3[265] = 2;
p3[465] = 2;

p3[66] = 2;
p3[266] = 2;
p3[466] = 2;

p1[69] = 2;
p1[269] = 2;
p1[469] = 2;

p2[70] = 1;
p2[270] = 1;
p2[470] = 1;

p2[71] = 1;
p2[271] = 1;
p2[471] = 1;

p3[72] = 2;
p3[272] = 2;
p3[472] = 2;

p1[74] = 2;
p1[274] = 2;
p1[474] = 2;

p1[75] = 1;
p1[275] = 1;
p1[475] = 1;

p3[75] = 2;
p3[275] = 2;
p3[475] = 2;

p1[79] = 2;
p1[279] = 2;
p1[479] = 2;

p1[80] = 2;
p1[280] = 2;
p1[480] = 2;

p2[80] = 1;
p2[280] = 1;
p2[480] = 1;

p2[81] = 2;
p2[281] = 2;
p2[481] = 2;

p2[82] = 2;
p2[282] = 2;
p2[482] = 2;

p3[83] = 3;
p3[283] = 3;
p3[483] = 3;

p1[96] = 2;
p1[296] = 2;
p1[496] = 2;

p2[97] = 1;
p2[297] = 1;
p2[497] = 1;

p2[98] = 1;
p2[298] = 1;
p2[498] = 1;

p3[99] = 3;
p3[299] = 3;
p3[499] = 3;


p1[100] = 3;
p1[300] = 3;
p1[500] = 3;

p2[101] = 2;
p2[301] = 2;
p2[501] = 2;

p1[102] = 2;
p1[302] = 2;
p1[502] = 2;

p3[102] = 2;
p3[302] = 2;
p3[502] = 2;

p2[103] = 2;
p2[303] = 2;
p2[503] = 2;

p2[104] = 2;
p2[304] = 2;
p2[504] = 2;

p2[105] = 2;
p2[305] = 2;
p2[505] = 2;

p2[106] = 2;
p2[306] = 2;
p2[506] = 2;

p3[107] = 3;
p3[307] = 3;
p3[507] = 3;

p1[115] = 2;
p1[315] = 2;
p1[515] = 2;

p2[116] = 1;
p2[316] = 1;
p2[516] = 1;

p1[117] = 3;
p1[317] = 3;
p1[517] = 3;

p2[118] = 2;
p2[318] = 2;
p2[518] = 2;

p3[119] = 4;
p3[319] = 4;
p3[519] = 4;

p1[120] = 2;
p1[320] = 2;
p1[520] = 2;

p2[121] = 1;
p2[321] = 1;
p2[521] = 1;

p2[122] = 1;
p2[322] = 1;
p2[522] = 1;

p2[123] = 1;
p2[323] = 1;
p2[523] = 1;

p3[124] = 2;
p3[324] = 2;
p3[524] = 2;

p1[125] = 3;
p1[325] = 3;
p1[525] = 3;

p1[126] = 2;
p1[326] = 2;
p1[526] = 2;

p2[126] = 1;
p2[326] = 1;
p2[526] = 1;

p2[127] = 2;
p2[327] = 2;
p2[527] = 2;

p1[128] = 2;
p1[328] = 2;
p1[528] = 2;

p3[128] = 4;
p3[328] = 4;
p3[528] = 4;

p2[129] = 2;
p2[329] = 2;
p2[529] = 2;

p2[130] = 2;
p2[330] = 2;
p2[530] = 2;

p2[131] = 2;
p2[331] = 2;
p2[531] = 2;

p2[132] = 2;
p2[332] = 2;
p2[532] = 2;

p3[133] = 4;
p3[333] = 4;
p3[533] = 4;

p1[148] = 2;
p1[348] = 2;
p1[548] = 2;

p3[148] = 4;
p3[348] = 4;
p3[548] = 4;

p2[149] = 2;
p2[349] = 2;
p2[549] = 2;

p2[150] = 2;
p2[350] = 2;
p2[550] = 2;

p2[151] = 2;
p2[351] = 2;
p2[551] = 2;

p2[152] = 2;
p2[352] = 2;
p2[552] = 2;

p2[153] = 2;
p2[353] = 2;
p2[553] = 2;

p3[154] = 3;
p3[354] = 3;
p3[554] = 3;

p1[163] = 2;
p1[363] = 2;
p1[563] = 2;

p1[164] = 2;
p1[364] = 2;
p1[564] = 2;

p2[164] = 1;
p2[364] = 1;
p2[564] = 1;

p2[165] = 1;
p2[365] = 1;
p2[565] = 1;

p3[165] = 2;
p3[365] = 2;
p3[565] = 2;

p3[166] = 2;
p3[366] = 2;
p3[566] = 2;

p1[169] = 2;
p1[369] = 2;
p1[569] = 2;

p2[170] = 1;
p2[370] = 1;
p2[570] = 1;

p2[171] = 1;
p2[371] = 1;
p2[571] = 1;

p3[172] = 2;
p3[372] = 2;
p3[572] = 2;

p1[174] = 2;
p1[374] = 2;
p1[574] = 2;

p1[175] = 1;
p1[375] = 1;
p1[575] = 1;

p3[175] = 2;
p3[375] = 2;
p3[575] = 2;

p1[179] = 2;
p1[379] = 2;
p1[579] = 2;

p1[180] = 2;
p1[380] = 2;
p1[580] = 2;

p2[180] = 1;
p2[380] = 1;
p2[580] = 1;

p2[181] = 2;
p2[381] = 2;
p2[581] = 2;

p2[182] = 2;
p2[382] = 2;
p2[582] = 2;

p3[183] = 3;
p3[383] = 3;
p3[583] = 3;

p1[195] = 4;
p1[395] = 4;
p1[595] = 4;

p2[196] = 2;
p2[396] = 2;
p2[596] = 2;

p2[197] = 2;
p2[397] = 2;
p2[597] = 2;

p2[198] = 1;
p2[398] = 1;
p2[598] = 1;

p3[199] = 3;
p3[399] = 3;
p3[599] = 3;


 table->SetNumberOfRows(600);
  for (int i = 0; i < 600; i++)
  {
    table->SetValue(i,0,i+1);
    table->SetValue(i,1,p2[i+1]);
  }
*/



table->SetNumberOfRows(array[0]);
for (int i = 0; i < array[0]; i++)
{
    table->SetValue(i,0,i+1);
    table->SetValue(i,1,array[i+1]);
}


  // Add multiple line plots, setting the colors etc
  vtkPlot *line = 0;

  line = chart->AddPlot(vtkChart::BAR);
#if VTK_MAJOR_VERSION <= 5
  line->SetInput(table, 0, 1);
#else
  line->SetInputData(table, 0, 1);
#endif

  line->SetColor(0, 255, 0, 255);

  chart->GetAxis(vtkAxis::LEFT)->SetTitle("Tokens");
  //chart->GetAxis(vtkAxis::LEFT)->SetRange(-1.5, 1.5); 
  //chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
  chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("Time");
  
  //chart->SetShowLegend(true); 


  // Finally render the scene and compare the image to a reference image

  hlayout = new QHBoxLayout;
  hlayout->addWidget(vtkwidget, 1, Qt::AlignVCenter);
  hlayout->setStretch(0,1);
  hlayout->setStretch(1,200);
  hlayout->setStretch(2,1);

  this->setLayout(hlayout);
}


void ChartWindow::displayindex()
{
    if(chart->chosenBar>=0)
       //cout << chart->chosenBar << endl;
       emit sendBarIndex(chart->chosenBar);
}
