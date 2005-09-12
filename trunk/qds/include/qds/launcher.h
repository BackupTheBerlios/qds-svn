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

#ifndef QDS_LAUNCHER_H
#define QDS_LAUNCHER_H

// forward declaration
class LauncherImpl;
class QString;
class QUrl;
class QWidget;

namespace QDS
{

//! Class for launching applications given a file or URL
/*! The Launcher's purpose is to provide an API for starting an
    application associated with a given file or URL, also known as the
    default handler or default application.

    Depending on the implementation the capabilites of the Launcher may
    vary, for example if the Launcher implementation is capable of
    detecting a file's MIME type or if it is capable of passing a given
    MIME type to the native desktop API.

    Examples:
    \code
    Launcher* launcher = ServiceFactory::instance()->launcher();

    // open Trolltech's website in the default browser
    launcher->launch( QUrl("http://www.trolltech.com/") );

    // open a PDF file from the application's current working directory in the PDF viewer
    launcher->launch( "example.pdf" );

    // open a HTML file but ask to have it treated like a plain text file
    launcher->launch( "example.html", "text/plain" );
    \endcode

    The first two examples can also be achieved by using the convenience functions
    declared in the QDS namespace: launch(const QString&) and launch(const QUrl&)

    \code
    QDS::launch( QUrl("http://www.trolltech.com/") );
    QDS::launch( "example.pdf" );
    \endcode
*/
class Launcher
{
    friend class ServiceFactoryImpl;

public:
    //! Launch a given file, requesting automatic detection of MIME type
    /*! Try to launch the given file, i.e. find an application handling the
        respective MIME type. MIME type detection is left to the Launcher
        implementation which will usually delegate it to the native desktop API.

        \param fileName file name with relative or absolute path. Relative paths are
                        treated relative to the application's current working directory

        \param window an optional widget pointer. Native implementations could be
                      capable of associating some context with it, e.g. for
                      avoiding starting launch operations of the same file while
                      the first launch is still in progress

        \return Returns true if an application could be started. This does not
                imply that the started application can actually handle the given file
    */
    bool launch(const QString& fileName, QWidget* window = 0);

    //! Launch a given file, requesting it be treated as begin of the given MIME type
    /*! Try to launch the given file, i.e. find an application handling the
        respective MIME type.

        Explicitly specifying a MIME type could be an advantage if it is hard
        to detect, for example if it doesn't have a file extension and the content
        is text but you know what kind of text it is, e.g HTML

        Another use case would be to ask for different treatment, for example
        having a HTML file opened in a text viewer like in "View source" options
        of webbrowsers.

        \warning overriding MIME type detection might not be possible for all
                 implementations
        
        \param fileName file name with relative or absolute path. Relative paths are
                        treated relative to the application's current working directory

        \param mimeType a mimeType string, for example "text/html"
                        
        \param window an optional widget pointer. Native implementations could be
                      capable of associating some context with it, e.g. for
                      avoiding starting launch operations of the same file while
                      the first launch is still in progress

        \return Returns true if an application could be started. This does not
                imply that the started application can actually handle the given file
    */
    bool launch(const QString& fileName, const QString& mimeType, QWidget* window = 0);

    //! Launch a given URL, requesting automatic detection of MIME type or protocol handler
    /*! Try to launch the given URL, i.e. find an application handling the
        respective protocol or MIME type. MIME type detection is left to the Launcher
        implementation which will usually delegate it to the native desktop API.

        \param url URL to be launched. Depending on the launcher implementation it
                   might use the protocol or, if the URL points to a file, the MIME
                   type of the file to decide which application to launch

        \param window an optional widget pointer. Native implementations could be
                      capable of associating some context with it, e.g. for
                      avoiding starting launch operations of the same URL while
                      the first launch is still in progress

        \return Returns true if an application could be started. This does not
                imply that the started application can actually handle the given URL

        \warning Be sure to pass a QUrl object to launch, because just passing a string
                 will result in a call to launch(const QString&, QWidget*)
    */
    bool launch(const QUrl& url, QWidget* window = 0);

    //! Launch a given URL, requesting it be treated as begin of the given MIME type
    /*! Try to launch the given URL, i.e. find an application handling the
        respective MIME type.

        Explicitly specifying a MIME type could be an advantage if it is hard
        to detect, for example if it doesn't have a file extension and the content
        is text but you know what kind of text it is, e.g HTML

        Another use case would be to ask for different treatment, for example
        having a HTML file opened in a text viewer like in "View source" options
        of webbrowsers.

        \warning overriding MIME type detection might not be possible for all
                 implementations.
        \warning the native launcher could give higher priority to the protocol
        
        \param url URL to be launched. Depending on the launcher implementation it
                   might use the protocol or, if the URL points to a file, the MIME
                   type of the file to decide which application to launch

        \param mimeType a mimeType string, for example "text/html"
        
        \param window an optional widget pointer. Native implementations could be
                      capable of associating some context with it, e.g. for
                      avoiding starting launch operations of the same URL while
                      the first launch is still in progress

        \return Returns true if an application could be started. This does not
                imply that the started application can actually handle the given URL

        \warning Be sure to pass a QUrl object to launch, because just passing a string
                 will result in a call to launch(const QString&, QWidget*)
    */
    bool launch(const QUrl& url, const QString& mimeType, QWidget* window = 0);

private:
    Launcher();
    ~Launcher();

private:
    LauncherImpl* m_impl;
};

}; // namespace QDS

#endif

// end of file
