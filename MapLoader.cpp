#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
    
private:
    vector<StreetSegment*> segments;
    int numSegments;
    
    bool makeAttraction(istream& inf, Attraction& a);
    bool getCoords(istream& inf, GeoCoord& g);
};

MapLoaderImpl::MapLoaderImpl()
{
    numSegments = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
    for (int i = 0; i<segments.size(); i++){
        delete segments[i];
    }
}

bool MapLoaderImpl::load(string mapFile)
{
    
    ifstream infile(mapFile);
    if ( ! infile )		        // Did opening the file fail?
    {
        cerr << "Error: Cannot open data.txt!" << endl;
        return false;
    }
    
    string street;
    string lat;
    string longitude;
    
    while (getline(infile, street)){
        if (street == ""){
            //cerr<<segments.size()<<endl;
            return true;
        }
        
        StreetSegment* s = new StreetSegment;
        s->streetName = street;
        //cerr<<street<<endl;
        //Get start coordinates
        GeoCoord start;
        if (!getCoords(infile, start))
            return false;
        
        //to ignore space
        //infile.ignore(10000, ' ');
        //get end coordinates
        GeoCoord end;
        if (!getCoords(infile, end))
            return false;
        //infile.ignore(10000, '\n');
        
        
        
        //make street geosegment
        GeoSegment seg(start, end);
        
        //Assign streetsegment geosegment to above;
        s->segment = seg;
        
        
        //Now figure out number of attractions
        
        int numAttractions = 0;
        infile>>numAttractions;
        //cerr<<numAttractions<<endl;
        infile.ignore(10000, '\n');
        
        //add attractions to vector in street segment
        for (int i = 0; i<numAttractions; i++){
            Attraction a;
            if (!makeAttraction(infile, a))
                return false;
            s->attractions.push_back(a);
        }
        segments.push_back(s);
        numSegments++;
        
    }
    
    
    //cerr<<segments.size()<<endl;
	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return numSegments; // This compiles, but may not be correct
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum>=numSegments)
        return false;
    
    seg = *(segments[segNum]);
    
    return true;
}

bool MapLoaderImpl::makeAttraction(istream& inf, Attraction& a){
    
    string name = "";
    char c = 0;
    
    //Build name
    while(true){
        if (!inf.get(c))
            return false;
        if (c=='|')
            break;
        //cerr<<c;
        name+=c;
    }
    //cerr<<endl;
    //get coordinates
    GeoCoord g;
    if (!getCoords(inf, g))
        return false;
    
    
    
    a.name = name;
    a.geocoordinates = g;
    
    return true;
}

bool MapLoaderImpl::getCoords(istream& inf, GeoCoord& g){
    //get first coord, until the comma
    string lat="";
    char c = 0;
    while(true){
        if (!inf.get(c))
            return false;
        if (c==',')
            break;
        lat+=c;
    }
    
    //get next coord, until a new line or space
    string lon = "";
    //if there is a space next, ignore it, otherwise add c string
    inf.get(c);
    if (c!=' ')
        lon+=c;
    while(true){
        if (!inf.get(c))
            return false;
        if (c=='\n' || c==' ')
            break;
        lon+=c;
    }
    //cerr<<lat<<","<<lon<<endl;
    g = GeoCoord(lat, lon);
    
    return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
