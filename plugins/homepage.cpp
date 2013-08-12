// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "homepage.h"

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

static bool byDate( const shared_ptr<Post> & c1, const shared_ptr<Post> & c2 )
{
    return c1->date() > c2->date();
}

Document HomePage::produce( const Path & path ) const
{
    if ( path.components() )
	return Plugin::produce( path );

    Document result( *t );

    auto posts = Post::all();
    sort( posts.begin(), posts.end(), byDate );
    auto p = posts.begin();
    int n = 0;
    shared_ptr<Node> parent = result.node( "postings" );
    if ( parent ) {
    while ( p != posts.end() && n < postings ) {
	shared_ptr<Node> r( new Node( (*p)->rootNode() ) );
	parent->children.push_back( r );
	++p;
	++n;
    }
    }
    return result;
}


/*! Reads the template file; nothing more is necessary. */

void HomePage::setup()
{
    t = new Template( tn );
}


/*!

*/

options_description HomePage::options()
{
    options_description conf( "Options for "
			      "the homepage plugin" );

    conf.add_options()
	( "home-template",
	  value<string>( &tn )->default_value( "home.template" ),
	  "home page template file name" )
	( "home-page-postings",
	  value<int>( &postings )->default_value( 10 ),
	  "number of postings to show on the home page" );

    return conf;
}
