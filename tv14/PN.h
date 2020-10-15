#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <math.h>
#include<iostream>
#include <assert.h>
#include "parser.h"
#include "interfaceutil.h"
//#include <direct.h>

using namespace std;

class StObjSummary
{
public:
  int FrameNumber; //this usually starts from initialFrameNumber which is 1 since our data index starts from 1.
  vector<int> ObjectID;  // consider converting this to vector<int> ObjectID, because of the previous objectIDs may be more than 1..
  int PlaceID;   // place number
  int transitionID; // place number
};

class PNvarible
{
  public:

  int Framenumber;  // 1: previous frame  2: current frame
  int AttributeNumber;  // column number in the attribute file
  string PNVariable;  // this is the string variable such as (F0A2 - F-1A2) etc
  string FilesPath;  // this is the generic file path
  string PlacesFileBaseName; // this is the file name in which we are looking for the attributenumber for the attributes. Note: this is just the base name for the file..
  string PlacesFileExtension; // this is the file extension
  string TransitionFileBaseName; // this is the file name in which we are looking for the attributenumber for the transitions. Note: this is just the base name for the file..
  string TransitionFileExtension; // this is the file extension


  bool UseDefaultActions;  // default actions are Merge, Split, Contiuation, dead and birth ..
};


class PNAllVaribles
{
  public:

  string FilePath;
  string PlaceFileBaseName;
  string PlaceFileExtension;
  string TransitionFileBaseName;
  string TransitionFileExtension;
  vector<string> transitionVector;
  vector<string> placeVector;
  vector<string> PetriNetVariables;
  vector<int> FinalPlaces;
  vector<int> InitialPlaces;
  //vector<Arc> outputArcs;
  //vector<Arc> inputArcs;
  int PNumber;
  int transitionNumber;
  int isTracking;
  int initialFrameNumber;
  int FinalFrameNumber;
  string TrackingFileName; // this is the file that keeps all the tracking history in it..
  string DefaultActionsFileExtention;
  string DefaultACtionFileBaseName;
};


class stObject
{

  public:

  int         RealObjectID; //line number in trak file , starts from 1
  bool        assignedObject;  // this is to check whether the object is assigned to any place before. This should be used if and only if an object is allowed to be in only one place at a time.
  bool        activeObject;  // 1 means active, 0 means object is dead. This is to check whether the object in petri net is still being tracked or just dead there for some reason...
  int         objsCurrentPlaceID;
  long        FrameNumber;
  vector<StObjSummary> ObjectSummary; // save the history of this object in this vector...
  vector<double> ObjectCurrentAttributes;
    long CurrentDurationInTheCurrentPlace;
};


class stObject1
{

  public:

  bool        assignedObject;  // this is to check whether the object is assigned to any place before. This should be used if and only if an object is allowed to be in only one place at a time.
  bool        activeObject;  // 1 means active, 0 means object is dead. This is to check whether the object in petri net is still being tracked or just dead there for some reason...
  int         objsCurrentPlaceID; //
//  float       objectStateValue;  // this is the float value
  int         objectsTransitionID; // when compared to the current and previous frames, if this object performs an action that defines one of the transitions, set this value.
  int       objectsActionValue;  // this is the actual value that is used to compare to the Transition, such as: growth value, expantion value, etc.
  vector<StObjSummary> ObjectSummary; // save the history of this object in this vector...
  vector<int> currentFiredArcID_List;
  vector<float> StatesVector;
  int         RealObjectID; //line number in trak file , starts from 1
  long        FrameNumber;
  long        objNum;
  long        numCell;
  long        maxNode;
  long        minNode;
  float       objMax;
  float       objMin;
  float       minX;
  float       minY;
  float       minZ;
  float       maxX;
  float       maxY;
  float       maxZ;
  long        volume;
  float       mass;
  float       centroidX;
  float       centroidY;
  float       centroidZ;
  float       Ixx;
  float       Iyy;
  float       Izz;
  float       Ixy;
  float       Iyz;
  float       Izx;
  float       LowerLeft_Corner_x;
  float       LowerLeft_Corner_y;
  float       LowerLeft_Corner_z;
  float       UpperRight_Corner_x;
  float       UpperRight_Corner_y;
  float       UpperRight_Corner_z;
  long        Packet_ID;
  bool        packetFlag;
};

class StEvent
{
  public:
  int FirstState_FrameNumber;
  int FirstState_ObjectID;
  int SecondState_FrameNumber;
  int SecondState_ObjectID;
  int ThirdState_FrameNumber;
  int ThirdState_ObjectID;
};

class Arc_self
{
  public:
  int TransitionID;
  int PlaceID;  //this nodeID is a placeID if the arc goes to a transition, and it is a transitionID if the arc goes to a place
  int arcweight;
  bool Fired; //fired = 1 if this arc is fired, fired=0 if not fired. // this is obsolete
};

class pnTransition
{
  public:
  int TransitionID; // this is unique to each transition
  float TransitionValue; // this can be the same for all transitions..<=== this is obselete.. can be deleted in the future..
  int Greater; // -1=smaller, +1=greater and 0=equal... <=== this is obselete.. can be deleted in the future..
  std::vector<Arc_self> incomingArcs; // incoming arcs are the input arcs defined in the Petrinet config file
  std::vector<Arc_self> outgoingArcs; // outgoing arcs are the output arcs defined in the Petrinet config file
  PNvarible transitionStatement;
  string TransitionCondition;
};

class pnPlace
{
  public:
  bool InitialPlace;
  bool FinalPlace;
  int PlaceID;
  int Greater;          // <=== this is obselete.. can be deleted in the future..
  int AttributeNumber;
  float StateValue;  // <=== this is obselete.. can be deleted in the future..
  vector<Arc_self> incomingArcs;  // these are the arcs from the conf file (inputarcs line)
  vector<Arc_self> outgoingArcs;  // these are the arcs from the conf file (outputarcs line)
  vector<stObject> currentObjectsInThisPlace; //marking
  string PlaceCondition;
};

class ThePetriNet
{
  public:
  PNAllVaribles ConfigFileInfo;
  vector<pnTransition> CurrentTransitions;
  vector<pnPlace> CurrentPlaces;
  vector<stObject> PreviousObjects;
  vector<stObject> ObjectsThatBelongtoNoPlace; // this is the place where one object change does not satisfy the place's state info and cant be put into another place..
  bool allowClonedObject; // 0 means one object can be only at one place at a time.. 1 means it can be in multiple states (places) at a time..
  int currentFrame; // this is the frame number that the objects are from in all the places..
  vector<string> CurrentFramefullTransitionAttributeFile;
  vector<string> PreviousFramefullTransitionAttributeFile;
  vector<string> CurrentFramefullPlaceAttributeFile;
  vector<string> PreviousFramefullPlaceAttributeFile;
};

vector<string>
Tokenize(const string &sentence)
{
  stringstream ss(sentence);
  istream_iterator<string> it(ss);
  istream_iterator<string> end;
  vector<string> tokens(it,end);
  return tokens;
}

void
stringtokenizer( vector<string> & OutputStringVector,  const  string  & InputString,  const  string  & lookfor )
{
  size_t  begin = 0, end = 0;
  while ( end != string::npos )
  {
    end = InputString.find( lookfor, begin );
    OutputStringVector.push_back( InputString.substr( begin, (end == string::npos) ? string::npos : end - begin ) );
    begin = (   ( end > (string::npos - lookfor.size()) ) ?  string::npos  :  end + lookfor.size()    );
  }
  //check for empty elements..
  for(int i=0; i< OutputStringVector.size();i++)
  {
    if (OutputStringVector[i].size() == 0)
      OutputStringVector.erase(OutputStringVector.begin() + i);
  }
}

void findAndReplace(string &MainExpression,string findThisString, string replaceWithThisString )
{
  // this method takes a string (MainExpression), looks for the "findThisString" and replaces it with "replaceWithThisString"..
  // then returns the new MainExpression ...
  string::size_type counter = 0;
  while ( (counter = MainExpression.find(findThisString, counter)) != string::npos )
  {
    MainExpression.replace( counter, findThisString.size(), replaceWithThisString );
    counter++;
  }
}


void readFile(string fullFilename, vector<string> &fullFrameInfo)
{
  fullFrameInfo.clear();
  ifstream input1;
  input1.open(fullFilename.c_str());

  if(input1.is_open() == false){
      cout<<"can't open .trak file"<<endl;
      exit(1);
  }

  string str1;
  getline(input1, str1);
  while(!input1.eof())
  {
    fullFrameInfo.push_back(str1);
    getline(input1, str1);
  }
  input1.close();

}


void ParseVariable(const string &sentence, int & Framenumber, int & AttributeNumber)
{
  // this method takes a string (inthe form of FXXAYY (or fXXaYY), where F means the Frame and XX means the Frame number, A means Attribute and YY means the Attribute number;
  // and tokenize it acording to Frame and Attribute numbers. It returns Framenumber and AttributeNumber ..

  string dum1 = "F";
  string dum2 = "f";
  string dum3 = "A";
  string dum4 = "a";
  string dum5 = "D";
  string dum6 = "d";
  string dummyval;
  dummyval = sentence;
  size_t found1, found2, found3, found4, found5, found6;
  vector<int> FrameNumberAndAttributeNumber;
  FrameNumberAndAttributeNumber.clear();
  found1 = sentence.find(dum1); // F
  found2 = sentence.find(dum2); // f
  found3 = sentence.find(dum3); // A
  found4 = sentence.find(dum4); // a
  found5 = sentence.find(dum5); // D
  found6 = sentence.find(dum6); // d
  if ((found1!=string::npos)|| (found2!=string::npos) )  // check if F or f is in the variable
  {
    if ( found2 < found1 )
    {
     found1 = found2;
    }
  }
  if ((found3!=string::npos)|| (found4!=string::npos) )  // check if A or a is in the variable
  {
    if ( found4 < found3 )
    {
     found3 = found4;
    }
  }
  if ((found5!=string::npos)|| (found6!=string::npos) )  // check if D or d is in the variable
  {
    if ( found6 < found5 )
    {
     found5 = found6;
    }
  }

  if ((found1!=string::npos)&&(found3!=string::npos))
  {
    if ( (found1 == 0) && ( found3 > found1 ) )
    {
      dummyval = sentence.substr(1,found3);
      Framenumber = atoi(dummyval.c_str());
      dummyval = sentence.substr(found3+1,sentence.size()-1);
      AttributeNumber = atoi(dummyval.c_str());
    }
    else
      cout<<"Error!! First letter in variables must be F (Ex: F0A2 or F0A10)."<< endl<<"Please check PetriNet config file for the variable definitions.!!!!"<<endl;

  }
  if ((found1!=string::npos)&&(found5!=string::npos)) // this the default Actions to be used here...
  {
    if ( (found1 == 0) && ( found5 > found1 ) )
    {
      dummyval = sentence.substr(1,found5);
      Framenumber = atoi(dummyval.c_str());
      dummyval = sentence.substr(found5+1,sentence.size()-1);
      AttributeNumber = atoi(dummyval.c_str());
      AttributeNumber = AttributeNumber * -1; // negate the attribute number for the default actions... This is how we will distinct between default actions and regular attributes
    }
    else if (Framenumber !=0)
      cout<<"Error!! First letter in variables must be F (Ex variables: F0A2, F0A10, etc.)."<< endl<<"Please check PetriNet config file for the variable definitions.!!!!"<<endl;

  }
}



void findVariablesintheAttributeFiles(ThePetriNet &CurrentPetriNet, string file,string fileExtention, int currentFrame,int Frame, int AttributeNumber, int ObjectID, double &VariableValue, string & StrVersionofVariable )
{
  // this method loads the attribute value for the given ObjectID. in order to do that, the PNVariables have to be parsed first so that we can get the Frame and attribute numbers to load from the proper attribute files..
  // This method returns the attribute value in both float and string forms (VariableValue and  StrVersionofVariable)...

  //objectID starts from 1 !!......
  if (Frame > 0)
    cout<< "Error: in PNVariable, Frame number cannot be greater than 0 !!"<<endl<<"Please check the PetriNet config file and correct the variable definitions..!!"<<endl;

  string fullfileName;
  string currentline;
  string  buf;
  string fName;
  float  time;
  int counter = 0;
  vector<string> tokensforcurrentline;
  tokensforcurrentline.clear();
  bool dummyTestVariable = 0;

  fullfileName = file.c_str() + TtoS<int>((int)( currentFrame + Frame  ))+ fileExtention.c_str();
  if ((currentFrame + Frame ) < 1 )
  {
    dummyTestVariable = 1;
    StrVersionofVariable = "0";
    VariableValue = 0;
  }
  else
  {
    //cout<<fullfileName<< " is being opened now!"<<endl;
    ifstream fp(fullfileName.c_str(), ios_base::in);
    if(!fp.is_open())
    {
      cout<<"ERROR: " <<fullfileName <<"cannot be opened!!!"<<endl;
      //   return false;
    }

    while(1)
    {
      counter++;
      getline(fp,buf,'\n');
      if(!fp.good())
    break;
      if (counter == ObjectID)
      {
    tokensforcurrentline = Tokenize(buf.c_str());
    VariableValue = atof(tokensforcurrentline[AttributeNumber - 1].c_str()); // attribute numbers start from 1 in the config file !!...
    StrVersionofVariable = tokensforcurrentline[AttributeNumber - 1].c_str();
    dummyTestVariable = 1;
    break;
      }
    }
    fp.close();
    if (dummyTestVariable == 0 )
    {
      cout<< "Error!!! : for the object: ["<< ObjectID <<"], the column (attribute number) ["<<AttributeNumber<<"] is not found in the file: "<<fullfileName<<endl;
    }
  }
}





string precision_time(int const time,int const precision)
{

     int quotient = time, remainder = 0;
     int count = 1;
     // THE TOTAL NO OF DIGITS SHOULD EQUAL ATLEAST PRECISION
     while(quotient!=0)
     {
          quotient = quotient/10;
      if(quotient!=0)
      {
           count = count+1;
      }
     }
     string buf1;

     for(int i = 0; i< (precision - count);i++)
     {
          buf1+="0";

     }
     buf1+=TtoS<int>(time);
     return buf1;
}



void ReadVtkDataFile(vector<string> &variableNamesvect,string FileName, vector<float> &allvariables,
             vector<float> &xdimension,vector<float> &ydimension,vector<float> &zdimension,
             vector<float> &minvals,vector<float> &maxvals, int &x_dim,int &y_dim,int &z_dim,int &nnodes)
{
  cout<<" -----------  Reading the data file: "<<FileName<<endl;
  string output,variable_name, dummy,x_name,y_name,z_name,dim_label;
  int i(0),ncells,cellpoints,nncomp,count,count1,count2,found(0);
  long mindummy2(9999999);

  x_name= string("Name=\"X_COORDINATES\"").c_str();
  y_name= string("Name=\"Y_COORDINATES\"").c_str();
  z_name= string("Name=\"Z_COORDINATES\"").c_str();
  dim_label=string("Extent=\"1").c_str();
  //cout<<"num "<<num<<endl;
  // extract the data values into one variable (vector);

  ifstream myreadfile;
  myreadfile.open(FileName.c_str(),ios_base::out);
  if (myreadfile.is_open())
  {
    while (!myreadfile.eof())
    {
      myreadfile >> output;

      if ( output == dim_label)
      {
    myreadfile >> output;
    x_dim = atoi(output.c_str());//cout<<"x dim is: "<<x_dim<<endl;
    myreadfile >> output; myreadfile >> output;
    y_dim = atoi(output.c_str());//cout<<"y dim is: "<<y_dim<<endl;
    myreadfile >> output; myreadfile >> output;
        int t = output.size();
    output.resize (t-2);
    z_dim = atoi(output.c_str());//cout<<"z dim is: "<<z_dim<<endl;
    myreadfile >> output;
      }
      if ( output == x_name)
      {
    myreadfile >> output; myreadfile >> output; myreadfile >> output;
    while ((output != "</DataArray>"))
    {
     // cout<<"inside the while loop"<<endl;
        float f = atof(output.c_str());
        xdimension.push_back(f);//
            myreadfile >> output;
    }
      }

       if ( output == y_name)
      {
    myreadfile >> output; myreadfile >> output; myreadfile >> output;
    while ((output != "</DataArray>"))
    {
     // cout<<"inside the while loop"<<endl;
        float f = atof(output.c_str());
        ydimension.push_back(f);//
            myreadfile >> output;
    }
      }
      if ( output == z_name)
      {
    myreadfile >> output; myreadfile >> output; myreadfile >> output;
    while ((output != "</DataArray>"))
    {
     // cout<<"inside the while loop"<<endl;
        float f = atof(output.c_str());
        zdimension.push_back(f);//
            myreadfile >> output;
    }
      }

      for(int i=0;i<variableNamesvect.size();i++)  // compare the variable name to the ones that are entered..
      {
    dummy = string(variableNamesvect[i]).c_str();
    //cout<<"DUMMY : "<<dummy<<endl;
    variable_name=string("Name=\""+dummy+"\"").c_str();
    if ( output == variable_name)
      found=1;
      }
      if (found==1)
      {
    myreadfile >> output; myreadfile >> output; myreadfile >> output;
    while ((output != "</DataArray>"))
    {
     // cout<<"inside the while loop"<<endl;
        float f = atof(output.c_str());
        allvariables.push_back(f);//
            myreadfile >> output;
        //j=j+1;
    }
      }
      found=0; //cout<<"DATAARRAY found"<<endl;
    }
 //   cout<<j<<" "<<i<<" :<<j & i value"<<endl;
  }
  myreadfile.close();
  cout<<" allvariables.size &[0] & [end] : "<<allvariables.size()<<" "<<allvariables[0]<<" "<<allvariables[allvariables.size()-1]<<endl;
  cout<<" xdimension.size &[0] & [end] : "<<xdimension.size()<<" "<<xdimension[0]<<" "<<xdimension[xdimension.size()-1]<<endl;
  cout<<" ydimension.size &[0] & [end] : "<<ydimension.size()<<" "<<ydimension[0]<<" "<<ydimension[ydimension.size()-1]<<endl;
  cout<<" zdimension.size &[0] & [end] : "<<zdimension.size()<<" "<<zdimension[0]<<" "<<zdimension[zdimension.size()-1]<<endl;
  ncells = ( (x_dim-1) * (y_dim-1) * (z_dim-1));
  if (ncells == 0)
  {
    cout<<"Data is not 3D. Check the data dimensions!!.."<<endl<<"Data is being converted to 3D!!"<<endl;

    mindummy2 = 99999999;
    if (z_dim <= 1 ) // find which dimension is equal to 1
    {
      z_dim = 2; // make it 3D and populate the second plane with the min value
      zdimension.push_back(zdimension[0]+0.0001);
    }
    else if (x_dim <= 1 )
    {
      x_dim = 2;
      xdimension.push_back(xdimension[0] + 0.0001);
    }
    else
    {
      ydimension.push_back(ydimension[0] + 0.0001);
      y_dim = 2;
    }
    for (i=0; i< allvariables.size(); i++ ) //find the min value
    {
      if ( allvariables[i] < mindummy2 )
        mindummy2 = allvariables[i];
    }
    int dummysize = allvariables.size();
    for (i=0; i< dummysize; i++ )
      allvariables.push_back(mindummy2); //fill the second z layer with all min values..
  }

}




