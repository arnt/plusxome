// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef ASSET_H
#define ASSET_H

#include "base.h"


class Asset {
public:
    Asset( const string & );

    static shared_ptr<Asset> find( const string & );

    void reload( const string & );
    
private:
    string c;
};


#endif
