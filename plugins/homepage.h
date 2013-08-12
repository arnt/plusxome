// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "plugin.h"


class HomePage: public Plugin {
public:
    HomePage();

    Document produce( const Path & ) const;

    void setup();

    static options_description options();

private:
    Template * t;
};


#endif
