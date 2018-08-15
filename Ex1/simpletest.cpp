#include <iostream>


#include "osm.h"

using namespace std;


int main(int argc, char* argv[])
{
    osm_init();// if you don't have an init in you library mark this line in comment
    timeMeasurmentStructure times = measureTimes(50000, 50000, 50000, 50000);

//    ofstream myfile;
  //  myfile.open ("test.txt");   
    cout << "hostname:" << times.machineName << endl;    
    cout << "inst_time:" << times.instructionTimeNanoSecond << endl;
    cout << "func_time:" << times.functionTimeNanoSecond << endl;
    cout << "trap_time:" << times.trapTimeNanoSecond << endl;
    cout << "disk_time:" << times.diskTimeNanoSecond << endl;
    cout << "trap_inst_ratio:" <<  times.trapInstructionRatio << endl;
    cout << "disk_inst_ratio:" <<  times.diskInstructionRatio << endl;
    
    //myfile.close();
    osm_finalizer();//if you don't have a finalizer in you library mark this line in comment
    return 0;
}