void CreateNodeConn_Array(int** node_conn_array,int z_dim,int y_dim,
              int x_dim,int numberOfPointsInCell,int ncells)
{ // this works for 8 point cubical cells (for rectilinear data)

//Though it game correct answers for the test data, please check this formulae again for coordinates to make sure that it is accurate...
  int count = 0;
  int *t_node_conn_array = new int[numberOfPointsInCell*ncells];

  for( int z = 0; z < z_dim-1; z++)
  {
    for( int y = 0; y < y_dim-1; y++)
    {
      for( int x = 0; x < x_dim-1; x++)
      {
    t_node_conn_array[count++] = x + y * x_dim + z*(x_dim*y_dim);
    t_node_conn_array[count++] = x + y * x_dim + 1 + z*(x_dim*y_dim);
    t_node_conn_array[count++] = x  + y * x_dim + z*(x_dim*y_dim) + x_dim;
    t_node_conn_array[count++] = x + 1 + y * x_dim + z*(x_dim*y_dim) + x_dim;
    t_node_conn_array[count++] = x  + y * x_dim + z*(x_dim*y_dim) + x_dim*y_dim;
    t_node_conn_array[count++] = x  + y * x_dim + 1 + z*(x_dim*y_dim) + x_dim*y_dim;
    t_node_conn_array[count++] = x  + y * x_dim + z*(x_dim*y_dim)+x_dim*y_dim+x_dim;
    t_node_conn_array[count++] = x  + y * x_dim + 1 + z*(x_dim*y_dim)+x_dim*y_dim+x_dim;
      }
    }
  }
  *node_conn_array = t_node_conn_array;
}


void CreateCoord_Array(vector<float> &allvariables,vector<float> &xdimension,
               vector<float> &ydimension, vector<float> &zdimension,
               float ** coord_array,float ** node_data,vector<float> &minvals,
               vector<float> &maxvals,int nspace,int nnodes,int ncells,
               int cellpoints,int nncomp)
{

  int count(0), count1(0),count2(0);
  int z_dim = zdimension.size();
  int y_dim = ydimension.size();
  int x_dim = xdimension.size();

  float *t_coord_array = new float[nnodes*nspace];
  float *t_node_data = new float[nnodes*nncomp];

  for(int z=0;z<z_dim;z++)
  {
    for(int y=0;y<y_dim;y++)
    {
      for(int x=0;x<x_dim;x++)
      {
    t_coord_array[count++] = xdimension[x];
    t_coord_array[count++] = ydimension[y];
    t_coord_array[count++] = zdimension[z];
    for(int xx=0;xx<nncomp;xx++)
    {
      t_node_data[count1++] = allvariables [count2+xx*nnodes];

      //------ this block finds min and max vals for the data
      if ((minvals.size()== xx) || ( minvals.size() == 0 ) )
        minvals.push_back(allvariables [count2+xx*nnodes]);
      else
      {
        if ( minvals[xx] > allvariables [count2+xx*nnodes])
          minvals[xx] = allvariables [count2+xx*nnodes];
      }
      if ((maxvals.size() == xx) || ( maxvals.size() == 0 ))
        maxvals.push_back(allvariables [count2+xx*nnodes]);
      else
      {
        if ( maxvals[xx] < allvariables [count2+xx*nnodes])
          maxvals[xx] = allvariables [count2+xx*nnodes];
      }
      //---- min - max ends here ---------------------------
    }
    count2++;
      }
    }
  }
  *coord_array = t_coord_array;
  * node_data = t_node_data;
}




//-------------------parseConfigFile  FUNCTION----------
//
//------------------------------------------------------

void parseConfigFile(string &Datapath, string &FileBaseName,string &FileExtention,vector<string> &variableNamesvect,string Configfilename, int &InitialtimeStep, int &FinaltimeStep,
            float &deltaxval, float &deltayval, float &deltazval,int &SmallestObjVol,
            int &TimePrecision, int &TimeIncrement, float &thresh1, float &thresh2)
{

  string output, datapath, base_filename, initialTimeStep, finalTimeStep,timestepIncrement,fileExtention,
  Threshold1,Threshold2, timestepPrecision, deltaxthreshold,deltaythreshold,deltazthreshold,
  smallestObjVolToTrack, variableNames;
  //vector<string> variableNamesvect;
  cout<<" -----------  Reading the "<<Configfilename<< " Config file ---------"<<endl;
  datapath = string("DATA_FILES_PATH:").c_str();
  base_filename = string("FILE_BASE_NAME:").c_str();
  initialTimeStep = string("INITIAL_TIME_STEP:").c_str();
  finalTimeStep = string("FINAL_TIME_STEP:").c_str();
  timestepIncrement = string("TIME_STEP_INCREMENT:").c_str();
  Threshold1 = string("THRESHOLD1:").c_str();
  Threshold2 = string("THRESHOLD2:").c_str();
  timestepPrecision = string("TIME_STEP_PRECISION:").c_str();
  deltaxthreshold = string("DELTA_X_THRESHOLD:").c_str();
  deltaythreshold = string("DELTA_Y_THRESHOLD:").c_str();
  deltazthreshold = string("DELTA_Z_THRESHOLD:").c_str();
  smallestObjVolToTrack = string("SMALLEST_OBJECT_VOLUME_TO_TRACK:").c_str();
  variableNames = string("VARIABLE_NAMES:").c_str();
  fileExtention = string("FILE_EXTENSION:").c_str();

  ifstream myreadfile;
  myreadfile.open(Configfilename.c_str(),ios_base::out);
  if (myreadfile.is_open())
  {

    while (!myreadfile.eof())
    {
      myreadfile >> output;

      if ( output == datapath)
      {
    myreadfile >> output;
    Datapath = output;
      }

      if ( output == fileExtention)
      {
    myreadfile >> output;
    FileExtention = output;
      }

      if ( output == base_filename)
      {
    myreadfile >> output;
    FileBaseName = output;
      }
      if ( output == initialTimeStep)
      {
    myreadfile >> output;
    InitialtimeStep = atoi(output.c_str());
      }
      if ( output == finalTimeStep)
      {
    myreadfile >> output;
    FinaltimeStep = atoi(output.c_str());
      }
      if ( output == timestepIncrement)
      {
    myreadfile >> output;
    TimeIncrement = atoi(output.c_str());
      }
      if ( output == Threshold1)
      {
    myreadfile >> output;
    thresh1 = atof(output.c_str());
      }
      if ( output == Threshold2)
      {
    myreadfile >> output;
    thresh2 = atof(output.c_str());
      }

      if ( output == timestepPrecision)
      {
    myreadfile >> output;
    TimePrecision = atoi(output.c_str());
      }

      if ( output == deltaxthreshold)
      {
    myreadfile >> output;
    deltaxval = atof(output.c_str());
      }
      if ( output == deltaythreshold)
      {
    myreadfile >> output;
    deltayval = atof(output.c_str());
      }
      if ( output == deltazthreshold)
      {
    myreadfile >> output;
    deltazval = atof(output.c_str());
      }
      if ( output == smallestObjVolToTrack)
      {
    myreadfile >> output;
    SmallestObjVol = atoi(output.c_str());
      }
      if ( output == variableNames)
      {
    myreadfile >> output;
    variableNamesvect.push_back(output.c_str());
    myreadfile >> output;
    variableNamesvect.push_back(output.c_str());
      }
    }
    myreadfile.close();
 //   cout<<j<<" "<<i<<" :<<j & i value"<<endl;
  }
  else
  {
    cout<< "Cannot open the FeatureTrack.Conf File.!!!"<<endl;
  }
  cout<<" Data Path: "<< Datapath <<endl;
  cout<<" base_filename: "<< FileBaseName <<endl;
  cout<<" file_extension: "<< FileExtention<<endl;
  cout<<" initialTimeStep: "<< InitialtimeStep <<endl;
  cout<<" FinaltimeStep: "<< FinaltimeStep <<endl;
  cout<<" TimeIncrement: "<< TimeIncrement <<endl;
  cout<<" thresh1: "<< thresh1 <<endl;
  cout<<" thresh2: "<< thresh2<<endl;
  cout<<" TimePrecision: "<< TimePrecision <<endl;
  cout<<" deltaxval: "<< deltaxval <<endl;
  cout<<" deltayval: "<< deltayval <<endl;
  cout<<" deltazval: "<< deltazval <<endl;
  cout<<" SmallestObjVol: "<< SmallestObjVol <<endl;
  for(int i=0;i<variableNamesvect.size();i++)
    cout<<" variableNamesvect[ "<<i<<"]"<< variableNamesvect[i]<<endl;

}
//------------------------------------
//   int a = CreateListFile(datapath,listfile,InitialtimeStep,FinaltimeStep,TimeIncrement,TimePrecision );

