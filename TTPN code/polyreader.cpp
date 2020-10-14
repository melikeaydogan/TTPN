#include "polyreader.h"

vtkPolyData* poly_reader(string filename, int targetNumber, int* targetID)
{
    /****************** Part 1: find the corresponding colormap for the poly data *****************/

    int pos = filename.find_first_of("0123456789");
    string number;
    string extension = ".txt";

    while (pos!= string::npos)
    {
        number = number + filename[pos];
        pos = filename.find_first_of("0123456789", pos+1);
    }

    int filename_size = filename.size();
    int pos2 = filename.find_last_of("/");
    string filepath = filename.substr(0, pos2+1);

    string colormap = filepath + "ColorMap" + number + extension;
    //cout << colormap << endl;

    FILE* pf;
    pf=fopen(colormap.c_str(),"r");
    if(pf == NULL){
        cout<< "Can't open colormap file"<<endl;
        exit(1);
    }
    /**************************************** End of Part 1 *************************************/


    /******************************** Part 2: open the poly data *******************************/

    int i,j;
    char *polyfile = new char [1000];
    strcpy(polyfile,filename.c_str());

    vtkFloatArray       *pcoords = vtkFloatArray::New();
    vtkCellArray        *polys = vtkCellArray::New();
    vtkPolyData         *polyData = vtkPolyData::New();

    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(4);
    colors->SetName("Colors");
     
    fstream pfile(polyfile,ios_base::in);
    if(!pfile.is_open())
    {
        cout << "cannot open poly file!\n";
        return  0;
    }


    vtkPoints *points = vtkPoints::New();
    pcoords->SetNumberOfComponents(3);

    int nobjects = 0;
    string  buf;
    int num_nodes = 0;
    double x = 0.0, y = 0.0, z = 0.0;

    vector<int> ncells;


    while(!pfile.eof())
    {
        if(nobjects == 0)
            ncells.push_back(0);
        else
            ncells.push_back(ncells.at(nobjects-1));

        getline(pfile,buf,'\n');
        if(!pfile.good())
            break;

        istringstream  bufstr(buf,ios_base::out);       //读取RGB信息，然后废弃不用

        unsigned char *colorarray = new unsigned char [4];
        int c1, c2, c3, c4;
        fscanf(pf,"%d %d %d %d", &c1, &c2, &c3, &c4);

        colorarray[0] = c1;
        colorarray[1] = c2;
        colorarray[2] = c3;
 
        int k;     
        if(targetNumber == -1)
           colorarray[3] = 255;
        else if(targetNumber > -1)
        {
           colorarray[3] = 20;

           for(k=0; k<targetNumber; k++)
           {
               //if(targetID[k] == nobjects+1)
               if(targetID[k] == nobjects)
                  colorarray[3] = 255;
           }
           
        }
           
        getline(pfile,buf,'\n');
        if(!pfile.good())
            break;

        istringstream  bufstr1(buf,ios_base::out);
        bufstr1 >> num_nodes;                           //把数据从bufstr1传递到num_nodes，其实就是从文件中获得num_nodes的值

        //cout<<num_nodes<<endl;

        for(i = 0; i < num_nodes;i++)
        {
            getline(pfile,buf,'\n');
            istringstream  bufstr2(buf,ios_base::out);
            bufstr2 >> x >> y >> z;
            pcoords->InsertNextTuple3(x,y,z);
        }

        int num_cells = 0;
        getline(pfile,buf,'\n');
        istringstream  bufstr3(buf,ios_base::out);
        bufstr3>>num_cells;

        //cout<<num_cells<<endl;
          
        int cell[3] = {0};
        int three = 0;

        for(i = 0; i < num_cells; i++)
        {
            getline(pfile,buf,'\n');
            istringstream  bufstr4(buf,ios_base::out);
            bufstr4>>three >>cell[0] >> cell[1] >> cell[2];

            if(nobjects>0)
            {
                for(j = 0; j < 3; j++)
                {
                    if(ncells.at(nobjects) < cell[j] +ncells.at(nobjects-1))
                        ncells.at(nobjects) = cell[j] +ncells.at(nobjects-1);
                }
            }

            else
            {
                for(j = 0; j < 3; j++)
                {
                    if(ncells.at(nobjects) < cell[j] )
                        ncells.at(nobjects) = cell[j];
                }
            }

            if(nobjects==0)
            {
                polys->InsertNextCell(3);
                polys->InsertCellPoint(cell[0]);
                polys->InsertCellPoint(cell[1]);
                polys->InsertCellPoint(cell[2]);

                //colors->InsertNextTupleValue(colorarray);             //li
            }

            else
            {
                polys->InsertNextCell(3);
                polys->InsertCellPoint(ncells.at(nobjects-1)+cell[0]);
                polys->InsertCellPoint(ncells.at(nobjects-1)+cell[1]);
                polys->InsertCellPoint(ncells.at(nobjects-1)+cell[2]);

                //colors->InsertNextTupleValue(colorarray);             //li
            }
            colors->InsertNextTypedTuple(colorarray);
        }

        ncells.at(nobjects) = ncells.at(nobjects)+1;
        nobjects = nobjects +1;
        getline(pfile,buf,'\n');
        getline(pfile,buf,'\n');

    }

   
     points->SetData(pcoords);
     polyData->SetPoints(points);
     points->Delete();
     pcoords->Delete();
     //pcoords = NULL;
     polyData->SetPolys(polys);

     polyData->GetCellData()->SetScalars(colors);                      //li

     polys->Delete();
     //polys = NULL;
     pfile.close();
     fclose(pf);
     delete [] polyfile;

     return polyData;
}
