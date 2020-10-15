#include <QObject>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
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

#include <vtkPolyData.h>
#include <vtkDICOMImageReader.h>
#include <vtkOutlineFilter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include "vtkContextMouseEvent.h"
#include "vtkContextKeyEvent.h"
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include "interactorstyle.h"
#include "objectreader.h"

using namespace std;

class vtkRenderer;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;


class PolyBrowser : public QWidget
{
    Q_OBJECT

public:
    QVTKWidget *vtkwidget;
    PolyBrowser();
    void onOpen(char *filename);
    void onAnimation(char *filename, int targetNumber, int* targetObject);
    //vtkPolyData* poly_reader(string filename);
    vtkEventQtSlotConnect* Connections;

    char *currentFileName;
    string FileName;
    vtkPolyData *polydata, *outlinedata;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkPolyDataMapper> mapper;

    //vtkSmartPointer<vtkOutlineFilter> outline;
    vtkSmartPointer<vtkPolyDataMapper> outlineMapper;
    vtkSmartPointer<vtkActor> outlineActor;
    vtkSmartPointer<vtkOrientationMarkerWidget> OrientationWidget;
    vtkSmartPointer<vtkAxesActor> axes;
    vtkSmartPointer<vtkTransform> transform;

    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkCamera *camera;
    MouseInteractorStyle *style;

public slots:
    void onFilteringOpen(int *array);
    void visualizeActivityActors(vector<vtkSmartPointer<vtkPolyData> >polydatagroup, int number);
    void print();

signals:
    void sendRow1(int pnum);
    void sendRow2(int cnum);
    void sendRow3(double *Xrange);
    void sendRow4(double *YXrange);
    void sendRow5(double *ZXrange);

private:
    //char filename[];
    QHBoxLayout *hlayout;
   
    //QLabel *label;
};



