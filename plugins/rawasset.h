// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef RAWASSET_H
#define RAWASSET_H

#include "plugin.h"


class RawAsset: public Plugin {
public:
    RawAsset();

    Rendering render( const Path & ) const;

    static options_description * options();

private:
    std::map<string, string> types;
};


#endif
