// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef POST_H
#define POST_H

#include "node.h"


class Post {
public:
    Post();
    
    const Node & rootNode() const;

    static boost::shared_ptr<Post> find( const std::string & );
};


#endif
