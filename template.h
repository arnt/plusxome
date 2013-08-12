// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "document.h"


class Template: public Document {
public:
    Template( const std::string & s );

    void reload();

    static Template * find( const string & );

private:
    string filename;
};


#endif
