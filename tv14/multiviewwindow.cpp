
#include "multiviewwindow.h"
#include "objectreader.h"
#include "box.h"


MultiviewWindow::MultiviewWindow(Activity chosenactivity, int activity_ID)
{
     activity_saved = chosenactivity;
     activityID_saved = activity_ID;

     hlayout = new QHBoxLayout;
     glayout1 = new QGridLayout;
     glayout2 = new QGridLayout;

     camera = vtkCamera::New();
     camera->SetPosition(0, 1, 0);
     camera->SetFocalPoint(0, 0, 0); 

     axes = vtkSmartPointer<vtkAxesActor>::New();
     axes->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(100);
     axes->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetColor(0, 0, 0);
     axes->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->BoldOff();
     axes->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(100);
     axes->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetColor(0, 0, 0);
     axes->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->BoldOff();
     axes->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(100);
     axes->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetColor(0, 0, 0);
     axes->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->BoldOff(); 


     OrientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New(); 
     OrientationWidget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
     OrientationWidget->SetOrientationMarker( axes );

     vtkwidget1 = new QVTKWidget;
      
     renderWindow1 = vtkSmartPointer<vtkRenderWindow>::New();
     double *xmins, *xmaxs, *ymins, *ymaxs; 
     int step_num = chosenactivity.endframe - chosenactivity.startframe + 1;

     QFont fnt("Times", 12, QFont::Bold);
     QLabel *title = new QLabel;
     QString title_str = "Activity " + QString::number(activity_ID+1);
     //title->setText("Activity 152");
     title->setText(title_str);
     title->setFont(fnt);

     vrButton = new QPushButton;
     vrButton->setText("Average Visualization");
     vrButton->setFont(fnt);
     connect(vrButton, SIGNAL(clicked()), this, SLOT(averageVisualization()));

     hlayout->addWidget(title, (int)(step_num/2), Qt::AlignCenter);
     hlayout->addWidget(vrButton);
     

     xmins = new double [step_num];
     xmaxs = new double [step_num];
     ymins = new double [step_num];
     ymaxs = new double [step_num];

     if (step_num<=3)
     {
        vtkwidget1->setFixedSize(520, 280);
        renderWindow1->SetSize(500, 240);         
     }
   
     else if (step_num>3 && step_num<=6)
     {
        vtkwidget1->setFixedSize(620, 280);
        renderWindow1->SetSize(600, 240);         
     }

     else if (step_num>6 && step_num<=9)
     {
        vtkwidget1->setFixedSize(860, 280);
        renderWindow1->SetSize(840, 240);         
     }   

     else if (step_num>9 && step_num<=12)
     {
        vtkwidget1->setFixedSize(1020, 280);
        renderWindow1->SetSize(1000, 240);         
     }

     else if (step_num>12)
     {
        vtkwidget1->setFixedSize(1220, 280);
        renderWindow1->SetSize(1200, 240);         
     }

     //float opacity_start = 0.2;
     //float opacity_step = 0.8/(step_num-1);

     for (unsigned i = 0; i < step_num; i++)
     {
         double *Xrange = new double [2];
         double *Yrange = new double [2];
         double *Zrange = new double [2];






         xmins[i] = (double)(i)/step_num;
         xmaxs[i] = (double)(i)/step_num+1.0/step_num;
         ymins[i] = 0;
         ymaxs[i] = 1;   

         renderer1 = vtkSmartPointer<vtkRenderer>::New();      
         renderWindow1->AddRenderer(renderer1);
         renderer1->SetViewport(xmins[i],ymins[i],xmaxs[i],ymaxs[i]);
         renderer1->SetActiveCamera(camera);

         int object_num = chosenactivity.timestep[i].object_num;
         int *objectlist = new int [object_num];
         for (int j=0; j<object_num; j++)
             objectlist[j] = chosenactivity.timestep[i].objectlist[j];

         polydata = vtkPolyData::New();
         polydata = object_reader(chosenactivity.filename[i], object_num, objectlist, QColor(255, 255, 255), false);

         double *bounds = new double [6];
         bounds = polydata->GetBounds();

       Xrange[0] = bounds[0];
       Xrange[1] = bounds[1];
       Yrange[0] = bounds[2];
       Yrange[1] = bounds[3];
       Zrange[0] = bounds[4];
       Zrange[1] = bounds[5];

         vtkPolyData *outlinedata = vtkPolyData::New();
    outlinedata = boundingbox(Xrange[0], Yrange[0], Zrange[0], Xrange[1], Yrange[1], Zrange[1]);    

    outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    #if VTK_MAJOR_VERSION <= 5
    outlineMapper->SetInputConnection(outlinedata->GetProducerPort());
    #else
    outlineMapper->SetInputData(outlinedata);
    #endif

    outlineActor = vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outlineMapper);
    outlineActor->GetProperty()->SetColor(0,0,0);

         // Visualize
         mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
         mapper1->SetInputData(polydata);
         actor1 = vtkSmartPointer<vtkActor>::New();
         actor1->SetMapper(mapper1);

         //float opacity_value = opacity_start + i*opacity_step;
         //actor1->GetProperty()->SetOpacity(opacity_value);

         renderer1->AddActor(actor1);
         renderer1->AddActor(outlineActor);
         renderer1->SetBackground(1,1,1); // Background color white
         renderer1->ResetCamera();
         
         QFont font("Times", 10, QFont::Bold);
         QString place = "Place " + QString::number(chosenactivity.timestep[i].PlaceID);
         QLabel *label1 = new QLabel;
         label1->setText(place);
         label1->setFont(font);

         QString timestep = "Timestep " + QString::number(chosenactivity.startframe+i+1);
         QLabel *label2 = new QLabel;
         label2->setText(timestep);
         label2->setFont(font);
 
         glayout1->addWidget(label1, 0, i, Qt::AlignCenter);
         glayout2->addWidget(label2, 0, i, Qt::AlignCenter);
     }

     vtkwidget1->SetRenderWindow(renderWindow1); 
     vlayout = new QVBoxLayout;
     vlayout->addLayout(hlayout);
     vlayout->addWidget(vtkwidget1);
     vlayout->addLayout(glayout2);
     vlayout->addLayout(glayout1);
     this->setLayout(vlayout);

     vtkwidget1->GetRenderWindow()->AddRenderer(renderer1);

     renderWindowInteractor = vtkwidget1->GetRenderWindow()->GetInteractor();
     OrientationWidget->SetInteractor( renderWindowInteractor );
     OrientationWidget->SetViewport( 0.0, 0.0, 0.3, 0.3 );
     OrientationWidget->SetEnabled( 1 );
     //OrientationWidget->InteractiveOn();     

     vtkwidget1->GetRenderWindow()->Render();
}


void MultiviewWindow::averageVisualization()
{
     QMainWindow *myNewWindow = new QMainWindow;
     vrwin = new VolRenderWindow(activity_saved, activityID_saved);

     myNewWindow->setCentralWidget(vrwin);
     myNewWindow->setWindowTitle(tr("Average Visualization"));
     myNewWindow->show();    
}
