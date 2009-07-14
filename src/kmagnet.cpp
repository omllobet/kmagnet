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

#include "kmagnet.h"
#include "settings.h"
#include "common.h"
#include "kmagnetcell.h"
#include <KGameDifficulty>

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

    m_view->setFixedSize(COLUMNS*20, ROWS*20);
    m_scene = new kmagnetScene(m_view, ROWS, COLUMNS);
    connect(m_scene, SIGNAL(advanceMovements(int)), this, SLOT(advanceMovements(int)));
    connect(m_scene, SIGNAL(itsover(bool)), this, SLOT(gameOver(bool)));
    m_view->setScene(m_scene);
    connect(m_view,SIGNAL(resizeScene(int , int )),m_scene,SLOT(resizeScene(int , int )));
    QWidget *contenidor = new QWidget(this);

    QHBoxLayout * gl = new QHBoxLayout();
    gl->addSpacing(0);
    gl->addWidget(m_view);
    gl->addSpacing(0);
    contenidor->setLayout(gl);

    m_printer=0;
    m_gameClock = new KGameClock(this, KGameClock::MinSecOnly);
    connect(m_gameClock, SIGNAL(timeChanged(const QString&)), SLOT(advanceTime(const QString&)));

    // accept dnd
    //setAcceptDrops(true);

    setCentralWidget(contenidor);
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
    this->grabKeyboard();
}

kmagnet::~kmagnet()
{
    delete m_gameClock;
}

void kmagnet::newGame()
{
    this->grabKeyboard();
    m_gameClock->restart();
    statusBar()->changeItem( i18n("Time: 00:00"), 0);
    advanceMovements(0);
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
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    KStandardAction::preferences( this, SLOT( configureSettings() ), actionCollection() );
    KStandardGameAction::pause( this, SLOT( pause(bool ) ), actionCollection() );
    KGameDifficulty::init(this,this, SLOT(levelChanged(KGameDifficulty::standardLevel)));
    KGameDifficulty::setRestartOnChange(KGameDifficulty::RestartOnChange);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
    KGameDifficulty::setLevel(KGameDifficulty::Hard);
    KAction *editModeAction= new KAction(i18n("Edit Mode"),this);
    editModeAction->setCheckable(true);
    editModeAction->setShortcut(Qt::CTRL + Qt::Key_T);
    actionCollection()->addAction("editmode", editModeAction);
    connect( editModeAction, SIGNAL( triggered(bool) ),this, SLOT( editingMode(bool) ) );
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
    //ui_prefs_base.setupUi(generalSettingsDlg);
    dialog->addPage(generalSettingsDlg, i18n("General"), "Game_setting");
//    connect(dialog, SIGNAL(settingsChanged(QString)), m_view, SLOT(settingsChanged()));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}
void kmagnet::load()
{
    newGame();
    QString loadFilename = KFileDialog::getOpenFileName (KUrl(KGlobal::dirs()->findResourceDir("appdata", "") + "data"),
                           "*.kmp", this, i18n("Load Puzzle"));
    if (loadFilename.isNull()) {
        return;
    }

    KConfig config (loadFilename, KConfig::SimpleConfig);

    if (! config.hasGroup ("kmagnet")) {
        KMessageBox::information (this,
                                  i18n("Sorry, This is not a valid KMagnet Puzzle"),
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
        emit levelChanged(level);
    }
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
    if (list.size()==2)
        m_scene->setBallPos(QPoint(list.at(0).toInt(), list.at(1).toInt()));
    list.clear();
    list = configGroup.readEntry ("startposition", notFound);
    if (list.size()==2)
        m_scene->setStartPosition(QPoint(list.at(0).toInt(), list.at(1).toInt()));
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
    this->releaseKeyboard();
    m_gameClock->pause();
    if (won) {
        KScoreDialog scoreDialog(KScoreDialog::Name | KScoreDialog::Time, this);
        scoreDialog.setConfigGroup(KGameDifficulty::localizedLevelString());
        //scoreDialog.setConfigGroupWeights(KGameDifficulty::levelWeights());
        QPair<QByteArray, QString> group = KGameDifficulty::localizedLevelString();
        scoreDialog.setConfigGroup( group );
        KScoreDialog::FieldInfo scoreInfo;
        scoreInfo[KScoreDialog::Score].setNum(1000- m_scene->getMovements());
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
        else
            newGame();
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

    QString newFilename = KFileDialog::getSaveFileName (KUrl(),
                          "*.kmp", this, i18n("Save Puzzle"));
    if (newFilename.isNull()) {
        return;
    }
    KConfig config (newFilename, KConfig::SimpleConfig);
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

    value.sprintf ("%d",m_gameClock->seconds());
    list.append (value);
    configGroup.writeEntry ("time", list);
    list.clear();

    QPointF p =m_scene->getBallPos();
    value.sprintf ("%d",(int)p.x() );
    list.append (value);
    value.sprintf ("%d",(int)p.y() );
    list.append (value);

    configGroup.writeEntry ("currentposition", list);

    list.clear();
    if (!m_scene->getEditorMode())
        p =m_scene->getStartPosition();
    value.sprintf ("%d",(int)p.x() );
    list.append (value);
    value.sprintf ("%d",(int)p.y() );
    list.append (value);
    configGroup.writeEntry ("startposition", list);
    list.clear();

    QStringList list2;
    QString     value2;
    QList<QGraphicsItem *>  ci =m_scene->items();
    for (int i=0; i < ci.size(); i++)
    {
        kmagnetCell * item = (kmagnetCell *)ci.at(i);
        if (item->getIsFinal())
        {
            QPointF p =item->pos();
            value.sprintf ("%d",(int)p.x() );
            list.append (value);
            value.sprintf ("%d",(int)p.y() );
            list.append (value);
        }
        else if (!item->getIsFree())
        {
            QPointF p =item->pos();
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
    advanceMovements(0);
    m_scene->restart();
    this->grabKeyboard();
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
        m_view->setFixedSize(5*20,7*20);
        m_scene->setSize(7,5);
    }
    else if (level==KGameDifficulty::Medium)
    {
        m_view->setFixedSize(10*20,14*20);
        m_scene->setSize(14,10);
    }
    else if (level==KGameDifficulty::Hard)
    {
        m_view->setFixedSize(20*20,25*20);
        m_scene->setSize(25,20);
    }
    //+4 for borders
    int theight=0;
    QList<KToolBar*> tlist = toolBars();
    for (int i=0;i< tlist.size(); i++)
    {
        theight=theight+ dynamic_cast<KToolBar*>(tlist.at(i))->height();
    }
    this->setMinimumSize(std::max(m_view->width()+4, std::max(menuBar()->width()+4, std::max(statusBar()->width()+4, toolBar()->width()+4))), m_view->height()+ statusBar()->height() + menuBar()->height()+theight+4);
    resize(this->minimumSize());

    emit newGame();
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

#include "kmagnet.moc"
