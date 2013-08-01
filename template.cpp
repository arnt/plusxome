// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "template.h"

#include "config.h"
#include "file.h"



/*! \class Template template.h

    The Template class provides a HTML document corresponding to a
    file name. It can be used to build HTML responses, by making a
    Document copy of this and filling in various Nodes.
*/


/*! Constructs a template based on the contents of \a s. */

Template::Template( const std::string & s )
    : Document( File( s ).contents() ), filename( s )
{
    // nothing needed
}


/*! Reloads the Template from its file. Called by FileWatcher when the
    file has changed.
*/

void Template::reload()
{
    parse( File( filename ).contents() );
}


static Template * spt = 0;

/*! Returns a reference to the Template for single-post pages. */

Template & Template::singlePostTemplate()
{
    if ( !spt )
	spt = new Template( Config::singlePostTemplate );
    return *spt;
}


static Template * ct = 0;

/*! Returns a reference to the Template for category pages. */

Template & Template::categoryTemplate()
{
    if ( !ct )
	ct = new Template( Config::singlePostTemplate );
    return *ct;
    
}


static Template * hpt = 0;

/*! Returns a reference to the Template for the blog's home page. */

Template & Template::homePageTemplate()
{
    if ( !hpt )
	hpt = new Template( Config::singlePostTemplate );
    return *hpt;
}
