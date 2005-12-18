/******************************************************************************
    Copyright 2005, Kevin Krammer <kevin.krammer@gmx.at>
    All Rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef QDS_SERVICEFACTORY_H
#define QDS_SERVICEFACTORY_H

// forward declarations
class QApplication;

namespace QDS
{

//! Services implemented by QDS
enum Service
{
    //! QNetworkProtocol implementations for accessing remote or virtual file systems
    Network = 0,

    //! Launcher implementation for starting applications for files and URLs
    Launching
};

// forward declarations
class Launcher;
class ServiceFactoryImpl;

//! Central access point to the QDS API
/*! The ServiceFactory handles all stages of the QDS life cycle.
    It handles the inter-process communication on Unix, service
    initialization and provides access to the services which are not
    covered by the standard Qt API, for example the Launcher.

    The ServiceFactory singleton instance can be accessed through the usual
    instance() method.

    Example:
    \code
    #include <qapplication.h>
    #include <qds/servicefactory.h>

    int main(int argc, char** argv)
    {
        QApplication app(argc, argv);

        QDS::ServiceFactory* qds = QDS::ServiceFactory::instance();

        qds->init(app.argc(), app.argv());

        // initialize the Launcher service
        qds->initService(QDS::Launching);

        // create main widget

        return app.exec();
    }
    \endcode

    If you just want to get started and are satisfied with the default set of services,
    you can use the convenience API declared in qds.h:
    \code
    #include <qapplication.h>
    #include <qds/qds.h>

    int main(int argc, char** argv)
    {
        QApplication app(argc, argv);

        // also initializes services Network and Launching
        QDS::init(app.argc(), app.argv());

        // create main widget

        return app.exec();
    }
    \endcode
*/
class ServiceFactory
{
public:
    //! Returns the ServiceFactory instance after creating it if necessary
    static ServiceFactory* instance()
    {
        if (m_instance == 0) m_instance = new ServiceFactory();
        return m_instance;
    }

    //! Initialize QDS
    /*! This is the QDS starting point.

        On Unix this handles the setup of the D-BUS connection to the service
        daemon, which in turn handles the adapting to the currently running
        desktop API.

        \param argc the number of program arguments. Usually the value passed to int main()
        \param argv the array of program arguments. Usually the value passed to int main()

        \return \c true if successfull, otherwise \c false
    */
    bool init(int argc, char** argv);

    //! Initializes a QDS service
    /*! QDS services are implemented using the native desktop APIs. On Unix, where there is
        no single desktop API but a couple of them, the services are implemented in
        service plugins loaded into a separate daemon or by suitable D-BUS interface
        implementations in desktop base components.

        The initService method tries to initialize the native service and either falls back to
        an internal solution or returns \c false on failure.

        For example the fallback service for Network are the QNetworkProtocol implementations
        shipped with Qt, i.e. failing to initialize native networking will result in
        a call to qInitNetworkProtocols()

        The fallback for Launching on Unix (there is always a native launcher on Windows and
        Mac OS X) is a external process launcher using MIME type mappings from mailcap files
        and examining environment variable $BROWSER for http URLs.

        \param service the QDS service to initialize

        \return \c true if initialization of the service or at least of the fallback succeeded
    */
    bool initService(Service service);

    //! Returns the Launcher instance
    /*! \return will return 0 if the respective service (Launching) was not or not successfully
        initialized

        \sa Launcher
    */
    Launcher* launcher();

private:
    ServiceFactory();
    ~ServiceFactory();

private:
    static ServiceFactory* m_instance;

    ServiceFactoryImpl* m_impl;
};

};

#endif

// End of File
