// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "feed.h"

#include "postset.h"

static Registration<Feed> r( "feed" );

static string title;
static string blogger;
static string email;
static string home;
static int postings;


/*! \class Feed feed.h

    The Feed class implements Atom (actually atompub, RFC 4287). I
    suppose I will eventually implement hubbub as well, but not now.

*/

/*! Constructs a feed plugin. */

Feed::Feed()
{
    // nothing needed
}


static long long scalarTime( const ptime & t )
{
    static ptime epoch( boost::gregorian::date( 1970, 1, 1 ) );
    time_duration diff( t - epoch );
    return diff.ticks();
}


static string formattedTime( const ptime & t )
{
    ostringstream o;
    boost::posix_time::time_facet *facet =
	new time_facet("%Y-%m-%dT%H:%M:%S%F+00:00");
    o.imbue( locale( o.getloc(), facet ) );
    o << t;
    return o.str();
}


static string escaped( const string & text ) {
    string output;
    auto c = text.begin();
    while ( c != text.end() ) {
	switch ( *c ) {
	case '&':
	    output.append( "&amp;" );
	    break;
	case '<':
	    output.append( "&lt;" );
	    break;
	case '>':
	    output.append( "&gt;" );
	    break;
	default:
	    output.push_back( *c );
	    break;
	}
	++c;
    }
    return output;
}

Rendering Feed::render( const Path & path ) const
{
    if ( path.canonical() == "/index.rss" ) {
	Document r( "<h1>Switch to atom, please</h1>\n" );
	r.setHttpResponseCode( 301 );
	return r;
    }

    if ( path.canonical() != "/index.atom" )
	return Plugin::render( path );

    string result =
	"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	"<feed xmlns=\"http://www.w3.org/2005/Atom\">\n"
	"<title>" + title + "</title>\n"
	"<link href=\"" + Path().absolute() + "\"/>\n"
	"<link rel=\"self\" type=\"application/atom+xml\" href=\"" +
	path.absolute() + "\"/>\n\n"
	"<author>\n<name>" + blogger + "</name>\n";
    if ( !email.empty() )
	result += "<email>" + email + "</email>\n";
    if ( !home.empty() )
	result += "<uri>" + home + "</uri>\n";
    result +=
	"</author>\n\n";

    auto posts = Post::all().published().mostRecentFirst();
    auto p = posts.begin();

    result += "<updated>" + formattedTime( (*p)->date() ) + "</updated>\n"
	      "<id>" + path.absolute() + "?" +
	      boost::lexical_cast<string>( scalarTime( (*p)->date() ) ) +
	      "</id>\n";

    int c = 0;
    while ( p != posts.end() && c < postings ) {
	string title;
	auto a = (*p)->linkHere();
	for ( auto child : a.children )
	    child->append( title );

	string all;
	auto & children = (*p)->rootNode().children;
	auto child = children.begin();
	++child;
	while(child != children.end())
	    (*child++)->append( all );

	result +=
	    "<entry>\n"
	    "<title type=\"html\">" + escaped( title ) + "</title>\n"
	    "<link href=\"" + (*p)->path().absolute() + "\"/>\n"
	    "<id>" + (*p)->path().absolute() + "</id>\n"
	    "<content type=\"html\">" + escaped( all ) + "</content>\n"
	    "<updated>" + formattedTime( (*p)->date() ) + "</updated>\n"
	    "</entry>\n\n";

	++p;
	++c;
    }

    result += "</feed>\n";
    return Rendering( result, "application/atom+xml", 86400 );
}


options_description * Feed::options()
{
    options_description * conf =
	new options_description( "Options for the feed plugin" );

    conf->add_options()
	( "blog-name",
	  value<string>( &title )->default_value( "Eloquence unleashed" ),
	  "blog's name" )
	( "blogger-name",
	  value<string>( &blogger )->default_value( "George Sand" ),
	  "blogger's name" )
	( "blogger-email",
	  value<string>( &email ),
	  "blogger's email address (optional)" )
	( "blogger-home",
	  value<string>( &home ),
	  "blogger's home page (optional)" )
	( "feed-postings",
	  value<int>( &postings )->default_value( 5 ),
	  "number of postings to include in the feed" );

    return conf;
}
