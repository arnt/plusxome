// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef LASTRESORT_H
#define LASTRESORT_H

#include "plugin.h"


class LastResort: public Plugin {
public:
    LastResort();

    Rendering render( const Path & ) const;
    Document produce( const Path & ) const;
    void decorate( Document &, const Path & ) const;

    Document produce404( const Path & ) const;
};


#endif
