#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPointPicker.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>
#include <vtkPointPicker.h>


class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static MouseInteractorStyle* New();
    double click_x, click_y, click_z;
    virtual void OnRightButtonDown() 
    {
      //std::cout << "Pressed left mouse button." << std::endl;
      //click_x = this->Interactor->GetEventPosition()[0];
      //click_y = this->Interactor->GetEventPosition()[1];


       
      this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0], 
                         this->Interactor->GetEventPosition()[1], 
                         0,  // always zero.
                         this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
      double picked[3];
      this->Interactor->GetPicker()->GetPickPosition(picked);

      click_x = picked[0];
      click_y = picked[1];
      click_z = picked[2];

    
      // Forward events
      vtkInteractorStyleTrackballCamera::OnRightButtonDown();
      
    }

    

  //signals:
    //void sendMouseCoordinate(int click_x, int click_y);
    
};