int CreateListFile(string infile,string &listfile,int InitialtimeStep, int FinaltimeStep,int TimeIncrement,int TimePrecision )
{

    // string infile(atts.GetInputBaseFilePath());
     //#std::cout << "Now creating list file ....." << std::endl;
     string trk_dir;
     int p1,p2;
        //1.6 check if file path is abs else give err and return 0
     p1=infile.rfind('/');
     if (p1==string::npos)
     {
          cout<< "Please use an absolute path when specifying the filename.\n"<< std::endl;
          return 0;
     }

     trk_dir=infile.substr(0,p1+1)+"GENERATED_TRACK_FILES/";
     p2=infile.rfind('.');
     string datafile=infile.substr(p1+1,p2-p1-1); // h5file=bvector
     struct stat st;
     // if trk_dir doesn't exist
     if(stat(trk_dir.c_str(),&st))
     {
          if(mkdir(trk_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==-1)
      {
           cout << "Unable to create directory: " << trk_dir << std::endl;
           return 0;
      }
      // else if trk_dir exists, but is not a directory
      else if(!(st.st_mode & S_IFDIR))
      {
               cout << "Found a file instead of a directory. Please rename this file, then try again: "<< trk_dir << std::endl;
           return 0;
      }
     }
     // else, use existing directory
     //1.8 create list file and write to list file
     //construct the list file name
     int comp = 0; // ### I have no clue why this comp is used : Rohini
     string listf = trk_dir + datafile + "_" + "comp" + TtoS<int>((int)comp) +
            "_" + TtoS<int>(InitialtimeStep) + "_" + TtoS<int>(FinaltimeStep) +
            "_" + TtoS<int>(TimeIncrement) + ".list";
     //create the list file
     ofstream fp;
     fp.open(listf.c_str(),ios::out|ios::trunc);
     if (!fp.is_open())
     {
          cout << "Unable to open list file: "<< listf << endl;
      return 0;
     }
     //write the directory path
     fp << trk_dir << endl;
     for(register int i=InitialtimeStep;i<FinaltimeStep+1;i+=TimeIncrement)
     {
           string temp;
       temp=datafile+precision_time(i,TimePrecision);
       fp<<temp<<endl;
     }
     fp.close();
     listfile = listf;
     return 1;
}




//void ReadTrak(string const baseName,float &mass, int &volume, float &cx, float &cy, float &cz, float &l_x, float &l_y, float &l_z,float &u_x, float &u_y, float &u_z, int&Pac_ID, float &minX, float &minY, float &minZ,float &maxX, float &maxY, float &maxZ)

void CreateObjectsFortheCurrentFrame(string const trackFName, int framenumber,vector<stObject> &currentobjects)
{
 //read the entered trackFName file to read and create objects for the current frame with the object attributes...
     //string const trackFName(baseName+".trak");
     currentobjects.clear();
     ifstream fp(trackFName.c_str(), ios_base::in);
     if(!fp.is_open())
     {
       cout<<"In CreateObjectsFortheCurrentFrame: cannot open "<<trackFName<<endl;
       //   return false;
     }
     string  buf;
     string fName;
     vector<string> tokensforcurrentline;
     float  time;
     int counter = 0;
     while(1){
      counter++;
      stObject obj;
      obj.assignedObject = 0;
      obj.activeObject = 1;
      obj.ObjectSummary.clear();
      //obj.EventStatenumber = -2;
      obj.RealObjectID = counter;
      obj.objsCurrentPlaceID = -5;
      obj.FrameNumber = framenumber;
      getline(fp,buf,'\n');
      tokensforcurrentline.clear();

      if(!fp.good())
        break;

      tokensforcurrentline = Tokenize(buf.c_str());
      for (int i = 0; i <tokensforcurrentline.size(); i++ )
      {
        double currentattribute;
        //if (tokensforcurrentline[i].size()>0)
        std::istringstream inpStream(tokensforcurrentline[i]);
        if (inpStream >> currentattribute)
          obj.ObjectCurrentAttributes.push_back(currentattribute);
      }
      currentobjects.push_back(obj);
     }
    fp.close();
  //   return true;
}


// ------------------ READ EVENT FILES --------------
bool ReadEvent(string const trackFName,vector<int> &EventVector)
{
     EventVector.clear();
     //string const trackFName(baseName+".event");
     ifstream fp(trackFName.c_str(), ios_base::in);
     if(!fp.is_open())
     {
          cout<<"ReadEvent:cannot open "<<trackFName<<endl;
          return false;
     }
     string  buf;
     int eventnumber(0);
     while(1)
    {
         getline(fp,buf,'\n');
         if(!fp.good())
              break;
         istringstream  bufstr(buf,ios_base::out);
         bufstr>>eventnumber;
     if ( eventnumber > 0)
     {
       EventVector.push_back(eventnumber);
       //totalEventVector.push_back(eventnumber);
     }
     }
     fp.close();
     return true;
}

int ActionDetection(int cycle,string label, vector<string> &trakTable, int totalobjnumber )
{
  //reads the action
  vector<string> tokensforTrakTable;
  vector<int> actions; // 1 = new born; 2 = dead;  3 =seperatation ; 4 = merge; 5 = continuation
  // however note that whenever we go forward in time, the dead will not be noticed. it will only notices when going backward in time
  actions.resize(totalobjnumber);
  string previouslabel = label.c_str();
  int objnumber = previouslabel.size();
  string  prevcycle = TtoS<int>(cycle+1);
  int cyclenum = prevcycle.size();
  previouslabel.resize(objnumber-cyclenum);
  previouslabel = string(previouslabel+TtoS<int>(cycle)).c_str();

  string const EventFName(label+".event");
  string const PrevEventFName(previouslabel+".event");
//  cout<<"Inside ActionDetection, PrevEventFName: "<<PrevEventFName.c_str()<<endl;
//  cout<<"EventFName is: "<<EventFName<<" and cycle is: "<<cycle<<endl;
  FILE *fpout;
  if((fpout = fopen(EventFName.c_str(), "w")) == NULL)
    cout << "cannot open EventFName file to write\n";

//----------------traktable action search begins here------------------------------------
   for(int linenumber=0; linenumber < trakTable.size();linenumber++) //find the currentObj within the trakTable
   {

     tokensforTrakTable.clear();
     tokensforTrakTable = Tokenize(trakTable[linenumber].c_str());

     for(int i=0; i < tokensforTrakTable.size(); i++)
     {
       if ( tokensforTrakTable[i] == "-1")
       {
     if (i == 0)
       actions[atoi(tokensforTrakTable[1].c_str() )-1] = 1; //newborn
     else if ((i == 1) && (tokensforTrakTable.size() == 3 ))
     {
       if ( cycle == 1 )
         actions[atoi(tokensforTrakTable[2].c_str() )-1] = 5;
       else
       {
         int objectID = atoi(tokensforTrakTable[2].c_str() ); //line number in the event file
         //------------find the object inside the .event file
             ifstream fp(PrevEventFName.c_str(), ios_base::in);
         if(!fp.is_open())
         {
           cout << "cannot open PrevEventFName file to write\n";
           return false;
         }
         int counterr = 0;
         string  buf;
         while ( counterr != objectID )
         {
           getline(fp,buf,'\n');
           counterr++;
         }
         fp.close();
         //-------------------------------------
         istringstream  bufstr(buf,ios_base::out);
         bufstr>>counterr;
         if (counterr < 5 )
           actions[atoi(tokensforTrakTable[2].c_str() )-1] = 5;
         else
           actions[atoi(tokensforTrakTable[2].c_str() )-1] = counterr + 1;
       }
     }
     else if ( ( (tokensforTrakTable.size() - i) == 2) && (i > 1) )
       actions[atoi(tokensforTrakTable[i+1].c_str() )-1] = 4;
     else if ( (i==1) && (tokensforTrakTable.size() > 3) )
     {
       for(int j=2; j < tokensforTrakTable.size(); j++)
       {
         actions[atoi(tokensforTrakTable[j].c_str())-1] = 3;
       }
     }
       }
     }
   }

   for(int i=0; i < actions.size(); i++)
   {
      // cout << "actions["<<i<<"]: "<<actions[i]<< "\n";
       fprintf(fpout,"%5.0d",actions[i] );
       fprintf(fpout," \n");
   }
  fclose(fpout);
  return 1;
}



void ReadCurrentFrameTrackInfo(string trakTableFile, int cycle, vector<string> &trakTable)
{
        trakTable.clear();
    ifstream input1;
    input1.open(trakTableFile.c_str());

    if(!input1)
    {
        cout<<"\n ReadCurrentFrameTrackInfo: Cannot open "<<trakTableFile<<" file "<<endl;
        return;
    }

        string find_frame = "Frame #" +TtoS<int>(cycle);
    string str1;

        //cout<<"find frame = "<<find_frame<<endl;

    bool frame_found = false;

    while(!input1.eof())
    {
        getline(input1, str1);

        if(str1[0] == 'F')
        {
            if (str1 == find_frame)
            {
                frame_found = true;
            }
            else
            {
                frame_found = false;
            }
        }

        if((frame_found && str1[0] != 'F') && (str1.size() > 0 ))
        {
            trakTable.push_back(str1);
        }
    }
    input1.close();
}

//next function takes the first object within the current frame and finds its ID(s) in the the next frame
void findObjIDinNextFrame(vector<int> & nextObjects,vector<string> &trakTable, int currentObj )
{
  nextObjects.clear();
  int linenumber(0),k(0), previousObjnum(0),secondframe;
  vector<string> objectIDsinThisLine;//tokensforTrakTable;
  bool found = 0;
  while((linenumber < trakTable.size()) && !found ) //find the currentObj within the trakTable
   {
     objectIDsinThisLine.clear();
     objectIDsinThisLine = Tokenize(trakTable[linenumber].c_str()); //get the current line of the trakTable
     k = 0; // is the index for the objectIDsinThisLine
     previousObjnum = -1;
     secondframe = 0;
     nextObjects.clear();
     while ( (!found)  && ( k < objectIDsinThisLine.size() ) ) // check along the each line within the traktable for the current object
     {
       // k is the tokensforTrakTable index
       // remember objects start from 0 within the Packets, however they start from 1 within the .trakTable file, that is why we add 1 to currentObj...
       if (atoi(objectIDsinThisLine[k].c_str()) == currentObj) // if the object ID is found
       {
     for (int secondframe1 = 0; secondframe1 < objectIDsinThisLine.size(); secondframe1++) // search for the "-1" operator
     {
       if ((objectIDsinThisLine[secondframe1] == "-1") && (secondframe1 > k) ) //
       {
         found = 1;
         if (secondframe1 != 0 )
         {
           int dum1 = secondframe1+1;
           if (dum1 < objectIDsinThisLine.size())
           {
        // cout<<"tokensforTrakTable.size(): "<<tokensforTrakTable.size()<<endl;
         while ( dum1 < objectIDsinThisLine.size())
             {
           previousObjnum = atoi(objectIDsinThisLine[dum1].c_str());
               nextObjects.push_back(previousObjnum);
               dum1++;
             }
           }
           else
           {
        // cout<<"tokensforTrakTable.size(): "<<tokensforTrakTable.size()<<endl;
         nextObjects.push_back(-1);
           }
         }
       }
     }
       }
       k++;
     }
     linenumber++;
   }
   if (!found)
     nextObjects.push_back(-1);
}



void findObjIDinPreviousFrame(vector<int> & previousObjects,vector<string> &trakTable, int currentObj )
{
  // takes the currentObj number (starts from 1) and checks it in the tracktable file to find it in the second half
  // then returns the objectIDs in the first half in the previousObjects vector..
  // currentObj starts from 1 since it is in traktable...
  // it returns -1 (if the object does not exist in the previous frame) or returns all the possible objectIDs in the previousObjects
  //
  int linenumber(0),k(0), previousObjnum(0),secondframe;
  vector<string> tokensforTrakTable;
  bool found = 0;
  previousObjects.clear();

  while((linenumber < trakTable.size()) && !found ) //find the currentObj within the trakTable
   {
     tokensforTrakTable.clear();
     tokensforTrakTable = Tokenize(trakTable[linenumber].c_str()); //get the current line of the trakTable
     k = 0;
     previousObjnum = -1;
     secondframe = 0;
     while ( (!found)  && ( k < tokensforTrakTable.size() ) ) // check along the each line within th traktable for the current object
     {
       // k is the tokensforTrakTable index
       // remember objects start from 0 within the Packets, however they start from 1 within the .trakTable file, consider that!!!...
       if (atoi(tokensforTrakTable[k].c_str()) == currentObj) // we found the currentObj
       {
     for (int secondframe1 = 0; secondframe1 < tokensforTrakTable.size(); secondframe1++)
     {
       if ((tokensforTrakTable[secondframe1] == "-1") && (secondframe1 < k) ) // check if the found object within the first frame or within the second frame
       {
         found = 1;
         if (secondframe1 != 0 )
         {
           int dum1 = secondframe1-1;
           while ( 0 <= dum1)
           {
         previousObjnum = atoi(tokensforTrakTable[dum1].c_str());
             previousObjects.push_back(previousObjnum);
             dum1--;
           }
         }
         else
         {
           previousObjnum = -1;
           previousObjects.push_back(previousObjnum);
         }
       }
     }
       }
       k++;
     }
     linenumber++;
   }
}



void findVariablesintheAttributeFiles1(ThePetriNet &CurrentPetriNet, string file,string fileExtention, int currentFrame,
                       int Frame, int AttributeNumber, int ObjectID, double &VariableValue, string & StrVersionofVariable)
{
  // this method loads the attribute value for the given ObjectID. in order to do that, the PNVariables have to be parsed first so that we can get the Frame and attribute numbers to load from the proper attribute files..
  // This method returns the attribute value in both float and string forms (VariableValue and  StrVersionofVariable)...

  //objectID starts from 1 !!......
  VariableValue = 0;
  int currentObj = ObjectID;
  if (Frame > 0)
    cout<< "Error: in PNVariable, Frame number cannot be greater than 0 !!"<<endl<<"Please check the PetriNet config file and correct the variable definitions..!!"<<endl;

  string fullfileName;
  string currentline;
  string  buf;
  string fName;
  float  time;
  int counter = 0;
  int theDifferenceFromtheCurrentFrame = 0;
  vector<string> tokensforcurrentline;
  vector<int> previousObjects;
  vector<int> TwopreviousObjects;
  vector<int> AllpreviousObjects;
  previousObjects.clear();
  TwopreviousObjects.clear();
  tokensforcurrentline.clear();
  bool dummyTestVariable = 0;
  bool ThisisTrue = 1;
  vector<string> dummytrakTable;
  dummytrakTable.clear();
  ReadCurrentFrameTrackInfo(CurrentPetriNet.ConfigFileInfo.TrackingFileName,currentFrame, dummytrakTable);
  Frame = Frame * (-1); //make it positive number.. take the absolute value
  int dummycurrentFrame = Frame;
  previousObjects.push_back(ObjectID);
  while ( (ThisisTrue) && (dummycurrentFrame != 0))
  {
    //check for all the objects in the previous frames..
    //ie. if there are two previous objects and we need to go one below frame, than check for those two objects' previous objects as well..
    theDifferenceFromtheCurrentFrame++;
    dummycurrentFrame--; // currentFrame number is reduced..
    AllpreviousObjects.clear();
    for (int i = 0; i < previousObjects.size(); i++ )
    {
      if (previousObjects[i] != -1 )
      {
    currentObj = previousObjects[i];
    findObjIDinPreviousFrame(TwopreviousObjects,dummytrakTable,currentObj );
    for (int j = 0; j < TwopreviousObjects.size(); j++ )
    {
      if (TwopreviousObjects[j] != -1 )
        AllpreviousObjects.push_back(TwopreviousObjects[j]);
    }
      }
    }
    previousObjects = AllpreviousObjects;
    // now AllpreviousObjects has all the objectIDs in the previous frame that are not -1..
    if ( (AllpreviousObjects.size() == 0) || ( theDifferenceFromtheCurrentFrame == Frame) ) // either this is one of the earliest frames or the object does not exist in the previous frames..
    {
      ThisisTrue = 0;  // we are done... break the while...
    }
    else
    {
      dummytrakTable.clear();
      ReadCurrentFrameTrackInfo(CurrentPetriNet.ConfigFileInfo.TrackingFileName, (currentFrame-theDifferenceFromtheCurrentFrame), dummytrakTable);
    }
  }


  if (previousObjects.size() == 0)
  {
    StrVersionofVariable = "0";
    VariableValue = 0;
  }
  else
  {
    fullfileName = file.c_str() + TtoS<int>((int)( currentFrame - Frame  ))+ fileExtention.c_str();
    VariableValue = 0;
    for(int i = 0; i < previousObjects.size(); i++ )
    {
      //cout<<fullfileName<< " is being opened now!"<<endl;
      ifstream fp(fullfileName.c_str(), ios_base::in);
      if(!fp.is_open())
      {
    cout<<"ERROR: " <<fullfileName <<"cannot be opened!!!"<<endl;
    //   return false;
      }
      counter = 0;
      while(1)
      {
    counter++;
    getline(fp,buf,'\n');
    if(!fp.good())
      break;
    if (counter == previousObjects[i])
    {
      tokensforcurrentline = Tokenize(buf.c_str());
      VariableValue = VariableValue + atof(tokensforcurrentline[AttributeNumber - 1].c_str()); // attribute numbers start from 1 in the config file !!...
    //  StrVersionofVariable = tokensforcurrentline[AttributeNumber - 1].c_str();
      dummyTestVariable = 1;
      break;
    }
      }
      StrVersionofVariable = TtoS<double>(VariableValue);
      fp.close();
      if (dummyTestVariable == 0 )
      {
    cout<< "Error!!! : for the object: ["<< previousObjects[i] <<"], the column (attribute number) ["<<AttributeNumber<<"] is not found in the file: "<<fullfileName<<endl;
      }
    }

  }
}



void findVariablesintheAttributeFiles2(ThePetriNet &CurrentPetriNet, int currentFrame,int Frame, int AttributeNumber, int ObjectID,
                                       double &VariableValue, string & StrVersionofVariable,int transitionOrPlace, long currentDurationinthePlace)
{
    // this method loads the attribute value for the given ObjectID (ObjectID starts from 1). in order to do that, the PNVariables have to be parsed first so that we can get the Frame and attribute numbers to load from the proper attribute files..
    // This method returns the attribute value in both float and string forms (VariableValue and  StrVersionofVariable)...
    // this one only accepts -1 and 0 as frame values.. any smaller value will return error...
    //objectID starts from 1 !!......

    // cout<<"Inside the findVariablesintheAttributeFiles2! The frame and AttributeNumber are: "<<Frame<<" & "<<AttributeNumber<<endl;

    StrVersionofVariable ="1>2";
    VariableValue = 0;
    int currentObj = ObjectID;

    string fullfileName;
    string currentline;
    string  buf;
    string fName;
    float  time;
    int counter = 0;
    int theDifferenceFromtheCurrentFrame = 0;
    vector<string> tokensforcurrentline;
    vector<int> previousObjects;
    vector<int> TwopreviousObjects;
    vector<int> AllpreviousObjects;
    previousObjects.clear();
    TwopreviousObjects.clear();
    tokensforcurrentline.clear();
    bool dummyTestVariable = 0;
    bool ThisisTrue = 1;
    //Frame = Frame * (-1); //make it positive number.. take the absolute value
    int dummycurrentFrame = Frame;
    previousObjects.push_back(ObjectID);
    // this is the same comparison as the above...
    if (Frame > 0)
        cout<< "Error: in PNVariable, Frame number cannot be greater than 0. !!"<<endl<<"Please check the PetriNet config file and correct the variable definitions..!!"<<endl;


    // cout<<"Inside the findVariablesintheAttributeFiles2! The frame and AttributeNumber are: "<<Frame<<" & "<<AttributeNumber<<endl;
    // ===================== For the negative attributes use belove ========================
    if ( ((Frame == 0) && (AttributeNumber < 0)) && (currentFrame > CurrentPetriNet.ConfigFileInfo.initialFrameNumber) )
    {
        // cout<<"Inside the findVariablesintheAttributeFiles2! 1 "<<endl;
        string fullfileName;
        string currentline;
        string fullpath = CurrentPetriNet.ConfigFileInfo.FilePath.c_str();
        string defaultActionFileName = CurrentPetriNet.ConfigFileInfo.DefaultACtionFileBaseName.c_str();
        string DefaultActionsFileExtention = CurrentPetriNet.ConfigFileInfo.DefaultActionsFileExtention.c_str();
        string  buf;
        string fName;
        int counter = 0;
        vector<string> tokensforcurrentline;
        tokensforcurrentline.clear();
        bool dummyTestVariable = 0;
        vector<int> currentActions;
        fullfileName = string (fullpath+defaultActionFileName+TtoS<int>((int)( currentFrame + Frame  ))+DefaultActionsFileExtention).c_str();
        // if (currentFrame > CurrentPetriNet.ConfigFileInfo.initialFrameNumber)
        bool a = ReadEvent(fullfileName.c_str(),currentActions);
        int currentObjAction = currentActions[ObjectID - 1];
        int dummycurrentAttribute = -1*AttributeNumber;


        if ( dummycurrentAttribute < 5 )
        {
            if (currentObjAction == dummycurrentAttribute)
            {
                StrVersionofVariable = "2>1"; // true
                VariableValue = 1;
            }
            else
            {
                StrVersionofVariable = "1>2"; // false
                VariableValue = 0;
            }


        }
        else if (dummycurrentAttribute == 5 )
        {
            currentObjAction = currentObjAction - 4; // for continuation number....
            if (currentObjAction < 0)
                currentObjAction = 0;
            StrVersionofVariable = TtoS<int>((int)( currentObjAction  ));
            VariableValue = (double) currentObjAction;
        }
        else if ( dummycurrentAttribute == 6 )
        {
            VariableValue = (double) currentDurationinthePlace;
            StrVersionofVariable = TtoS<long>((long)( currentDurationinthePlace  ));

        }
        else
        {

            StrVersionofVariable = "1>2"; // false
            VariableValue = 0;
        }
        //read event file.. and extract the related value and return it..
        //cout<<fullfileName<< " is being opened now!"<<endl;
    }

    // ===================== For the positive attributes use belove ========================
    else if ((Frame == 0) && (AttributeNumber > 0))
    {
        //cout<<"Inside the findVariablesintheAttributeFiles2! 2 "<<endl;
        string  buf;
        vector<string> tokensforcurrentline;
        tokensforcurrentline.clear();
        if (transitionOrPlace == 0 ) // this is a place check...
            buf = CurrentPetriNet.CurrentFramefullPlaceAttributeFile[ObjectID -1 ];
        else if (transitionOrPlace == 1 ) // this is a transition check
            buf = CurrentPetriNet.CurrentFramefullTransitionAttributeFile[ObjectID -1 ];
        else
            cout<<"transitionOrPlace parameter can be only 1 or 0. Please fix that in your code !!"<<endl;
        string fullfileName;
        string currentline;
        tokensforcurrentline = Tokenize(buf.c_str());
        VariableValue = atof(tokensforcurrentline[AttributeNumber - 1].c_str()); // attribute numbers start from 1 in the config file !!...
        StrVersionofVariable = tokensforcurrentline[AttributeNumber - 1].c_str();
    }
    else if ( ((Frame == -1) && (AttributeNumber > 0)) && (currentFrame > CurrentPetriNet.ConfigFileInfo.initialFrameNumber) )
    {
        //cout<<"Inside the findVariablesintheAttributeFiles2! 3 "<<endl;
        string  buf;
        vector<string> tokensforcurrentline;
        tokensforcurrentline.clear();
        vector<int> previousObjects;
        vector<string>  dummytrakTable;
        dummytrakTable.clear();

        ReadCurrentFrameTrackInfo(CurrentPetriNet.ConfigFileInfo.TrackingFileName, currentFrame, dummytrakTable);

        findObjIDinPreviousFrame(previousObjects,dummytrakTable, ObjectID );

        if (previousObjects[0] == -1)// this object does not exist in the previous frame. A new born object...
        {
            VariableValue = 0;
            StrVersionofVariable = TtoS<double>((double)( VariableValue  ));
        }
        else
        {
            VariableValue = 0;
            for (int k=0; k <previousObjects.size(); k++ )
            {

                int dummyObjId = previousObjects[k];
                //cout<<"Inside the findVariablesintheAttributeFiles2! 4 !,k & transitionOrPlace & dummyObjId are: "<< k << " & " <<transitionOrPlace <<" & "<<dummyObjId<<endl;
                if (transitionOrPlace == 0 ) // this is a place check...
                    buf = CurrentPetriNet.PreviousFramefullPlaceAttributeFile[dummyObjId -1 ];
                else if (transitionOrPlace == 1 ) // this is a transition check
                    buf = CurrentPetriNet.PreviousFramefullTransitionAttributeFile[dummyObjId -1 ];
                else
                    cout<<"Error: In findVariablesintheAttributeFiles2: transitionOrPlace parameter can be only 1 or 0 !!"<<endl;
                string fullfileName;
                string currentline;
                //cout<<"Inside the findVariablesintheAttributeFiles2! 5 !"<<endl;
                tokensforcurrentline = Tokenize(buf.c_str());
                VariableValue = VariableValue + atof(tokensforcurrentline[AttributeNumber - 1].c_str()); // attribute numbers start from 1 in the config file !!...
            }
            StrVersionofVariable = TtoS<double>((double)( VariableValue  ));
        }
    }
    else
    {
        cout<<"There is a wrong parameter entered into findVariablesintheAttributeFiles2 function!!"<<endl;
        // cout<<" The currentFrame, ObjectID, transitionOrPlace, Frame, AttributeNumber are: "<<currentFrame<< " & "<<ObjectID<< " & "<<transitionOrPlace<< " & "<<Frame<< " & "<< AttributeNumber<<endl;
    }
}



void EvaluateExpression(ThePetriNet &CurrentPetriNet, int index,int TransitionOrPlace, double & returnValue, int currentFrame,int ObjectID, long currentDurationinthePlace)
{
    //TransitionOrPlace = 0 means we are evaluating the place expression, 1 means the transition expression
    //evaluates the expression in the petrinet transition (or place) index and returns the double value of the evaluated expression for that transition (or place).

    //vector<string> trakTable;

    if (TransitionOrPlace == 1 ) // transition
    {
        string currentExpression = CurrentPetriNet.ConfigFileInfo.transitionVector[index];
        cout<<"IN EvaluateExpression for transition!! The currentExpression is: ["<< currentExpression.c_str()<<"] CurrentPetriNet.ConfigFileInfo.PetriNetVariables.size():["<<CurrentPetriNet.ConfigFileInfo.PetriNetVariables.size()<<"]"<<   endl;
        double VariableValue;
        string strValue;
        // string file = string(CurrentPetriNet.ConfigFileInfo.FilePath + CurrentPetriNet.ConfigFileInfo.TransitionFileBaseName).c_str();//CurrentPetriNet.ConfigFileInfo.FilePath + CurrentPetriNet.ConfigFileInfo.TransitionFileBaseName;
        //  string fileExtention = string(CurrentPetriNet.ConfigFileInfo.TransitionFileExtension).c_str();
        int frame,attributenumber;

        for (int i = 0; i < CurrentPetriNet.ConfigFileInfo.PetriNetVariables.size(); i++ ) // go over each Petrinet variable that are defined in the config file..
        {
            ParseVariable(CurrentPetriNet.ConfigFileInfo.PetriNetVariables[i], frame, attributenumber); // extract the frame and attribute numbers from the petrinetvariable
            cout<<"frame:["<<frame<<"], attributenumber:["<<attributenumber<<"]"<<endl;
            string findThisString = CurrentPetriNet.ConfigFileInfo.PetriNetVariables[i];
            string::size_type counter = 0;
            counter = currentExpression.find(findThisString, counter); //<============================================= use this before calling findVariablesintheAttributeFiles2 and findAndReplace functions to avoid confusions for unused variables in this place / transition
            cout<<"currentExpression:["<<currentExpression<<"], findThisString:["<<findThisString<<"], counter:["<<counter<<"], string::npos:["<<string::npos<<"]"<<endl;
            if (counter != string::npos )
            {
                cout<<"inside evaluateExpression before findVariablesintheAttributeFiles2!VariableValue;["<<VariableValue<<"], strValue:[" <<strValue<<"], "<<endl;
                findVariablesintheAttributeFiles2(CurrentPetriNet, currentFrame,frame,attributenumber,ObjectID,VariableValue, strValue,1,currentDurationinthePlace );
                findAndReplace(currentExpression , CurrentPetriNet.ConfigFileInfo.PetriNetVariables[i].c_str(), strValue );
                cout<<"strValue:["<<strValue<<"]"<<endl;
            }
        }
        // cout<<"in EvaluateExpression for Transition: currentExpression is: "<< currentExpression <<endl;
        Parser p ( currentExpression.c_str() );
        returnValue = p.Evaluate ();
        //  cout<<"IN EvaluateExpression!! The currentExpression and returnValue are: "<< currentExpression.c_str()<<" & "<<returnValue<<endl;
    }
    else if (TransitionOrPlace == 0 ) // place
    {
        string currentExpression = CurrentPetriNet.ConfigFileInfo.placeVector[index] ;
        double VariableValue;
        string strValue;

        int frame,attributenumber;
        cout<<"IN EvaluateExpression for place!! The currentExpression is: "<< currentExpression.c_str()<<endl;

        for (int i = 0; i < CurrentPetriNet.ConfigFileInfo.PetriNetVariables.size(); i++ )
        {
            ParseVariable(CurrentPetriNet.ConfigFileInfo.PetriNetVariables[i], frame, attributenumber); // extract the frame and attribute numbers from the petrinetvariable
            string findThisString = CurrentPetriNet.ConfigFileInfo.PetriNetVariables[i];
            string::size_type counter = 0;
            counter = currentExpression.find(findThisString, counter); //<============================================= use this before calling findVariablesintheAttributeFiles2 and findAndReplace functions to avoid confusions for unused variables in this place / transition
            if (counter != string::npos )
            {
                findVariablesintheAttributeFiles2(CurrentPetriNet,currentFrame,frame,attributenumber,ObjectID,VariableValue, strValue,0,currentDurationinthePlace );
                findAndReplace(currentExpression , CurrentPetriNet.ConfigFileInfo.PetriNetVariables[i].c_str(), strValue );
            }
        }
        //cout<<"in EvaluateExpression for Place: currentExpression is: "<< currentExpression <<endl;
        Parser p ( currentExpression.c_str() );
        returnValue = p.Evaluate ();
    }
    else
        cout<<"Please enter a valid parameter into the EvaluateExpression. the parameter: TransitionOrPlace can be either 0 or 1 only.."<<endl;
}




void readPT_ConfFile(string &filename,PNAllVaribles & currentAllVaribles,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs)
{

    string outputval;
    string PNFullTrackingFileName = string("TrackingFileName").c_str();
    string PNDefaultACtionFileBaseName = string("DefaultActionsFileBaseName").c_str();
    string PNDefaultActionsFileExtention = string("DefaultActionsFileExtention").c_str();
    string PNYes = string("YES").c_str();
    string PNNo = string("NO").c_str();
    string PNFileFinalValue = string("FinalFrameNumber").c_str();
    string PNFileInitialValue = string("InitialFrameNumber").c_str();
    string PNTracking = string("IsTrackingInfoProvided").c_str();
    string PNPlacesFileExtension = string("PlacesFileExtension").c_str();
    string PNPlacesFileBaseName = string("PlacesFileBaseName").c_str();
    string PNTransitionsFileExtension = string("TransitionsFileExtension").c_str();
    string PNTransitionsFileBaseName = string("TransitionsFileBaseName").c_str();
    string PNFilePath = string("FilesPath").c_str();
    string PNVariables = string("Variables").c_str();
    string initialplc = string("InitialPlaceIDs").c_str();
    string Trns = string("Transitions").c_str();
    string Plcs = string("Places").c_str();
    string incm = string("inputarcs").c_str();
    string outgn = string("outputarcs").c_str();
    string finalplc = string("FinalPlaceIDs").c_str();
    string dummySentence;
    currentAllVaribles.PNumber = 0;
    currentAllVaribles.transitionNumber = 0;
    ifstream myreadfile;
    vector<string> dummyVector;
    myreadfile.open(filename.c_str(),ios_base::out);
    if (myreadfile.is_open())
    {
        while (myreadfile.peek()!=EOF)
        {
            getline(myreadfile,outputval);
            dummyVector = Tokenize(outputval.c_str());

            //      for( int i=0;i<dummyVector.size();i++)
            //	cout<<"dummyVector["<<i<<"] is: "<<dummyVector[i]<<endl;
            if ( dummyVector[0].c_str() == Trns)
            {
                dummySentence = dummyVector[2].c_str(); // ignore the first two elements which are "Transitions" and "="
                for(int kk=1;kk<dummyVector.size() - 2; kk++)
                {
                    dummySentence = dummySentence + dummyVector[kk+2].c_str();
                    //transitionVector.push_back(dummyVector[kk+2]);
                }
                stringtokenizer( currentAllVaribles.transitionVector,dummySentence,";" );
                currentAllVaribles.transitionNumber = currentAllVaribles.transitionVector.size();
            }


            if ( dummyVector[0].c_str() == PNVariables)
            {
                dummySentence = dummyVector[2].c_str();
                for (int k=2; k<dummyVector.size();k++)
                {
                    currentAllVaribles.PetriNetVariables.push_back(dummyVector[k].c_str());
                }
            }



            if ( dummyVector[0].c_str() == Plcs)
            {
                dummySentence = dummyVector[2].c_str();
                for(int kk=1;kk<dummyVector.size() - 2; kk++)
                {
                    dummySentence = dummySentence + dummyVector[kk+2].c_str();
                    //transitionVector.push_back(dummyVector[kk+2]);
                }
                stringtokenizer( currentAllVaribles.placeVector,dummySentence,";" );
                currentAllVaribles.PNumber = currentAllVaribles.placeVector.size();
            }

            if ( dummyVector[0].c_str() == PNTracking)
            {
                dummySentence = dummyVector[2].c_str();
                if (dummySentence.c_str() == PNYes)
                {
                    currentAllVaribles.isTracking = 1;
                }
                else if (dummySentence.c_str() == PNNo)
                    currentAllVaribles.isTracking = 0;
                else
                    cout<<"There is an error in the PetriNet config file. Please check the \"Tracking\" line. It can be only YES or NO"<<endl;
            }
            if ( dummyVector[0].c_str() == PNFileInitialValue)
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.initialFrameNumber = atoi(dummySentence.c_str());
            }
            if ( dummyVector[0].c_str() == PNFileFinalValue)
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.FinalFrameNumber = atoi(dummySentence.c_str());
            }
            if ( dummyVector[0].c_str() == PNFilePath)
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.FilePath = dummySentence.c_str();
            }
            if ( dummyVector[0].c_str() == PNDefaultActionsFileExtention)
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.DefaultActionsFileExtention = dummySentence.c_str();
            }
            if ( dummyVector[0].c_str() == PNDefaultACtionFileBaseName)  // <======================================================
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.DefaultACtionFileBaseName = dummySentence.c_str();
            }
            if ( dummyVector[0].c_str() == PNPlacesFileExtension)
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.PlaceFileExtension = dummySentence.c_str();
            }
            if ( dummyVector[0].c_str() == PNPlacesFileBaseName)
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.PlaceFileBaseName = dummySentence.c_str();
            }
            if ( dummyVector[0].c_str() == PNTransitionsFileBaseName)
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.TransitionFileBaseName = dummySentence.c_str();
            }
            if ( dummyVector[0].c_str() == PNFullTrackingFileName) //  <==================================  TrackingFile Name
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.TrackingFileName = dummySentence.c_str();
            }
            if ( dummyVector[0].c_str() == PNTransitionsFileExtension)
            {
                dummySentence = dummyVector[2].c_str();
                currentAllVaribles.TransitionFileExtension = dummySentence.c_str();
            }
            if ( dummyVector[0].c_str() == incm)
            {
                for (int k=0; k<(dummyVector.size()-2)/3;k++)
                {
                    Arc_self currentArc;
                    currentArc.Fired = 0; // this is obselete. "Fired" is not being used... to be deleted soon. :)
                    outputval = dummyVector[3*k+2];
                    currentArc.PlaceID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+3];
                    currentArc.TransitionID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+4];
                    currentArc.arcweight = atoi(outputval.c_str());
                    inputArcs.push_back(currentArc);
                }
            }
            if ( dummyVector[0].c_str() == outgn)
            {
                for (int k = 0; k < (dummyVector.size()-2)/3; k++)
                {
                    Arc_self currentArc;
                    currentArc.Fired = 0; // this is obselete. "Fired" is not being used... to be deleted soon. :)
                    outputval = dummyVector[3*k+2];
                    currentArc.PlaceID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+3];
                    currentArc.TransitionID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+4];
                    currentArc.arcweight = atoi(outputval.c_str());
                    outputArcs.push_back(currentArc);
                }
            }


            if ( dummyVector[0].c_str() == finalplc)
            {
                for (int k=2; k<dummyVector.size();k++)
                {
                    int dummyname = atoi(dummyVector[k].c_str());
                    currentAllVaribles.FinalPlaces.push_back(dummyname);
                }

            }

            if ( dummyVector[0].c_str() == initialplc)
            {
                for (int k=2; k<dummyVector.size();k++)
                {
                    int dummyname = atoi(dummyVector[k].c_str());
                    currentAllVaribles.InitialPlaces.push_back(dummyname);
                }

            }


        }
        myreadfile.close();
    }
    else
        cout<<"PetriNet.Conf file cannot be opened!!!"<<endl;

    for(int tt=0; tt < currentAllVaribles.transitionVector.size();tt++ )
    {
        cout<<"PARSED with ; TransitionVector["<<tt<<"]: &  its size are: "<< currentAllVaribles.transitionVector[tt].c_str()<<" & "<<currentAllVaribles.transitionVector[tt].size() <<endl;
    }
}





