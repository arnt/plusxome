// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TAGPAGE_H
#define TAGPAGE_H

#include "plugin.h"


class TagPage: public Plugin {
public:
    TagPage( options_description & );

    Document produce( const Path & ) const;

    void setup();

private:
    Template * t;
};


#endif
