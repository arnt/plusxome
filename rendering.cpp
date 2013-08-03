// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "rendering.h"

#include <boost/thread/locks.hpp>


/*! \class Rendering rendering.h

    A Rendering object is anything that has been rendered for output
    via HTTP. Generally its source is a Document, but for non-HTML is
    can also be other things. I suspect a Sitemap plugin will produce
    a Rendering based on its own data structure, and an Asset plugin
    will serve JPEGs and other files as-is.
*/

/*! Constructs an empty uncachable Rendering.
*/

Rendering::Rendering()
{

}


/*! Constructs a Rendering based on \a document. I haven't decided yet
    how its metadata (cache lifetime above all) will be produced.
*/

Rendering::Rendering( Document & document )
{
    tmp = "<!doctype html>\n";
    document.rootNode()->append( tmp );
}


/*! Returns the HTTP response suitable for this Rendering, starting
    with "HTTP/1.1" and ending with the body.
*/

string Rendering::httpResponse()
{
    return "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n" + tmp;
}


boost::shared_mutex renderlock;


/*! Returns a reference to the giant rendering lock. FileWatcher needs
   to grab that exclusively so it can delete items, HttpServer needs
   to grab that in shared mode so it can trust that things aren't
   deleted.
*/

boost::shared_mutex & Rendering::lock()
{
    return renderlock;
}