void readPT_ConfFile0(string &filename, int &PNumber, int &transitionNumber,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs, vector<int> &FinalPlaces,
             vector<string> &transitionVector,vector<string> &placeVector,vector<int> &InitialPlaces,vector<string> &PetriNetVariables)
{

    string outputval;


    string PNFileFinalValue = string("FinalFrameNumber").c_str();
    string PNFileInitialValue = string("InitialFrameNumber").c_str();
    string PNTracking = string("TrackingInfoProvided").c_str();
    string PNPlacesFileExtension = string("PlacesFileExtension").c_str();
    string PNPlacesFileBaseName = string("PlacesFileBaseName").c_str();
    string PNTransitionsFileExtension = string("TransitionsFileExtension").c_str();
    string PNTransitionsFileBaseName = string("TransitionsFileBaseName").c_str();
    string PNFilePath = string("FilesPath").c_str();
    string PNVariables = string("Variables").c_str();
    string initialplc = string("InitialPlaceIDs").c_str();
    string Trns = string("Transitions").c_str();
    string Plcs = string("Places").c_str();
    string incm = string("inputarcs").c_str();
    string outgn = string("outputarcs").c_str();
    string finalplc = string("FinalPlaceIDs").c_str();
    PNumber = 0;
    transitionNumber = 0;
    ifstream myreadfile;
    vector<string> dummyVector;
    myreadfile.open(filename.c_str(),ios_base::out);
    if (myreadfile.is_open())
    {
        while (!myreadfile.eof())
        {
            getline(myreadfile,outputval);
            dummyVector = Tokenize(outputval.c_str());

            //      for( int i=0;i<dummyVector.size();i++)
            //	cout<<"dummyVector["<<i<<"] is: "<<dummyVector[i]<<endl;
            if ( dummyVector[0].c_str() == Trns)
            {
                transitionNumber = dummyVector.size() - 2; // ignore the first two
                for(int kk=0;kk<dummyVector.size() - 2; kk++)
                {
                    transitionVector.push_back(dummyVector[kk+2]);
                }
            }

            if ( dummyVector[0].c_str() == PNVariables)
            {
                transitionNumber = dummyVector.size() - 2; // ignore the first two
                for(int kk=0;kk<dummyVector.size() - 2; kk++)
                {
                    PetriNetVariables.push_back(dummyVector[kk+2]);
                }
            }
            if ( dummyVector[0].c_str() == Plcs)
            {
                PNumber = dummyVector.size() - 2; // ignore the first two
                for(int kk=0;kk<dummyVector.size() - 2; kk++)
                {
                    placeVector.push_back(dummyVector[kk+2]);
                }
            }

            if ( dummyVector[0].c_str() == incm)
            {
                for (int k=0; k<(dummyVector.size()-2)/3;k++)
                {
                    Arc_self currentArc;
                    outputval = dummyVector[3*k+2];
                    currentArc.PlaceID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+3];
                    currentArc.TransitionID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+4];
                    currentArc.arcweight = atoi(outputval.c_str());
                    inputArcs.push_back(currentArc);
                }
            }
            if ( dummyVector[0].c_str() == outgn)
            {
                for (int k = 0; k < (dummyVector.size()-2)/3; k++)
                {
                    Arc_self currentArc;
                    outputval = dummyVector[3*k+2];
                    currentArc.PlaceID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+3];
                    currentArc.TransitionID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+4];
                    currentArc.arcweight = atoi(outputval.c_str());
                    outputArcs.push_back(currentArc);
                }
            }


            if ( dummyVector[0].c_str() == finalplc)
            {
                for (int k=2; k<dummyVector.size();k++)
                {
                    int dummyname = atoi(dummyVector[k].c_str());
                    FinalPlaces.push_back(dummyname);
                }
            }

            if ( dummyVector[0].c_str() == initialplc)
            {
                for (int k=2; k<dummyVector.size();k++)
                {
                    int dummyname = atoi(dummyVector[k].c_str());
                    InitialPlaces.push_back(dummyname);
                }
            }
        }
        myreadfile.close();
    }
    else
        cout<<"PetriNet.Conf file cannot be opened!!!"<<endl;
}


