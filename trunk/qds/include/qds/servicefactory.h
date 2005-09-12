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
    It handles the plugin loading on Unix, creation of the QApplication object,
    service initialization and provides access to the services which are not
    covered by the standard Qt API, for example the Launcher.

    The ServiceFactory singleton instance can be accessed through the usual
    instance() method.

    Example:
    \code
    #include <qapplication.h>
    #include <qds/servicefactory.h>

    int main(int argc, char** argv)
    {
        QDS::ServiceFactory* qds = QDS::ServiceFactory::instance();
        
        QApplication* app = qds->createApplication(argc, argv);
        
        // initialize the Launcher service
        qds->initService(QDS::Launching);
        
        // create main widget

        int returnValue = app->exec();

        delete app;

        return returnValue;
    }
    \endcode
    
    If you just want to get started and are staisfied with the default set of services,
    you can use the convenience API declared in qds.h:
    \code
    #include <qapplication.h>
    #include <qds/qds.h>

    int main(int argc, char** argv)
    {
        // also initializes services Network and Launching
        QApplication* app = QDS::createApplication(argc, argv);

        // create main widget

        int returnValue = app->exec();

        delete app;

        return returnValue;
    }
    \endcode

    Possible QDS configuration file qdsrc (for users in $HOME.qt/) specifying to
    use the KDE plugin (libqds_kde.so) if no override is given on commandline
    \code
    [General]
    Plugin=kde
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

    //! Initialize QDS and create a QApplication instance
    /*! This is the QDS starting point.
        It handles the plugin loading on Unix and creation of the program's QApplication
        instance.

        The the creation of the QApplication object is handled by QDS so it can
        delegate to the plugin if necessary, i.e. in case the plugin needs a special
        QApplication subclass to integrate with the desktop API's event loop.

        On Unix the access to desktop services depends on the currently available
        desktop API.
        QDS handles this by delegating the service initialization to a service plugin.
        Which plugin is used depends on three items:
        - if there is a plugin name specified on the application's commandline
        - if there is a plugin name specified in the QDS configuration file
        - if QDS can detect which desktop API it is being executed on

        A name specified on the commandline using the commandline switch --qds=pluginname
        overrules any plugin name found in the configuration file.
        When there is neither a name given on commandline nor a name specified in
        the configuration file, QDS will try to detect known desktop APIs before it
        falls back to built in services.

        \param argc the number of program arguments. Usually the value passed to int main()
        \param argv the array of program arguments. Usually the value passed to int main()
        \param useGUI wether to create a Qt application with GUI features

        \return the pointer to the created QApplication instance. Equal to qApp
    */
    QApplication* createApplication(int argc, char** argv, bool useGUI = true);

    //! Initializes a QDS service
    /*! QDS services are implemented using the native desktop APIs. On Unix, where there is
        no single desktop API but a couple of them, the services are implemented in
        service plugins.

        The initService method tries to initialize the native service and either falls back to
        an internal solution or returns false on failure.

        For example the fallback service for Network are the QNetworkProtocol implementations
        shipped with Qt, i.e. failing to initialize native networking will result in
        a call to qInitNetworkProtocols()

        The fallback for Launching on Unix (there is always a native launcher on Windows and
        Mac OS X) is a external process launcher using MIME type mappings from mailcap files
        and examining environment variable $BROWSER for http URLs.

        \param service the QDS service to initialize

        \return true if initialization of the service or at least of the fallback succeeded
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
