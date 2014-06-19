/*
Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue
 
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <memory>
#include <vector>
#include <string>
#include "MapSelectionView.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/StrManager.hpp"
#include "Views/GameViewGlobals.hpp"
#include "HostOptionsView.hpp"
#include "Classes/MapFile.hpp"
#include "Util/FileSystem.hpp"
#include "Util/FileStream.hpp"
#include "Util/Exception.hpp"
#include "Util/Log.hpp"

#include "Actions/Action.hpp"
#include "Views/Components/Button.hpp"

class ChangeSelectedMapAction : public Action
{
public:
    MapSelectionView * view;
    int change;
    ChangeSelectedMapAction(MapSelectionView * view, int change) : Action(false), view(view), change(change) {}
    void execute()
    {
        int num_maps = view->getNumMaps();
        if ( num_maps == 0 )
        {
            return;
        }
        
        int new_val = view->getCurrentSelectedMapNumber() + change;
        if ( new_val < 0 )
        {
            new_val = num_maps -1;
        }
        else if ( new_val >= num_maps )
        {
            new_val = 0;
        }
        
        view->setSelectedMap(new_val);
    }
};

// MapSelectionView
//---------------------------------------------------------------------------
MapSelectionView::MapSelectionView() : RMouseHackView()
{
    setSearchName("MapSelectionView");
    setTitle(_("Map Selection"));
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);

    moveTo(bodyTextRect.min.x, bodyTextRect.min.y + 50);

    resizeClientArea(bodyTextRect.getSizeX() / 2 - 10 + 30, MAP_SIZE + BORDER_SPACE * 2);

    curMap = -1;
    
    init();

} // end MapSelectionView::MapSelectionView

MapSelectionView::~MapSelectionView()
{
    std::vector<MapInfo*>::iterator i;
    for(i = mapList.begin(); i != mapList.end(); i++)
        delete *i;
}

// init
//---------------------------------------------------------------------------
void MapSelectionView::init()
{
    const int arrowButtonWidth = (getClientRect().getSizeX() - MAP_SIZE - BORDER_SPACE * 3) / 2;

    iXY pos(MAP_SIZE + BORDER_SPACE * 2, getClientRect().getSizeY() - 14 - BORDER_SPACE);

    add( Button::createTextButton( "<", pos, arrowButtonWidth - 1, new ChangeSelectedMapAction(this, -1)));
    pos.x += arrowButtonWidth + 2;
    add( Button::createTextButton( ">", pos, arrowButtonWidth - 1, new ChangeSelectedMapAction(this, 1)));

    loadMaps();
    HostOptionsView::updateWindSpeedString();

} // end MapSelectionView::init

// doDraw
//---------------------------------------------------------------------------
void MapSelectionView::doDraw(Surface &viewArea, Surface &clientArea)
{
    //iRect r(getViewRect());
    //viewArea.bltLookup(r, Palette::darkGray256.getColorArray());

    if (mapList.size() <= 0) {
        // Since there are no maps loaded, try to load some maps.

        int result = loadMaps();

        if (result == 0) {
            clientArea.bltStringCenter(_("No Maps Found"), Color::white);
        } else if (result == 1) {
            clientArea.bltStringCenter(_("Outpost file error"), Color::white);
        }
    }

    if (mapList.size() > 0) {
        // Since maps were found, draw the selected map.
        mapList[curMap]->thumbnail.blt(clientArea, BORDER_SPACE, BORDER_SPACE);
        drawCurMapInfo(clientArea, iXY(MAP_SIZE + BORDER_SPACE * 2, BORDER_SPACE));
    }
    View::doDraw(viewArea, clientArea);

} // end MapSelectionView::doDraw

void
MapSelectionView::setSelectedMap(int map_number)
{
    curMap = map_number;
    GameConfig::game_mapcycle->assign( mapList[curMap]->name );
}

// loadMaps
//---------------------------------------------------------------------------
int MapSelectionView::loadMaps()
{
    const char mapsPath[] = "maps/";

    LOGGER.info("Loading maps.");

    // scan directory for .npm files
    std::string suffix = ".npm";
    char **list = filesystem::enumerateFiles(mapsPath);
    std::vector<std::string> mapfiles;
    for (char **i = list; *i != NULL; i++) {
        std::string filename = mapsPath;
        filename.append(*i);
        if (!filesystem::isDirectory(filename.c_str())) {
            if (filename.size() >= suffix.size()
                    && (filename.compare(filename.size() - suffix.size(),
                            suffix.size(), suffix) == 0))
            {
                std::string mapname;
                size_t p = 0;
                char c;
                while( (c = (*i)[p++]) != 0) {
                    if(c == '.')
                        break;
                    mapname += c;
                }
                
                mapfiles.push_back(mapname);
            }
        }
    }
    filesystem::freeList(list);

    if(mapfiles.size() == 0) {
        throw Exception("not found any maps in '%s'", mapsPath);
    }

    for (unsigned int i = 0; i < mapfiles.size(); i++) {
        try {
            std::string filename = mapsPath;
            filename += mapfiles[i];
            filename += ".npm";
	    std::auto_ptr<filesystem::ReadFile> file 
		(filesystem::openRead(filename));

	    MapFile netPanzerMapHeader;
	    netPanzerMapHeader.load(*file);

    	    MapInfo* mapinfo = new MapInfo;

            std::string mapname;
            const char* filestring = mapfiles[i].c_str();
            size_t i = 0;
            char c;
            while( (c = filestring[i++]) != 0) {
                if(c == '.')
                    break;
                mapname += c;
            }
            
            mapinfo->name = mapname;
	    mapinfo->description = netPanzerMapHeader.description;

	    mapinfo->cells.x = netPanzerMapHeader.width;
	    mapinfo->cells.y = netPanzerMapHeader.height;

	    int seekAmount = mapinfo->cells.getArea() * sizeof(Uint16);

	    file->seek(file->tell()+seekAmount);

	    iXY pix;
	    pix.x = netPanzerMapHeader.thumbnail_width;
	    pix.y = netPanzerMapHeader.thumbnail_height;

	    mapinfo->thumbnail.create(pix.x, pix.y, 1);
	    
	    int numBytes = pix.getArea();

	    file->read(mapinfo->thumbnail.getFrame0(), numBytes, 1);

	    mapinfo->thumbnail.scale(100,100);
	
	    // Now try to get the outpost count from the outpost file.
	    int objectiveCount = 0;
            filename = mapsPath + mapinfo->name + ".opt";
            IFileStream in(filename);
      
            std::string dummy;
            in >> dummy >> objectiveCount;
            if(!in.good())
                continue;

	    mapinfo->objectiveCount = objectiveCount;
	    mapList.push_back(mapinfo);
	} catch(std::exception& e) {
	    LOGGER.warning("cannot open map file '%s': %s", 
		    mapfiles[i].c_str(), e.what());
	    continue;
	}	
    }

    if (mapList.size() == 0) {
        throw Exception("ERROR: No maps in map directory");
    }

    setSelectedMap(0);
//    curMap = 0;
//    GameConfig::game_mapcycle->assign( MapSelectionView::mapList[curMap]->name );

    // Success
    return -1;
} // end MapSelectionView::loadMaps

// drawCurMapInfo
//---------------------------------------------------------------------------
void MapSelectionView::drawCurMapInfo(Surface &dest, const iXY &pos)
{
    // Draw the text.
    char strBuf[256];

    int x = pos.x;
    int y = pos.y;

    const int yOffset = 15;

    sprintf(strBuf, "%-11s: %s", _("Name"), mapList[curMap]->name.c_str());
    dest.bltStringShadowed(x, y, strBuf, windowTextColor, windowTextColorShadow);
    y += yOffset;

    int sizeX = (mapList[curMap]->cells.y * 32) / 800;
    int sizeY = (mapList[curMap]->cells.x * 32) / 600;
    sprintf(strBuf, "%-11s: %d x %d", _("Size"),sizeX, sizeY);
    dest.bltStringShadowed(x, y, strBuf, windowTextColor, windowTextColorShadow);
    y += yOffset;

    sprintf(strBuf, "%-11s: %d", _("Objectives"),mapList[curMap]->objectiveCount);
    dest.bltStringShadowed(x, y, strBuf, windowTextColor, windowTextColorShadow);

} // end MapSelectionView::drawMapInfo
