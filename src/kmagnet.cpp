/*************************************************************************************
 *  Copyright (C) 2009 by Oscar Martinez <omllobet@gmail.com>                        *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 3                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/


#include "kmagnet.h"
#include "settings.h"
#include "common.h"
#include "kmagnetcell.h"
#include "kmagnetsolver.h"
#include <knewstuff2/engine.h>

#include <KConfigDialog>
#include <KStatusBar>
#include <KAction>
#include <KActionCollection>
#include <KDE/KLocale>
#include <KToolBar>
#include <KMessageBox>
#include <KFileDialog>
#include <KMenuBar>
#include <KScoreDialog>
#include <KStandardGameAction>
#include <KGameDifficulty>
#include <KStandardDirs>
#include <KDirSelectDialog>
#include <KGameThemeSelector>

#include <QHBoxLayout>
#include "kmagnetrenderer.h"

kmagnet::kmagnet() : KXmlGuiWindow()
{
    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    ROWS=25;
    COLUMNS=20;
    m_view= new kmagnetView(this);
    m_view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_view->setFrameStyle(QFrame::NoFrame);
    m_view->setCacheMode( QGraphicsView::CacheBackground );
    m_view->setOptimizationFlags( QGraphicsView::DontClipPainter |
                                  QGraphicsView::DontSavePainterState
                                  //| QGraphicsView::DontAdjustForAntialiasing 
				  );

    m_scene = new kmagnetScene(this, ROWS, COLUMNS);
    //m_scene->setBackgroundBrush(Qt::lightGray);//now in themes
    
    connect(m_scene, SIGNAL(advanceMovements(int)), this, SLOT(advanceMovements(int)));
    connect(m_scene, SIGNAL(itsover(bool)), this, SLOT(gameOver(bool)));
    m_view->setScene(m_scene);
    connect(m_view,SIGNAL(resizeScene(int , int )),m_scene,SLOT(resizeScene(int , int )));

    //solver
    m_solver= new kmagnetSolver(this);
    connect(m_solver, SIGNAL(finished()),this,SLOT(solutionFound()));

    m_gameClock = new KGameClock(this, KGameClock::MinSecOnly);
    connect(m_gameClock, SIGNAL(timeChanged(const QString&)), SLOT(advanceTime(const QString&)));

    setCentralWidget(m_view);
    // add a status bar
    statusBar()->insertItem( i18n("Time: 00:00"), 0);
    statusBar()->insertItem( i18n("Movements: 0"), 1);
    statusBar()->show();

    // then, setup our actions
    setupActions();
    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI();     
    //toolbar action
    puzzleAction = new KActionMenu(KIcon("arrow-down"), i18n("Play random") ,this);
    fillPuzzleList(puzzles, puzzlesList, "puzzle_list", SLOT(puzzleSelected()));
    connect (puzzleAction, SIGNAL(triggered()), SLOT(playRandomPuzzle()));  
    toolBar()->insertAction(action("game_load"), puzzleAction); 
    setFocus();
}

kmagnet::~kmagnet()
{
    delete m_gameClock;
}

const kmagnet::PuzzleItem kmagnet::puzzles [] = {
    {"puzzle1.kmp",		I18N_NOOP("Hard Puzzle 1")},
    {"puzzle2.kmp",		I18N_NOOP("Hard Puzzle 2")},
    {"puzzle3.kmp",		I18N_NOOP("Hard Puzzle 3")},
    {"END",			""}
};

void kmagnet::newGame()
{
    m_gameClock->restart();
    m_gameClock->pause();
    statusBar()->changeItem( i18n("Time: 00:00"), 0);
    advanceMovements(0);
    QAction * editingModeAction = this->action("editmode");
    if (!editingModeAction->isChecked())
        editingModeAction->trigger();
    QAction * pauseAction = this->action("game_pause");
    if (pauseAction->isChecked())
        pauseAction->activate(KAction::Trigger);
    KGameDifficulty::setRunning(true);
    m_scene->newGame();    
}

void kmagnet::showHighscores()
{
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Time, this);
    ksdialog.exec();
}

void kmagnet::setupActions()
{
    KStandardGameAction::gameNew(this, SLOT(newGame()), actionCollection());
    KStandardGameAction::restart(this, SLOT (restart()), actionCollection());
    KStandardGameAction::load(this, SLOT(load()), actionCollection());
    KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
    KStandardGameAction::saveAs(this, SLOT(save()), actionCollection());
    KStandardGameAction::solve(this, SLOT(solveFunc()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    KStandardAction::preferences( this, SLOT( configureSettings()) , actionCollection() );
    KStandardGameAction::pause( this, SLOT( pause(bool ) ), actionCollection() );
    KAction *editModeAction= new KAction(i18n("Editor Mode"),this);
    editModeAction->setCheckable(true);
    editModeAction->setShortcut(Qt::CTRL + Qt::Key_T);
    actionCollection()->addAction("editmode", editModeAction);
    connect( editModeAction, SIGNAL( triggered(bool) ),this, SLOT( editingMode(bool) ) );
    editModeAction->trigger();
    KGameDifficulty::init(this,this, SLOT(levelChanged(KGameDifficulty::standardLevel)));
    KGameDifficulty::setRestartOnChange(KGameDifficulty::RestartOnChange);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
    KGameDifficulty::setLevel(KGameDifficulty::Hard);
    //hotnewstuff
    KAction *hotNewStuffAction= new KAction(i18n("Get puzzles!"),this);
    hotNewStuffAction->setShortcut(Qt::CTRL + Qt::Key_H);
    hotNewStuffAction->setIcon(KIcon("get-hot-new-stuff"));
    actionCollection()->addAction("hotnewstuff", hotNewStuffAction);
    connect( hotNewStuffAction, SIGNAL( triggered() ),this, SLOT( getHotNewStuff()) );
}

void kmagnet::configureSettings()
{
    // The preference dialog is derived from prefs_base.ui
    //
    // compare the names of the widgets in the .ui file
    // to the names of the variables in the .kcfg file
    //avoid to have 2 dialogs shown
    if ( KConfigDialog::showDialog( i18n("Settings") ))  {
        return;
    }
    KConfigDialog *dialog = new KConfigDialog(this, i18n("Settings"), Settings::self());
    QWidget *generalSettingsDlg = new QWidget;
    ui_prefs_base.setupUi(generalSettingsDlg);
    //ui settings
    connect(ui_prefs_base.selectPath, SIGNAL(released()), this,SLOT(choosePath()));
    connect(this, SIGNAL(valueChanged(QString)), ui_prefs_base.kcfg_kmagnetDataPath, SLOT(setText(QString)));
    dialog->addPage(generalSettingsDlg, i18n("General"), "games-config-options");
    dialog->addPage( new KGameThemeSelector( dialog, Settings::self(), KGameThemeSelector::NewStuffDisableDownload ), i18n( "Theme" ), "games-config-theme" );
    connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(settingsChanged()));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

void kmagnet::load()
{
    QString path = Settings::kmagnetDataPath();
    if (path.isEmpty())//don't need it anymore
      choosePath();
    if (!QDir(path).exists())
      path = QDir::home().path() + "/";
    QString loadFilename = KFileDialog::getOpenFileName (KUrl(path + "/"),
                           "*.kmp", this, i18n("Load Puzzle"));
    if (loadFilename.isNull()) {
        return;
    }
    loadfile(loadFilename);
}

void kmagnet::loadfile(QString loadFilename)
{
    QFile file( loadFilename);
    if (!file.exists())
    {
        KMessageBox::information (this,
                                  i18n("Sorry, The file %1 doesn't exist").arg(loadFilename),
                                  i18n("File doesn't exist"));
        return;
    }
    KConfig config (loadFilename, KConfig::SimpleConfig);

    if (! config.hasGroup ("kmagnet")) {
        KMessageBox::information (this,
                                  i18n("Sorry, This is not a valid kMagnet Puzzle file"),
                                  i18n("File Not Valid"));
        return;
    }

    KConfigGroup configGroup = config.group ("kmagnet");

    QStringList list;
    QStringList notFound;
    list = configGroup.readEntry ("difficulty", notFound);
    if (list.size()==1) {
        KGameDifficulty::standardLevel level = static_cast<KGameDifficulty::standardLevel>(list.at(0).toInt());
        KGameDifficulty::setLevel(level);
    }
    newGame();
    QAction * editingModeAction = this->action("editmode");
    if (editingModeAction->isChecked())
        editingModeAction->activate(QAction::Trigger);
    list.clear();
    list = configGroup.readEntry ("movements", notFound);
    if (list.size()==1) {
        int movs=list.at(0).toInt();
        m_scene->setMovements(movs);
        advanceMovements(movs);
    }
    list.clear();
    list = configGroup.readEntry ("time", notFound);
    if (list.size()==1) {
        m_gameClock->setTime(list.at(0).toInt());
    }
    list.clear();
    list = configGroup.readEntry ("currentposition", notFound);
    if (list.size()==1)
        m_scene->setBallPos(list.at(0).toUInt());
    list.clear();
    list = configGroup.readEntry ("startposition", notFound);
    if (list.size()==1)
        m_scene->setStartPosition(list.at(0).toUInt());
    list.clear();
    list = configGroup.readEntry ("final", notFound);
    if (list.size()!=0)
    {
        for (int i=0; i < list.size(); i++)
        {
            m_scene->setFinalPosition(list.at(i).toUInt());
        }
    }
    list.clear();
    list = configGroup.readEntry ("notfree", notFound);
    if (list.size()!=0)
    {
        for (int i=0; i < list.size(); i++)
        {
            m_scene->setNotFreePosition(list.at(i).toUInt());
        }
    }
    m_scene->update();
    m_scene->setFocus(Qt::NoFocusReason);
}

void kmagnet::advanceTime(const QString& timeStr)
{
    statusBar()->changeItem( i18n("Time: %1", timeStr), 0 );
}

void kmagnet::advanceMovements(int movement)
{
    statusBar()->changeItem( i18n("Movements: %1", movement), 1 );
}

void kmagnet::gameOver(bool won)
{
    m_gameClock->pause();
    if (won) {
        KScoreDialog scoreDialog(KScoreDialog::Name | KScoreDialog::Time, this);
        scoreDialog.setConfigGroup(KGameDifficulty::localizedLevelString());
        //scoreDialog.setConfigGroupWeights(KGameDifficulty::levelWeights());//KDE4.2
        QPair<QByteArray, QString> group = KGameDifficulty::localizedLevelString();
        scoreDialog.setConfigGroup( group );
        KScoreDialog::FieldInfo scoreInfo;
        scoreInfo[KScoreDialog::Score].setNum(1000 - m_scene->getMovements());
        //score-as-time will be shown
        scoreInfo[KScoreDialog::Time] = m_gameClock->timeString();

        // we keep highscores as number of seconds
        if ( scoreDialog.addScore(scoreInfo, KScoreDialog::LessIsMore) != 0 )
            scoreDialog.exec();
    }
    else
    {
        int answer=KMessageBox::questionYesNo(this, i18n("Thanks for playing. Do you want to try again?"),i18n("Game ended"));
        if (answer==KMessageBox::Yes)
            restart();
    }
        KGameDifficulty::setRunning(false);
}

void kmagnet::editingMode(bool b)
{
    KGameDifficulty::setRunning(false);
    m_scene->toogleEditorMode(b);
    m_gameClock->restart();
    (b) ? m_gameClock->pause():m_gameClock->resume();
}

void kmagnet::save()
{
    QString path = Settings::kmagnetDataPath();
    if (path.isEmpty())
      choosePath();
    KUrl startDir = KUrl(path + "/puzzle-" + QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss") + ".kmp");
    QString newFilename = KFileDialog::getSaveFileName ( startDir,
                          "*.kmp", this, i18n("Save Puzzle"));
    if (newFilename.isNull()) {
        return;
    }
    KConfig config (newFilename, KConfig::SimpleConfig);
    if (!config.isConfigWritable(true))
      return;
    KConfigGroup configGroup = config.group("kmagnet");
    QStringList list;
    QString    value;

    value.sprintf ("%d",KGameDifficulty::level() );
    list.append (value);
    configGroup.writeEntry ("difficulty", list);
    list.clear();
    if (m_scene->getEditorMode())
        value.sprintf ("%d",0 );
    else
        value.sprintf ("%d",m_scene->getMovements() );
    list.append (value);
    configGroup.writeEntry ("movements", list);
    list.clear();
    if (m_scene->getEditorMode())
        value.sprintf ("%d",0 );
    else
        value.sprintf ("%d",m_gameClock->seconds());
    list.append (value);
    configGroup.writeEntry ("time", list);
    list.clear();

    uint p =m_scene->getCurrentCell();
    value.sprintf ("%u",p );
    list.append (value);
    configGroup.writeEntry ("currentposition", list);
    list.clear();

    p =m_scene->getStartPosition();
    value.sprintf ("%u",p );
    list.append (value);
    configGroup.writeEntry ("startposition", list);
    list.clear();

    QStringList list2;
    QString     value2;

    for (uint i=0; i < m_scene->getNumCells(); i++)
    {
        kmagnetCell * item = m_scene->getCell(i);
        if (item->getIsFinal())
        {
            value.sprintf ("%u",i );
            list.append (value);
        }
        else if (!item->getIsFree())
        {
            value2.sprintf ("%u",i );
            list2.append (value2);
        }
    }
    configGroup.writeEntry ("final", list);
    configGroup.writeEntry ("notfree", list2);
    configGroup.sync();
}

void kmagnet::restart()
{
    m_gameClock->restart();
    m_gameClock->resume();
    (m_scene->getEditorMode()) ? m_gameClock->pause():m_gameClock->resume();
    QAction * pauseAction = this->action("game_pause");
    if (pauseAction->isChecked())
        pauseAction->activate(KAction::Trigger);
    advanceMovements(0);
    m_scene->restart();
    this->action("move_solve")->setEnabled(true);
    KGameDifficulty::setRunning(true);
}

void kmagnet::pause(bool b)
{
    (b) ? m_scene->setForegroundBrush(QBrush(QColor(25, 12, 0, 220),Qt::SolidPattern)) : m_scene->setForegroundBrush(QBrush());
    (b) ? m_gameClock->pause(): m_gameClock->resume();
}

void kmagnet::levelChanged(KGameDifficulty::standardLevel level)
{
    if (level==KGameDifficulty::Easy)
    {
        m_scene->setSize(15,10);
    }
    else if (level==KGameDifficulty::Medium)
    {
        m_scene->setSize(20,15);
    }
    else if (level==KGameDifficulty::Hard)
    {
        m_scene->setSize(25,20);
    }
    newGame();
    this->setFocus();
}

void kmagnet::keyReleaseEvent ( QKeyEvent * keyEvent)
{
    switch ( keyEvent->key() )
    {
    case Qt::Key_Down:
        m_scene->process(Moves::DOWN);
        break;
    case Qt::Key_Up:
        m_scene->process(Moves::UP);
        break;
    case Qt::Key_Left:
        m_scene->process(Moves::LEFT);
        break;
    case Qt::Key_Right:
        m_scene->process(Moves::RIGHT);
        break;
    }
}

void kmagnet::solveFunc()
{
    QAction * pauseAction = this->action("game_pause");
    if (pauseAction->isChecked())
        pauseAction->activate(KAction::Trigger);
    this->action("game_restart")->setEnabled(false);
    this->action("move_solve")->setEnabled(false);
    m_solver->findSolution();
    KGameDifficulty::setRunning(false);
}

void kmagnet::solutionFound()
{
//convert data to vector
    m_scene->setHasLost(true);
    m_gameClock->restart();
    m_gameClock->pause();
    advanceMovements(0);
    std::vector<Moves::Move> lm=m_solver->getSolution();
    if (lm.size()!=0)
    {
        //DEBUG CODE
        for (unsigned int i=0; i< lm.size(); i++) {
            //	System.out.println(lm.get(i));
            QString str=QString("");
            switch (lm.at(i))
            {
            case 0:
                str= "UP";
                break;
            case 1:
                str= "DOWN";
                break;
            case 2:
                str= "LEFT";
                break;
            case 3:
                str= "RIGHT";
                break;
            default:
                str="MOONMOVE";
                break;
            }
            qDebug() << str;
        }
        //FI_DEBUG_CODE
        QVector<Moves::Move> lm2= QVector<Moves::Move>::fromStdVector(lm);
        m_scene->replay(lm2);
    }
    else
    {
        this->action("move_solve")->setEnabled(true);
        this->action("game_restart")->setEnabled(true);
    }
}

void kmagnet::settingsChanged()
{
    if ( !kmagnetRenderer::self()->loadTheme(Settings::theme()) )
    {
        KMessageBox::error( this,  i18n( "Failed to load \"%1\" theme. Please check your installation.", Settings::theme() ) );
        return;
    }

    m_view->resetCachedContent();
    // trigger complete redraw
    m_scene->resizeScene( (int)m_scene->sceneRect().width(),
                          (int)m_scene->sceneRect().height() );
    //save changes on disk
    Settings::self()->writeConfig();
}

void kmagnet::choosePath()
{
    KUrl dirUrl = KDirSelectDialog::selectDirectory(KUrl(QDir::homePath()), false,
                  parentWidget(),
                  i18n("Choose where to load/save puzzles from/to"));
    if (dirUrl.isValid() )
    {
	 dirUrl.adjustPath(KUrl::AddTrailingSlash);
         emit  valueChanged(dirUrl.directory(KUrl::ObeyTrailingSlash));
    }
}

void kmagnet::fillPuzzleList  (const PuzzleItem itemList [], QList<QAction*> &list,
			const char *uilist, const char *slot)
{//part of the code similar to kubrick
    //Add random action
    KAction * t = new KAction (i18n ("Random predefined"), this);
    actionCollection()->addAction("random", t);
    connect(t, SIGNAL(triggered()), SLOT(playRandomPuzzle()));
    list.append(t);
    // Generate an action list with one action for each item in the list.
    for (uint i = 0; (strcmp (itemList[i].filename, "END") != 0); i++) 
    {
        KAction * t = new KAction (i18n (itemList[i].menuText), this);
        actionCollection()->addAction (QString ("%1%2").arg(uilist).arg(i), t);
        t->setData (i);		// Save the index of the item inside the action.
        list.append (t);
        connect (t, SIGNAL (triggered()), slot);
        puzzleAction->addAction(t);//put the actions in the menu
    }

    // Plug the action list into the Puzzles menu.
    plugActionList (uilist, list);
}

void kmagnet::puzzleSelected()
{
//part of the code similar to kubrick
    const KAction * action = static_cast <const KAction *> (sender());
    int index = action->data().toInt();

    if (index >= 0)
    {
      loadPredefinedPuzzle(puzzles[index].filename);
    }
}

void kmagnet::playRandomPuzzle()
{
  int high=3;//FIXME
  int low=1;
  int random= qrand() % ((high + 1) - low) + low;
  loadPredefinedPuzzle ("puzzle" + QString::number(random) + ".kmp");
}

void kmagnet::loadPredefinedPuzzle(QString name)
{
   QString path;
  QStringList dataDir = KStandardDirs().findDirs("data", "kMagnet/data/");
    if (!dataDir.isEmpty())
	  path=dataDir.first();
  loadfile(path + name);
}

void kmagnet::getHotNewStuff()
{
  qDebug() << KGlobal::activeComponent().componentName();
  KNS::Entry::List entries = KNS::Engine::download();
    // list of changed entries
     // qDebug() << QDir::home().path() + "/";
    foreach(KNS::Entry* entry, entries) {
     /*  QString path= QDir::home().path() + "/" + ".kMagnet";
      QDir dir=QDir(path);
      if (!dir.exists()) dir.mkdir(path);*/
	//  qDebug() << entry->idNumber();
        // care only about installed ones
        if (entry->status() == KNS::Entry::Installed) {
            // do something with the installed entries
	    //could put an actionlistmenu and update it here
	  //  qDebug() << entry->idNumber();
	  //  qDebug() << entry->installedFiles();
            }
        }
    
    qDeleteAll(entries);
 }

#include "kmagnet.moc"
