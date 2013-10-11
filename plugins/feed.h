// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef FEED_H
#define FEED_H

#include "plugin.h"


class Feed: public Plugin {
public:
    Feed();

    Rendering render( const Path & ) const;

    static options_description * options();
};


#endif
