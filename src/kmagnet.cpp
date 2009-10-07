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

#include <QHBoxLayout>

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
#include <KGameDifficulty>
#include <KDirSelectDialog>

#include "kmagnet.h"
#include "settings.h"
#include "common.h"
#include "kmagnetcell.h"
#include "kmagnetsolver.h"

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
                                  QGraphicsView::DontSavePainterState |
                                  QGraphicsView::DontAdjustForAntialiasing );

    //m_view->setFixedSize(COLUMNS*Global::itemSize, ROWS*Global::itemSize);
    m_scene = new kmagnetScene(this, ROWS, COLUMNS);
    m_scene->setBackgroundBrush(Qt::lightGray);
    
    connect(m_scene, SIGNAL(advanceMovements(int)), this, SLOT(advanceMovements(int)));
    connect(m_scene, SIGNAL(itsover(bool)), this, SLOT(gameOver(bool)));
    m_view->setScene(m_scene);
   //TODO tema size// connect(this, SIGNAL(resizeScene(int, int )), m_view, SLOT(resizeScene(int, int)));
    connect(m_view,SIGNAL(resizeScene(int , int )),m_scene,SLOT(resizeScene(int , int )));

    //solver
    m_solver= new kmagnetSolver(m_scene);
    connect(m_solver, SIGNAL(finished()),this,SLOT(solutionFound()));
    //QWidget *contenidor = new QWidget(this);
    
    //QHBoxLayout * gl = new QHBoxLayout();
    //gl->addSpacing(0);
    //gl->addWidget(m_view);
    //gl->addSpacing(0);
    //contenidor->setLayout(gl);

    m_gameClock = new KGameClock(this, KGameClock::MinSecOnly);
    connect(m_gameClock, SIGNAL(timeChanged(const QString&)), SLOT(advanceTime(const QString&)));

    // accept dnd
    //setAcceptDrops(true);
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
    setFocus();
}

kmagnet::~kmagnet()
{
    delete m_gameClock;
    delete m_solver;
    delete m_scene;
    delete m_view;
}

void kmagnet::newGame()
{
    //adjustSize();
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
    KStandardAction::preferences( this, SLOT( configureSettings() ), actionCollection() );
    KStandardGameAction::pause( this, SLOT( pause(bool ) ), actionCollection() );
    KGameDifficulty::init(this,this, SLOT(levelChanged(KGameDifficulty::standardLevel)));
    KGameDifficulty::setRestartOnChange(KGameDifficulty::RestartOnChange);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
    //KGameDifficulty::setLevel(KGameDifficulty::Hard);
    KAction *editModeAction= new KAction(i18n("Editor Mode"),this);
    editModeAction->setCheckable(true);
    editModeAction->setShortcut(Qt::CTRL + Qt::Key_T);
    actionCollection()->addAction("editmode", editModeAction);
    connect( editModeAction, SIGNAL( triggered(bool) ),this, SLOT( editingMode(bool) ) );
    editModeAction->trigger();
    KGameDifficulty::setLevel(KGameDifficulty::Hard);
}

void kmagnet::configureSettings()
{
    // The preference dialog is derived from prefs_base.ui
    //
    // compare the names of the widgets in the .ui file
    // to the names of the variables in the .kcfg file
    //avoid to have 2 dialogs shown
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }
    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
    QWidget *generalSettingsDlg = new QWidget;
    ui_prefs_base.setupUi(generalSettingsDlg);
//ui settings
    connect(ui_prefs_base.selectPath, SIGNAL(released()), this,SLOT(choosePath()));
    connect(this, SIGNAL(valueChanged(QString)), ui_prefs_base.kcfg_kmagnetDataPath, SLOT(setText(QString)));
    dialog->addPage(generalSettingsDlg, i18n("General"), "games-config-options");
    connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(settingsChanged()));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

