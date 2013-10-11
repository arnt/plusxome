// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef SINGLEPOST_H
#define SINGLEPOST_H

#include "plugin.h"


class SinglePost: public Plugin {
public:
    SinglePost();

    Document produce( const Path & ) const;

    void setup();

    static options_description * options();

private:
    Template * t;
};


#endif
