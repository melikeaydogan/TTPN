#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>
#include <vtkPoints.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vector>
#include <sstream>
#include <string>

#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

using     std::string;
using namespace std;


vtkPolyData* polyfiltering_reader(string filename, int targetNumber, int* targetID);
