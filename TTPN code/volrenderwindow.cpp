#include "volrenderwindow.h"
#include <ctime> 
#include <cerrno>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <vector>
#include <string>

using namespace std;

bool comparevalue(const element &a, const element &b )  
{  
     //return a.value < b.value;   //(ascending order)  
     return a.value > b.value;     //(descending order)  
}

VolRenderWindow::VolRenderWindow(Activity chosenactivity, int activity_ID)
{
     

     int step_num = chosenactivity.endframe - chosenactivity.startframe + 1;

     rows = 384;
     cols = 256;
     slices = 69;

     float *data = new float [rows*cols*slices];
     float *valuemap = new float [rows*cols*slices];
     int *frequencymap = new int [rows*cols*slices];

     for (long ii=0; ii<rows*cols*slices; ii++)
     {
         data[ii] = 0;
         valuemap[ii] = 0;
         frequencymap[ii] = 0;
     } 
       
     unsigned long totalvolume = 0, averagevolume = 0;

     QString eventFile = "/home/liliu/eventreconbt.list"; 
     QByteArray ba = eventFile.toLatin1();
     char *name = ba.data();

     ifstream infile;
     infile.open(name);

     string event, temp;
     int *eventFrame = new int [5000];
     int *eventObject = new int [5000];
     int *eventPlace = new int [5000];
     int num_eventFrame = 0;
     int num_eventObject = 0;
     int num_eventPlace = 0;

     while(!infile.eof())
     {
         getline(infile, event);
         stringstream os(event);
       
         int num = 0;
         while(os >> temp)
         {
             if (num%3==0)
             {
                eventObject[num_eventObject] = atoi(temp.c_str());    
                num_eventObject++;
             }            

             else if (num%3==1)
             {
                eventFrame[num_eventFrame] = atoi(temp.c_str());
                num_eventFrame++;  
              }         
               
             else if(num%3==2)  
             {          
                eventPlace[num_eventPlace] = atoi(temp.c_str());   
                num_eventPlace++; 
             }                     
             num++;
            
         }          
     } 


     //int *record = new int [46];
     //for (int i=0; i<46; i++)
         //record[i] = 0;

     step_num = 0;
     for (int i=0; i<num_eventFrame; i++)
     {
         if (eventPlace[i]==4)
         { 
            step_num++; 
            int *objectlist;
            int object_num=0;

            string packetname_base = "/home/liliu/reconbt/reconbt";
            stringstream ss1;  
            ss1 << eventFrame[i];          
            string packetname =  packetname_base + ss1.str() + ".packet"; 
            string  buf;
            fstream pfile(packetname.c_str(),ios_base::in);  
           
            int line = 0;
            while (!pfile.eof())
            {
                line++;
                getline(pfile,buf,'\n');

                if (line==eventObject[i])
                {
                   float t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12;
                   int t13;
                   istringstream bufstr2(buf,ios_base::out);
                   bufstr2 >> t1 >> t2 >> t3 >> t4 >> t5 >> t6 >> t7 >> t8 >> t9 >> t10 >> t11 >> t12 >> t13;              
                
                   object_num = t13;
                   objectlist = new int [object_num];

                   for (int j=0; j<t13; j++)
                   {
                       int tempID;
                       bufstr2 >> tempID;
                       objectlist[j] = tempID;
                   }
                }
            }


            //int object_num = 1;        
            //int *objectlist = new int [1];
            //objectlist[0] = eventObject[i];
    
            string extension = ".uocd";
            stringstream ss;        
            ss << eventFrame[i]; 

            string uocdfilename = "/home/aydogan/Desktop/generated_files/rst_one" + ss.str() + extension;
         
            unsigned long thisvolume = volumereader(uocdfilename, object_num, objectlist, frequencymap, valuemap, rows, cols, slices);            
            totalvolume = totalvolume + thisvolume;

            //int index = eventFrame[i];
            //record[index-1] = 1;
         }        
     }
/*
     int count = 46;
     for (int i=0; i<46; i++)
     {
        if (record[i]==0)
           count--;
     }
*/

     averagevolume = totalvolume/step_num;
     cout << "Volume: " << averagevolume << endl; 


     vector< element > dataspace;  

     for (int i=0; i<rows; i++)
         for (int j=0; j<cols; j++)
             for (int k=0; k<slices; k++) 
     {
         int times = *(frequencymap + i + j*rows + k*rows*cols);  
         float value = *(valuemap + i + j*rows + k*rows*cols);   
         value = value/step_num;        
         dataspace.push_back( element(times, value, i, j, k) );  
     }

     sort(dataspace.begin(), dataspace.end(), comparevalue);

     for (unsigned long i=0; i<averagevolume; i++)
     {
         int x_temp = dataspace[i].x;
         int y_temp = dataspace[i].y;
         int z_temp = dataspace[i].z;

         float value = dataspace[i].value;

         data[x_temp + y_temp*rows +z_temp*rows*cols] = value*100;
     }

     
     FILE *write_ptr;
     write_ptr = fopen("averageactor.vol", "wb");  
     fwrite(data, rows*cols*slices, sizeof(float), write_ptr);
     fclose(write_ptr);         

     SaveVTKFile("averageactor.vol", rows, cols, slices);
     cout << "Average activity actor saved!" << endl;
}











