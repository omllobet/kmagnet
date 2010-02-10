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

#include "kmagnetview.h"
/*This class represents the view for the scene*/
kmagnetView::kmagnetView(QWidget * parent): QGraphicsView(parent)
{
    this->setFocusPolicy(Qt::NoFocus);
    //this->setCacheMode(CacheBackground);
}

kmagnetView::~kmagnetView()
{
}

/*trigger a resize of the scene in case we resize the window*/
void kmagnetView::resizeEvent( QResizeEvent *ev )
{
    emit resizeScene( ev->size().width(), ev->size().height() );
}
