

#include <QTextStream>
 
#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KFileDialog>
#include <KMessageBox>
#include <KDebug>
#include <KIO/NetAccess>
#include <KSaveFile>
#include <KRuler>
#include <KConfigDialog>


#include <mlt++/Mlt.h>

#include "mainwindow.h"
#include "trackview.h"
#include "kdenlivesettings.h"
#include "ui_configmisc_ui.h"
 
MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent),
      fileName(QString())
{
  m_timelineArea = new KTabWidget(this);
  m_timelineArea->setHoverCloseButton(true);
  m_timelineArea->setTabReorderingEnabled(true);
  connect(m_timelineArea, SIGNAL(currentChanged (int)), this, SLOT(activateDocument()));
  setCentralWidget(m_timelineArea);

  projectListDock = new QDockWidget(i18n("Project Tree"), this);
  projectListDock->setObjectName("project_tree");
  m_projectList = new ProjectList(NULL, this);
  projectListDock->setWidget(m_projectList);
  addDockWidget(Qt::TopDockWidgetArea, projectListDock);

  effectListDock = new QDockWidget(i18n("Effect List"), this);
  effectListDock->setObjectName("project_tree");
  effectList = new KListWidget(this);
  effectListDock->setWidget(effectList);
  addDockWidget(Qt::TopDockWidgetArea, effectListDock);
  
  effectStackDock = new QDockWidget(i18n("Effect Stack"), this);
  effectStackDock->setObjectName("project_tree");
  effectStack = new KListWidget(this);
  effectStackDock->setWidget(effectStack);
  addDockWidget(Qt::TopDockWidgetArea, effectStackDock);
  
  transitionConfigDock = new QDockWidget(i18n("Transition"), this);
  transitionConfigDock->setObjectName("project_tree");
  transitionConfig = new KListWidget(this);
  transitionConfigDock->setWidget(transitionConfig);
  addDockWidget(Qt::TopDockWidgetArea, transitionConfigDock);

  Mlt::Factory::init(NULL);

  clipMonitorDock = new QDockWidget(i18n("Clip Monitor"), this);
  clipMonitorDock->setObjectName("project_tree");
  m_clipMonitor = new Monitor("clip", this);
  clipMonitorDock->setWidget(m_clipMonitor);
  addDockWidget(Qt::TopDockWidgetArea, clipMonitorDock);

  projectMonitorDock = new QDockWidget(i18n("Project Monitor"), this);
  projectMonitorDock->setObjectName("project_tree");
  m_projectMonitor = new Monitor("project", this);
  projectMonitorDock->setWidget(m_projectMonitor);
  addDockWidget(Qt::TopDockWidgetArea, projectMonitorDock);

  setupActions();
  tabifyDockWidget (effectListDock, projectListDock);
  tabifyDockWidget (effectListDock, effectStackDock);
  tabifyDockWidget (effectListDock, transitionConfigDock);

  tabifyDockWidget (clipMonitorDock, projectMonitorDock);

  connect(m_projectList, SIGNAL(clipSelected(const QDomElement &)), m_projectMonitor, SLOT(slotSetXml(const QDomElement &)));

  connect(m_projectList, SIGNAL(getFileProperties(const KUrl &, uint)), m_projectMonitor->render, SLOT(getFileProperties(const KUrl &, uint)));

  connect(m_projectMonitor->render, SIGNAL(replyGetImage(const KUrl &, int, const QPixmap &, int, int)), m_projectList, SLOT(slotReplyGetImage(const KUrl &, int, const QPixmap &, int, int)));

  connect(m_projectMonitor->render, SIGNAL(replyGetFileProperties(const QMap < QString, QString > &, const QMap < QString, QString > &)), m_projectList, SLOT(slotReplyGetFileProperties(const QMap < QString, QString > &, const QMap < QString, QString > &)));

}
 
