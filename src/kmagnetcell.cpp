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


#include "kmagnetcell.h"
#include <kstandarddirs.h>

#include <QPainter>
#include <QGraphicsScene>

kmagnetcell::kmagnetcell(QGraphicsItem * parent, QGraphicsScene * scene)
        : QGraphicsItem(parent, scene),
        isfree(true),
        isfinal(false),
        visited(false),
        myscene(scene)
{
    cache =((kmagnetScene *) myscene)->getcache();
}

QRectF kmagnetcell::boundingRect() const
{
    return QRectF(0 , 0, 20, 20);
}

void kmagnetcell::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPixmap pixmap;
    if (isfinal)
    {
        if (!(cache->find("final", pixmap)))
        {
            pixmap=QPixmap(KStandardDirs::locate("appdata", "images/final.png"));
        }
        painter->drawPixmap(0,0,pixmap);
    }
    else if (!isfree)
    {
        if (!(cache->find("notfree", pixmap)))
        {
            pixmap=QPixmap(KStandardDirs::locate("appdata", "images/notfree.png"));
        }
        painter->drawPixmap(0,0,pixmap);
    }
    else if (isfree && !isfinal)
    {
        if (!(cache->find("free", pixmap)))
        {
            pixmap=QPixmap(KStandardDirs::locate("appdata", "images/free.png"));
        }
        painter->drawPixmap(0,0,pixmap);
    }
}

bool kmagnetcell::getisfree()
{
    return isfree;
}

bool kmagnetcell::getisfinal()
{
    return isfinal;
}

bool kmagnetcell::getvisited()
{
    return visited;
}

void kmagnetcell::setisfree(bool b)
{
    this->isfree=b;
    this->update();
}

void kmagnetcell::setisfinal(bool b)
{
    this->isfinal=b;
    this->update();
}

void kmagnetcell::setvisited(bool b)
{
    this->visited=b;
}

void kmagnetcell::reset()
{
    this->visited=false;
    this->isfinal=false;
    this->isfree=true;
    this->update();
}
