#ifndef MULTIVIEW_WINDOW_FILE_
#define MULTIVIEW_WINDOW_FILE_

#include <QObject>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMainWindow>
#include <QVTKWidget.h>
#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <qapplication.h>
#include <qobject.h>
#include <QtGui>
#include <QtWidgets>

#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkOutlineFilter.h>
#include <sstream>
#include <vtkVersion.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>

#include "volrenderwindow.h"
#include "structs.h"


using namespace std;

class MultiviewWindow : public QWidget
{
    Q_OBJECT

public:
    QVTKWidget *vtkwidget1, *vtkwidget2;
    MultiviewWindow(Activity activity, int activity_ID);

    vtkSmartPointer<vtkOrientationMarkerWidget> OrientationWidget;
    vtkSmartPointer<vtkAxesActor> axes;
    vtkPolyData *polydata;
    vtkSmartPointer<vtkActor> actor1;
    vtkSmartPointer<vtkPolyDataMapper> mapper1;

    vtkSmartPointer<vtkOutlineFilter> outline;
    vtkSmartPointer<vtkPolyDataMapper> outlineMapper;
    vtkSmartPointer<vtkActor> outlineActor;

    vtkSmartPointer<vtkRenderer> renderer1;
    vtkSmartPointer<vtkRenderWindow> renderWindow1;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkCamera *camera;

private slots: 
    void averageVisualization();

private:
    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout;
    QGridLayout *glayout1, *glayout2;  
    VolRenderWindow *vrwin;
    QPushButton *vrButton;
    Activity activity_saved;
    int activityID_saved;
};

#endif
