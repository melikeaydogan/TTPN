#include "box.h"

vtkPolyData* boundingbox(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
    //Coordinates of 8 points
    double P1[3] = {minX, minY, minZ};
    double P2[3] = {maxX, minY, minZ};
    double P3[3] = {maxX, maxY, minZ};
    double P4[3] = {minX, maxY, minZ};
    double P5[3] = {minX, minY, maxZ};
    double P6[3] = {maxX, minY, maxZ};
    double P7[3] = {maxX, maxY, maxZ};
    double P8[3] = {minX, maxY, maxZ};


    vtkSmartPointer <vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();

    pts->InsertNextPoint(P1);
    pts->InsertNextPoint(P2);
    pts->InsertNextPoint(P3);
    pts->InsertNextPoint(P4);
    pts->InsertNextPoint(P5);
    pts->InsertNextPoint(P6);
    pts->InsertNextPoint(P7);
    pts->InsertNextPoint(P8);

    vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
    line1->GetPointIds()->SetId(0,0);
    line1->GetPointIds()->SetId(1,1);

    vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
    line2->GetPointIds()->SetId(0,1);
    line2->GetPointIds()->SetId(1,2);

    vtkSmartPointer<vtkLine> line3 = vtkSmartPointer<vtkLine>::New();
    line3->GetPointIds()->SetId(0,2);
    line3->GetPointIds()->SetId(1,3);

    vtkSmartPointer<vtkLine> line4 = vtkSmartPointer<vtkLine>::New();
    line4->GetPointIds()->SetId(0,3);
    line4->GetPointIds()->SetId(1,0);

    vtkSmartPointer<vtkLine> line5 = vtkSmartPointer<vtkLine>::New();
    line5->GetPointIds()->SetId(0,0);
    line5->GetPointIds()->SetId(1,4);

    vtkSmartPointer<vtkLine> line6 = vtkSmartPointer<vtkLine>::New();
    line6->GetPointIds()->SetId(0,1);
    line6->GetPointIds()->SetId(1,5);

    vtkSmartPointer<vtkLine> line7 = vtkSmartPointer<vtkLine>::New();
    line7->GetPointIds()->SetId(0,2);
    line7->GetPointIds()->SetId(1,6);

    vtkSmartPointer<vtkLine> line8 = vtkSmartPointer<vtkLine>::New();
    line8->GetPointIds()->SetId(0,3);
    line8->GetPointIds()->SetId(1,7);

    vtkSmartPointer<vtkLine> line9 = vtkSmartPointer<vtkLine>::New();
    line9->GetPointIds()->SetId(0,4);
    line9->GetPointIds()->SetId(1,5);

    vtkSmartPointer<vtkLine> line10 = vtkSmartPointer<vtkLine>::New();
    line10->GetPointIds()->SetId(0,5);
    line10->GetPointIds()->SetId(1,6);

    vtkSmartPointer<vtkLine> line11 = vtkSmartPointer<vtkLine>::New();
    line11->GetPointIds()->SetId(0,6);
    line11->GetPointIds()->SetId(1,7);

    vtkSmartPointer<vtkLine> line12 = vtkSmartPointer<vtkLine>::New();
    line12->GetPointIds()->SetId(0,7);
    line12->GetPointIds()->SetId(1,4);

//    //12条边
//    vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
//    line1->GetPointIds()->SetId(0,0); //第一个0表示这条线的第0个点，第二个点表示点集的第0个点，在这里就是P1。
//    line1->GetPointIds()->SetId(1,1); //第一个1表示这条线的第1个点，第二个点表示点集的第1个点，在这里就是P2。
//    vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
//    line2->GetPointIds()->SetId(0,1); //第一个0表示这条线的第0个点，第二个点表示点集的第1个点，在这里就是P2。
//    line2->GetPointIds()->SetId(1,2); //第一个1表示这条线的第1个点，第二个点表示点集的第2个点，在这里就是P3。
//    vtkSmartPointer<vtkLine> line3 = vtkSmartPointer<vtkLine>::New();
//    line3->GetPointIds()->SetId(0,2); //第一个0表示这条线的第0个点，第二个点表示点集的第2个点，在这里就是P3。
//    line3->GetPointIds()->SetId(1,3); //第一个1表示这条线的第1个点，第二个点表示点集的第3个点，在这里就是P4。

//    vtkSmartPointer<vtkLine> line4 = vtkSmartPointer<vtkLine>::New();
//    line4->GetPointIds()->SetId(0,3); //第一个0表示这条线的第0个点，第二个点表示点集的第3个点，在这里就是P4。
//    line4->GetPointIds()->SetId(1,0); //第一个1表示这条线的第1个点，第二个点表示点集的第0个点，在这里就是P1。

//    vtkSmartPointer<vtkLine> line5 = vtkSmartPointer<vtkLine>::New();
//    line5->GetPointIds()->SetId(0,0); //第一个0表示这条线的第0个点，第二个点表示点集的第0个点，在这里就是P1。
//    line5->GetPointIds()->SetId(1,4); //第一个1表示这条线的第1个点，第二个点表示点集的第4个点，在这里就是P5。
//    vtkSmartPointer<vtkLine> line6 = vtkSmartPointer<vtkLine>::New();
//    line6->GetPointIds()->SetId(0,1); //第一个0表示这条线的第0个点，第二个点表示点集的第1个点，在这里就是P2。
//    line6->GetPointIds()->SetId(1,5); //第一个1表示这条线的第1个点，第二个点表示点集的第5个点，在这里就是P6。
//    vtkSmartPointer<vtkLine> line7 = vtkSmartPointer<vtkLine>::New();
//    line7->GetPointIds()->SetId(0,2); //第一个0表示这条线的第0个点，第二个点表示点集的第2个点，在这里就是P3。
//    line7->GetPointIds()->SetId(1,6); //第一个1表示这条线的第1个点，第二个点表示点集的第6个点，在这里就是P7。
//    vtkSmartPointer<vtkLine> line8 = vtkSmartPointer<vtkLine>::New();
//    line8->GetPointIds()->SetId(0,3); //第一个0表示这条线的第0个点，第二个点表示点集的第3个点，在这里就是P4。
//    line8->GetPointIds()->SetId(1,7); //第一个1表示这条线的第1个点，第二个点表示点集的第7个点，在这里就是P8。
//    vtkSmartPointer<vtkLine> line9 = vtkSmartPointer<vtkLine>::New();
//    line9->GetPointIds()->SetId(0,4); //第一个0表示这条线的第0个点，第二个点表示点集的第4个点，在这里就是P5。
//    line9->GetPointIds()->SetId(1,5); //第一个1表示这条线的第1个点，第二个点表示点集的第5个点，在这里就是P6。

//    vtkSmartPointer<vtkLine> line10 = vtkSmartPointer<vtkLine>::New();
//    line10->GetPointIds()->SetId(0,5); //第一个0表示这条线的第0个点，第二个点表示点集的第5个点，在这里就是P6。
//    line10->GetPointIds()->SetId(1,6); //第一个1表示这条线的第1个点，第二个点表示点集的第6个点，在这里就是P7。
//    vtkSmartPointer<vtkLine> line11 = vtkSmartPointer<vtkLine>::New();
//    line11->GetPointIds()->SetId(0,6); //第一个0表示这条线的第0个点，第二个点表示点集的第6个点，在这里就是P7。
//    line11->GetPointIds()->SetId(1,7); //第一个1表示这条线的第1个点，第二个点表示点集的第7个点，在这里就是P8。
//    vtkSmartPointer<vtkLine> line12 = vtkSmartPointer<vtkLine>::New();
//    line12->GetPointIds()->SetId(0,7); //第一个0表示这条线的第0个点，第二个点表示点集的第7个点，在这里就是P8。
//    line12->GetPointIds()->SetId(1,4); //第一个1表示这条线的第1个点，第二个点表示点集的第4个点，在这里就是P5。
 

    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(line1);
    lines->InsertNextCell(line2);
    lines->InsertNextCell(line3);
    lines->InsertNextCell(line4);
    lines->InsertNextCell(line5);
    lines->InsertNextCell(line6);
    lines->InsertNextCell(line7);
    lines->InsertNextCell(line8);
    lines->InsertNextCell(line9);
    lines->InsertNextCell(line10);
    lines->InsertNextCell(line11);
    lines->InsertNextCell(line12);
 
    //Build a polydata to save points and edge
    vtkPolyData *linesPolyData = vtkPolyData::New();
    linesPolyData->SetPoints(pts);
    linesPolyData->SetLines(lines);

    return linesPolyData;
}
