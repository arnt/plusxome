// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "tagpage.h"


class HomePage: public TagPage {
public:
    HomePage();

    Document produce( const Path & ) const;

    void setup();

    static options_description * options();

private:
    Template * t;
};


#endif
