// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "plugin.h"

#include "lastresort.h"

Plugin * Plugin::firstPlugin = 0;

#include <boost/algorithm/string.hpp>
using namespace boost;


/*! \class Plugin plugin.h

    The Plugin class is the roof of Plusxome's plugin
    interface. Writing a plugin is writing a subclass of the Plugin
    class.

    Virtuals, blah.
*/

/*! Constructs a Plugin and makes first() return a pointer to this
    Plugin.
*/

Plugin::Plugin()
    : nextPlugin( firstPlugin )
{
    firstPlugin = this;
}


/*! Destroys the object and frees any allocated resources. */

Plugin::~Plugin()
{
    // nothing needed, except the vtable.
}


/*! This plugin interface renders a given \a path, if it can. If not,
    it may give the next plugin a chance by calling Plugin::produce(
    \a path ), which eventuall leads to produce() being called.

    Plugins that want to produce HTML are generally better off
    implementing produce(); a plugin to produce e.g. JPEG needs to do
    it in render().
*/

Rendering Plugin::render( const Path & path ) const
{
    return next().render( path );
}


/*! This plugin interface produces a given \a path, if it feels
    responsible for \a path. If not, it may give the next plugin a
    chance by calling Plugin::produce( \a path );
*/

Document Plugin::produce( const Path & path ) const
{
    return next().produce( path );
}


/*! This plugin interface decorates an already-produced \a document
    (for \a path), or does nothing.

    All reimplementations should also call Plugin::decorate( \a
    document, \a path ) in order to let other plugins have a go.
*/

void Plugin::decorate( Document & document, const Path & path ) const
{
    next().decorate( document, path );
}


/*! This plugin interface produces a 404 page for \a path, or punts to
    the next plugin by calling Plugin::produce404( \a path );
*/

Document Plugin::produce404( const Path & path ) const
{
    return next().produce404( path );
}


static map<string, PluginRegistration *> * registered = 0;

/*! Sets the list of active plugins to be that in \a plugins, which
    must be a comma-separated string of valid names.
*/

void Plugin::setActivePlugins( const string & plugins )
{
    (void)new LastResort();
    if ( plugins == "all" ) {
	auto i = registered->begin();
	while ( i != registered->end() ) {
	    (*(i->second))();
	    ++i;
	}
    } else {
	list<string> tokens;
	split( tokens, plugins, is_any_of("," ) );
	auto t = tokens.begin();
	while ( t != tokens.end() ) {
	    string name = *t;
	    ++t;
	    trim( name );
	    PluginRegistration * p = 0;
	    if ( registered )
		p = (*registered)[name];
	    if ( p )
		(void)(*p)();
	    else
		throw "Unknown plugin type: '" + name + "'";
	}
    }
}


/*! \class PluginRegistration plugin.h

    The PluginRegistration class is a helper to let plugins register
    themselves. Each Plugin subclass can register its own name and let
    Plugin create an instance by making a variable (static, in the
    .cpp file) of type Registration<>,
    e.g. Registration<SinglePost> in the case of SinglePost.
*/

/*! Registers a Plugin subtype as being named \a name.
*/

PluginRegistration::PluginRegistration( const string & name )
{
    if ( !registered )
	registered = new map<string, PluginRegistration *>;
    (*registered)[name] = this;
}


/*! This should never be called. It cannot work safely. What should it
    do?
 */

PluginRegistration::~PluginRegistration()
{
    if ( registered )
	registered->clear();
}


/*! This function is called after Plusxome has read its configuration
    and all plugins are enabled, but before the first HTTP request has
    been served.
*/

void Plugin::setup()
{
}


/*! Calls setup() on all active plugins.
*/

void Plugin::setupPlugins()
{
    Plugin * p = firstPlugin;
    while ( p ) {
	p->setup();
	p = p->nextPlugin;
    }
}


options_description * Plugin::pluginOptions()
{
    // static map<string, PluginRegistration *> * registered = 0;
    auto r = registered->begin();
    options_description * cf = new options_description();
    while ( r != registered->end() ) {
	cf->add( *(r->second->options()) );
	++r;
    }
    return cf;
}
