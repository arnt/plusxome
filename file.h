// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef FILE_H
#define FILE_H

#include "base.h"


class File {
public:
    File( const string & );

    string contents() const;
    
private:
    string n;
    string c;
};

#endif
