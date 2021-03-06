// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "filewatcher.h"

#include "httpserver.h"
#include "rendering.h"
#include "template.h"
#include "postset.h"
#include "config.h"
#include "asset.h"
#include "post.h"
#include "tag.h"

#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "boost/filesystem.hpp"
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>


/*! \class FileWatcher filewatcher.cpp

    The (single object instantiated of the) FileWatcher class watches
    over Plusxome's basedir, and creates/updates Post, Asset and
    Template objects.
*/

/*! Constructs a FileWatcher. This is not necessarily a singleton, but
    in practice there is no point in having two.
*/

FileWatcher::FileWatcher()
    : fd( 0 )
{
    boost::thread( *this );
    // when run() exits the object will be deallocated.
}


/*! This private helper adds an inotify watch for \a path. */

void FileWatcher::addWatch( const string & path )
{
    int n = ::inotify_add_watch( fd, path.c_str(),
				 IN_CLOSE_WRITE |
				 IN_CREATE |
				 IN_DELETE |
				 IN_MOVED_FROM |
				 IN_MOVED_TO );
    if(path == ".")
	watches[n] = "";
    else
	watches[n] = path;
}


/*! Scans the entire subtree once and calls processPaths() for all
    files and directories; this is for startup.
*/

void FileWatcher::scanOnce()
{
    boost::filesystem::recursive_directory_iterator end;
    boost::filesystem::recursive_directory_iterator dir("./");
    set<string> paths;
    while ( dir != end ) {
	string p = dir->path().native();
	++dir;
	if ( p[0] == '.' && p[1] == '/' )
	    p = p.substr( 2 );
	if ( p[0] != '.' )
	    paths.insert( p );
    }

    processPaths( paths );
}


/*! Loops forever around inotify(), calling processPaths() whenever a
    file has changed.
*/

void FileWatcher::start()
{
    fd = ::inotify_init();
    addWatch( "." );

    scanOnce();

    while ( true ) {
	set<string> paths;
	char buffer[32768];
	bool done = false;
	while ( !done ) {
	    int r = ::read( fd, buffer, 32767 );
	    int o = 0;
	    while ( o < r ) {
		struct inotify_event e;
		::memmove( &e, buffer + o, sizeof(inotify_event) );
		o += sizeof(inotify_event);
		if ( e.len > 0 ) {
		    // filename does not get the trailing null or nulls
		    string dirname = watches[e.wd];
		    // it needs to get the path, though
		    string filename( buffer + o );
		    if ( !dirname.empty() )
			filename = dirname + "/" + filename;
		    paths.insert( filename );
		    // but we skip the nulls before processing the next event
		    o += e.len;
		}
	    }
	    int more = 0;
	    r = ioctl( fd, FIONREAD, &more );
	    if ( r >= 0 && more == 0 )
		done = true;
	}
	processPaths( paths );
    }
}


/*! Consider each filename in \a paths and update Plusxome's Post,
    Asset and Template sets.
*/

void FileWatcher::processPaths( const set<string> & paths )
{
    boost::unique_lock<boost::shared_mutex> lock(Rendering::lock());
    HttpServer::clearCache();

    string postdir = Config::postDirectory;
    unsigned int pds = postdir.size();
    unsigned int ads = Config::assetDirectory.size();
    unsigned int tagsBefore = Tag::count();

    auto p = paths.begin();
    while ( p != paths.end() ) {
	struct stat st;
        if ( ::stat( p->c_str(), &st ) ) {
	    st.st_mode = 0;
	    if ( errno != ENOENT ) {
		// strange error - should notify blogger
	    }
	}
	if ( S_ISDIR( st.st_mode ) ) {
	    addWatch( *p );
	} else if ( p->size() > 1 && (*p)[p->size()-1] == '~') {
	    // ignore backup files
	} else if ( p->size() > pds + 1 + 5 &&
	     (*p)[pds] == '/' &&
	     p->substr(0, pds) == postdir &&
	     p->substr(p->size() - 5) == ".post" ) {
	    string path = p->substr( pds + 1, p->size() - pds - 1 -5 );
	    Post::find( Path(path), true )->reload( *p );
	} else if ( p->size() > ads + 1 &&
		    (*p)[ads] == '/' &&
		    p->substr( 0, ads ) == Config::assetDirectory ) {
	    string path = p->substr( ads );
	    std::shared_ptr<Asset> asset = Asset::find( path );
	    if ( asset )
		asset->reload( *p );
	    else
		(new Asset( path ))->reload( *p );
	} else if ( ads == 0 && !Template::find( *p ) ) {
	    string path = "/" + *p;
	    std::shared_ptr<Asset> asset = Asset::find( path );
	    if ( asset )
		asset->reload( *p );
	    else
		(new Asset( path ))->reload( *p );
	} else {
	    Template * t = Template::find( *p );
	    if ( t )
		t->reload();
	}
	++p;
    }
    if ( tagsBefore < Tag::count() )
	addImplicitTags();
}


/*! Asks each Post to add any implicit tags to its tag set, which may
    grow as a result of a tag being created in another Post.
*/

void FileWatcher::addImplicitTags()
{
    for( auto p : Post::all() )
	p->addImplicitTags();
}
