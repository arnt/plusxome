// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "plugin.h"

Plugin * Plugin::firstPlugin = 0;


/*! \class Plugin plugin.h

    The Plugin class is the roof of Plusxome's plugin
    interface. Writing a plugin is writing a subclass of the Plugin
    class.

    Virtuals, blah.
*/

/*! Constructs a Plugin and makes first() return a pointer to this
    Plugin.
*/

Plugin::Plugin()
    : nextPlugin( firstPlugin )
{
    firstPlugin = this;
}


/*! Destroys the object and frees any allocated resources. */

Plugin::~Plugin()
{
    // nothing needed, except the vtable.
}


/*! This plugin interface renders a given \a path, if it can. If not,
    it may give the next plugin a chance by calling Plugin::produce(
    \a path ), which eventuall leads to produce() being called.

    Plugins that want to produce HTML are generally better off
    implementing produce(); a plugin to produce e.g. JPEG needs to do
    it in render().
*/

Rendering Plugin::render( const Path & path ) const
{
    return next().produce( path );
}


/*! This plugin interface produces a given \a path, if it feels
    responsible for \a path. If not, it may give the next plugin a
    chance by calling Plugin::produce( \a path );
*/

Document Plugin::produce( const Path & path ) const
{
    return next().produce( path );
}


/*! This plugin interface decorates an already-produced \a document
    (for \a path), or does nothing.

    All reimplementations should also call Plugin::decorate( \a
    document, \a path ) in order to let other plugins have a go.
*/

void Plugin::decorate( Document & document, const Path & path ) const
{
    next().decorate( document, path );
}


/*! This plugin interface produces a 404 page for \a path, or punts to
    the next plugin by calling Plugin::produce404( \a path );
*/

Document Plugin::produce404( const Path & path ) const
{
    return next().produce404( path );
}
