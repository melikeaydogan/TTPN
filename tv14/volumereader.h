#ifndef VOLUME_READER_FILE_
#define VOLUME_READER_FILE_


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <iostream>  
#include <cmath>  
#include <time.h>  
 
#include <algorithm> 

using namespace std;  

unsigned long volumereader(string uocdfile, int object_num, int *objectlist, int *frequencymap,
                            float *valuemap, int rows, int cols, int slices)
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

/*
     for (int i1=0; i1<object_num; i1++)    
         cout << objectlist[i1] << "  ";
     
     cout << endl;
*/
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
            fp>>objID>>vol>>mass>>cx>>cy>>cz>>Ixx>>Iyy>>Izz>>Ixy>>Iyz>>Izx>>vertID>>x1>>y1>>z1>>val;
        
            volume = volume + vol;
            for (int j=0; j<vol; j++)
            {
                fp>>vertID>>x1>>y1>>z1>>val;
                int coordx = (int)(x1+0.5);
                int coordy = (int)(y1+0.5);
                int coordz = (int)(z1+0.5);

                int value = *(space + coordx + coordy*rows + coordz*rows*cols);
                *(space + coordx + coordy*rows + coordz*rows*cols) = value + 1;
            }
         }
     }

     fp.close();
 
     return volume;
}

#endif
