// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TAG_H
#define TAG_H

#include "postset.h"


class Tag {
public:
    Tag();
    Tag( const string &, bool = true );

    void unlearnPosts();

    PostSet postSet() const;

    static Tag * find( const string & );

    string name() const;

    bool regular() const;

    static void ensure( const string &, bool = true );

private:
    bool e;
    mutable bool l;
    bool r;
    string n;
    mutable PostSet posts;
    mutable shared_mutex m;
};


#endif
