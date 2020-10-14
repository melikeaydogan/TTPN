#include <QtGui>
#include <QFile>
#include <QTextStream>

#include "polybrowser.h"
#include "polyreader.h"
#include "polyfilteringreader.h"
#include "box.h"
#include <sys/stat.h>

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkVersion.h>
#include <vtkPoints.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkOutlineFilter.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include "vtkContextMouseEvent.h"
#include "vtkContextKeyEvent.h"
#include <vtkEventQtSlotConnect.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkTransform.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>

using namespace std;


PolyBrowser::PolyBrowser()
{
    camera = vtkCamera::New();
    //camera->SetPosition(0, -0.85, 0.5);  //Enrique
    //camera->SetPosition(0, 0, 1);   //Reconbt
    camera->SetPosition(0, 0, 1); //Plume
    camera->SetFocalPoint(0, 0, 0);
    //camera->SetViewAngle(45);
    camera->SetViewUp(0, 0, 0);

    vtkwidget = new QVTKWidget;
    vtkwidget->setFixedSize(850,1290);
    //vtkwidget->setFixedSize(850,500);

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkwidget->SetRenderWindow(renderWindow);

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
/*
    transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(9.0, 0.0, 0.0);
    axes->SetUserTransform(transform);
*/

    OrientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    OrientationWidget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
    OrientationWidget->SetOrientationMarker( axes );

    hlayout = new QHBoxLayout;
    hlayout->addWidget(vtkwidget, 1, Qt::AlignVCenter);

    this->setLayout(hlayout);
}


void PolyBrowser::onOpen(char *filename)
{
    renderer->SetActiveCamera(camera);
    renderer->RemoveActor(actor);
    renderer->RemoveActor(outlineActor);

    polydata = vtkPolyData::New();
    FileName = string(filename);

    currentFileName = new char [FileName.length()+1];
    memcpy(currentFileName, FileName.c_str(), FileName.length()+1);

    int *objectlist = new int [1000];
    polydata = poly_reader(FileName, -1, objectlist);


    int fullname_size = FileName.size();
    unsigned int pos1 = FileName.find_last_of(".");
    unsigned int pos2 = FileName.find_last_of("/\\");

    string extension = ".boundingbox";
    string polyfilename = FileName.substr(pos2+1, pos1-pos2-1);
    string filepath = FileName.substr(0, pos2+1);

    int pos = polyfilename.find_first_of("0123456789");
    string number;

    while (pos!= string::npos)
    {
        number = number + polyfilename[pos];
        pos = polyfilename.find_first_of("0123456789", pos+1);
    }

    int len1 = polyfilename.length();
    int len2 = number.length();
    string basename = polyfilename.substr(0, len1-len2);
    string boxfile = filepath + basename + extension;
    ifstream fp(boxfile.c_str(), ios_base::in);

    emit sendRow1(polydata->GetNumberOfPoints());
    emit sendRow2(polydata->GetNumberOfCells());

    double *Xrange = new double [2];
    double *Yrange = new double [2];
    double *Zrange = new double [2];

    if (fp.is_open())
    {
       string  buf;
       getline(fp, buf, '\n');
       istringstream  bufstr1(buf,ios_base::out);
       bufstr1 >> Xrange[0] >> Yrange[0] >> Zrange[0];

       getline(fp, buf, '\n');
       istringstream  bufstr2(buf,ios_base::out);
       bufstr2 >> Xrange[1] >> Yrange[1] >> Zrange[1];
    }

    else if (!fp.is_open())
    {
       double *bounds = new double [6];
       bounds = polydata->GetBounds();

       Xrange[0] = bounds[0];
       Xrange[1] = bounds[1];
       Yrange[0] = bounds[2];
       Yrange[1] = bounds[3];
       Zrange[0] = bounds[4];
       Zrange[1] = bounds[5];
    }

    fp.close();

    emit sendRow3(Xrange);
    emit sendRow4(Yrange);
    emit sendRow5(Zrange);

    // Visualize
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    #if VTK_MAJOR_VERSION <= 5
    mapper->SetInputConnection(polydata->GetProducerPort());
    #else
    mapper->SetInputData(polydata);
    #endif

    //mapper->SetColorModeToDefault();                       //li
    //mapper->ScalarVisibilityOff();                         //li

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    //actor->SetOrigin(polydata->GetCenter());

    outlinedata = vtkPolyData::New();
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

    renderer->AddActor(actor);
    renderer->AddActor(outlineActor);
    renderer->SetBackground(1,1,1); // Background color white
    renderer->ResetCamera();

    vtkwidget->GetRenderWindow()->AddRenderer(renderer);

    /**/
    style = new MouseInteractorStyle;
    vtkwidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

    Connections = vtkEventQtSlotConnect::New();
    Connections->Connect( vtkwidget->GetRenderWindow()->GetInteractor(), vtkCommand::RightButtonPressEvent, this, SLOT( print() ) );

    renderWindowInteractor = vtkwidget->GetRenderWindow()->GetInteractor();
    OrientationWidget->SetInteractor( renderWindowInteractor );
    OrientationWidget->SetViewport( 0.0, 0.0, 0.5, 0.5 );
    //OrientationWidget->SetEnabled( 1 );
    //OrientationWidget->InteractiveOn();
/**/

    vtkwidget->GetRenderWindow()->Render();

    QString saveFileName = "1.png";
    QFileInfo fi(saveFileName);

    vtkWindowToImageFilter* f = vtkWindowToImageFilter::New();

    f->Modified();
    f->SetInput(vtkwidget->GetRenderWindow());
    f->SetInputBufferTypeToRGBA();
    f->Update();

    vtkPNGWriter* w = vtkPNGWriter::New();

    w->SetFileName(saveFileName.toLatin1());
    w->SetInputData(f->GetOutput());
    w->Write();
    w->Delete();

    f->Delete();



}


