// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "singlepost.h"

#include "template.h"
#include "postset.h"
#include "tagpage.h"

#include <boost/program_options.hpp>

static Registration<SinglePost> r( "singlepost" );

static string tn;


/*! \class Singlepost singlepost.h

    The SinglePost class provides access to single postings. Quite
    marvellously simple.
*/

/*! Constructs a SinglePost plugin and checks the configuration. */

SinglePost::SinglePost()
    : Plugin(), t( 0 )
{
}


Document SinglePost::produce( const Path & path ) const
{
    std::shared_ptr<Post> post = Post::find( path.canonical() );
    if ( !post )
	return Plugin::produce( path );

    PostSet single;
    single.push_back(post);
    return TagPage::produce( single, t );
}


/*! This reimplementation enables post.template. */

void SinglePost::setup()
{
    t = new Template( tn );
}


options_description * SinglePost::options()
{
    options_description * conf =
	new options_description( "Configuration file options for "
				 "the singlepost plugin" );

    conf->add_options()
	( "post-template",
	  value<string>( &tn )->default_value( "post.template" ),
	  "post template file name" );

    return conf;
}
