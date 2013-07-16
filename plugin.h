// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef PLUGIN_H
#define PLUGIN_H

#include "base.h"

#include "path.h"
#include "post.h"
#include "config.h"
#include "template.h"
#include "document.h"
#include "rendering.h"


class Plugin {
public:
    Plugin();
    virtual ~Plugin();

    virtual Rendering * render( const Path & ) const;
    virtual void decorate( Rendering &, const Path & ) const;

    virtual Rendering * render404( const Path & ) const;



    const Plugin & next() const { return *nextPlugin; }

private:
    Plugin * nextPlugin;
};

#endif
