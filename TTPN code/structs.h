#ifndef MY_STRUCTURES_FILE_
#define MY_STRUCTURES_FILE_

#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

struct FrameTable
{
   int *objects;
   int object_num;
   int *places;
};


struct Packet
{
   int *objectlist; 
   int object_num;
};


struct PacketTable
{
   vector<Packet> packetlist;
   int packet_num;
};


struct SingleActivity
{
   vector<string> filename;
   int count;
   int *framelist;
   int *placelist;
   int *objectlist;
   int startframe;
   int startplace;
   int startobject;
   int endframe;
   int endplace;
   int endobject;
};


struct TimeStep
{
    int PlaceID;
    vector<int> objectlist;
    int object_num;
};


struct Activity
{
    int *singleactivitylist;
    int singleactivity_num;
    TimeStep *timestep;
    int startframe;
    int endframe;
    vector<string> filename;
    int *framelist;
    int *placelist;
};

#endif
