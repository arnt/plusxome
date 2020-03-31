// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "tagged404.h"

#include "tag.h"
#include "tagpage.h"

#include <boost/algorithm/string.hpp>

static Registration<Tagged404> r( "tagged404" );


static string tn;


/*! \class Tagged404 homepage.h

    The Tagged404 plugin produces a page containing all postings with tags or
    path that contain a word from the request.
*/

/*! Constructs a Tagged404 plugin. */

Tagged404::Tagged404()
{
}


static std::set<std::string> splitPath( const Path & path ) {
    std::string lowered = boost::algorithm::to_lower_copy(path.canonical());
    std::set<std::string> result;
    uint e = 0;
    while ( e < lowered.length() ) {
	while ( e < lowered.length() &&
		( lowered[e] < 'a' || lowered[e] > 'z' ) )
	    e++;
	uint s = e;
	while ( e < lowered.length() && lowered[e] >= 'a' && lowered[e] <= 'z' )
	    e++;
	if ( e > s )
	    result.insert( lowered.substr( s, e-s ) );
    }
    return result;
}


Document Tagged404::produce404( const Path & path ) const
{
    std::set<std::shared_ptr<Post>> withMatchingTags;
    auto words = splitPath( path );
    for ( auto word : words ) {
	Tag * mentioned = Tag::find( word );
	if ( mentioned )
	    for ( auto post : mentioned->postSet() )
		withMatchingTags.insert( post );
    }

    if ( withMatchingTags.size() < 5 ) {
	// if brute force doesn't work, you're not using enough of it
	for ( auto post : Post::all() )
	    for ( auto word : splitPath( post->path() ) )
		if ( words.count( word ) )
		    withMatchingTags.insert( post );
    }

    if ( withMatchingTags.empty() )
	return Plugin::produce404( path );

    PostSet reported;
    for ( auto post : withMatchingTags )
	reported.push_back( post );
    Document r( TagPage::produce( reported.mostRecentFirst().section( 0, 16 ), t ) );
    r.setHttpResponseCode( 404 );
    r.setCacheLifetime( 404 );
    return r;
}

/*! Reads the template file; nothing more is necessary. */

void Tagged404::setup()
{
    t = new Template( tn );
}


options_description * Tagged404::options()
{
    options_description * conf
	= new options_description( "Options for "
				   "the tagged404 plugin" );

    conf->add_options()
	( "404-page-template",
	  value<string>( &tn )->default_value( "404.template" ),
	  "404 page template file name" );

    return conf;
}
