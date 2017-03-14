#include "provided.h"
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include "MyMap.h"
using namespace std;




class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
    
    
    
private:
    struct Node{
        StreetSegment seg;
        double h;
        double g;
        double f;
        Node* parent;
        bool visited;
        double length;
        bool startStart; //true if direction traveled is start to end
        
        
        //start is whether this is connected by start of past segment
        Node(StreetSegment s, Node* p, bool traveled, bool start){
            seg = s;
            parent = p;
            h = 0;
            g = parent->g;
            if (traveled)
                g += parent->length;
            f = 0;
            visited = false;
            
            length = distanceEarthMiles(seg.segment.start, seg.segment.end);
            
            checkDir(start);
            
        }
        
        Node(StreetSegment s){
            seg = s;
            parent = nullptr;
            h = 0;
            g = 0;
            f = 0;
            visited = false;
            length = 0;
            startStart = false;
        }
        void calcF(){
            f = g+h;
        }
        
        void checkDir(bool start){
            if (start)
                startStart = (seg.segment.start==parent->seg.segment.start);
            else
                startStart = (seg.segment.start==parent->seg.segment.end);
        }
    };
    
    struct Compare{
        bool operator()(Node* n1, Node* n2){
            return n1->f>n2->f;
        }
    };
    
    MapLoader* loader;
    AttractionMapper* aMap;
    SegmentMapper* sMap;
    priority_queue<Node *, vector<Node *>, Compare> * q;
    MyMap<StreetSegment, Node *> * nMap;
    
    void findH(Node * n, GeoCoord end) const;
    NavSegment makeProceed(Node* next) const;
    NavSegment makeTurn(Node * next, const NavSegment& preSeg) const;
    NavSegment makeLast(Node* n, GeoCoord& end) const;
    string getDirection(GeoSegment& seg) const;
};


NavigatorImpl::NavigatorImpl()
{
    loader = new MapLoader;
    aMap = new AttractionMapper;
    sMap = new SegmentMapper;
    nMap = new MyMap<StreetSegment, Node *>;
    q = new priority_queue<Node *, vector<Node *>, Compare>;
}

