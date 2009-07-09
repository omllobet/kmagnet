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


#ifndef KMAGNETVIEW_H
#define KMAGNETVIEW_H

#include <QGraphicsView>
#include <QResizeEvent>

class kmagnetView : public QGraphicsView
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    kmagnetView(QWidget * parent = 0);

    /**
     * Default Destructor
     */
    virtual ~kmagnetView();
signals:
    void resizeScene( int w, int h);
private:
    virtual void resizeEvent( QResizeEvent *ev );

};

#endif // KMAGNETVIEW_H
