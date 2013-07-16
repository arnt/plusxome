// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "singlepost.h"

#include "template.h"


/*! \class Singlepost singlepost.h

    The SinglePost class provides access to single postings. Quite
    marvellously simple.
*/

/*! Constructs a SinglePost plugin and checks the configuration. */

SinglePost::SinglePost()
    : Plugin(), t( Config::singlePostTemplate )
{
    // TODO: check config
    // Specifically, this nededs to make the template
}


Document SinglePost::produce( const Path & path ) const
{
    shared_ptr<Post> post = Post::find( path.canonical() );
    if ( !post )
	return Plugin::produce( path );

    Document result( t );
    result.node( "posting" ) = post->rootNode();
    return result;
}