void MainWindow::setupActions()
{
  KAction* clearAction = new KAction(this);
  clearAction->setText(i18n("Clear"));
  clearAction->setIcon(KIcon("document-new"));
  clearAction->setShortcut(Qt::CTRL + Qt::Key_W);
  actionCollection()->addAction("clear", clearAction);
  /*connect(clearAction, SIGNAL(triggered(bool)),
          textArea, SLOT(clear()));*/
 
  KStandardAction::quit(kapp, SLOT(quit()),
                        actionCollection());
 
  KStandardAction::open(this, SLOT(openFile()),
                        actionCollection());
 
  KStandardAction::save(this, SLOT(saveFile()),
                        actionCollection());
 
  KStandardAction::saveAs(this, SLOT(saveFileAs()),
                        actionCollection());
 
  KStandardAction::openNew(this, SLOT(newFile()),
                        actionCollection());

  KStandardAction::openNew(this, SLOT(newFile()),
                        actionCollection());

  KStandardAction::preferences(this, SLOT(slotPreferences()),
	    actionCollection());
 
  setupGUI();
}
 
void MainWindow::newFile()
{
  KdenliveDoc *doc = new KdenliveDoc(KUrl(), 25, 720, 576);
  TrackView *trackView = new TrackView(doc);
  m_timelineArea->addTab(trackView, "New Project");
}

void MainWindow::activateDocument()
{
  TrackView *currentTab = (TrackView *) m_timelineArea->currentWidget();
  KdenliveDoc *currentDoc = currentTab->document();
  connectDocument(currentDoc);
}
 
void MainWindow::saveFileAs(const QString &outputFileName)
{
  KSaveFile file(outputFileName);
  file.open();
  
  QByteArray outputByteArray;
  //outputByteArray.append(textArea->toPlainText());
  file.write(outputByteArray);
  file.finalize();
  file.close();
  
  fileName = outputFileName;
}
 
void MainWindow::saveFileAs()
{
  saveFileAs(KFileDialog::getSaveFileName());
}
 
void MainWindow::saveFile()
{
  if(!fileName.isEmpty())
  {
    saveFileAs(fileName);
  }
  else
  {
    saveFileAs();
  }
}
 
void MainWindow::openFile() //changed
{
  openFile(KFileDialog::getOpenFileName(KUrl(), "application/vnd.kde.kdenlive"));
}
 
void MainWindow::openFile(const QString &inputFileName) //new
{
  KdenliveDoc *doc = new KdenliveDoc(KUrl(inputFileName), 25, 720, 576);
  TrackView *trackView = new TrackView(doc);
  m_timelineArea->setCurrentIndex(m_timelineArea->addTab(trackView, QIcon(), doc->documentName()));
  connectDocument(doc);
  
}

void MainWindow::connectDocument(KdenliveDoc *doc) //changed
{
  m_projectList->populate(doc->producersList());
  //connect(doc, SIGNAL(addClip(QDomElement &)), m_projectList, SLOT(slotAddClip(QDomElement &)));
}


void MainWindow::slotPreferences()
{
  //An instance of your dialog could be already created and could be
  // cached, in which case you want to display the cached dialog
  // instead of creating another one
  if ( KConfigDialog::showDialog( "settings" ) )
    return;

  // KConfigDialog didn't find an instance of this dialog, so lets
  // create it :
  KConfigDialog* dialog = new KConfigDialog(this, "settings",
                                          KdenliveSettings::self());

  QWidget *page1 = new QWidget;
  Ui::ConfigMisc_UI* confWdg = new Ui::ConfigMisc_UI( );
  confWdg->setupUi(page1);

  dialog->addPage( page1, i18n("Misc"), "misc" );

  //User edited the configuration - update your local copies of the
  //configuration data
  connect( dialog, SIGNAL(settingsChanged()), this, SLOT(updateConfiguration()) );

  dialog->show();
}

#include "mainwindow.moc"
