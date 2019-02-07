// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TAGGED404_H
#define TAGGED404_H

#include "plugin.h"


class Tagged404: public Plugin {
public:
    Tagged404();

    Document produce404( const Path & ) const override;

    void setup();

    static options_description * options();

private:
    Template * t;
};


#endif
