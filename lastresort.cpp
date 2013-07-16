// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "lastresort.h"



/*! Constructs the last-resort plugin; this provides the default
    behaviour for each plugin interface. In most cases the default
    behaviour is precisely what you do not want, hence the name, but
    each particular plugin wants to override only one or two aspects
    of plugindom.

    Plugin takes care of mixing the plugins' behaviour, and this takes
    care of doing something when none of the "real" plugins do.
*/

LastResort::LastResort()
    : Plugin()
{
    // nothing needed
}


/*! The last-resort render() implementation calls produce( \a path ),
    which isn't really a last-resort action, more of a default.
*/

Rendering LastResort::render( const Path & path ) const
{
    Document d( Plugin::first().produce( path ) );
    Plugin::first().decorate( d, path );
    return Rendering( d );
}


/*! The last-resort produce() implementation calls produce404().
*/

Document LastResort::produce( const Path & path ) const
{
    return Plugin::first().produce404( path );
}


/*! Documents receive no decoration by default. */

void LastResort::decorate( Document &, const Path & ) const
{
}


/*! The last-resort 404 handler produces a constant 404 page.
*/

Document LastResort::produce404( const Path & path ) const
{
    return Document( "<html><body><h1>No such page</h1>\n<p>" +
		     path.canonical() +
		     " does not exist on this site."
		     "Very sorry about that.</html>" );
}
