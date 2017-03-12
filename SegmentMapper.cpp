#include "provided.h"
#include "MyMap.h"
#include <iostream>
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
    
private:
    MyMap<GeoCoord, vector<StreetSegment>> map;
    void addToMap(GeoCoord &c, StreetSegment s);
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    int numSegments = ml.getNumSegments();
    StreetSegment s;
    for (int i = 0; i<numSegments; i++){
        ml.getSegment(i, s);
        addToMap(s.segment.start, s);
        addToMap(s.segment.end, s);
        for (int j = 0; j<s.attractions.size(); j++){
            GeoCoord c = s.attractions[j].geocoordinates;
            addToMap(c, s);
        }
    }
}

void SegmentMapperImpl::addToMap(GeoCoord &c, StreetSegment s){
    //see if its already in there
    vector<StreetSegment> *v = map.find(c);
    if (v==nullptr){
        vector<StreetSegment> newV;
        newV.push_back(s);
        map.associate(c, newV);
    }
    else{ //modify existing vector for that geocoord
        v->push_back(s);
    }
    
    
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    const vector<StreetSegment>* segments = map.find(gc);
    
    if (segments==nullptr){
        vector<StreetSegment> d;
        return d;
    }
    
    return *segments;
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
