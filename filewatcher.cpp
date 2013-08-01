// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "filewatcher.h"

#include "rendering.h"
#include "template.h"
#include "config.h"
#include "asset.h"
#include "post.h"

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
{
    boost::thread( *this );
    // when run() exits the object will be deallocated.

    boost::filesystem::recursive_directory_iterator end;
    boost::filesystem::recursive_directory_iterator dir("./");
    set<string> paths;
    while ( dir != end ) {
	string p = dir->path().native();
	++dir;
	if ( p[0] == '.' && p[1] == '/' )
	    p = p.substr( 2 );
	paths.insert( p );
    }

    processPaths( paths );
}


/*!

*/

void FileWatcher::start()
{
    
}


/*! 

*/

void FileWatcher::processPaths( const set<string> & paths )
{
    boost::unique_lock<boost::shared_mutex> lock(Rendering::lock());

    string postdir = Config::postDirectory;
    unsigned int pds = postdir.size() + 1;
    unsigned int ads = Config::assetDirectory.size() + 1;

    auto p = paths.begin();
    while ( p != paths.end() ) {
	if ( p->size() > pds + 1 + 5 &&
	     (*p)[pds] == '/' &&
	     p->substr(0, pds) == postdir &&
	     p->substr(p->size() - 5) == ".post" ) {
	    string path = p->substr( pds + 1, p->size() - pds - 1 -5 );
	    shared_ptr<Post> post = Post::find( path );
	    if ( post )
		post->reload( *p );
	    else
		(new Post( path ))->reload( *p );
	} else if ( p->size() > ads + 1 + 5 &&
		    (*p)[ads] == '/' &&
		    p->substr(0, pds) == Config::assetDirectory ) {
	    string path = p->substr( ads + 1 );
	    shared_ptr<Asset> asset = Asset::find( path );
	    if ( asset )
		asset->reload( *p );
	    else
		(new Asset( path ))->reload( *p );
	} else if ( *p == Config::singlePostTemplate ) {
	    Template::singlePostTemplate().reload();
	} else if ( *p == Config::categoryTemplate ) {
	    Template::categoryTemplate().reload();
	} else if ( *p == Config::homePageTemplate ) {
	    Template::homePageTemplate().reload();
	}
	++p;
    }
}
