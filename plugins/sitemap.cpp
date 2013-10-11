// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "sitemap.h"

#include "postset.h"

static Registration<Sitemap> r( "sitemap" );


/*! \class Sitemap sitemap.h

    The Sitemap class produces a sitemap.xml with the information it
    has about the posts, assets etc.

*/

/*! Constructs a Sitemap plugin. */

Sitemap::Sitemap()
{
    // nothing needed
}


/*!

*/

Rendering Sitemap::render( const Path & path ) const
{
    if ( path.canonical() != "/sitemap.xml" )
	return Plugin::render( path );

    string result =
	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	"<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\"\n"
        " xmlns:mobile=\"http://www.google.com/schemas/sitemap-mobile/1.0\">\n";

    auto posts = Post::all();
    auto p = posts.begin();
    while ( p != posts.end() ) {
	result += ( " <url>\n"
		    "  <loc>" +
		    (*p)->path().canonical() +
		    "</loc>\n"
		    "  <lastmod>" +
		    to_iso_extended_string( (*p)->date().date() ) +
		    "</lastmod>\n"
		    "  <mobile:mobile/>\n"
		    " </url>\n" );
	++p;
    }

    result += "</urlset>\n";

    return Rendering( result, "application/xml" );
}


/*! This plugin has no configuration options, but the function still
    has to exist.
*/

options_description * Sitemap::options()
{
    return new options_description();
}
