// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "httpserver.h"

#include <stdio.h>

#include "base.h"

#include "rendering.h"
#include "plugin.h"


#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

boost::shared_mutex renderlock;

map<string,Rendering> renderings;


/*! \class HttpServer httpserver.h

  The HttpServer class provdes a HTTP server for Plusxome. It expects
  to run in a thread of its own; the start() function does all the
  work, then exits.

  I couldn't find embeddable HTTP server source I liked (technically
  plus BSD), so on the advice of James Antill, I applied some
  cut-and-paste. This server does not support the ASCII art from
  http://www.and.org/texts/server-http. It's even more restrictive than
  Apache, and MUCH more restrictive than RFC 2616.

  The main purpose (I hesitate to say benefit) of these restrictions
  is to avoid allocating memory. Memory never allocated is memory
  never leaked.

  The member functions may be sorted into three groups: start() is the
  do-it-all function (operator()() is a wrapper for start()),
  readRequest(), parseRequest(), readBody() and respond() contain the
  bulk of the code and are separated out for proper testing, and the
  four accessors operation(), path(), body() and and contentLength()
  exist for testing.
*/


/*! Constructs a new HttpServer for \a fd.
*/

HttpServer::HttpServer( int fd )
    : o( Invalid ), f ( fd )
{
    // nothing needed (yet?)
}


/*! Parses input, acts on it. Returns only in case of error.

    This function is not testable.
*/

void HttpServer::start()
{
    try {
	while ( true ) {
	    parseRequest( readRequest() );
	    if ( f >= 0 )
		respond();
	}
    } catch (...) {
	close();
    }
}


/*! Reads and returns a single request. Throws nothing.

    Aborts after 32k; the common requests will be <500 bytes and
    practically all <2k, so 32k is a good sanity limit.
*/

string HttpServer::readRequest()
{
    // first, we read the header, one byte at a time. this is
    // generally considered inefficient, but if we're going to spin up
    // a JVM as a result of this request, who cares about a few hundred
    // system calls more or less?
    string s;

    bool done = false;
    int i = 0;
    while ( i < 32768 && !done ) {
	char x[2];
	int r = ::read( f, &x, 1 );
	if ( r < 0 ) {
	    // an error. we don't care.
	    throw "Read error from client";
	}

	if ( r > 0 && x == 0 ) {
	    // some fun-loving client sent us a null byte. we have no
	    // patience with such games.
	    throw "Null byte from client";
	}

	x[1] = 0;
	s += x;
	i++;

	// there are two ways to end a header: LFLF and CRLFCRLF
	if ( i >= 2 && s[i-1] == 10 && s[i-2] == 10 )
	    done = true;
	if ( i >= 3 && s[i-1] == 10 && s[i-2] == 13 && s[i-3] == 10 )
	    done = true; // LFCRLF. arguably even that's allowed.

	if ( i >= 32768 && !done ) {
	    // the sender sent 32k and didn't actually send a valid header.
	    // is the client buggy, blackhat or just criminally talkative?
	    throw "Overlong request from client";
	}
    }

    return s;
}


static string urldecoded( const string & escaped )
{
    string r;
    auto i = escaped.begin();
    while ( i != escaped.end() ) {
	if ( *i == '%' ) {
	    ++i;
	    bool ok = true;
	    char buffer[3];
	    if ( i == escaped.end() ) 
		ok = false;
	    else
		buffer[0] = *i++;
	    if ( i == escaped.end() ) 
		ok = false;
	    else
		buffer[1] = *i++;
	    buffer[2] = 0;
	    if ( ok && isxdigit( buffer[0] ) && isxdigit( buffer[1] ) )
		r.push_back( (unsigned char)strtol( buffer, 0, 16 ) );
	} else {
	    r.push_back( *i );
	}
	++i;
    }
    return r;
}



/*! Parses \a h as a HTTP request. May set operation() to Invalid, but
    does nothing else to signal errors.

    The parser is quite amazingly strict when it does parse, but
    mostly it doesn't. The client can tell us what Content-Type it
    wants for the report about its RESTfulness, but we're don't
    atually care what it says, so we don't even parse its sayings. As
    I write these words, the only header field we really parse is
    Content-Length, which is necessary for POST.
*/

void HttpServer::parseRequest( string h )
{
    o = Invalid;

    if ( !h.compare( 0, 4, "GET " ) ) {
	o = Get;
    } else {
	o = Invalid;
    }

    int n = 0;
    int l = h.size();
    while( n < l && h[n] != ' ' )
	n++;
    while( n < l && h[n] == ' ' )
	n++;
    int s = n;
    while( n < l && h[n] != ' ' && h[n] != 10 )
	n++;

    p = Path( urldecoded( h.substr( s, n-s ) ) );
}


/*! Responds to the request, such as it is.

    Effectively untestable. Could be separated out into smaller
    chunks, but I don't care right now.
*/

void HttpServer::respond()
{
    if ( o == Invalid ) {
	send( httpResponse( 400, "text/plain", "Utterly total parse error" ) );
	return;
    }

    boost::shared_lock<boost::shared_mutex> lock( renderlock );
    Rendering r;
    string canonicalPath = p.canonical();
    if ( renderings.count( canonicalPath ) ) {
	r = renderings[canonicalPath];
    } else {
	// this may happen twice if two requests come in at the same
	// time, so it has to be idempotent.
	r = Plugin::first().render( p );
	renderings[canonicalPath] = r;
    }
    send( r.httpResponse() );
}


/*! Closes the socket and updates the state machine as needed. */

void HttpServer::close()
{
    ::close( f );
    f = -1;
}


/*! Returns a HTTP response string with \a numeric status, \a textual
    explanation and \a contentType.

    This function does most of what send() ought to do, but this is
    easily testable and the same logic in send() would not be.
*/

string HttpServer::httpResponse( int numeric, const string & contentType,
				 const string & textual,
				 const string & body )
{
    string r = "HTTP/1.0 ";
    // we blithely assume that 100<=numeric<=999
    r += boost::lexical_cast<string>( numeric );
    r += " ";
    r += textual;
    r += "\r\n"
	 "Connection: close\r\n"
	 "Server: Plusxome\r\n"
	 "Content-Type: ";
    r += contentType;
    if ( !body.empty() ) {
	r += "\r\n"
	     "Content-Length: ";
	r += boost::lexical_cast<string>( body.length() );
    }
    r += "\r\n\r\n";
    if ( !body.empty() ) {
	r += body;
    }
    return r;
}


/*! Sends \a response. This function is untested, borderline
    untestable, which is why it's simple.
*/

void HttpServer::send( string response )
{
    int o = 0;
    int l = response.length();
    while ( o < l ) {
	int r = ::write( f, o + response.data(), l - o );
	if ( r <= 0 ) {
	    close();
	    return;
	}
	o += r;
    }
    close();
}
