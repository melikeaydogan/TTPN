#ifndef VOL_RENDERWINDOW_FILE_
#define VOL_RENDERWINDOW_FILE_


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

#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkPointData.h>
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
 
#include <sstream>
#include <vector>

#include "vtkImageReader.h"
#include "vtkImageCast.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
//#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkVolume.h"
#include "vtkInteractorStyleTrackballCamera.h"
//#include "vtkVolumeRayCastIsosurfaceFunction.h"

#include <vtkVersion.h>
#include <vtkPoints.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkLine.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>  
#include <cmath>  
#include <time.h>  
#include <vector>  
#include <algorithm>  

#include "structs.h"


using namespace std;


class element
{  
public:  
    element(int t, float v, int a, int b, int c)
    {  
        value = v;  
        times = t;
        x = a;  
        y = b;
        z = c;
    }
 
    float value;  
    int times, x, y, z;  
};  





class VolRenderWindow : public QWidget
{
    //Q_OBJECT

public:
    QVTKWidget *vtkwidget;
    VolRenderWindow(Activity activity, int activity_ID);
    unsigned long volumereader(string uocdfile, int object_num, int *objectlist, int *frequencymap, float *valuemap,
                                int rows, int cols, int slices);
    void SaveVTKFile(string FileName, int x_dim, int y_dim, int z_dim);

    vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> volumeMapper;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkCamera *camera;
    vtkSmartPointer<vtkOrientationMarkerWidget> OrientationWidget;
    vtkSmartPointer<vtkAxesActor> axes;

private:
    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout;
    int rows, cols, slices;
};

#endif