unsigned long VolRenderWindow::volumereader(string uocdfile, int object_num, int *objectlist, int *frequencymap, float *valuemap, int rows, int cols, int slices)
{
     //cout << uocdfile << endl;

     ifstream fp;
     fp.open(uocdfile.c_str(), ios_base::in);
     if (!fp.good())
        cout << "ReadOct:cannot open uocd file " << uocdfile.c_str() << " to read\n";
    
     float time = 0.0;
     fp>>time;
     int numObjs = 0;
     fp>>numObjs;

     float mass = 0.0, cx=0.0, cy=0.0, cz=0.0, Ixx=0.0, Iyy=0.0, Izz=0.0, Ixy=0.0, Iyz=0.0, Izx=0.0;
     unsigned long numNodes = 0, vol = 0, objID = 0;
     register unsigned long i = 0, j = 0;
     unsigned long x = 0, y = 0 , z = 0,  xsize = 0, ysize = 0, zsize = 0, vertID = 0;
     float xPos = 0, yPos = 0, zPos = 0, val = 0, x1 = 0, y1 = 0, z1 = 0;
     unsigned long point_num = 0; 

     unsigned long volume = 0;
     for (int i=0; i<numObjs; i++)
     {
         bool match = false;
         for (int i1=0; i1<object_num; i1++)
         {
             if (objectlist[i1] == i)
                match = true;
         }

         if (match == true)
         {           
            fp>>objID>>vol>>mass>>cx>>cy>>cz>>Ixx>>Iyy>>Izz>>Ixy>>Iyz>>Izx;
        
            volume = volume + vol;
            for (int j=0; j<vol; j++)
            {
                fp>>vertID>>x1>>y1>>z1>>val;
                float x2 = x1/0.0183;
                float y2 = (y1+0.863298)/0.00679762;
                float z2 = 0.0;                

                if ( (z1-0.000744)*(z1-0.000744)<1e-12 )
                   z2 = 1.0;
                else if ( (z1-0.001539)*(z1-0.001539)<1e-12 )
                   z2 = 2.0;
                else if ( (z1-0.002389)*(z1-0.002389)<1e-12 )
                   z2 = 3.0;
                else if ( (z1-0.003298)*(z1-0.003298)<1e-12 )
                   z2 = 4.0;
                else if ( (z1-0.004270)*(z1-0.004270)<1e-12 )
                   z2 = 5.0;
                else if ( (z1-0.005309)*(z1-0.005309)<1e-12 )
                   z2 = 6.0;
                else if ( (z1-0.006419)*(z1-0.006419)<1e-12 )
                   z2 = 7.0;
                else if ( (z1-0.007606)*(z1-0.007606)<1e-12 )
                   z2 = 8.0;
                else if ( (z1-0.008875)*(z1-0.008875)<1e-12 )
                   z2 = 9.0;
                else if ( (z1-0.010232)*(z1-0.010232)<1e-12 )
                   z2 = 10.0;
                else if ( (z1-0.011683)*(z1-0.011683)<1e-12 )
                   z2 = 11.0;
                else if ( (z1-0.013234)*(z1-0.013234)<1e-12 )
                   z2 = 12.0;
                else if ( (z1-0.014892)*(z1-0.014892)<1e-12 )
                   z2 = 13.0;
                else if ( (z1-0.016664)*(z1-0.016664)<1e-12 )
                   z2 = 14.0;
                else if ( (z1-0.018559)*(z1-0.018559)<1e-12 )
                   z2 = 15.0;
                else if ( (z1-0.020585)*(z1-0.020585)<1e-12 )
                   z2 = 16.0;
                else if ( (z1-0.022751)*(z1-0.022751)<1e-12 )
                   z2 = 17.0;
                else if ( (z1-0.025066)*(z1-0.025066)<1e-12 )
                   z2 = 18.0;
                else if ( (z1-0.027541)*(z1-0.027541)<1e-12 )
                   z2 = 19.0;
                else if ( (z1-0.030187)*(z1-0.030187)<1e-12 )
                   z2 = 20.0;
                else if ( (z1-0.033016)*(z1-0.033016)<1e-12 )
                   z2 = 21.0;
                else if ( (z1-0.036040)*(z1-0.036040)<1e-12 )
                   z2 = 22.0;
                else if ( (z1-0.039274)*(z1-0.039274)<1e-12 )
                   z2 = 23.0;
                else if ( (z1-0.042730)*(z1-0.042730)<1e-12 )
                   z2 = 24.0;
                else if ( (z1-0.046425)*(z1-0.046425)<1e-12 )
                   z2 = 25.0;
                else if ( (z1-0.050376)*(z1-0.050376)<1e-12 )
                   z2 = 26.0;
                else if ( (z1-0.054599)*(z1-0.054599)<1e-12 )
                   z2 = 27.0;
                else if ( (z1-0.059114)*(z1-0.059114)<1e-12 )
                   z2 = 28.0;
                else if ( (z1-0.063941)*(z1-0.063941)<1e-12 )
                   z2 = 29.0;
                else if ( (z1-0.069102)*(z1-0.069102)<1e-12 )
                   z2 = 30.0;
                else if ( (z1-0.074619)*(z1-0.074619)<1e-12 )
                   z2 = 31.0;
                else if ( (z1-0.080517)*(z1-0.080517)<1e-12 )
                   z2 = 32.0;
                else if ( (z1-0.086822)*(z1-0.086822)<1e-12 )
                   z2 = 33.0;
                else if ( (z1-0.093563)*(z1-0.093563)<1e-12 )
                   z2 = 34.0;
                else if ( (z1-0.100769)*(z1-0.100769)<1e-12 )
                   z2 = 35.0;
                else if ( (z1-0.108473)*(z1-0.108473)<1e-12 )
                   z2 = 36.0;
                else if ( (z1-0.116709)*(z1-0.116709)<1e-12 )
                   z2 = 37.0;
                else if ( (z1-0.125515)*(z1-0.125515)<1e-12 )
                   z2 = 38.0;
                else if ( (z1-0.134928)*(z1-0.134928)<1e-12 )
                   z2 = 39.0;
                else if ( (z1-0.144992)*(z1-0.144992)<1e-12 )
                   z2 = 40.0;
                else if ( (z1-0.155750)*(z1-0.155750)<1e-12 )
                   z2 = 41.0;
                else if ( (z1-0.167252)*(z1-0.167252)<1e-12 )
                   z2 = 42.0;
                else if ( (z1-0.179548)*(z1-0.179548)<1e-12 )
                   z2 = 43.0;
                else if ( (z1-0.192694)*(z1-0.192694)<1e-12 )
                   z2 = 44.0;
                else if ( (z1-0.206748)*(z1-0.206748)<1e-12 )
                   z2 = 45.0;
                else if ( (z1-0.221772)*(z1-0.221772)<1e-12 )
                   z2 = 46.0;
                else if ( (z1-0.237834)*(z1-0.237834)<1e-12 )
                   z2 = 47.0;
                else if ( (z1-0.255005)*(z1-0.255005)<1e-12 )
                   z2 = 48.0;
                else if ( (z1-0.273363)*(z1-0.273363)<1e-12 )
                   z2 = 49.0;
                else if ( (z1-0.292989)*(z1-0.292989)<1e-12 )
                   z2 = 50.0;
                else if ( (z1-0.313970)*(z1-0.313970)<1e-12 )
                   z2 = 51.0;
                else if ( (z1-0.336400)*(z1-0.336400)<1e-12 )
                   z2 = 52.0;
                else if ( (z1-0.360380)*(z1-0.360380)<1e-12 )
                   z2 = 53.0;
                else if ( (z1-0.386016)*(z1-0.386016)<1e-12 )
                   z2 = 54.0;
                else if ( (z1-0.413422)*(z1-0.413422)<1e-12 )
                   z2 = 55.0;
                else if ( (z1-0.442722)*(z1-0.442722)<1e-12 )
                   z2 = 56.0;
                else if ( (z1-0.474046)*(z1-0.474046)<1e-12 )
                   z2 = 57.0;
                else if ( (z1-0.507533)*(z1-0.507533)<1e-12 )
                   z2 = 58.0;
                else if ( (z1-0.543333)*(z1-0.543333)<1e-12 )
                   z2 = 59.0;
                else if ( (z1-0.581606)*(z1-0.581606)<1e-12 )
                   z2 = 60.0;
                else if ( (z1-0.622522)*(z1-0.622522)<1e-12 )
                   z2 = 61.0;
                else if ( (z1-0.666265)*(z1-0.666265)<1e-12 )
                   z2 = 62.0;
                else if ( (z1-0.713029)*(z1-0.713029)<1e-12 )
                   z2 = 63.0;
                else if ( (z1-0.763023)*(z1-0.763023)<1e-12 )
                   z2 = 64.0;
                else if ( (z1-0.816470)*(z1-0.816470)<1e-12 )
                   z2 = 65.0;
                else if ( (z1-0.873609)*(z1-0.873609)<1e-12 )
                   z2 = 66.0;
                else if ( (z1-0.934695)*(z1-0.934695)<1e-12 )
                   z2 = 67.0;
                else if ( (z1-1)*(z1-1)<1e-12 )
                   z2 = 68.0;


                int coordx = (int)(x2+0.5);
                int coordy = (int)(y2+0.5);
                int coordz = (int)z2;

                if (coordx>383)
                   cout << "Warning X: " << x1 << endl;
                else if (coordy>255)
                   cout << "Warning Y: " << y1 << endl;
                else if (coordz>68)
                   cout << "Warning Z: " << z1 << endl;


                int times = *(frequencymap + coordx + coordy*rows + coordz*rows*cols);
                *(frequencymap + coordx + coordy*rows + coordz*rows*cols) = times + 1;

                float value = *(valuemap + coordx + coordy*rows + coordz*rows*cols);
                *(valuemap + coordx + coordy*rows + coordz*rows*cols) = value + val;
            }
         }
     }

     fp.close();
 
     return volume;
}