void readPT_ConfFile2(string &filename, int &PNumber, int &transitionNumber,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs, vector<int> &FinalPlaces,
                      vector<string> &transitionVector,vector<string> &placeVector,vector<int> &InitialPlaces,vector<string> &PetriNetVariables)
{

    string outputval;


    string PNFileFinalValue = string("FinalFrameNumber").c_str();
    string PNFileInitialValue = string("InitialFrameNumber").c_str();
    string PNTracking = string("TrackingInfoProvided").c_str();
    string PNPlacesFileExtension = string("PlacesFileExtension").c_str();
    string PNPlacesFileBaseName = string("PlacesFileBaseName").c_str();
    string PNTransitionsFileExtension = string("TransitionsFileExtension").c_str();
    string PNTransitionsFileBaseName = string("TransitionsFileBaseName").c_str();
    string PNFilePath = string("FilesPath").c_str();
    string PNVariables = string("Variables").c_str();
    string initialplc = string("InitialPlaceIDs").c_str();
    string Trns = string("Transitions").c_str();
    string Plcs = string("Places").c_str();
    string incm = string("inputarcs").c_str();
    string outgn = string("outputarcs").c_str();
    string finalplc = string("FinalPlaceIDs").c_str();
    PNumber = 0;
    transitionNumber = 0;
    ifstream myreadfile;
    vector<string> dummyVector;
    myreadfile.open(filename.c_str(),ios_base::out);
    if (myreadfile.is_open())
    {
        while (!myreadfile.eof())
        {
            getline(myreadfile,outputval);
            dummyVector = Tokenize(outputval.c_str());

            //      for( int i=0;i<dummyVector.size();i++)
            //	cout<<"dummyVector["<<i<<"] is: "<<dummyVector[i]<<endl;
            if ( dummyVector[0].c_str() == Trns)
            {
                transitionNumber = dummyVector.size() - 2; // ignore the first two
                for(int kk=0;kk<dummyVector.size() - 2; kk++)
                {
                    transitionVector.push_back(dummyVector[kk+2]);

                }
            }

            if ( dummyVector[0].c_str() == PNVariables)
            {
                transitionNumber = dummyVector.size() - 2; // ignore the first two
                for(int kk=0;kk<dummyVector.size() - 2; kk++)
                {
                    PetriNetVariables.push_back(dummyVector[kk+2]);

                }
            }
            if ( dummyVector[0].c_str() == Plcs)
            {
                PNumber = dummyVector.size() - 2; // ignore the first two
                for(int kk=0;kk<dummyVector.size() - 2; kk++)
                {
                    placeVector.push_back(dummyVector[kk+2]);

                }
            }

            if ( dummyVector[0].c_str() == incm)
            {
                for (int k=0; k<(dummyVector.size()-2)/3;k++)
                {
                    Arc_self currentArc;
                    outputval = dummyVector[3*k+2];
                    currentArc.PlaceID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+3];
                    currentArc.TransitionID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+4];
                    currentArc.arcweight = atoi(outputval.c_str());
                    inputArcs.push_back(currentArc);
                }
            }
            if ( dummyVector[0].c_str() == outgn)
            {
                for (int k = 0; k < (dummyVector.size()-2)/3; k++)
                {
                    Arc_self currentArc;
                    outputval = dummyVector[3*k+2];
                    currentArc.PlaceID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+3];
                    currentArc.TransitionID = atoi(outputval.c_str());
                    outputval = dummyVector[3*k+4];
                    currentArc.arcweight = atoi(outputval.c_str());
                    outputArcs.push_back(currentArc);
                }
            }


            if ( dummyVector[0].c_str() == finalplc)
            {
                for (int k=2; k<dummyVector.size();k++)
                {
                    int dummyname = atoi(dummyVector[k].c_str());
                    FinalPlaces.push_back(dummyname);
                }

            }

            if ( dummyVector[0].c_str() == initialplc)
            {
                for (int k=2; k<dummyVector.size();k++)
                {
                    int dummyname = atoi(dummyVector[k].c_str());
                    InitialPlaces.push_back(dummyname);
                }

            }


        }
        myreadfile.close();
    }
    else
        cout<<"PetriNet.Conf file cannot be opened!!!"<<endl;
}


//============ CHECK GREATER HERE: this function is obselete.. Delete in the future====================
//============ CHECK GREATER HERE ====================
//============ CHECK GREATER HERE ====================


int checkGreater(ThePetriNet &CurrentPetriNet, int PlaceOrTransition,int location, float currentValue)
{
  //returns 1 if the current transition or state satisfies the condition
  // returns 0 if the current transition or state does NOT satisfy the condition

  int currentswitcher = 0;
  if ( PlaceOrTransition == 0 ) // 0 means Place,   1 means Transition comparison
  {
      switch(CurrentPetriNet.CurrentPlaces[location].Greater)
      {
    case 0:
      if (CurrentPetriNet.CurrentPlaces[location].StateValue == currentValue)
        currentswitcher = 1;
      break;
    case -1:
      if (CurrentPetriNet.CurrentPlaces[location].StateValue > currentValue)
        currentswitcher = 1;
      break;
    case 1:
      if (CurrentPetriNet.CurrentPlaces[location].StateValue < currentValue)
        currentswitcher = 1;
      break;
      }

  }
  if ( PlaceOrTransition == 1 ) // 0 means Place,   1 means Transition comparison
  {

      switch(CurrentPetriNet.CurrentTransitions[location].Greater)
      {
    case 0:
      if (CurrentPetriNet.CurrentTransitions[location].TransitionValue == currentValue)
        currentswitcher = 1;
      break;
    case -1:
      if (CurrentPetriNet.CurrentTransitions[location].TransitionValue > currentValue)
        currentswitcher = 1;
      break;
    case 1:
      if (CurrentPetriNet.CurrentTransitions[location].TransitionValue < currentValue)
        currentswitcher = 1;
      break;
      }

  }
  return currentswitcher;
}



//============ CREATE PETRI NET HERE ====================
//============ CREATE PETRI NET HERE ====================
//============ CREATE PETRI NET HERE ====================



void createPT2(ThePetriNet &CurrentPetriNet, int PNumber, int transitionNumber,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs,vector<int> &finalPlaces,vector<string> &transitionVector,vector<string> &placeVector,vector<int> &initialPlaces,vector<int> &AttributeNumbers)
{
    CurrentPetriNet.CurrentTransitions.clear();
    CurrentPetriNet.CurrentPlaces.clear();
    CurrentPetriNet.currentFrame = -5; // there is no proper frame is being processed yet..
    CurrentPetriNet.allowClonedObject = 0; //From Sedat to Sedat: this is to be read from the config file in the future...


}

void createPT(ThePetriNet &CurrentPetriNet, vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs)
{
    int PNumber = CurrentPetriNet.ConfigFileInfo.PNumber;
    int transitionNumber = CurrentPetriNet.ConfigFileInfo.transitionNumber;
    CurrentPetriNet.CurrentTransitions.clear();
    CurrentPetriNet.CurrentPlaces.clear();
    CurrentPetriNet.currentFrame = -5; // there is no proper frame is being processed yet..
    CurrentPetriNet.allowClonedObject = 0; //From Sedat to Sedat: this is to be read from the config file in the future...


    for (int i = 0; i < transitionNumber; i++) // create this many transitions in a vector
    {

        // cout<<"Transition["<<i<<"] value is: "<<CurrentPetriNet.ConfigFileInfo.transitionVector[i]<<endl;
        pnTransition currentTransition;
        currentTransition.TransitionValue = 0; // this is no more being used. Obsolete. Can be deleted later. We are using transitioncondition anymore. :)
        currentTransition.TransitionCondition = CurrentPetriNet.ConfigFileInfo.transitionVector[i];
        currentTransition.Greater = 0; // 1 means T (transition) is greater than dummyval and 0 means T is smaller than dummyval....
        currentTransition.TransitionID = i+1;  // naming starts from 1 but i starts from 0...
        currentTransition.incomingArcs.clear();
        currentTransition.outgoingArcs.clear();
        for (int j = 0; j < inputArcs.size(); j++ )
        {
            Arc_self currentArc;
            currentArc = inputArcs[j];
            if (currentArc.TransitionID == i+1)
                currentTransition.incomingArcs.push_back(currentArc);
        }
        // input arcs are set for the current transition
        for (int j = 0; j < outputArcs.size(); j++ )
        {
            Arc_self currentArc;
            currentArc = outputArcs[j];
            if (currentArc.TransitionID == i+1)
                currentTransition.outgoingArcs.push_back(currentArc);
        }
        CurrentPetriNet.CurrentTransitions.push_back(currentTransition);
    }
    for (int i = 0; i < PNumber; i++) // create this many transitions in a vector
    {
        pnPlace currentPlace;
        currentPlace.PlaceCondition = CurrentPetriNet.ConfigFileInfo.placeVector[i];
        currentPlace.PlaceID = i+1; // assuming that transion and Place names (IDs) start from 1
        currentPlace.incomingArcs.clear();
        currentPlace.outgoingArcs.clear();
        currentPlace.currentObjectsInThisPlace.clear();
        currentPlace.Greater = 0;

        //atribute Numbers start from 1..
        for (int j = 0; j < inputArcs.size(); j++ )
        {
            Arc_self currentArc;
            currentArc = inputArcs[j];
            if (currentArc.PlaceID == i+1)
                currentPlace.incomingArcs.push_back(currentArc);
        }
        // input arcs are set for the current transition
        for (int j = 0; j < outputArcs.size(); j++ )
        {
            Arc_self currentArc;
            currentArc = outputArcs[j];
            if (currentArc.PlaceID == i+1)
                currentPlace.outgoingArcs.push_back(currentArc);
        }
        currentPlace.FinalPlace = 0;
        currentPlace.InitialPlace = 0;
        for (int j = 0; j < CurrentPetriNet.ConfigFileInfo.FinalPlaces.size(); j++ )
        {
            if (CurrentPetriNet.ConfigFileInfo.FinalPlaces[j] == i+1 )
                currentPlace.FinalPlace = 1;
        }
        for (int j = 0; j < CurrentPetriNet.ConfigFileInfo.InitialPlaces.size(); j++ )
        {
            if (CurrentPetriNet.ConfigFileInfo.InitialPlaces[j] == i+1 )
                currentPlace.InitialPlace = 1;
        }
        CurrentPetriNet.CurrentPlaces.push_back(currentPlace);
    }

}



// ===============   CHECK TRANSITIONFIRES =====================
// ===============   CHECK TRANSITIONFIRES =====================
// ===============   CHECK TRANSITIONFIRES =====================



int checkTransitionFired(ThePetriNet &CurrentPetriNet, int TransitionID, stObject curDummyObj,int currentFrame)
{
    // remember that TransitionID starts from 1
    // this function checks the state conditions of all the incoming places along with the activated transition.. and returns one of 3 possible values (-1 ,0, 1)
    // -1: does NOT satisfy the Transition Condition, 0: does not satisfy at least one of the incoming places' condition.. 1: it satisfies all and fires...
    int returnvalue = -1;
    int dummycounter = 0;
    int dummygreater = 0;
    double returnValue;
    //cout<<"EvaluateExpression is being called inside the checkTransitionFired!! currentFrame & curDummyObj.RealObjectID are: "<<currentFrame<<" & "<<curDummyObj.RealObjectID     <<endl;
     cout<<"In checkTransitionFired! before The EvaluateExpression !! " <<endl;

    EvaluateExpression(CurrentPetriNet,(TransitionID -1 ), 1, returnValue, currentFrame,curDummyObj.RealObjectID, curDummyObj.CurrentDurationInTheCurrentPlace);  // if the given object satisfies the transition condition then check if it satisfies all the incoming state conditions as well
    cout<<"In checkTransitionFired in currentframe: "<< currentFrame<<" ! The EvaluateExpression result is: "<< returnValue <<endl;
    //  dummygreater = checkGreater(CurrentPetriNet, 1, (TransitionID -1 ), curDummyObj.objectsActionValue); // if the given object satisfies the transition condition then check if it satisfies all the incoming state conditions as well
    //  if (dummygreater == 1 )
    if ( returnValue == 1 )
    {
        for (int dummyT=0; dummyT < CurrentPetriNet.CurrentTransitions[ TransitionID -1 ].incomingArcs.size(); dummyT++) // check for other possible places that may be connected to this transition
        {
            int dummyPlaceID = CurrentPetriNet.CurrentTransitions[ TransitionID -1 ].incomingArcs[dummyT].PlaceID;
            //check for each input to see if they are all ok to fire this transition..
            // cout<<"EvaluateExpression is being called inside the checkTransitionFired at part 2!!"<<endl;
            //  cout<<"In checkTransitionFired! before The EvaluateExpression !! " <<endl;
            EvaluateExpression(CurrentPetriNet,(dummyPlaceID -1), 0, returnValue, currentFrame,curDummyObj.RealObjectID, curDummyObj.CurrentDurationInTheCurrentPlace);
            //    cout<<"In checkTransitionFired2 in currentframe: "<< currentFrame<<" ! The EvaluateExpression result is: "<< returnValue <<endl;
            //  cout<<"In checkTransitionFired 2! The EvaluateExpression result is: "<< returnValue <<endl;
            //dummycounter = dummycounter + checkGreater(CurrentPetriNet, 0,(dummyPlaceID -1), curDummyObj.StatesVector[dummyPlaceID-1]);
            dummycounter = dummycounter + (int) returnValue;
        }
        if (dummycounter == CurrentPetriNet.CurrentTransitions[ TransitionID -1 ].incomingArcs.size() )
            returnvalue = 1;
        else if (CurrentPetriNet.CurrentTransitions[ TransitionID -1 ].incomingArcs.size() == 1)
        {
            returnvalue = 1;
        }
        else
            returnvalue = 0;
    }
    return returnvalue;
}



