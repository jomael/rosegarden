/*
    Rosegarden
    A sequencer and musical notation editor.
    Copyright 2000-2011 the Rosegarden development team.
    See the AUTHORS file for more details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _PROPERTY_MAP_H_
#define _PROPERTY_MAP_H_

#include "Property.h"
#include "base/PropertyName.h"

#include <map>

namespace Rosegarden {

class PropertyMap : public std::map<PropertyName, PropertyStoreBase *>
{
public:
    PropertyMap() { }
    PropertyMap(const PropertyMap &pm);

    ~PropertyMap();
    
    void clear();
    
    std::string toXmlString();

private:
    PropertyMap &operator=(const PropertyMap &); // not provided
};

typedef PropertyMap::value_type PropertyPair;

}

#endif
