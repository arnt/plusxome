// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef PLUGIN_H
#define PLUGIN_H

#include "base.h"

#include "path.h"
#include "post.h"
#include "config.h"
#include "template.h"
#include "document.h"
#include "rendering.h"


class Plugin {
public:
    Plugin();
    virtual ~Plugin();

    virtual Rendering render( const Path & ) const;
    virtual Document produce( const Path & ) const;
    virtual void decorate( Document &, const Path & ) const;

    virtual Document produce404( const Path & ) const;

    virtual void setup();
    static void setupPlugins();

    static void setActivePlugins( const string & );
    static options_description * pluginOptions();

    static const Plugin & first() { return *firstPlugin; }
    const Plugin & next() const { return *nextPlugin; }

private:
    Plugin * nextPlugin;
    static Plugin * firstPlugin;
};


class PluginRegistration {
protected:
    PluginRegistration( const string & );
public:
    virtual ~PluginRegistration();
    virtual Plugin * operator()() = 0;

    virtual options_description * options() = 0;

private:
    string name;
};


template<typename T>
class Registration: public PluginRegistration {
public:
    Registration( const string & n ): PluginRegistration( n ) {}
    virtual ~Registration() {}
    virtual Plugin * operator()() { return new T; }
    virtual options_description * options() { return T::options(); }
};

#endif