void updateObjects2(ThePetriNet &CurrentPetriNet, vector<stObject> &currentObjects, int currentFrame,vector<int> &initialPlaces,
                    vector<int> &finalPlaces, int InitialtimeStep, vector<string> &trakTable,string trakTableFile )
{
    // from Sedat to Sedat: in the future seperate the updateobjets from this function... for a better computation...   DONE!!!
    // create a newpetri net to move new objects into.. In the future, think another way than creating a new Petrinet.

    int Tsize = CurrentPetriNet.CurrentTransitions.size();
    int Psize = CurrentPetriNet.CurrentPlaces.size();
    bool changed = 1;
    int currentswitcher = 0;
    //cout<<"In updateObjects 0!  "<<endl;
    // if this is the initial frame, then put all the objects to initial place(s) only..
    if ( (currentFrame == InitialtimeStep) && (CurrentPetriNet.currentFrame < currentFrame) )  // put all the satisfying objects to initial state.
    {
        CurrentPetriNet.currentFrame = currentFrame;
        for(int dummyplace = 0; dummyplace < Psize; dummyplace++ )
        {
            if (CurrentPetriNet.CurrentPlaces[dummyplace].InitialPlace)
            {
                for(int curObject = 0; curObject < currentObjects.size(); curObject++ )
                {
                    double returnValue;
                    // cout<<"EvaluateExpression is being called inside the updateobjects 1!!!"<<endl;
                    currentObjects[curObject].CurrentDurationInTheCurrentPlace = 0;
                    long currentDuration = currentObjects[curObject].CurrentDurationInTheCurrentPlace;
                    EvaluateExpression(CurrentPetriNet,dummyplace, 0, returnValue, currentFrame,curObject + 1,currentDuration );
                    //  cout<<"In updateObjects 1! The EvaluateExpression result is: "<< returnValue <<endl;
                    //currentswitcher = checkGreater(CurrentPetriNet, 0, dummyplace, currentObjects[curObject].StatesVector[dummyplace]);
                    //if ( ( currentswitcher == 1 )  ) // object state satisfies the state condition.. therefore put it to the current initial place..
                    if ( returnValue == 1 )
                    {
                        //  if ((CurrentPetriNet.allowClonedObject == 0) && (currentObjects[curObject].assignedObject == 0) ) // <===== consider this in the future...
                        currentObjects[curObject].assignedObject = 1;  // else this will remain 0 ...
                        currentObjects[curObject].objsCurrentPlaceID = dummyplace + 1;
                        currentObjects[curObject].CurrentDurationInTheCurrentPlace = 0;
                        CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.push_back(currentObjects[curObject]);
                    }
                }
            }
        }
    }
    // This if statement works only once since it compares currentframes (and that is what we want since we are updating object attributes)...
    // Therefore Petrinet Needs to be implemented and run again seperately...
    if ((currentFrame > InitialtimeStep) && (CurrentPetriNet.currentFrame < currentFrame) ) // update the object IDs for the current Frame..
    {
        CurrentPetriNet.currentFrame = currentFrame;
        vector<stObject> currentPlaceObjects;
        vector<stObject> possiblyStateChangedObjects;
        currentPlaceObjects.clear();
        possiblyStateChangedObjects.clear();

        for(int dummyplace = 0; dummyplace < Psize; dummyplace++ )    // for each place
        {
            currentPlaceObjects.clear(); //first copy the currentobjects in the current place, and then clear it for the new (updated) objects..
            for (int Placeobj = 0; Placeobj < CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.size() ; Placeobj++)
            {
                currentPlaceObjects.push_back( CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace[Placeobj] );
            }
            CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.clear();
            //for (int Placeobj = 0; Placeobj < currentPlaceObjects.size() ; Placeobj++)
            while ( 0 < currentPlaceObjects.size() ) // go for each object in the current place and update its attributes, ID etc
            {
                stObject curDummyObj;
                curDummyObj = currentPlaceObjects[0];
                currentPlaceObjects.erase( currentPlaceObjects.begin() + 0);
                int PrevObjID = curDummyObj.RealObjectID; //find it in the next (this) frame and update its features...
                vector<int> nextObjIDs;
                nextObjIDs.clear();

                if (curDummyObj.activeObject == 1) // if the object is still in petrinet evaluation...
                {
                    StObjSummary currObjsSummary;
                    currObjsSummary.PlaceID = dummyplace + 1;
                    currObjsSummary.FrameNumber = currentFrame - 1;
                    //currObjsSummary.ObjectID.push_back( curDummyObj.RealObjectID );
//                    curDummyObj.ObjectSummary.push_back( currObjsSummary );

                    findObjIDinNextFrame(nextObjIDs, trakTable, PrevObjID);
                    if ( (nextObjIDs.size() == 1) && (nextObjIDs[0] == -1) ) // if object disappears
                    {
                        curDummyObj.activeObject = 0; // <============= this means object dissappears or somehow stops being in one of the PetriNet states.
                        CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.push_back( curDummyObj );
                    }
                    else //update new object(s) history..
                    {
                        for(int dummyNextObj=0; dummyNextObj < nextObjIDs.size() ; dummyNextObj++ )
                        {
                            int objectIndexInCurrentObjects;
                            objectIndexInCurrentObjects = nextObjIDs[dummyNextObj] - 1 ; // this is the index not ID... Therefore it starts from 0...

                            vector<int> dummyObjectListVect;
                            dummyObjectListVect.clear();
                            findObjIDinPreviousFrame(dummyObjectListVect,trakTable,objectIndexInCurrentObjects+1 );

                            if (dummyObjectListVect.size()>1) // if there is a merge happened in this frame, we should consider all the merged objects..
                            {
                                for (int dumyCounter = 0; dumyCounter< dummyObjectListVect.size(); dumyCounter++)
                                {
                                    currObjsSummary.ObjectID.push_back( dummyObjectListVect[dumyCounter]);
                                }
                            }
                            else
                            {
                                currObjsSummary.ObjectID.push_back( curDummyObj.RealObjectID );
                            }
                            curDummyObj.ObjectSummary.push_back( currObjsSummary );

                            currentObjects[ objectIndexInCurrentObjects ].objsCurrentPlaceID = dummyplace + 1; // this is actually the last known place for this new object. But we will take care of this later :)
                            // we subtrack 1 becs object indexes start from 1 in traktable. but in the array they start from 0 :)
                            for(int dummysummary = 0; dummysummary < curDummyObj.ObjectSummary.size() ; dummysummary++ )
                            {
                                currentObjects[ objectIndexInCurrentObjects ].ObjectSummary.push_back(curDummyObj.ObjectSummary[dummysummary]); //fill in the object summaries for the newobject
                            }

                            double returnValue;
                            //cout<<"In updateObjects 2! before The EvaluateExpression " <<endl;
                            //cout<<" dummyplace is[" <<dummyplace <<"],currentFrame is["<<currentFrame <<"],objectIndexInCurrentObjects is[" <<objectIndexInCurrentObjects<<"]"  << endl;
                            EvaluateExpression(CurrentPetriNet,dummyplace, 0, returnValue, currentFrame,objectIndexInCurrentObjects + 1,curDummyObj.CurrentDurationInTheCurrentPlace);
                            //cout<<"In updateObjects 2! The EvaluateExpression result is: "<< returnValue <<endl;
                            //int currentswitcher = checkGreater(CurrentPetriNet, 0, dummyplace, currentObjects[ objectIndexInCurrentObjects ].StatesVector[dummyplace]);
                            //if ( ( currentswitcher == 1 ) && (currentObjects[objectIndexInCurrentObjects].assignedObject == 0) )
                            if ( ( returnValue == 1 ) && (currentObjects[objectIndexInCurrentObjects].assignedObject == 0) ) // object state satisfies the state condition.. therefore put it to the current place
                            {
                                currentObjects[objectIndexInCurrentObjects].assignedObject = 1;
                                currentObjects[objectIndexInCurrentObjects].CurrentDurationInTheCurrentPlace = curDummyObj.CurrentDurationInTheCurrentPlace  + 1;
                                CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.push_back( currentObjects[objectIndexInCurrentObjects] );
                            }
                            else if (currentObjects[objectIndexInCurrentObjects].assignedObject == 0) // this object changed its state (place)... so check if it is eligible to be put into another place.. If not.. put it in another state...
                            {
                                currentObjects[objectIndexInCurrentObjects].CurrentDurationInTheCurrentPlace = 0;
                                possiblyStateChangedObjects.push_back(currentObjects[objectIndexInCurrentObjects]);
                            }
                        }
                    }
                }
                else  //if the object is not an active object anymore... just put it in its last known active place back again...
                {
                    CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.push_back( curDummyObj );
                }
            }

            //now check for each object in possiblyStateChangedObjects to see if they can be put into another state by evaluating the PN.
            //for(int dummyobj=0; dummyobj < possiblyStateChangedObjects.size() ; dummyobj++ )

        }

        while( 0 < possiblyStateChangedObjects.size())
        {

            stObject curDummyObj;
            curDummyObj = possiblyStateChangedObjects[0];
            possiblyStateChangedObjects.erase( possiblyStateChangedObjects.begin() + 0);
            int dummyplaceID = curDummyObj.objsCurrentPlaceID;
            //cout<<"inside the while loop: dummyplaceID["<<dummyplaceID << "] &  its incomingArcs.size["<<CurrentPetriNet.CurrentPlaces[ dummyplaceID - 1 ].incomingArcs.size()<<"]" <<endl;
            for (int dummyarc = 0; dummyarc < CurrentPetriNet.CurrentPlaces[ dummyplaceID - 1 ].incomingArcs.size(); dummyarc++)
            {
                int dummyTransitionID = CurrentPetriNet.CurrentPlaces[ dummyplaceID -1 ].incomingArcs[dummyarc].TransitionID;
                int checkvalue = checkTransitionFired(CurrentPetriNet, dummyTransitionID , curDummyObj,currentFrame );
                //cout<<"Inside while loop for possiblyStateChangedObjects!!! currentFrame["<< currentFrame<<"], dummyTransitionID["<<dummyTransitionID <<"], curDummyObj.objsCurrentPlaceID["<< curDummyObj.objsCurrentPlaceID<<"], checkvalue["<<checkvalue<<"]!"<<endl;
                if (checkvalue == 1)
                {
                    for(int dummyT=0; dummyT < CurrentPetriNet.CurrentTransitions[ dummyTransitionID -1 ].outgoingArcs.size(); dummyT++)
                    {
                        int newdummyPlaceID = CurrentPetriNet.CurrentTransitions[ dummyTransitionID -1 ].outgoingArcs[dummyT].PlaceID;
                        if (CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].FinalPlace == 1)
                        {
                            stObject curDummyObjClone;
                            curDummyObjClone.RealObjectID = curDummyObj.RealObjectID;
                            curDummyObjClone.assignedObject = curDummyObj.assignedObject;
                            curDummyObjClone.activeObject = 0;
                            curDummyObjClone.CurrentDurationInTheCurrentPlace = curDummyObj.CurrentDurationInTheCurrentPlace;
                            curDummyObjClone.objsCurrentPlaceID = curDummyObj.objsCurrentPlaceID;
                            curDummyObjClone.FrameNumber = curDummyObj.FrameNumber;
                            for(int dumyt=0; dumyt<curDummyObj.ObjectSummary.size(); dumyt++)
                            {
                                curDummyObjClone.ObjectSummary.push_back(curDummyObj.ObjectSummary[dumyt]);
                            }
                            for(int dumyt=0; dumyt<curDummyObj.ObjectCurrentAttributes.size(); dumyt++)
                            {
                                curDummyObjClone.ObjectCurrentAttributes.push_back(curDummyObj.ObjectCurrentAttributes[dumyt]);
                            }
                            CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObjClone);

                        }
                        curDummyObj.objsCurrentPlaceID = newdummyPlaceID;
                        curDummyObj.assignedObject = 1 ;
                        CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
                    }

                }
            }
            if (curDummyObj.assignedObject == 0)
                CurrentPetriNet.ObjectsThatBelongtoNoPlace.push_back(curDummyObj); // <============ in the future use this vector efficiently and properly..!! :)

        }
        for(int dumobj=0; dumobj <currentObjects.size(); dumobj++ )  // check for new objects to put them into the initial places...
        {
            if (currentObjects[dumobj].objsCurrentPlaceID < 0) // meaning that this object is not assigned to any place yet and they were not in any other place previously either...
            {
                for(int dummyplace = 0; dummyplace < Psize; dummyplace++ )
                {
                    if (CurrentPetriNet.CurrentPlaces[ dummyplace ].InitialPlace == 1)
                    {
                        double returnValue;
                        // cout<<"EvaluateExpression is being called inside the updateobjects at part 3!!!"<<endl;
                        EvaluateExpression(CurrentPetriNet,dummyplace, 0, returnValue, currentFrame, dumobj + 1, 0 );
                        //  cout<<"In updateObjects 3! The EvaluateExpression result is: "<< returnValue <<endl;
                        //int dummygreater = checkGreater(CurrentPetriNet, 0,dummyplace, currentObjects[dumobj].StatesVector[dummyplace]);
                        int dummygreater = (int) returnValue;
                        if (dummygreater == 1)
                        {
                            currentObjects[dumobj].objsCurrentPlaceID = dummyplace + 1;
                            currentObjects[dumobj].assignedObject = 1;
                            currentObjects[dumobj].CurrentDurationInTheCurrentPlace = 0;
                            CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.push_back(currentObjects[dumobj]);
                        }
                    }

                }

            }



        }
        //check for the disappering objects and deactivate them...

    }

    // cout<<"End OF updateObjects"<<endl;
}

// after all the above is done, we still need to evaluate the petri net... so.... we need to write that method again as well...




// ===============   EVALUATE PETRI NET =====================
// ===============   EVALUATE PETRI NET =====================
// ===============   EVALUATE PETRI NET =====================

void evaluatePT(ThePetriNet &CurrentPetriNet,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs,vector<int> &finalPlaces, vector<float> &ActionVector,int currentFrame,int InitialtimeStep,int FinaltimeStep)
{
  int Tsize = CurrentPetriNet.CurrentTransitions.size();
  int Psize = CurrentPetriNet.CurrentPlaces.size();
  ThePetriNet NewPetriNet;
  NewPetriNet.CurrentTransitions.clear();
  NewPetriNet.CurrentPlaces.clear();
  NewPetriNet.PreviousObjects.clear();
  NewPetriNet.ObjectsThatBelongtoNoPlace.clear();
  NewPetriNet.allowClonedObject = CurrentPetriNet.allowClonedObject;
  NewPetriNet.currentFrame = CurrentPetriNet.currentFrame;

  for(int i = 0; i < CurrentPetriNet.CurrentTransitions.size() ; i++)
  {
    NewPetriNet.CurrentTransitions.push_back(CurrentPetriNet.CurrentTransitions[i]);
  }
  for(int i = 0; i < CurrentPetriNet.CurrentPlaces.size() ; i++)
  {
    NewPetriNet.CurrentPlaces.push_back(CurrentPetriNet.CurrentPlaces[i]);
    NewPetriNet.CurrentPlaces[i].currentObjectsInThisPlace.clear();
  }
  for(int i = 0; i < CurrentPetriNet.ObjectsThatBelongtoNoPlace.size() ; i++)
  {
    NewPetriNet.ObjectsThatBelongtoNoPlace.push_back(CurrentPetriNet.ObjectsThatBelongtoNoPlace[i]);
  }


  for(int dummyplace = 0; dummyplace < Psize; dummyplace++ )
  {
    while(0 < CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.size()) //    for(int dummyObj = 0; dummyObj < CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.size()  ;dummyObj++)
    {
      stObject curDummyObj;
      curDummyObj = CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace[0];
      CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.erase( CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.begin() + 0);
      int objectfindsitsplace = 0;
      for (int dummyarc = 0; dummyarc < CurrentPetriNet.CurrentPlaces[ dummyplace ].incomingArcs.size(); dummyarc++)
      {
    int dummyTransitionID = CurrentPetriNet.CurrentPlaces[ dummyplace ].incomingArcs[dummyarc].TransitionID; // each incoming arc means a transition from this place..
    int transitionfires;

    transitionfires = checkTransitionFired( CurrentPetriNet, dummyTransitionID, curDummyObj,currentFrame );

    if ((transitionfires == 1 ) && (curDummyObj.activeObject == 1)) // this transition fires...
    {
      for(int dummyT=0; dummyT < CurrentPetriNet.CurrentTransitions[ dummyTransitionID -1 ].outgoingArcs.size(); dummyT++)
      {
        int dummyPlaceID = CurrentPetriNet.CurrentTransitions[ dummyTransitionID -1 ].outgoingArcs[dummyT].PlaceID;
        curDummyObj.objsCurrentPlaceID = dummyPlaceID;
       // cout<<" THere is a transition just fired NOW, currentFrame& dummyTransitionID & dummyPlaceID: "<<currentFrame<<" & "<<dummyTransitionID <<" & "<< dummyPlaceID<<endl;

        NewPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
        objectfindsitsplace = 1;
      }

    }
    if (objectfindsitsplace == 0)
    {
      objectfindsitsplace = 1;
      NewPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.push_back(curDummyObj);
    }
      }
    }
  }

  //CurrentPetriNet = NewPetriNet;
  CurrentPetriNet.CurrentPlaces.clear();
  for(int dummyplace = 0; dummyplace < Psize; dummyplace++ )
  {
    pnPlace dummyPlace1;
    dummyPlace1 = NewPetriNet.CurrentPlaces[dummyplace];
    CurrentPetriNet.CurrentPlaces.push_back(dummyPlace1);
  }

}



// ===============   EVALUATE PETRI NET 2=====================
// ===============   EVALUATE PETRI NET 2=====================
// ===============   EVALUATE PETRI NET 2=====================



