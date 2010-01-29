/*************************************************************************************
 *  Copyright (C) 2007 Dmitry Suzdalev <dimsuz@gmail.com>                            *
 *  Copyright (C) 2010 by Oscar Martinez <omllobet@gmail.com>                        *
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

#include "kmagnetrenderer.h"

#include <QPainter>

#include <KSvgRenderer>
#include <KGameTheme>
#include <kpixmapcache.h>

#include <settings.h>

QString kmagnetRenderer::elementToSvgId( SvgElement e ) const
{
    switch(e)
    {
        case kmagnetRenderer::CellUp:
            return "cell_up";
        case kmagnetRenderer::CellDown:
            return "cell_down";
        case kmagnetRenderer::Flag:
            return "flag";
        case kmagnetRenderer::Question:
            return "question";
        case kmagnetRenderer::Digit1:
            return "arabicOne";
        case kmagnetRenderer::Digit2:
            return "arabicTwo";
        case kmagnetRenderer::Digit3:
            return "arabicThree";
        case kmagnetRenderer::Digit4:
            return "arabicFour";
        case kmagnetRenderer::Digit5:
            return "arabicFive";
        case kmagnetRenderer::Digit6:
            return "arabicSix";
        case kmagnetRenderer::Digit7:
            return "arabicSeven";
        case kmagnetRenderer::Digit8:
            return "arabicEight";
        case kmagnetRenderer::Mine:
            return "mine";
        case kmagnetRenderer::ExplodedMine:
            return QString(); // dummy. shouldn't be called
        case kmagnetRenderer::Explosion:
            return "explosion";
        case kmagnetRenderer::Error:
            return "error";
        case kmagnetRenderer::Hint:
            return "hint";
        case kmagnetRenderer::BorderEdgeNorth:
            return "border.edge.north";
        case kmagnetRenderer::BorderEdgeSouth:
            return "border.edge.south";
        case kmagnetRenderer::BorderEdgeEast:
            return "border.edge.east";
        case kmagnetRenderer::BorderEdgeWest:
            return "border.edge.west";
        case kmagnetRenderer::BorderOutsideCornerNE:
            return "border.outsideCorner.ne";
        case kmagnetRenderer::BorderOutsideCornerNW:
            return "border.outsideCorner.nw";
        case kmagnetRenderer::BorderOutsideCornerSW:
            return "border.outsideCorner.sw";
        case kmagnetRenderer::BorderOutsideCornerSE:
            return "border.outsideCorner.se";
        case kmagnetRenderer::NumElements:
            return QString();
    }
    return QString();
}

kmagnetRenderer* kmagnetRenderer::self()
{
    static kmagnetRenderer instance;
    return &instance;
}

kmagnetRenderer::kmagnetRenderer()
    : m_cellSize(0)
{
    m_renderer = new KSvgRenderer();
    m_cache = new KPixmapCache("kmines-cache");
    m_cache->setCacheLimit(3*1024);

    if(!loadTheme( Settings::theme() ))
        kDebug() << "Failed to load any game theme!";
}

bool kmagnetRenderer::loadTheme( const QString& themeName )
{
    // variable saying whether to discard old cache upon successful new theme loading
    // we won't discard it if m_currentTheme is empty meaning that
    // this is the first time loadTheme() is called
    // (i.e. during startup) as we want to pick the cache from disc
    bool discardCache = !m_currentTheme.isEmpty();

    if( !m_currentTheme.isEmpty() && m_currentTheme == themeName )
    {
        kDebug() << "Notice: not loading the same theme";
        return true; // this is not an error
    }

    m_currentTheme = themeName;

    KGameTheme theme;
    if ( themeName.isEmpty() || !theme.load( themeName ) )
    {
        kDebug()<< "Failed to load theme" << Settings::theme();
        kDebug() << "Trying to load default";
        if(!theme.loadDefault())
            return false;
        // in this case we need to discard any previously cached theme:
        // we loading default
        discardCache = true;
        // set it to something - not be empty - so we
        // do discard cache on next loadTheme
        // (see discardCache above)
        m_currentTheme = "default";
        Settings::setTheme("themes/default.desktop");
    }

    bool res = m_renderer->load( theme.graphics() );
    kDebug() << "loading" << theme.graphics();
    if ( !res )
        return false;

    if(discardCache)
    {
        kDebug() << "discarding cache";
        m_cache->discard();
    }

    return true;
}


QPixmap kmagnetRenderer::backgroundPixmap( const QSize& size ) const
{
    QPixmap bkgnd;
    QString cacheName = QString("mainWidget%1x%2").arg(size.width()).arg(size.height());
    if(!m_cache->find( cacheName, bkgnd ))
    {
        kDebug() << "re-rendering bkgnd";
        bkgnd = QPixmap(size);
        bkgnd.fill(Qt::transparent);
        QPainter p(&bkgnd);
        m_renderer->render(&p, "mainWidget");
        p.end();
        m_cache->insert(cacheName, bkgnd);
        kDebug() << "cache size:" << m_cache->size() << "kb";
    }
    return bkgnd;
}

kmagnetRenderer::~kmagnetRenderer()
{
    delete m_renderer;
    delete m_cache;
}

#define RENDER_SVG_ELEMENT(SVG_ID)                      \
    p.begin( &pix );                                    \
    m_renderer->render( &p, elementToSvgId(SVG_ID) );   \
    p.end();

    
/*QPixmap kmagnetRenderer::pixmapForCellState( KMinesState::CellState state ) const
{
    QPainter p;
    switch(state)
    {
        case KMinesState::Released:
        {
            QPixmap pix;
            QString cacheName = elementToSvgId(CellUp)+QString::number(m_cellSize);
            if(!m_cache->find(cacheName, pix))
            {
//                kDebug() << "putting" << cacheName << "to cache";
                pix = QPixmap(m_cellSize, m_cellSize);
                pix.fill( Qt::transparent);
                RENDER_SVG_ELEMENT(CellUp);
                m_cache->insert(cacheName, pix);
            }
            return pix;
        }
        case KMinesState::Pressed:
        case KMinesState::Revealed:// i.e. revealed & digit=0 case
        {
            QPixmap pix;
            QString cacheName = elementToSvgId(CellDown)+QString::number(m_cellSize);
            if(!m_cache->find(cacheName, pix))
            {
//                kDebug() << "putting" << cacheName << "to cache";
                pix = QPixmap(m_cellSize, m_cellSize);
                pix.fill( Qt::transparent);
                RENDER_SVG_ELEMENT(CellDown);
                m_cache->insert(cacheName, pix);
            }
            return pix;
        }
        case KMinesState::Questioned:
        {
            QPixmap pix;
            QString cacheName = elementToSvgId(Question)+QString::number(m_cellSize);
            if(!m_cache->find(cacheName, pix))
            {
//                kDebug() << "putting" << cacheName << "to cache";
                // question (on top of cellup)
                pix = pixmapForCellState( KMinesState::Released );
                RENDER_SVG_ELEMENT(Question);
                m_cache->insert(cacheName, pix);
            }
            return pix;
        }
        case KMinesState::Flagged:
        {
            QPixmap pix;
            QString cacheName = elementToSvgId(Flag)+QString::number(m_cellSize);
            if(!m_cache->find(cacheName, pix))
            {
                // flag (on top of cellup)
//                kDebug() << "putting" << cacheName << "to cache";
                pix = pixmapForCellState( KMinesState::Released );
                RENDER_SVG_ELEMENT(Flag);
                m_cache->insert(cacheName, pix);
            }
            return pix;
        }
        case KMinesState::Error:
        {
            QPixmap pix;
            QString cacheName = elementToSvgId(Error)+QString::number(m_cellSize);
            if(!m_cache->find(cacheName, pix))
            {
//                kDebug() << "putting" << cacheName << "to cache";
                // flag (on top of mine)
                pix = pixmapMine();
                RENDER_SVG_ELEMENT(Error);
                m_cache->insert(cacheName, pix);
            }
            return pix;
        }
        case KMinesState::Hint:
        {
            QPixmap pix;
            QString cacheName = elementToSvgId(Hint)+QString::number(m_cellSize);
            if(!m_cache->find(cacheName, pix))
            {
//                kDebug() << "putting" << cacheName << "to cache";
                // hint (on top of cellup)
                pix = pixmapForCellState( KMinesState::Released );
                RENDER_SVG_ELEMENT(Hint);
                m_cache->insert(cacheName, pix);
            }
            return pix;
        }
        // no default! this way we'll get compiler warnings if
        // something is forgotten
    }
    return QPixmap();
} */   
    
void kmagnetRenderer::setCellSize( int size )
{
    m_cellSize = size;
}