// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TAG_H
#define TAG_H

#include "post.h"


class Tag {
public:
    Tag();
    Tag( const string &, bool = true );

    void unlearnPosts();

    list< shared_ptr<Post> > posts() const;

    Tag * find( const string & );

    bool regular() const;

    static void ensure( const string &, bool = true );

private:
    bool e;
    mutable bool l;
    bool r;
    string n;
    mutable list< shared_ptr<Post> > c;
    mutable shared_mutex m;
};


#endif