void PolyBrowser::visualizeActivityActors(vector<vtkSmartPointer<vtkPolyData> >polydatagroup, int number)
{
     renderer->SetActiveCamera(camera);
     renderer->RemoveActor(actor);

     for (int i=0; i<number; i++)
     {
         mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
         #if VTK_MAJOR_VERSION <= 5
         mapper->SetInputConnection(polydatagroup[i]->GetProducerPort());
         #else
         mapper->SetInputData(polydatagroup[i]);
         #endif

         actor = vtkSmartPointer<vtkActor>::New();
         actor->SetMapper(mapper);

         renderer->AddActor(actor);
         renderer->SetBackground(1,1,1);
         renderer->ResetCamera();

         vtkwidget->GetRenderWindow()->AddRenderer(renderer);
      }

      vtkwidget->GetRenderWindow()->Render();
}


void PolyBrowser::onFilteringOpen(int *array)
{
    renderer->SetActiveCamera(camera);
    renderer->RemoveActor(actor);

    int *targetObject = new int [array[0]];
    for(int i=0; i<array[0]; i++)
        targetObject[i] = array[i+1];

    FileName = string(currentFileName);
    polydata = vtkPolyData::New();
    polydata = polyfiltering_reader(FileName, array[0], targetObject);

    // Visualize
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    #if VTK_MAJOR_VERSION <= 5
    mapper->SetInputConnection(polydata->GetProducerPort());
    #else
    mapper->SetInputData(polydata);
    #endif

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1);
    renderer->ResetCamera();

    vtkwidget->GetRenderWindow()->AddRenderer(renderer);

    /**/
    style = new MouseInteractorStyle;
    vtkwidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

    Connections = vtkEventQtSlotConnect::New();
    Connections->Connect( vtkwidget->GetRenderWindow()->GetInteractor(), vtkCommand::RightButtonPressEvent, this, SLOT( print() ) );

    renderWindowInteractor = vtkwidget->GetRenderWindow()->GetInteractor();
    OrientationWidget->SetInteractor( renderWindowInteractor );
    OrientationWidget->SetViewport( 0.0, 0.0, 0.5, 0.5 );
    //OrientationWidget->SetEnabled( 1 );
    //OrientationWidget->InteractiveOn();
/**/

    vtkwidget->GetRenderWindow()->Render();
}



void PolyBrowser::onAnimation(char *filename, int targetNumber, int* targetObject)
{
    renderer->SetActiveCamera(camera);
    renderer->RemoveActor(actor);

    polydata = vtkPolyData::New();
    FileName = string(filename);
    polydata = poly_reader(FileName, targetNumber, targetObject);

    currentFileName = new char [FileName.length()+1];
    memcpy(currentFileName, FileName.c_str(), FileName.length()+1);

    // Visualize
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    #if VTK_MAJOR_VERSION <= 5
    mapper->SetInputConnection(polydata->GetProducerPort());
    #else
    mapper->SetInputData(polydata);
    #endif

    //mapper->SetColorModeToDefault();                         //li
    //mapper->ScalarVisibilityOff();                           //li

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1); // Background color white
    renderer->ResetCamera();
    vtkwidget->GetRenderWindow()->AddRenderer(renderer);

/**/
    style = new MouseInteractorStyle;
    vtkwidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

    Connections = vtkEventQtSlotConnect::New();
    Connections->Connect( vtkwidget->GetRenderWindow()->GetInteractor(), vtkCommand::RightButtonPressEvent, this, SLOT( print() ) );

    renderWindowInteractor = vtkwidget->GetRenderWindow()->GetInteractor();
    OrientationWidget->SetInteractor( renderWindowInteractor );
    OrientationWidget->SetViewport( 0.0, 0.0, 0.5, 0.5 );
    //OrientationWidget->SetEnabled( 1 );
    //OrientationWidget->InteractiveOn();
/**/

    vtkwidget->GetRenderWindow()->Render();

//    QString saveFileName = QString(filename);
//    saveFileName = saveFileName.left(saveFileName.length()-5).+ ".png";
//        QString saveFileName = "2.png";

    vtkWindowToImageFilter* f = vtkWindowToImageFilter::New();

    QFileInfo fi(filename);
    QString folder = fi.absolutePath()+"/screenshot";
    mkdir(folder.toStdString().c_str(), S_IRWXU);
    QString saveFileName = fi.fileName();
    saveFileName = folder + "/" + saveFileName.left(saveFileName.length()-5) + ".png";
    QString save_PNname = folder + "/" + saveFileName.left(saveFileName.length()-5) + "_PN.png";

    f->Modified();
    f->SetInput(vtkwidget->GetRenderWindow());
    f->SetInputBufferTypeToRGBA();
    f->Update();

    vtkPNGWriter* w = vtkPNGWriter::New();

    w->SetFileName(saveFileName.toStdString().c_str());
    w->SetInputData(f->GetOutput());
    w->Write();
    w->Delete();

    f->Delete();



}


void PolyBrowser::print()
{
     cout << "X = " << style->click_x << endl;
     cout << "Y = " << style->click_y << endl;
     cout << "Z = " << style->click_z << endl;
}
