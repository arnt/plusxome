// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "document.h"


class Template: public Document {
public:
    Template( const std::string & s );

    void reload();

    static Template & singlePostTemplate();
    static Template & categoryTemplate();
    static Template & homePageTemplate();

private:
    string filename;
};


#endif
