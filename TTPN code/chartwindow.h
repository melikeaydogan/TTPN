#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <vtkIntArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVersion.h>

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
#include "myvtkChartXY.h"

class vtkRenderer;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;


class ChartWindow : public QWidget
{
Q_OBJECT

public:    
    ChartWindow(int *array);
    vtkEventQtSlotConnect* Connections;

signals:
    void sendBarIndex(int index);

public slots:
    void displayindex();

private:
    QVTKWidget *vtkwidget;
    QHBoxLayout *hlayout;
    myvtkChartXY *chart;
  
};
