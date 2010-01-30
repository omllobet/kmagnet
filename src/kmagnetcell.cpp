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
#include "kmagnetrenderer.h"

#include <KStandardDirs>

#include <QPainter>

kmagnetCell::kmagnetCell(QGraphicsItem * parent, QGraphicsScene * scene)
        : QGraphicsItem(parent, scene),
        isFree(true),
        isFinal(false),
        visited(false)
{
}

QRectF kmagnetCell::boundingRect() const
{
    return QRectF(0 , 0, Global::itemSize, Global::itemSize);
}

void kmagnetCell::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPixmap pixmap;
    if (isFinal)
    {
          pixmap=kmagnetRenderer::self()->pixmapForFinalCell();
    }
    else if (!isFree)
    {
            pixmap=kmagnetRenderer::self()->pixmapForNonFreeCell();
    }
    else if (isFree && !isFinal)
    {
            pixmap=kmagnetRenderer::self()->pixmapForFreeCell();
    }
    painter->drawPixmap(0,0,pixmap);
}

bool kmagnetCell::getIsFree()
{
    return isFree;
}

bool kmagnetCell::getIsFinal()
{
    return isFinal;
}

bool kmagnetCell::getVisited()
{
    return visited;
}

void kmagnetCell::setIsFree(bool b)
{
    this->isFree=b;
    if (!b) this->isFinal=false;
    this->update();
}

void kmagnetCell::setIsFinal(bool b)
{
    this->isFinal=b;
    if (b) this->isFree=true;
    this->update();
}

void kmagnetCell::setVisited(bool b)
{
    this->visited=b;
    //this->update();
}

void kmagnetCell::reset()
{
    this->visited=false;
    this->isFinal=false;
    this->isFree=true;
    this->update();
}
