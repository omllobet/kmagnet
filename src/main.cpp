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

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KDE/KLocale>
#include <KUrl>
#include <KStandardDirs>

#include "kmagnet.h"

static const char description[] =
    I18N_NOOP("A KDE 4 Puzzle-like Game");

static const char version[] = "0.01beta";

int main(int argc, char **argv)
{
    KAboutData about("kmagnet", 0, ki18n("kmagnet"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2007 Oscar Martinez"), KLocalizedString(), 0, "omllobet@gmail.com");
    about.addAuthor( ki18n("Oscar Martinez"), KLocalizedString(), "omllobet@gmail.com" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[file]", ki18n( "Puzzle to open" ));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    KGlobal::locale()->insertCatalog("libkdegames");
    kmagnet *widget = new kmagnet;
    
    // see if we are starting with session management
    if (app.isSessionRestored())
    {
        RESTORE(kmagnet);
    }
    else
    {
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0)
        {
            widget->show();
        }
        else
        {
	  if (args->count() == 1)
	  {
	    QString path= args->arg(0);
	    KUrl url=KUrl(path);
	    if (url.isRelative())
	    {
		path.prepend(QUrl(KGlobal::dirs()->findResourceDir("appdata", "")).toString(QUrl::RemoveScheme) + "data/");
	    }
	    widget->loadfile(path);
	    widget->show();
	  }
	  else
	  {
	    widget->show();
	  }
	}
    }
    return app.exec();
}