void evaluatePT2(ThePetriNet &CurrentPetriNet,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs,vector<int> &finalPlaces, vector<float> &ActionVector,int currentFrame,int InitialtimeStep,int FinaltimeStep)
{
  int Tsize = CurrentPetriNet.CurrentTransitions.size();
  int Psize = CurrentPetriNet.CurrentPlaces.size();
  bool changehappened = 1;
  vector <stObject> tempObjVectorFortheCurrentPlace;
  tempObjVectorFortheCurrentPlace.clear();
  while (changehappened)
  {
    changehappened = 0;
    for(int dummyplace = 0; dummyplace < Psize; dummyplace++ )
    {
      for(int dummyObjIndx = 0; dummyObjIndx < CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.size(); dummyObjIndx++ )
      {
    tempObjVectorFortheCurrentPlace.push_back(CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace[dummyObjIndx]);
      }
      CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.clear();
      stObject curDummyObj;
      for(int dummyObjIndx = 0; dummyObjIndx < tempObjVectorFortheCurrentPlace.size(); dummyObjIndx++ )
      {
    curDummyObj = tempObjVectorFortheCurrentPlace[dummyObjIndx];
    int objectfindsitsplace = 0;
    for (int dummyarc = 0; dummyarc < CurrentPetriNet.CurrentPlaces[ dummyplace ].incomingArcs.size(); dummyarc++)
    {
      int dummyTransitionID = CurrentPetriNet.CurrentPlaces[ dummyplace ].incomingArcs[dummyarc].TransitionID;
      int transitionfires;
      transitionfires = checkTransitionFired( CurrentPetriNet, dummyTransitionID, curDummyObj,currentFrame );
      if ((transitionfires == 1 ) && (curDummyObj.activeObject == 1))
      {
        for(int dummyT=0; dummyT < CurrentPetriNet.CurrentTransitions[ dummyTransitionID -1 ].outgoingArcs.size(); dummyT++)
        {
          changehappened = 1;
          int dummyPlaceID = CurrentPetriNet.CurrentTransitions[ dummyTransitionID -1 ].outgoingArcs[dummyT].PlaceID;
          curDummyObj.objsCurrentPlaceID = dummyPlaceID;
          CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
          objectfindsitsplace = 1;
        }
        // we also need to check for the same object in the incoming places to remove them if there is any in those... (since they are all moved into the outgoing places)

      }
      if (objectfindsitsplace == 0)
      {
        objectfindsitsplace = 1;
        CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.push_back(curDummyObj);
      }
    }
      }

    }

  }

}


void evaluatePT3(ThePetriNet &CurrentPetriNet,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs,vector<int> &finalPlaces, vector<float> &ActionVector,int currentFrame,int InitialtimeStep,int FinaltimeStep)
{
  int Tsize = CurrentPetriNet.CurrentTransitions.size();
  int Psize = CurrentPetriNet.CurrentPlaces.size();
  vector<stObject> tempObjVectorFortheCurrentPlace;

  for(int dummyplace = 0; dummyplace < Psize; dummyplace++ )
  {
    for(int dummyObjIndx = 0; dummyObjIndx < CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.size(); dummyObjIndx++ )
    {
      tempObjVectorFortheCurrentPlace.push_back(CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace[dummyObjIndx]);
    }
    CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.clear();
    while(0 < tempObjVectorFortheCurrentPlace.size()) //    for(int dummyObj = 0; dummyObj < CurrentPetriNet.CurrentPlaces[ dummyplace ].currentObjectsInThisPlace.size()  ;dummyObj++)
    {
      stObject curDummyObj;
      curDummyObj = tempObjVectorFortheCurrentPlace[0];
      tempObjVectorFortheCurrentPlace.erase( tempObjVectorFortheCurrentPlace.begin() + 0);
      int objectfindsitsplace = 0;
      for (int dummyarc = 0; dummyarc < CurrentPetriNet.CurrentPlaces[ dummyplace ].incomingArcs.size(); dummyarc++)
      {
    int dummyTransitionID = CurrentPetriNet.CurrentPlaces[ dummyplace ].incomingArcs[dummyarc].TransitionID; // each incoming arc means a transition from this place..
    int transitionfires;

    transitionfires = checkTransitionFired( CurrentPetriNet, dummyTransitionID, curDummyObj,currentFrame );

    if ((transitionfires == 1 ) && (curDummyObj.activeObject == 1)) // this transition fires...
    {
      for(int dummyT=0; dummyT < CurrentPetriNet.CurrentTransitions[ dummyTransitionID -1 ].outgoingArcs.size(); dummyT++)
      {
        cout<<" There is a transition just fired NOW"<<endl;
        int dummyPlaceID = CurrentPetriNet.CurrentTransitions[ dummyTransitionID -1 ].outgoingArcs[dummyT].PlaceID;
        curDummyObj.objsCurrentPlaceID = dummyPlaceID;
        CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
        if (CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].FinalPlace == 1)
          cout<<"an OBJECT PUT INTO THE FINAL PLACE "<<endl;
        objectfindsitsplace = 1;
      }
    }
    if (objectfindsitsplace == 0)
    {
      objectfindsitsplace = 1;
      CurrentPetriNet.CurrentPlaces[dummyplace].currentObjectsInThisPlace.push_back(curDummyObj);
    }
      }
    }
  }

}



void evaluatePT4(ThePetriNet &CurrentPetriNet,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs,vector<int> &finalPlaces,int currentFrame,int InitialtimeStep,int FinaltimeStep)
{
  int Tsize = CurrentPetriNet.CurrentTransitions.size();
  int Psize = CurrentPetriNet.CurrentPlaces.size();
  vector<stObject> tempObjVectorFortheCurrentPlace;
  //  dont fotget to put while loop there to make sure it goes on state base.
  int reruntheNet = 1;
  while (reruntheNet == 1)
  {
    reruntheNet = 0;
    for(int dummytransition = 0; dummytransition < Tsize; dummytransition++ )
    {

      for(int dummyarc = 0; dummyarc <  CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs.size();dummyarc++)
      {
    int dummyPlaceID = CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs[dummyarc].PlaceID;
    for(int dumObjIndex = 0; dumObjIndex < CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace.size();dumObjIndex++)
    {

      int testcheck = 0;
      stObject curDummyObj;
      int ThisObjID = curDummyObj.RealObjectID;
      curDummyObj = CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace[dumObjIndex];
      int transitionfires = 0;
      if (curDummyObj.activeObject == 1)
        transitionfires = checkTransitionFired( CurrentPetriNet, dummytransition+1, curDummyObj,currentFrame );
      if ((transitionfires == 1 ) && (curDummyObj.activeObject == 1))
      {
        testcheck = 1;
        CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace[dumObjIndex].activeObject = 0;
        for(int dummyoutputarc=0; dummyoutputarc < CurrentPetriNet.CurrentTransitions[ dummytransition ].outgoingArcs.size(); dummyoutputarc++)
        {
          reruntheNet = 1;
          int newdummyPlaceID = CurrentPetriNet.CurrentTransitions[ dummytransition ].outgoingArcs[dummyoutputarc].PlaceID;
          curDummyObj.objsCurrentPlaceID = newdummyPlaceID;
          CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
        }
      }
      if(testcheck == 1)  // // check for the moved object in the previous states and delete (deactivate) it from their list..
      {
        for(int dummyarc1 = 0; dummyarc1 <  CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs.size();dummyarc1++)
        {
          int checkAndRemoveOnlyOncefortheObject = 1;
          int dummyPlaceID2 = CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs[dummyarc].PlaceID;
          for(int dumObjIndex1 = 0; dumObjIndex1 < CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace.size();dumObjIndex1++)
          {
        if ((ThisObjID == CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace[dumObjIndex1].RealObjectID)  && (checkAndRemoveOnlyOncefortheObject == 1))
        {
          checkAndRemoveOnlyOncefortheObject = 0;
          CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace[dumObjIndex1].activeObject = 0;
        }
          }
        }
      }
    }
      }
    }
  }
}

void evaluatePT5(ThePetriNet &CurrentPetriNet,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs,vector<int> &finalPlaces,int currentFrame,int InitialtimeStep,int FinaltimeStep)
{
  int Tsize = CurrentPetriNet.CurrentTransitions.size();
  int Psize = CurrentPetriNet.CurrentPlaces.size();
  vector<stObject> tempObjVectorFortheCurrentPlace;
  //  dont fotget to put while loop there to make sure it goes on state base.
  int reruntheNet = 1;
  // from Sedat to Sedat : this doesnt have to be a while loop.. Can be a for loop too. Consider this in the future!!! :)
  while (reruntheNet == 1)
  {
    reruntheNet = 0;
    for(int dummytransition = 0; dummytransition < Tsize; dummytransition++ )
    {

      for(int dummyarc = 0; dummyarc <  CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs.size();dummyarc++)
      {
    int dummyPlaceID = CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs[dummyarc].PlaceID;
    for(int dumObjIndex = 0; dumObjIndex < CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace.size();dumObjIndex++)
    {

      int testcheck = 0;
      stObject curDummyObj;
      int ThisObjID = curDummyObj.RealObjectID;
      curDummyObj = CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace[dumObjIndex];
      int transitionfires = 0;
      if (curDummyObj.activeObject == 1)
        transitionfires = checkTransitionFired( CurrentPetriNet, dummytransition+1, curDummyObj,currentFrame );
      if ((transitionfires == 1 ) && (curDummyObj.activeObject == 1))
      {
        testcheck = 1;
        CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace[dumObjIndex].activeObject = 0;
        for(int dummyoutputarc=0; dummyoutputarc < CurrentPetriNet.CurrentTransitions[ dummytransition ].outgoingArcs.size(); dummyoutputarc++)
        {
          reruntheNet = 1;
          int newdummyPlaceID = CurrentPetriNet.CurrentTransitions[ dummytransition ].outgoingArcs[dummyoutputarc].PlaceID;
          curDummyObj.objsCurrentPlaceID = newdummyPlaceID;
          // CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
          //----- implement this part.....
              cout<<"Transition fires..Before the if statement!!currentFrame, thedummytransition+1, newdummyPlaceID and objectsRealID are: "<<currentFrame<< " & "<<dummytransition+1<<" & " <<newdummyPlaceID  <<" & " <<curDummyObj.RealObjectID  <<endl;

          if (CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].FinalPlace == 1) // if this is a finalplace
          {
        stObject curDummyObjClone;
        //------ copy everything in curDummyObj into curDummyObjClone -----------------
        curDummyObjClone.RealObjectID = curDummyObj.RealObjectID;
        curDummyObjClone.assignedObject = curDummyObj.assignedObject;
        curDummyObjClone.activeObject = 0; // the same object with activeObject = 0 means the clone of this object stays in this place. :)
        curDummyObjClone.objsCurrentPlaceID = curDummyObj.objsCurrentPlaceID;
        curDummyObjClone.FrameNumber = curDummyObj.FrameNumber;
        for(int dumyt=0; dumyt<curDummyObj.ObjectSummary.size(); dumyt++)
        {
          curDummyObjClone.ObjectSummary.push_back(curDummyObj.ObjectSummary[dumyt]);
        }
        for(int dumyt=0; dumyt<curDummyObj.ObjectCurrentAttributes.size(); dumyt++)
        {
          curDummyObjClone.ObjectCurrentAttributes.push_back(curDummyObj.ObjectCurrentAttributes[dumyt]);
        }
        //--------------------- copying is done here ---------------------------------
        CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObjClone); // put the clone into the same place too..
        if (CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].incomingArcs.size() > 0) // this object can go somewhere else from here too...
        {
          CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
        }
          }
          else
        CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
          //-------------------------------
        }
      }
      if(testcheck == 1)  // // check for the moved object in the previous states and delete (deactivate) it from their list..
      {
        for(int dummyarc1 = 0; dummyarc1 <  CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs.size();dummyarc1++)
        {
          int checkAndRemoveOnlyOncefortheObject = 1;
          int dummyPlaceID2 = CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs[dummyarc].PlaceID;
          for(int dumObjIndex1 = 0; dumObjIndex1 < CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace.size();dumObjIndex1++)
          {
        if ((ThisObjID == CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace[dumObjIndex1].RealObjectID)  && (checkAndRemoveOnlyOncefortheObject == 1))
        {
          checkAndRemoveOnlyOncefortheObject = 0;
          CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace[dumObjIndex1].activeObject = 0;
        }
          }
        }
      }
    }
      }
    }
  }
}






void evaluatePT6(ThePetriNet &CurrentPetriNet,vector<Arc_self> &inputArcs, vector<Arc_self> &outputArcs,vector<int> &finalPlaces,int currentFrame,int InitialtimeStep,int FinaltimeStep)
{
    int Tsize = CurrentPetriNet.CurrentTransitions.size();
    int Psize = CurrentPetriNet.CurrentPlaces.size();
    vector<stObject> tempObjVectorFortheCurrentPlace;
    //  dont fotget to put while loop there to make sure it goes on state base.
    int reruntheNet = 1;
    // from Sedat to Sedat : this doesnt have to be a while loop like this.. In fact, for large data, this shouldnt be a this kind of a while loop. This can be a for loop too. Consider this in the future!!! :)
    while (reruntheNet == 1)
    {
        reruntheNet = 0;
        for(int dummytransition = 0; dummytransition < Tsize; dummytransition++ )
        {

            for(int dummyarc = 0; dummyarc <  CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs.size();dummyarc++)
            {
                int dummyPlaceID = CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs[dummyarc].PlaceID;
                for(int dumObjIndex = 0; dumObjIndex < CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace.size();dumObjIndex++)
                {
                    //cout<<"Inside evaluatePT5 2 !!"<<endl;
                    int testcheck = 0;
                    stObject curDummyObj;
                    int ThisObjID = curDummyObj.RealObjectID;
                    curDummyObj = CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace[dumObjIndex];
                    int transitionfires = 0;
                    if (curDummyObj.activeObject == 1)
                    {
                        cout<<"Inside evaluatePT6 2 before checkTransitionFired Function!!"<<endl;
                        transitionfires = checkTransitionFired( CurrentPetriNet, dummytransition+1, curDummyObj,currentFrame );
                        if (transitionfires == 1)
                            cout<<"Transition fired inside evaluatePT6 2 after checkTransitionFired Function!!"<<endl;
                        else
                            cout<<"Transition not fired inside evaluatePT6 2 after checkTransitionFired Function!!"<<endl;
                    }
                    if ((transitionfires == 1 ) && (curDummyObj.activeObject == 1))
                    {
                        testcheck = 1;
                        CurrentPetriNet.CurrentPlaces[ dummyPlaceID - 1 ].currentObjectsInThisPlace[dumObjIndex].activeObject = 0;
                        for(int dummyoutputarc=0; dummyoutputarc < CurrentPetriNet.CurrentTransitions[ dummytransition ].outgoingArcs.size(); dummyoutputarc++)
                        {
                            //cout<<"A transition fired.. currentFrame, ObjID, dummyPlaceID & fired transition IDs are: "<<currentFrame   <<" & " <<ThisObjID   <<" & "<<dummyPlaceID   <<" & "<<dummytransition+1  <<endl;
                            // cout<<"Inside evaluatePT5 3 !!"<<endl;
                            reruntheNet = 1;
                            int newdummyPlaceID = CurrentPetriNet.CurrentTransitions[ dummytransition ].outgoingArcs[dummyoutputarc].PlaceID;
                            curDummyObj.objsCurrentPlaceID = newdummyPlaceID;
                            // CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
                            //----- implement this part.....
                            // cout<<"Before the if statement!!currentFrame, thedummytransition+1, newdummyPlaceID and objectsRealID are: "<<currentFrame<< " & "<<dummytransition+1<<" & " <<newdummyPlaceID  <<" & " <<curDummyObj.RealObjectID  <<endl;
                            if (CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].FinalPlace == 1) // if this is a finalplace
                            {
                                //cout<<"inside the else. object put into the final place."<<endl;
                                //cout<<"Inside evaluatePT5 4 !!"<<endl;
                                stObject curDummyObjClone;
                                //------ copy everything in curDummyObj into curDummyObjClone -----------------
                                curDummyObjClone.RealObjectID = curDummyObj.RealObjectID;
                                curDummyObjClone.assignedObject = curDummyObj.assignedObject;
                                curDummyObjClone.activeObject = 0; // the same object with activeObject = 0 means the clone of this object stays in this place. :)
                                curDummyObjClone.objsCurrentPlaceID = curDummyObj.objsCurrentPlaceID;
                                curDummyObjClone.FrameNumber = curDummyObj.FrameNumber;
                                curDummyObjClone.CurrentDurationInTheCurrentPlace = curDummyObj.CurrentDurationInTheCurrentPlace;
                                curDummyObj.CurrentDurationInTheCurrentPlace = 0;
                                for(int dumyt=0; dumyt<curDummyObj.ObjectSummary.size(); dumyt++)
                                {
                                    curDummyObjClone.ObjectSummary.push_back(curDummyObj.ObjectSummary[dumyt]);
                                }
                                for(int dumyt=0; dumyt<curDummyObj.ObjectCurrentAttributes.size(); dumyt++)
                                {
                                    curDummyObjClone.ObjectCurrentAttributes.push_back(curDummyObj.ObjectCurrentAttributes[dumyt]);
                                }
                                //--------------------- copying is done here ---------------------------------
                                //cout<<"YAY!! inside the evaluatept, currentframe, objectRealID and placeID are "<<curDummyObj.FrameNumber<<" & "<<curDummyObjClone.RealObjectID<<" & "<<curDummyObjClone.objsCurrentPlaceID <<endl;
                                CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObjClone); // put the clone into the same place too..
                                CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);
                            }
                            else
                                CurrentPetriNet.CurrentPlaces[ newdummyPlaceID - 1 ].currentObjectsInThisPlace.push_back(curDummyObj);

                            //-------------------------------
                        }
                    }
                    if(testcheck == 1)  // // check for the moved object in the previous states and delete (deactivate) it from their list..
                    {
                        for(int dummyarc1 = 0; dummyarc1 <  CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs.size();dummyarc1++)
                        {
                            int checkAndRemoveOnlyOncefortheObject = 1;
                            int dummyPlaceID2 = CurrentPetriNet.CurrentTransitions[dummytransition].incomingArcs[dummyarc].PlaceID;
                            for(int dumObjIndex1 = 0; dumObjIndex1 < CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace.size();dumObjIndex1++)
                            {
                                if ((ThisObjID == CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace[dumObjIndex1].RealObjectID)  && (checkAndRemoveOnlyOncefortheObject == 1))
                                {
                                    checkAndRemoveOnlyOncefortheObject = 0;
                                    CurrentPetriNet.CurrentPlaces[ dummyPlaceID2 - 1 ].currentObjectsInThisPlace[dumObjIndex1].activeObject = 0;
                                }
                            }
                        }
                    }

                }
            }
        }
    }

    //check if the object stuck in one of the final places...
    for(int dumyplace = 0; dumyplace <CurrentPetriNet.CurrentPlaces.size(); dumyplace++)
    {
        if ((CurrentPetriNet.CurrentPlaces[dumyplace].FinalPlace == 1) && (CurrentPetriNet.CurrentPlaces[ dumyplace].incomingArcs.size() == 0))
        {
            for(int ii=0;ii< CurrentPetriNet.CurrentPlaces[dumyplace].currentObjectsInThisPlace.size() ;ii++)
            {
                if (CurrentPetriNet.CurrentPlaces[dumyplace].currentObjectsInThisPlace[ii].activeObject == 1) // this object stuck in this final place. move it back to the initial places.
                {
                    for(int dumyplace1 = 0; dumyplace1 <CurrentPetriNet.CurrentPlaces.size(); dumyplace1++)
                    {
                        if (CurrentPetriNet.CurrentPlaces[dumyplace1].InitialPlace == 1)
                        {
                            double dumyreturnval = 0;
                            long testduration = CurrentPetriNet.CurrentPlaces[dumyplace].currentObjectsInThisPlace[ii].CurrentDurationInTheCurrentPlace;
                            EvaluateExpression(CurrentPetriNet,dumyplace1, 0, dumyreturnval, currentFrame,CurrentPetriNet.CurrentPlaces[dumyplace].currentObjectsInThisPlace[ii].RealObjectID,testduration);
                            if (dumyreturnval ==1)
                            {
                                stObject currObj;
                                currObj = CurrentPetriNet.CurrentPlaces[dumyplace].currentObjectsInThisPlace[ii];
                                currObj.objsCurrentPlaceID = dumyplace1 + 1;
                                currObj.CurrentDurationInTheCurrentPlace = 0;
                                StObjSummary currObjsSummary;
                                currObjsSummary.PlaceID = dumyplace + 1;
                                currObjsSummary.FrameNumber = currentFrame;
                                currObjsSummary.ObjectID.push_back( currObj.RealObjectID );
                                currObj.ObjectSummary.push_back( currObjsSummary );
                                CurrentPetriNet.CurrentPlaces[dumyplace].currentObjectsInThisPlace.erase(CurrentPetriNet.CurrentPlaces[dumyplace].currentObjectsInThisPlace.begin()+ii);
                                CurrentPetriNet.CurrentPlaces[dumyplace1].currentObjectsInThisPlace.push_back(currObj);

                            }
                        }
                    }

                }

            }
        }
    }



}