NavigatorImpl::~NavigatorImpl()
{
    cerr<<nMap->size()<<endl;
    
    int num = loader->getNumSegments();
    
    StreetSegment s;
    Node **n;
    int deleted = 0;
    for (int i = 0; i<num; i++){
        loader->getSegment(i, s);
        n = nMap->find(s);
        if (n!=nullptr){
            delete *n;
            deleted++;
        }
    }
    cerr<<deleted<<endl;
    
    delete loader;
    delete aMap;
    delete sMap;
    delete q;
    delete nMap;
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    if (!loader->load(mapFile))
        return false;
    
    aMap->init(*loader);
    sMap->init(*loader);
    
	return true;  // This compiles, but may not be correct
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    GeoCoord startCoord;
    GeoCoord endCoord;
    
    if (!aMap->getGeoCoord(start, startCoord))
        return NAV_BAD_SOURCE;
    if (!aMap->getGeoCoord(end, endCoord))
        return NAV_BAD_DESTINATION;
    
    
    vector<StreetSegment> segs = sMap->getSegments(startCoord);
    StreetSegment beginningSeg = segs[0];
    segs = sMap->getSegments(endCoord);
    StreetSegment endSeg = segs[0];
    
    if (beginningSeg==endSeg)
        return NAV_SUCCESS;
    
    //Create initial node
    Node * first = new Node(beginningSeg);
    nMap->associate(beginningSeg, first);
    
    //Find all possibilities for first next segment
    
    int initLengthToStart = distanceEarthMiles(startCoord, beginningSeg.segment.start);
    int initLengthToEnd = distanceEarthMiles(startCoord, beginningSeg.segment.end);
    
    //Check segments at start of first segment
    segs = sMap->getSegments(beginningSeg.segment.start);
    for (int i = 0; i<segs.size(); i++){
        if (!(segs[i]==beginningSeg)){
            Node * n = new Node(segs[i], first, false, true);
            nMap->associate(segs[i], n);
            n->g = initLengthToStart;
            findH(n, endCoord);
            q->push(n);
        }
    }
    
    //check segments at end of first segment
    segs = sMap->getSegments(beginningSeg.segment.end);
    for (int i = 0; i<segs.size(); i++){
        if (!(segs[i]==beginningSeg)){
            Node * n = new Node(segs[i], first, false, false);
            nMap->associate(segs[i], n);
            n->g = initLengthToEnd;
            findH(n, endCoord);
            q->push(n);
        }
    }
    
    //*******STARTING SEGMENT DEALT WITH*********
    
    Node * current;
    while(true){
        if (q->empty())
            return NAV_NO_ROUTE;
        current = q->top();
        q->pop();
        current->visited = true;
        if (current->seg == endSeg)
            break;
        
        //add Nodes at other end of current than the end it was added
        
        if (current->startStart){
            //add nodes at end of segment
            segs = sMap->getSegments(current->seg.segment.end);
            for (int i = 0; i<segs.size(); i++){
                if (segs[i]==current->seg)
                    continue;
                Node ** prev = nMap->find(segs[i]);
                if (prev==nullptr){
                    Node *n = new Node(segs[i], current, true, false);
                    nMap->associate(segs[i], n);
                    findH(n, endCoord);
                    q->push(n);
                }
                else{  //Node already exists for this segment
                    Node *n = *prev;
                    if (current->visited)
                        continue;
                    if ((current->g + current->length)<n->g){
                        n->g = current->g + current->length;
                        n->parent = current;
                        n->checkDir(false);
                        findH(n, endCoord);
                        //resort queue
                        Node * top = q->top();
                        q->push(top);
                    }
                }
            }
        }
        
        //otherwise, add nodes at start of segment
        else{
            segs = sMap->getSegments(current->seg.segment.start);
            for (int i = 0; i<segs.size(); i++){
                if (segs[i]==current->seg)
                    continue;
                Node ** prev = nMap->find(segs[i]);
                if (prev==nullptr){
                    Node *n = new Node(segs[i], current, true, true);
                    nMap->associate(segs[i], n);
                    findH(n, endCoord);
                    q->push(n);
                }
                else{  //Node already exists for this segment
                    Node *n = *prev;
                    if (current->visited)
                        continue;
                    if ((current->g + current->length)<n->g){
                        n->g = current->g + current->length;
                        n->parent = current;
                        n->checkDir(true);
                        findH(n, endCoord);
                        //resort queue
                        Node * top = q->top();
                        q->push(top);
                    }
                }
            }
        }
    }
    
    
    //when loop breaks, curr points to node of segment containing final GeoCoord
    //set length of that node to be correct (only until attraction)
    
    if (current->startStart)
        current->length = distanceEarthMiles(current->seg.segment.start, endCoord);
    
    else
        current->length = distanceEarthMiles(current->seg.segment.end, endCoord);
    
    
    stack<Node *> dirStack;
    
    Node* iter = current;
    
    while (iter!=nullptr){
        dirStack.push(iter);
        iter = iter->parent;
    }
    
    //Now stack is set up with first direction at the top
    
    //Create nav seg starting from starting attraction
    Node * firstSeg = dirStack.top();
    dirStack.pop();
    Node * second = dirStack.top();
    
    GeoCoord endFirstSeg;
    
    if (second->startStart)
        endFirstSeg = second->seg.segment.start;
    else
        endFirstSeg = second->seg.segment.end;
    
    GeoSegment firstDist(startCoord, endFirstSeg);
    string firstDir = getDirection(firstDist);
    NavSegment n(firstDir, firstSeg->seg.streetName, second->g, firstDist);
    directions.push_back(n);
    
    string currSt = firstSeg->seg.streetName;
    
    //when this loop starts, the second segment is at the top
    //start making navsegments
    while (!dirStack.empty()){
        iter = dirStack.top();
        dirStack.pop();
        if (iter->seg.streetName==currSt)
            directions.push_back(makeProceed(iter));
        else{
            //iter points to street seg after turn, make turn navseg
            directions.push_back(makeTurn(iter, directions[directions.size()-1]));
            currSt = iter->seg.streetName;
            //But now we still need to make a proceed for the current seg
            if (iter->seg==endSeg){
                directions.push_back(makeLast(iter, endCoord));
                return NAV_SUCCESS;
            }
            else{
                directions.push_back(makeProceed(iter));
            }
        }
        int i = directions.size()-1;
        cerr<<directions[i].m_distance<<" mi "<<directions[i].m_direction<<" on "<<directions[i].m_streetName<<endl;
    }
    
    
    cerr<<"Problem NavSegging"<<endl;
    
    //FOR NOW print out length and street from curr back
//    Node * iter = current;
//    double kmTotal=0;
//    string curSt = iter->seg.streetName;
//    double totaltotal = 0;
//    while (iter!=nullptr){
//        if (iter->seg.streetName==curSt){
//            kmTotal+=iter->length;
//            totaltotal+=iter->length;
//        }
//        else{
//            cerr<<kmTotal<<" km on "<<curSt<<endl;
//            kmTotal=iter->length;
//            curSt = iter->seg.streetName;
//        }
//        //cerr<<iter->length<<" km on "<<iter->seg.streetName<<endl;
//        iter = iter->parent;
//    }
//    cerr<<kmTotal<<" km on "<<curSt<<endl;
//    
//    cerr<<"TOTAL: "<<totaltotal<<" km"<<endl;
//    cerr<<endl<<endl;
    
    cerr<<"MAKE ATTRACTIONS CASE INSENSITIVE"<<endl;
    cerr<<"CHECK COURSE WEBSITE FOR UPDATED NAVSEGMENT"<<endl;
	return NAV_SUCCESS;  // This compiles, but may not be correct
}

