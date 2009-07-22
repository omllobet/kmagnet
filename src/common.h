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

#ifndef COMMON_H
#define COMMON_H
#include <QPoint>

namespace Moves
{
enum Move { UP, DOWN,LEFT,RIGHT };
};

namespace Global
{
extern int itemSize;
};

class nextMove
{
private:
    bool isPossible;
    QPoint destination;

public:
    nextMove(bool possible, QPoint point)
    {
        isPossible=possible;
        destination=point;
    };

    bool getIsPossible()
    {
        return isPossible;
    };

    QPoint getPosition()
    {
        return destination;
    };
};

#endif // MOVES_H
