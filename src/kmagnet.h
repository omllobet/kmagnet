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

#include "kmagnetscene.h"
#include "kmagnetview.h"

class KToggleAction;

/**
 * This class serves as the main window for kmagnet.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Oscar Martinez omllobet@gmail.com
 * @version 0.01
 */
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
    void save();
    void restart();
    void pause(bool b);
    void levelChanged(KGameDifficulty::standardLevel);

private:
    void setupActions();

private:

    int ROWS;
    int COLUMNS;
    int moves;
    kmagnetView *m_view;
    QPrinter   *m_printer;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
    KGameClock* m_gameClock;
    kmagnetScene *m_scene;
};

#endif // _KMAGNET_H_
