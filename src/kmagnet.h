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

#ifndef KMAGNET_H
#define KMAGNET_H

#include <KXmlGuiWindow>
#include <KGameClock>
#include <KGameDifficulty>
#include <KActionMenu>

#include "kmagnetscene.h"
#include "kmagnetview.h"
#include "kmagnetsolver.h"
#include "kmagnetgenerator.h"
#include "ui_prefs_base.h"

class kmagnetScene;
class kmagnetSolver;
class kmagnetGenerator;
class KToggleAction;

/**
 * This class serves as the main window for kmagnet.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Oscar Martinez omllobet@gmail.com
 */
using namespace std;

class kmagnet : public KXmlGuiWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    kmagnet();

    /**
     * Default Destructor
     */
    virtual ~kmagnet();
    virtual void keyReleaseEvent ( QKeyEvent * keyEvent);
    void calculateMinimiumSize();
    kmagnetScene* get_scene() {
        return this->m_scene;
    };

public slots:
    void loadfile(QString loadFilename);

private slots:
    void newGame();
    void showHighscores();
    void configureSettings();
    void load();
    void advanceTime(const QString& timeStr);
    void advanceMovements(int movement);
    void gameOver(bool won);
    void editingMode(bool b);
    void clearBoard(bool b);
    void save();
    void solveFunc();
    void restart();
    void pause(bool b);
    void levelChanged(KGameDifficulty::standardLevel);
    void solutionFound(QVectorMoves);
    void settingsChanged();
    void choosePath();
    void puzzleSelected();
    void playRandomPuzzle();
    void getHotNewStuff();

signals:
    void valueChanged(QString newValue);

private:
    void setupActions();
    void generate();
    
private:

    int ROWS;
    int COLUMNS;
    int moves;
    kmagnetView *m_view;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
    KActionMenu *puzzleAction;
    KGameClock *m_gameClock;
    kmagnetScene *m_scene;
    kmagnetGenerator *m_generator;
    Ui::prefs_base ui_prefs_base;

    struct PuzzleItem {
        const char * filename;      // File containing demo or "END".
        const char * menuText;      // Description of the pattern or moves.
    };
    static const PuzzleItem puzzles [];

    QList<QAction*> puzzlesList;

    void fillPuzzleList  (const PuzzleItem itemList [], QList<QAction*> &list,
                          const char *uilist, const char *slot);
    void loadPredefinedPuzzle(QString name);
};

#endif // _KMAGNET_H_
