// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef SINGLEPOST_H
#define SINGLEPOST_H

#include "plugin.h"


class SinglePost: public Plugin {
public:
    SinglePost();

    virtual Rendering * render( const Path & ) const;

private:
    Template t;
};


#endif
