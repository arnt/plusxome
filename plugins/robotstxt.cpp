// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "robotstxt.h"

static Registration<RobotsTxt> r( "robotstxt" );


/*! \class RobotsTxt robotstxt.h

    The RobotsTxt plugin provides a robots.txt, which helps achieve
    two goals: Avoiding 404 errors in the logfile, and point to
    sitemap.xml.
*/

/*! Constructs a RobotsTxt handler. */

RobotsTxt::RobotsTxt()
{
}


/*! /robots.txt is text, not HTML, so render() is the right function
    to produce it.
*/

Rendering RobotsTxt::render( const Path & path ) const
{
    if ( path.components() != 1 || path.component( 0 ) != "robots.txt" )
	return Plugin::render( path );

    return Rendering( "User-Agent: *\r\nAllow: /\r",
		      "text/plain",
		      365 * 86400 );
}


/*! This plugin adds no new options.

*/

options_description * RobotsTxt::options()
{
    return new options_description();
}
