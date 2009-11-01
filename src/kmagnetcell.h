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

#ifndef KMAGNETCELL_H
#define KMAGNETCELL_H

#include "kmagnetscene.h"

#include "common.h"

#include <QGraphicsItem>

class kmagnetCell : public QGraphicsItem
{
public:
    explicit kmagnetCell(QGraphicsItem* parent=0, QGraphicsScene * scene=0);

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool getIsFree();
    bool getIsFinal();
    bool getVisited();

    void setIsFree(bool b);
    void setIsFinal(bool b);
    void setVisited(bool b);

    void reset();

// enable use of qgraphicsitem_cast
    enum { Type = UserType + 1 };
    virtual int type() const {
        return Type;
    }

private:
    bool isFree;
    bool isFinal;
    bool visited;//not useful yet

    int x_pos;
    int y_pos;
    int corx;
    int cory;

    QGraphicsScene* myScene;
    QPixmapCache* cache;
};

#endif // KMAGNETCELL_H