void VolRenderWindow::SaveVTKFile(string FileName, int x_dim, int y_dim, int z_dim)
{  
    FILE *pFile;
    float *Xarray, *Yarray, *Zarray;   
    float *OriginalData = new float [x_dim*y_dim*z_dim];

    pFile = fopen(FileName.c_str(), "r");
    unsigned long point_num = fread(OriginalData, sizeof(float), x_dim*y_dim*z_dim, pFile);

    vtkFloatArray *coords[3] = {0, 0, 0};   

    coords[0] = vtkFloatArray::New();
    coords[0]->SetNumberOfTuples((vtkIdType) x_dim);  
    float *xarray = (float *) coords[0]->GetVoidPointer(0);
    for (int i=0; i < x_dim; i++)
        xarray[i] = i;
      
    coords[1] = vtkFloatArray::New();
    coords[1]->SetNumberOfTuples((vtkIdType) y_dim);
    float *yarray = (float *) coords[1]->GetVoidPointer(0);
    for (int i=0; i < y_dim; i++)
        yarray[i] = i;
 
    coords[2] = vtkFloatArray::New();
    coords[2]->SetNumberOfTuples((vtkIdType) z_dim);
    float *zarray = (float *) coords[2]->GetVoidPointer(0);
    for (int i=0; i < z_dim; i++)
        zarray[i] = i;

    vtkRectilinearGrid *rgrid = vtkRectilinearGrid::New();
    rgrid->SetDimensions(x_dim, y_dim, z_dim);
    
    rgrid->SetXCoordinates(coords[0]);
    coords[0]->Delete();
    rgrid->SetYCoordinates(coords[1]);
    coords[1]->Delete();
    rgrid->SetZCoordinates(coords[2]);
    coords[2]->Delete();

    vtkDataSet *mesh; 
    mesh = rgrid;
     
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();   
    scalars->SetNumberOfTuples ((vtkIdType) point_num);
    float *ptr = (float *) scalars->GetVoidPointer (0);
    memcpy (ptr, OriginalData, sizeof (float) * point_num);
    
    vtkDataArray *var = scalars;      
    mesh->GetPointData()->SetScalars(var);

    // Write file
    ofstream mywritefile;
    mywritefile.open("averageactor.vtk");
	
    mywritefile<<"# vtk DataFile Version 3.0 "<<endl;
    mywritefile<<"vtk output "<<endl;
    mywritefile<<"ASCII \nDATASET RECTILINEAR_GRID \nDIMENSIONS ";
    mywritefile<<x_dim<<" "<<y_dim<<" "<<z_dim<<endl; 

    mywritefile<<"X_COORDINATES "<<x_dim<<" float"<<endl;
    for (int i = 0; i < x_dim; i++)
        mywritefile << i << endl;
    mywritefile << endl;

    mywritefile<<"Y_COORDINATES "<<y_dim<<" float"<<endl;	
    for (int i = 0; i < y_dim; i++)
        mywritefile << i << endl;
    mywritefile<<endl;
        
    mywritefile<<"Z_COORDINATES "<<z_dim<<" float"<<endl;	
    for (int i = 0; i < z_dim; i++)
        mywritefile << i << endl;
    mywritefile<<endl;

    mywritefile<<"POINT_DATA "<<x_dim*y_dim*z_dim<<endl;
    mywritefile<<"SCALARS scalars float \nLOOKUP_TABLE default"<<endl;

    for (int i = 0; i < x_dim*y_dim*z_dim; i++)
        mywritefile << OriginalData[i] << endl;
	
    mywritefile.close();

    delete [] OriginalData;
    fclose(pFile);
}

