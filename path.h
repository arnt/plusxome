// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef PATH_H
#define PATH_H

#include "base.h"


class Path {
public:
    Path();
    Path( const std::string & s );
    Path( const Path & p );

    std::string component( int ) const;
    int components() const;

    std::string canonical() const;
    std::string absolute() const;

private:
    std::vector<std::string> parsed;
    bool trailingSlash;
};

#endif
