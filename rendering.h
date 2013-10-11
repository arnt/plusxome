// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef RENDERING_H
#define RENDERING_H

#include "base.h"
#include "document.h"


class Rendering {
public:
    Rendering();
    Rendering( Document & );
    Rendering( const string &, const string & );

    string httpResponse();

    static boost::shared_mutex & lock();

    void setHttpResponseCode( int );
    int httpResponseCode() const;

private:
    string tmp;
    unsigned int responseCode;
    string ctype;
};

#endif
