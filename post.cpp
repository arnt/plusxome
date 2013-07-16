// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "post.h"

static map<string,shared_ptr<Post> > posts;



/*! \class Post post.h
  
    The Post class models a single .post file.
*/

/*! Constructs an empty Post. This will likely become private... or?
*/

Post::Post()
{
    
}


/*! Returns a reference to the root DOM node of this posting.

*/

const Node & Post::rootNode() const
{
    return *root;
}


/*! Returns a pointer to the Post corresponding to \a path. \a path should
    not include the leading base directory or trailing .post.

    Returns a null pointer if there is no such Post.
*/

shared_ptr<Post> Post::find( const std::string & path )
{
    return posts[path];
}
