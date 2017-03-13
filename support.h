//
//  support.h
//  Project 4
//
//  Created by Alex Longerbeam on 3/11/17.
//  Copyright © 2017 Alex Longerbeam. All rights reserved.
//

#ifndef support_h
#define support_h

#include "provided.h"

inline
bool operator==(const GeoCoord& lhs, const GeoCoord& rhs){
    return lhs.latitude==rhs.latitude && lhs.longitude == rhs.longitude;
}

inline
bool operator==(const StreetSegment& lhs, const StreetSegment& rhs){
    return lhs.segment.start==rhs.segment.start && lhs.segment.end == rhs.segment.end;
}

inline
bool operator<(const GeoCoord& lhs, const GeoCoord& rhs){
    return lhs.latitude<rhs.latitude;
}

inline
bool operator<(const StreetSegment& lhs, const StreetSegment& rhs){
    return lhs.segment.start<rhs.segment.start;
}





#endif /* support_h */
