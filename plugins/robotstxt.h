// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef ROBOTSTXT_H
#define ROBOTSTXT_H

#include "plugin.h"


class RobotsTxt: public Plugin {
public:
    RobotsTxt();

    Rendering render( const Path & ) const;

    static options_description options();
};


#endif
