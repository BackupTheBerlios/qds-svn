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

#ifndef QDS_H
#define QDS_H

/*! \mainpage Q Desktop Services

    \section intro Introduction
    Q Desktop Services, like Qt itself, provides crossplatform interfaces
    to platform specific service APIs.

    While Qt is abstracting operating system and windowing system APIs, QDS
    wraps higher level service APIs, common on desktop system but not necessarily
    available on all Qt supported target platforms.

    Nevertheless the goal of QDS is to provide at least a suitable fallback
    implementation in case the system it is running is not capable of
    providing the services.

    \subsection single_de Fixed desktop API platforms
    On systems where a single desktop service API is guaranteed to exists,
    all glue code is directly implemented in the QDS library and applications
    statically linking against it will get tighter platform integration with
    only minimal required changes to the application code.

    \subsection multi_de Exchangable desktop API platforms
    On systems where more than one desktop service API is used, Unix and Linux
    systems, the actual desktop specific service implementations are
    delegated to plugins and the QDS library built-ins are only meant to
    be fallback implementations for the unfortunate situation that a plugin
    based desktop access is not available.

    The functionality available to an application in this platforms will
    depend on loaded plugin.

    \subsection summary Summary
    QDS can provide access to additional features commonly found on
    todays desktop systems, but cannot guarantee the same quality of
    service implementations across the whole range of possible target platforms.

    \section services Services
    The services provided by QDS can be put into two categories: Qt backends
    and additional APIs.

    \subsection backends Qt backends
    Service from this category do not introduce new APIs but are implemented as
    backends for Qt classes/subsystem that allow extensions by applications.

    An example for a service from this category is the Network service, which
    is implemented as additional protocol handlers for Qt's network abstraction
    QNetworkProtocol.
    As a result the natural fallback implementation is Qt's own set of protocol
    handlers, so initializing the Network service will never fail and at least
    provide the protocols supported by Qt itself.

    \subsection addons Additional APIs
    Services from this category introduce new classes and functions an
    application developer can use additionally to the ones provided by Qt.

    An example for a service from this category is the Launcher service, which
    is implemented as an additional class with methods for starting applications
    associated with files or URLs, also known as default application or default handler.
*/

// forward declarations
class QApplication;
class QString;
class QUrl;

//! Q Desktop Services classes and stand-alone convenience functions
/*! All classes and stand-alone functions of the Q Desktop Services library are
    within the QDS namespace.

    The header file qds.h contains some stand-alone convenience functions for
    developers that do not need all the control options that the service classes
    offer.
*/
namespace QDS
{
    //! Create a QApplication instance and initialize the default service set
    /*! This function serves as a replacement for the direct construction of
        a QApplication instance using the constructor.

        It will pass the given parameters unchanged to the QApplication constructor
        but will examine them for QDS specific commandline switches, for example
        which the plugin override switch.

        On Unix this will also try to load the QDS plugin prior to application creation.

        After application creation it will try to initialize the default set of QDS
        services: Network and Launcher

        \param argc the number of program arguments. Usually the value passed to int main()
        \param argv the array of program arguments. Usually the value passed to int main()
        \param useGUI wether to create a Qt application with GUI features

        \return the pointer to the created QApplication instance. Equal to qApp

        Example main.cpp
        \code
        #include <qapplication.h>
        #include <qds/qds.h>

        int main(int argc, char** argv)
        {
            QApplication* app = QDS::createApplication(argc, argv);

            // create main widget

            int returnValue = app->exec();

            delete app;

            return returnValue;
        }
        \endcode

        \sa ServiceFactory
    */
    QApplication* createApplication(int argc, char** argv, bool useGUI = true);

    //! Launch a given file, requesting automatic detection of MIME type
    /*! Try to launch the given file, i.e. find an application handling the
        respective MIME type. MIME type detection is left to the Launcher
        implementation which will usually delegate it to the native desktop API.

        Example:
        \code
        // open a PDF file from the application's current working directory in the PDF viewer
        QDS::launch( "example.pdf" );
        \endcode
        
        \param fileName file name with relative or absolute path. Relative paths are
                        treated relative to the application's current working directory

        \return Returns true if an application could be started. This does not
                imply that the started application can actually handle the given file

        \sa Launcher
    */
    bool launch(const QString& fileName);

    //! Launch a given URL, requesting automatic detection of MIME type or protocol handler
    /*! Try to launch the given URL, i.e. find an application handling the
        respective protocol or MIME type. MIME type detection is left to the Launcher
        implementation which will usually delegate it to the native desktop API.

        Example:
        \code
        // open Trolltech's website in the default browser
        QDS::launch( QUrl("http://www.trolltech.com/") );
        \endcode
        
        \param url URL to be launched. Depending on the launcher implementation it
                   might use the protocol or, if the URL points to a file, the MIME
                   type of the file to decide which application to launch

        \return Returns true if an application could be started. This does not
                imply that the started application can actually handle the given URL

        \warning Be sure to pass a QUrl object to launch, because just passing a string
                 will result in a call to launch(const QString&)
        
        \sa Launcher
    */
    bool launch(const QUrl& url);
};

#endif

// End of File
