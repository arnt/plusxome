// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "template.h"

#include "config.h"
#include "file.h"


static map<string, Template *> templates;


/*! \class Template template.h

    The Template class provides a HTML document corresponding to a
    file name. It can be used to build HTML responses, by making a
    Document copy of this and filling in various Nodes.
*/


/*! Constructs a template based on the contents of \a s. */

Template::Template( const std::string & s )
    : Document( File( s ).contents() ), filename( s )
{
    templates[s] = this;
}


/*! Reloads the Template from its file. Called by FileWatcher when the
    file has changed.
*/

void Template::reload()
{
    parse( File( filename ).contents() );
}


/*! Returns a pointer to the Template constructed from a filename, or
    a null pointer if there is no such template.
*/

Template * Template::find( const string & filename )
{
    return templates[filename];
}
