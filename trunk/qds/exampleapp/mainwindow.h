#if !defined(MAINWINDOW_H)
#define MAINWINDOW_H

// Qt includes
#include <qmemarray.h>
#include <qurl.h>

// local includes
#include "mainwindowbase.h"

// forward declarations
class QBuffer;
class QNetworkOperation;
class QProgressBar;
class QUrlOperator;

class MainWindow : public MainWindowBase
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow();

protected:
    virtual void slotNew();
    virtual void slotOpen();
    virtual void slotSave();
    virtual void slotSaveAs();
    virtual void slotLaunch();

private:
    QUrl m_url;

    QUrlOperator* m_urlOp;
    QBuffer* m_buffer;

    QProgressBar* m_progressBar;
    
private:    
    bool startDownload(const QUrl& url);
    bool startUpload(const QUrl& url);
    void setCaptionFromUrl(const QUrl& url);

private slots:
    void slotNetOpFinished(QNetworkOperation* op);
    void slotNetOpData(const QByteArray& data, QNetworkOperation* op);
    void slotNetOpProgress(int done, int total, QNetworkOperation* op);
    
private:
    MainWindow(const MainWindow&);
    MainWindow& operator=(const MainWindow&);
};

#endif

// End of File
