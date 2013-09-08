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
    : responseCode( 200 )
{

}


/*! Constructs a Rendering based on \a document. I haven't decided yet
    how its metadata (cache lifetime above all) will be produced.
*/

Rendering::Rendering( Document & document )
{
    responseCode = document.httpResponseCode();
    tmp = "<!doctype html>\n";
    ctype = "text/html";
    document.rootNode()->append( tmp );
}


/*! Constructs a Rendering that returns \a content with \a contentType.
*/

Rendering::Rendering( const string & content, const string & contentType )
{
    responseCode = 200;
    tmp = content;
    ctype = contentType;
}


/*! Returns the HTTP response suitable for this Rendering, starting
    with "HTTP/1.1" and ending with the body.
*/

string Rendering::httpResponse()
{
    return "HTTP/1.1 " +
	boost::lexical_cast<string>( responseCode ) +
	" Have a nice day\r\n"
	"Content-Type: " + ctype + "\r\n"
	"Server: Plusxome/0.1 (http://rant.gulbrandsen.priv.no/plusxome)\r\n"
	"Connection: close\r\n"
	"\r\n" + tmp;
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


/*! Records that this rendering's HTTP response code is \a code. The
    value set at construction time is 200.
*/

void Rendering::setHttpResponseCode( int code )
{
    responseCode = code;
}


/*! Returns what was recorded by setHttpResponseCode(), or 200 if
    setHttpResponseCode() has not been called.
*/

int Rendering::httpResponseCode() const
{
    return responseCode;
}