void NavigatorImpl::findH(Node* n, GeoCoord end) const{
    GeoCoord g;
    if (n->startStart)
        g = n->seg.segment.end;
    else
        g = n->seg.segment.start;
    
    n->h = distanceEarthMiles(g, end);
    n->calcF();
}


NavSegment NavigatorImpl::makeProceed(Node* next) const{
    GeoSegment seg;
    
    if (next->startStart){
        seg = next->seg.segment;
    }
    else{
        seg = GeoSegment(next->seg.segment.end, next->seg.segment.start);
    }
        
    string dir = getDirection(seg);
    
    NavSegment n(dir, next->seg.streetName, next->length, seg);
    return n;
}

NavSegment NavigatorImpl::makeTurn(Node *next, const NavSegment& prevSeg) const{
    GeoSegment nextSeg;
    
    if (next->startStart){
        nextSeg = next->seg.segment;
    }
    else{
        nextSeg = GeoSegment(next->seg.segment.end, next->seg.segment.start);
    }
    double angle = angleBetween2Lines(prevSeg.m_geoSegment, nextSeg);
    
    string dir = "TURNERROR";
    if (angle<180)
        dir = "left";
    else
        dir = "right";
        
    
    
    NavSegment n(dir, next->seg.streetName);
    return n;
}


NavSegment NavigatorImpl::makeLast(Node* n, GeoCoord& end) const{
    GeoCoord start;
    
    if (n->startStart)
        start = n->seg.segment.start;
    else
        start = n->seg.segment.start;
    
    double dist = distanceEarthMiles(start, end);
    GeoSegment s(start, end);
    string dir = getDirection(s);
    
    NavSegment nS(dir, n->seg.streetName, dist, s);
    return nS;
}
string NavigatorImpl::getDirection(GeoSegment &seg) const{
    
    double angle = angleOfLine(seg);
    
    if (angle<=22.5)
        return "east";
    if (angle<=67.5)
        return "northeast";
    if (angle<=112.5)
        return "north";
    if (angle<=157.5)
        return "northwest";
    if (angle<=202.5)
        return "west";
    if (angle<=247.5)
        return "southwest";
    if (angle<=292.5)
        return "south";
    if (angle<=337.5)
        return "southeast";
    if (angle<360)
        return "east";
    
    return "PROBLEMDIR";
    
    
}
//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}



