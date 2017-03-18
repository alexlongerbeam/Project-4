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
#include <string>
#include <cctype>

inline
bool operator==(const GeoCoord& lhs, const GeoCoord& rhs){
    return lhs.latitudeText==rhs.latitudeText && lhs.longitudeText == rhs.longitudeText;
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


inline
std::string toLower(std::string original){
    std::string result = "";
    
    for (int i = 0; i<original.size(); i++){
        result += tolower(original[i]);
    }
    return result;
}



#endif /* support_h */
