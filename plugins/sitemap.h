// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef SITEMAP_H
#define SITEMAP_H

#include "plugin.h"


class Sitemap: public Plugin {
public:
    Sitemap();

    Rendering render( const Path & ) const;

    static options_description options();
};


#endif