void Generate_EventHistory(string FileName, ThePetriNet &CurrentPetriNet)
{

  ofstream output;
  output.open(FileName.c_str());
  if(!output)
  {
    cout<<" Cannot create event file: "<<FileName <<" !!! "<<endl;
  }


  for(int dummyplaceID = 0; dummyplaceID < CurrentPetriNet.CurrentPlaces.size(); dummyplaceID++ )
  {
    if ((CurrentPetriNet.CurrentPlaces[dummyplaceID].FinalPlace == 1))
//        if ((CurrentPetriNet.CurrentPlaces[dummyplaceID].FinalPlace == 1) ||(CurrentPetriNet.CurrentPlaces[dummyplaceID].PlaceID == 2) ||(CurrentPetriNet.CurrentPlaces[dummyplaceID].PlaceID == 3))
    {
      for (int dummyvar = 0; dummyvar < CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace.size() ; dummyvar++ )
      {
        int dummyplacee = -1;
        for(int dummyvar2 = 0; dummyvar2 < CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary.size() ; dummyvar2++ )
        {
          int Frame = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].FrameNumber;
          int Place = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].PlaceID;
          cout<<"frame is "<< Frame  <<endl;
          cout<<"Objnumber is "<<  CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID.size() <<endl;
          int ObjID = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID[0];
          cout<<"ObjID is "<< ObjID<<"\n"<<endl;
         // int Place = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].PlaceID;
          if(ObjID == 58)
              ObjID =58;
          output<<"  "<<ObjID<<"  "<<Frame<<"  "<<Place; // <=================== if you remove the if statement, use this line
        }
        output<<"  "<<CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].RealObjectID<<"  "<<CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].FrameNumber;
        output<<"  "<<dummyplaceID+1<<endl;
      }
    }
  }
  output.close();
  output.clear();
}

void Generate_EventHistory1(string FileName, ThePetriNet &CurrentPetriNet)
{
  ofstream output;
  output.open(FileName.c_str());
  if(!output)
  {
    cout<<" Cannot create event file: "<<FileName <<" !!! "<<endl;
  }
  int Event = 0;

  for(int dummyplaceID = 0; dummyplaceID < CurrentPetriNet.CurrentPlaces.size(); dummyplaceID++ )
  {
    if (CurrentPetriNet.CurrentPlaces[dummyplaceID].FinalPlace == 1 )
    {
      for (int dummyvar = 0; dummyvar < CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace.size() ; dummyvar++ )
      {
    int dummyplacee = 1;
    int NextFrame = 0;
    int NextObjID = 0;
    int NextPlace = 0;
    int Frame(0),ObjID(0),Place(0),ObjNumber(0);
    int totalsummarysize = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary.size();
    for(int dummyvar2 = 0; dummyvar2 < totalsummarysize-1 ; dummyvar2++ )
    {
//	  ObjID = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID[0];

      Frame = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].FrameNumber;
      Place = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].PlaceID;

      NextFrame = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2+1].FrameNumber;
      NextPlace = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2+1].PlaceID;
      //NextObjNumber = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2+1].ObjectID.size();


             //========== below is to write everything in the summary.
      ObjNumber = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID.size();
      for(int dummyObject = 0;dummyObject< ObjNumber; dummyObject++ ) // <=================== this is to write everything in the summary.
      {
        ObjID = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID[dummyObject];
        output<<"  "<<ObjID<<"  "<<Frame<<"  "<<Place;
      }
    }

    //-------------below is the last summary object in the history, since we didnt include inthe above loop, we included it here ----------
    Frame = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[totalsummarysize-1].FrameNumber;
    Place = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[totalsummarysize-1].PlaceID;
    ObjNumber = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[totalsummarysize-1].ObjectID.size();
    for(int dummyObject = 0;dummyObject< ObjNumber; dummyObject++ ) // <=================== this is to write everything in the summary.
    {
      ObjID = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[totalsummarysize-1].ObjectID[dummyObject];
      output<<"  "<<ObjID<<"  "<<Frame<<"  "<<Place;
    }
        //========= below is the stobject info below and we need to include that as well as the objecthistory...
    output<<"  "<<CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].RealObjectID<<"  "<<CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].FrameNumber;
    output<<"  "<<dummyplaceID+1<<endl;
      }
    }
  }
  output.close();
  output.clear();

}



void Generate_EventHistory2(string FileName, ThePetriNet &CurrentPetriNet)
{
  ofstream output;
  output.open(FileName.c_str());
  if(!output)
  {
    cout<<" Cannot create event file: "<<FileName <<" !!! "<<endl;
  }
  int Event = 0;
  vector<int> listOfInitialPlaces;
  listOfInitialPlaces.clear();
  for(int dummyplaceID = 0; dummyplaceID < CurrentPetriNet.CurrentPlaces.size(); dummyplaceID++ )
  {
    if (CurrentPetriNet.CurrentPlaces[dummyplaceID].InitialPlace == 1 )
    {
      listOfInitialPlaces.push_back(dummyplaceID+1); // saving the actual ID . that is why we have +1.
    }
  }


  for(int dummyplaceID = 0; dummyplaceID < CurrentPetriNet.CurrentPlaces.size(); dummyplaceID++ )
  {
    if (CurrentPetriNet.CurrentPlaces[dummyplaceID].FinalPlace == 1 )
    {
      for (int dummyvar = 0; dummyvar < CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace.size() ; dummyvar++ )
      {
    int dummyplacee = dummyplaceID+1;
    int NextFrame = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].FrameNumber;
    int NextObjID = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].RealObjectID;
    output<<"  "<<NextObjID<<"  "<<NextFrame<<"  "<<dummyplacee;
    int NextPlace = 0;
    int Frame(0),ObjID(0),Place(0),ObjNumber(0);
    int totalsummarysize = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary.size();
    int dummyvar2 = totalsummarysize-1;
    int initialFramefound = 0;
    while ((dummyvar2 > -1) && (initialFramefound == 0)) // for each summary
    {
      while(CurrentPetriNet.CurrentPlaces[Place-1].InitialPlace == 0 )
      {
        Place = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].PlaceID;
        Frame = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].FrameNumber;
        ObjNumber = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID.size();
        for(int dummyObject = 0;dummyObject< ObjNumber; dummyObject++ ) // <=================== this is to write everything in the summary.
        {
          ObjID = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID[dummyObject];
          output<<"  "<<ObjID<<"  "<<Frame<<"  "<<Place;
        }
        dummyvar2= dummyvar2-1;
      }
      Place = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].PlaceID;
      if (CurrentPetriNet.CurrentPlaces[Place-1].InitialPlace == 1 )
      {
        Frame = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].FrameNumber;
        Place = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].PlaceID;
        ObjNumber = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID.size();
        for(int dummyObject = 0;dummyObject< ObjNumber; dummyObject++ ) // <=================== this is to write everything in the summary.
        {
          ObjID = CurrentPetriNet.CurrentPlaces[dummyplaceID].currentObjectsInThisPlace[dummyvar].ObjectSummary[dummyvar2].ObjectID[dummyObject];
          output<<"  "<<ObjID<<"  "<<Frame<<"  "<<Place;
        }
        output<<endl;
        initialFramefound = 1;
      }
    }





      }
    }
  }
  output.close();
  output.clear();

}





void testCreatedPT(ThePetriNet &CurrentPetriNet)
{
  int totalPlaces = CurrentPetriNet.CurrentPlaces.size();
  int totalTransitions = CurrentPetriNet.CurrentTransitions.size();
  cout<<endl<<" PetriNet is being tested.."<<endl<<" the totalPlace &  totalTransition numbers are: "<<totalPlaces<<" & "<< totalTransitions <<endl<<endl;
  for( int i = 0; i < totalPlaces ; i++)
  {
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].InitialPlace: "<<CurrentPetriNet.CurrentPlaces[i].InitialPlace<<endl;
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].AttributeNumber: "<<CurrentPetriNet.CurrentPlaces[i].AttributeNumber<<endl;
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].PlaceCondition: "<<CurrentPetriNet.CurrentPlaces[i].PlaceCondition<<endl;
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].Greater: "<<CurrentPetriNet.CurrentPlaces[i].Greater<<endl;
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].FinalPlace: "<<CurrentPetriNet.CurrentPlaces[i].FinalPlace<<endl;
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].PlaceID: "<<CurrentPetriNet.CurrentPlaces[i].PlaceID<<endl;
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].currentObjectsInThisPlace.size(): "<<CurrentPetriNet.CurrentPlaces[i].currentObjectsInThisPlace.size()<<endl;
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].incomingArcs.size(): "<<CurrentPetriNet.CurrentPlaces[i].incomingArcs.size()<<endl;

    for(int j=0; j< CurrentPetriNet.CurrentPlaces[i].incomingArcs.size(); j++)
    {
      cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].incomingArcs["<<j<<"].TransitionID: "<<CurrentPetriNet.CurrentPlaces[i].incomingArcs[j].TransitionID<<endl;
      cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].incomingArcs["<<j<<"].PlaceID: "<<CurrentPetriNet.CurrentPlaces[i].incomingArcs[j].PlaceID<<endl;
      cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].incomingArcs["<<j<<"].arcweight: "<<CurrentPetriNet.CurrentPlaces[i].incomingArcs[j].arcweight<<endl;
      cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].incomingArcs["<<j<<"].Fired: "<<CurrentPetriNet.CurrentPlaces[i].incomingArcs[j].Fired<<endl;
    }
    cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].outgoingArcs.size(): "<<CurrentPetriNet.CurrentPlaces[i].outgoingArcs.size()<<endl;
    for(int j=0; j< CurrentPetriNet.CurrentPlaces[i].outgoingArcs.size(); j++)
    {
      cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].outgoingArcs["<<j<<"].TransitionID: "<<CurrentPetriNet.CurrentPlaces[i].outgoingArcs[j].TransitionID<<endl;
      cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].outgoingArcs["<<j<<"].PlaceID: "<<CurrentPetriNet.CurrentPlaces[i].outgoingArcs[j].PlaceID<<endl;
      cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].outgoingArcs["<<j<<"].arcweight: "<<CurrentPetriNet.CurrentPlaces[i].outgoingArcs[j].arcweight<<endl;
      cout<<"CurrentPetriNet.CurrentPlaces["<<i<<"].outgoingArcs["<<j<<"].Fired: "<<CurrentPetriNet.CurrentPlaces[i].outgoingArcs[j].Fired<<endl;
    }
    cout<<endl<<endl;

  }

  cout<<"=== LISTING PLACES is DONE!!.. Now Checking Transitions... ========="<<endl<<endl<<endl;

  for( int i = 0; i < totalTransitions ; i++)
  {
    cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].TransitionID: "<<CurrentPetriNet.CurrentTransitions[i].TransitionID<<endl;
    cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].TransitionCondition: "<<CurrentPetriNet.CurrentTransitions[i].TransitionCondition<<endl;
    cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].Greater: "<<CurrentPetriNet.CurrentTransitions[i].Greater<<endl;
    cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].incomingArcs.size(): "<<CurrentPetriNet.CurrentTransitions[i].incomingArcs.size()<<endl;
    for(int j=0; j< CurrentPetriNet.CurrentTransitions[i].incomingArcs.size() ;j++)
    {
      cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].incomingArcs["<<j<<"].TransitionID: "<<CurrentPetriNet.CurrentTransitions[i].incomingArcs[j].TransitionID<<endl;
      cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].incomingArcs["<<j<<"].PlaceID: "<<CurrentPetriNet.CurrentTransitions[i].incomingArcs[j].PlaceID<<endl;
      cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].incomingArcs["<<j<<"].arcweight: "<<CurrentPetriNet.CurrentTransitions[i].incomingArcs[j].arcweight<<endl;
      cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].incomingArcs["<<j<<"].Fired: "<<CurrentPetriNet.CurrentTransitions[i].incomingArcs[j].Fired<<endl;
    }
    cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].outgoingArcs.size(): "<<CurrentPetriNet.CurrentTransitions[i].outgoingArcs.size()<<endl;
    for(int j=0; j< CurrentPetriNet.CurrentTransitions[i].outgoingArcs.size() ;j++)
    {
      cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].outgoingArcs["<<j<<"].TransitionID: "<<CurrentPetriNet.CurrentTransitions[i].outgoingArcs[j].TransitionID<<endl;
      cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].outgoingArcs["<<j<<"].PlaceID: "<<CurrentPetriNet.CurrentTransitions[i].outgoingArcs[j].PlaceID<<endl;
      cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].outgoingArcs["<<j<<"].arcweight: "<<CurrentPetriNet.CurrentTransitions[i].outgoingArcs[j].arcweight<<endl;
      cout<<"CurrentPetriNet.CurrentTransitions["<<i<<"].outgoingArcs["<<j<<"].Fired: "<<CurrentPetriNet.CurrentTransitions[i].outgoingArcs[j].Fired<<endl;
    }

    cout<<endl<<endl;

  }

}
