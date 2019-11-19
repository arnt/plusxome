// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "homepage.h"
#include "postset.h"

static Registration<HomePage> r( "homepage" );


static string tn;
static int postings;


/*! \class HomePage homepage.h

    The HomePage plugin produces a sensible home page, with a
    configurable number of most recent postings.
*/

/*! Constructs a HomePage plugin and adds its configurability to \a cf. */

HomePage::HomePage()
{
}


Document HomePage::produce( const Path & path ) const
{
    if ( path.components() )
	return Plugin::produce( path );

    return TagPage::produce( Post::all().
			     published().
			     mostRecentFirst().
			     section( 0, postings ),
			     t );
}


/*! Reads the template file; nothing more is necessary. */

void HomePage::setup()
{
    t = new Template( tn );
}


options_description * HomePage::options()
{
    options_description * conf =
	new options_description( "Options for "
				 "the homepage plugin" );

    conf->add_options()
	( "home-template",
	  value<string>( &tn )->default_value( "home.template" ),
	  "home page template file name" )
	( "home-page-postings",
	  value<int>( &postings )->default_value( 10 ),
	  "number of postings to show on the home page" );

    return conf;
}