void kmagnet::load()
{
    QString path = Settings::kmagnetDataPath();
    if (path.isEmpty())
    {
	QStringList dataDir = KStandardDirs().findDirs("data", "kmagnet/data/");
	if (!dataDir.isEmpty())
	    path.prepend(dataDir.first());
    }
    QString loadFilename = KFileDialog::getOpenFileName (KUrl(path),
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
                                  i18n("Sorry, This is not a valid KMagnet Puzzle File"),
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
        //emit levelChanged(level);
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
    if (list.size()==2)//FIXME
        m_scene->setBallPos(list.at(0).toInt());
    list.clear();
    list = configGroup.readEntry ("startposition", notFound);
    if (list.size()==2)//FIXME
        m_scene->setStartPosition(list.at(0).toInt());
    list.clear();
    list = configGroup.readEntry ("final", notFound);
    if (list.size()%2==0)
    {
        for (int i=0; i < list.size(); i=i+2)
        {
            m_scene->setFinalPosition(QPoint(list.at(i).toInt(), list.at(i+1).toInt()));
        }
    }
    list.clear();
    list = configGroup.readEntry ("notfree", notFound);
    if (list.size()%2==0)
    {
        for (int i=0; i < list.size(); i=i+2)
        {
            m_scene->setNotFreePosition(QPoint(list.at(i).toInt(), list.at(i+1).toInt()));
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
        //else
        //newGame();
    }
}

void kmagnet::editingMode(bool b)
{
    m_scene->toogleEditorMode(b);
    m_gameClock->restart();
    (b) ? m_gameClock->pause():m_gameClock->resume();
}

void kmagnet::save()
{
//TODO better safe cell num and use the vector
//need function to get cell from position
  QString path = Settings::kmagnetDataPath();
    QString newFilename = KFileDialog::getSaveFileName (KUrl(path),
                          "*.kmp", this, i18n("Save Puzzle"));
    if (newFilename.isNull()) {
        return;
    }
    KConfig config (newFilename, KConfig::SimpleConfig);
    if (config.isConfigWritable(true))
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

    //QPointF p =m_scene->getBallPos();
    QPoint p =m_scene->getCurrentPosition();
    value.sprintf ("%d",(int)p.x() );
    list.append (value);
    value.sprintf ("%d",(int)p.y() );
    list.append (value);

    configGroup.writeEntry ("currentposition", list);

    list.clear();
    //if (!m_scene->getEditorMode())
    p =m_scene->getStartPosition();
    value.sprintf ("%d",(int)p.x() );
    list.append (value);
    value.sprintf ("%d",(int)p.y() );
    list.append (value);
    configGroup.writeEntry ("startposition", list);
    list.clear();

    QStringList list2;
    QString     value2;
    QList<QGraphicsItem *>  ci =m_scene->items();//Use m_cells?
    for (int i=0; i < ci.size(); i++)
    {
        QGraphicsItem* graphicItem=ci.at(i);
        if (graphicItem->zValue()==5.0) continue;//FIXME //ball is the only item at z=5.0
        kmagnetCell * item = dynamic_cast<kmagnetCell *>(graphicItem);
        QPointF p =item->pos();
        if (item->getIsFinal())
        {
            value.sprintf ("%d",(int)p.x() );
            list.append (value);
            value.sprintf ("%d",(int)p.y() );
            list.append (value);
        }
        else if (!item->getIsFree())
        {
            value2.sprintf ("%d",(int)p.x() );
            list2.append (value2);
            value2.sprintf ("%d",(int)p.y() );
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
        //m_view->setFixedSize(10*Global::itemSize,15*Global::itemSize);
        m_scene->setSize(15,10);
    }
    else if (level==KGameDifficulty::Medium)
    {
        //m_view->setFixedSize(15*Global::itemSize,20*Global::itemSize);
        m_scene->setSize(20,15);
    }
    else if (level==KGameDifficulty::Hard)
    {
        //m_view->setFixedSize(20*Global::itemSize,25*Global::itemSize);
        m_scene->setSize(25,20);
    }
    emit newGame();
}

/*void kmagnet::calculateMinimiumSize()
{
//+4 for borders
    int theight= toolBar("mainToolBar")->height();// if theres no toolbar (hidden)will this crash?
    QList<KToolBar*> tlist = toolBars();
    for (int i=1;i< tlist.size(); i++)
    {
        theight=theight+ dynamic_cast<KToolBar*>(tlist.at(i))->height();
    }
    //this->setMinimumSize(std::max(m_view->width(), std::max(menuBar()->width(), std::max(statusBar()->width(), toolBar()->width()))), m_view->height()+ statusBar()->height() + menuBar()->height()+theight+4+28);
//    adjustSize();
    this->setMinimumSize(m_view->width()+2,std::max(m_view->height()+ statusBar()->height() + menuBar()->height()+theight+4, this->height()));
    //qDebug() << "theight" << theight << "mview" << m_view->height() << "statusbar" << statusBar()->height() << " menubar" << menuBar()->height();
    //resize(this->minimumSize());
    
    resize(this->width(), this->minimumHeight());
    
    //setMinimumSize(size());
    //resize(minimumSize());
}*/

void kmagnet::keyReleaseEvent ( QKeyEvent * keyEvent)
{
    //if (m_scene->getEditorMode()) return;
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
    //if (m_scene->gameIsLost() || m_scene->gameIsWon()) restart();
    QAction * pauseAction = this->action("game_pause");
    if (pauseAction->isChecked())
        pauseAction->activate(KAction::Trigger);
    this->action("game_restart")->setEnabled(false);
    this->action("move_solve")->setEnabled(false);
    m_solver->findSolution();
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
        for (unsigned int i=0; i< lm.size(); i++) {
            //	System.out.println(lm.get(i));
            QString str=QString();
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
    //qDebug() << "max calls" << Settings::maxCalls();
    Settings::self()->writeConfig();
}

void kmagnet::choosePath()
{
    KUrl dirUrl = KDirSelectDialog::selectDirectory(KUrl(""), false,
                                               parentWidget(),
                                               i18n("Save Puzzles To"));
    if (dirUrl.isValid() ) 
      {
        emit  valueChanged(dirUrl.prettyUrl());
      }
}

#include "kmagnet.moc"
