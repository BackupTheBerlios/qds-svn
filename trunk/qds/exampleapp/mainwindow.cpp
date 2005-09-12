// Qt includes
#include "qbuffer.h"
#include "qfiledialog.h"
#include "qprogressbar.h"
#include "qstatusbar.h"
#include "qtextedit.h"
#include "qtextstream.h"
#include "qurloperator.h"

// QDS includes
#include "qds/qds.h"

// local includes
#include "mainwindow.h"

///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : MainWindowBase(0, "MainWindow"),
    m_urlOp(0),
    m_buffer(0),
    m_progressBar(0)
{
    m_progressBar = new QProgressBar(statusBar());
    statusBar()->addWidget(m_progressBar, 0, true);
    m_progressBar->hide();
    
    slotNew();
}

///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    delete m_buffer;
    delete m_urlOp;
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::slotNew()
{
    m_url = QUrl();
    m_editor->clear();
    
    setCaptionFromUrl(m_url);
    statusBar()->message(tr("Ready"));
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::slotOpen()
{    
    statusBar()->message(tr("Opening document ..."));
        
    QFileDialog dialog(this, "Open Dialog", true);

    QUrl baseUrl = dialog.url();
    if (m_url.isValid())
    {
        baseUrl = m_url;
        baseUrl.setFileName(QString::null);
    }
    dialog.setUrl(QUrlOperator(baseUrl));

    dialog.setMode(QFileDialog::ExistingFile);
    dialog.setFilters(tr("Text files (*.txt);; All files (*)"));

    if (dialog.exec() == QDialog::Accepted)
    {
        QUrl url(dialog.selectedFile());
        if (url.isValid())
        {
            startDownload(url);
        }
    }
    else
    {
        statusBar()->message(tr("Open cancelled"), 500);
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::slotSave()
{
    statusBar()->message(tr("Saving document ..."));
    
    if (m_url.isValid())
    {
        startUpload(m_url);
    }
    else
    {
        slotSaveAs();
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::slotSaveAs()
{    
    statusBar()->message(tr("Saving document ..."));
    
    QFileDialog dialog(this, "SaveAs Dialog", true);

    QUrl baseUrl = dialog.url();
    if (m_url.isValid())
    {
        baseUrl = m_url;
        baseUrl.setFileName(QString::null);
    }
    dialog.setUrl(QUrlOperator(baseUrl));
    
    dialog.setMode(QFileDialog::AnyFile);
    dialog.setFilters(tr("Text files (*.txt);; All files (*)"));

    QUrl url;
    while (!url.isValid() || url.fileName().isEmpty())
    {
        if (dialog.exec() == QDialog::Rejected)
        {
            statusBar()->message(tr("Saving cancelled"), 500);
            return;
        }
        url = dialog.selectedFile();
    }
    
    startUpload(url);
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::slotLaunch()
{
    qDebug("Trying to launch");
    
    statusBar()->message(tr("Launching document ..."));
    if (!QDS::launch(m_url))
    {
        statusBar()->message(tr("Launching failed"), 500);
    }
    else
    {
        statusBar()->message(tr("Launching successfull"), 1000);
    }
}

///////////////////////////////////////////////////////////////////////////////

bool MainWindow::startDownload(const QUrl& url)
{
    if (m_urlOp != 0) return false;

    statusBar()->message(tr("Preparing download ..."));
    
    m_urlOp = new QUrlOperator(url);
    QObject::connect(m_urlOp, SIGNAL(finished(QNetworkOperation*)),
                     this, SLOT(slotNetOpFinished(QNetworkOperation*)));
    QObject::connect(m_urlOp, SIGNAL(data(const QByteArray&, QNetworkOperation*)),
                     this, SLOT(slotNetOpData(const QByteArray&, QNetworkOperation*)));
    QObject::connect(m_urlOp, SIGNAL(dataTransferProgress(int, int, QNetworkOperation*)),
                     this, SLOT(slotNetOpProgress(int, int, QNetworkOperation*)));
    
    if (m_urlOp->get() == 0)
    {
        delete m_urlOp;
        m_urlOp = 0;
        
        statusBar()->message(tr("Download not possible"), 500);
    }
    else
    {
        if (m_buffer == 0) m_buffer = new QBuffer();
    
        m_buffer->close();
        m_buffer->open(IO_WriteOnly);
    }

    statusBar()->message(tr("Download in progress"));
    m_progressBar->reset();
    m_progressBar->show();
    
    return m_urlOp != 0;
}

///////////////////////////////////////////////////////////////////////////////

bool MainWindow::startUpload(const QUrl& url)
{
    if (m_urlOp != 0) return false;
    
    statusBar()->message(tr("Preparing upload ..."));
    
    m_urlOp = new QUrlOperator(url);
    QObject::connect(m_urlOp, SIGNAL(finished(QNetworkOperation*)),
                     this, SLOT(slotNetOpFinished(QNetworkOperation*)));
    QObject::connect(m_urlOp, SIGNAL(dataTransferProgress(int, int, QNetworkOperation*)),
                     this, SLOT(slotNetOpProgress(int, int, QNetworkOperation*)));

    if (m_buffer == 0) m_buffer = new QBuffer();

    m_buffer->close();
    m_buffer->open(IO_WriteOnly);
    
    QTextStream stream(m_buffer);
    stream << m_editor->text();
    
    m_buffer->close();
    
    if (m_urlOp->put(m_buffer->buffer()) == 0)
    {
        delete m_urlOp;
        m_urlOp = 0;
    
        statusBar()->message(tr("Upload not possible"), 500);
    }
    else
    {
        statusBar()->message(tr("Upload in progress"));
        m_progressBar->reset();
        m_progressBar->show();
    }

    delete m_buffer;
    m_buffer = 0;
        
    return m_urlOp != 0;
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::setCaptionFromUrl(const QUrl& url)
{
    if (!url.isValid())
    {
        setCaption(tr("QDS Editor Example - unnamed"));
    }
    else
    {
        // remove password
        QUrl captionUrl = url;
        captionUrl.setPassword(QString::null);

        setCaption(tr("QDS Editor Example - %1").arg(captionUrl.toString()));
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::slotNetOpFinished(QNetworkOperation* op)
{
    qDebug("Network operation finished");
    if (m_buffer != 0)
    {
        m_buffer->close();

        if (op->state() == QNetworkProtocol::StDone)
        {
            m_editor->setText(QString(m_buffer->buffer()));
        }
    
        delete m_buffer;
        m_buffer = 0;
    }

    if (op->state() == QNetworkProtocol::StDone)
    {
        m_url = op->arg(0);
        statusBar()->message(tr("Transfer successfull"), 1000);
    }
    else
    {
        statusBar()->message(tr("Transfer failed"), 1000);
    }
    
    m_urlOp->deleteLater();
    m_urlOp = 0;

    setCaptionFromUrl(m_url);
    m_progressBar->hide();
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::slotNetOpData(const QByteArray& data, QNetworkOperation* op)
{
    Q_UNUSED(op);
    
    m_buffer->writeBlock(data);
    
    statusBar()->message(tr("Download in progress: %1 bytes").arg(m_buffer->size()));
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::slotNetOpProgress(int done, int total, QNetworkOperation* op)
{
    Q_UNUSED(op);
    
    m_progressBar->setTotalSteps(total);
    m_progressBar->setProgress(done);
}

// End of File
