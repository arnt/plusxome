// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef POSTSET_H
#define POSTSET_H

#include "post.h"



class PostSet: public vector< std::shared_ptr<Post> > {
public:
    PostSet();

    PostSet section( int, int ) const;

    PostSet mostRecentFirst() const;
};


#endif
