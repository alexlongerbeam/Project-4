// The main.cpp you can use for testing will replace this file soon.

#include "provided.h"
#include "MyMap.h"
#include <string>
#include <iostream>

using namespace std;

void foo();
void testLoad();
int main()
{
    //foo();
    testLoad();
}


void foo() {
    MyMap<string,double> nameToGPA; // maps student name to GPA
    //add new items to the binary search tree-based map
    nameToGPA.associate("Carey", 3.5); // Carey has a 3.5 GPA
    nameToGPA.associate("David", 3.99); // David beat Carey
    nameToGPA.associate("Abe", 3.2); // Abe has a 3.2 GPA
    nameToGPA.associate("Linda", 8.1);
    double* davidsGPA = nameToGPA.find("David");
    if (davidsGPA != nullptr)
        *davidsGPA = 1.5; // after a re-grade of David’s exam
    nameToGPA.associate("Carey", 4.0); // Carey deserves a 4.0 // replaces old 3.5 GPA
    double* lindasGPA = nameToGPA.find("Linda");
    if (lindasGPA == nullptr)
        cout << "Linda is not in the roster!" << endl;
    else
        cout << "Linda’s GPA is: " << *lindasGPA<<endl;
}



void testLoad(){
    MapLoader m;
    
    if (m.load("/Users/alexlongerbeam/Desktop/mapdata.txt"))
        cout<<"Good "<<m.getNumSegments()<<endl;
    else
        cout<<"problem "<<m.getNumSegments()<<endl;
    
    AttractionMapper am;
    
    am.init(m);
    GeoCoord g;
    am.getGeoCoord("Twentieth Century Fox Film Corporation", g);
    
    cout<<g.latitudeText<<","<<g.longitudeText<<endl;
    
}
