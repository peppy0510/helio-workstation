/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "MidiTrackMenu.h"
#include "MidiTrackNode.h"
#include "PianoTrackNode.h"
#include "Icons.h"
#include "CommandIDs.h"

#include "MainLayout.h"
#include "AudioCore.h"
#include "Instrument.h"
#include "MidiSequence.h"
#include "HybridRoll.h"
#include "ProjectNode.h"
#include "ModalDialogInput.h"

#include "MidiSequence.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "MidiTrackActions.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "UndoStack.h"
#include "Workspace.h"

MidiTrackMenu::MidiTrackMenu(MidiTrackNode &node) :
    trackNode(node)
{
    this->initDefaultMenu();
}

void MidiTrackMenu::initDefaultMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::selectAll, CommandIDs::SelectAllEvents, TRANS(I18n::Menu::trackSelectall))->closesMenu());
    menu.add(MenuItem::item(Icons::colour, TRANS(I18n::Menu::trackChangeColour))->withSubmenu()->withAction([this]()
    {
        this->initColorSelectionMenu();
    }));
    
    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    const int numInstruments = info.size();
    if (numInstruments > 1)
    {
        menu.add(MenuItem::item(Icons::instrument, TRANS(I18n::Menu::trackChangeInstrument))->withSubmenu()->withAction([this]()
        {
            this->initInstrumentSelectionMenu();
        }));
    }
    
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameTrack,
        TRANS(I18n::Menu::trackRename))->closesMenu());
        
    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteTrack, TRANS(I18n::Menu::trackDelete)));
    this->updateContent(menu, MenuPanel::SlideRight);
}

void MidiTrackMenu::initColorSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withTimer()->withAction([this]()
    {
        this->initDefaultMenu();
    }));
    
    const StringPairArray colours(MenuPanel::getColoursList());
    for (int i = 0; i < colours.getAllKeys().size(); ++i)
    {
        const String name(colours.getAllKeys()[i]);
        const String colourString(colours[name]);
        const Colour colour(Colour::fromString(colourString));
        const bool isSelected = (colour == this->trackNode.getTrackColour());
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::colour, name)->
            colouredWith(colour)->withAction([this, colourString]()
        {
            this->trackNode.getChangeColourCallback()(colourString);
            this->initDefaultMenu();
        }));
    }

    this->updateContent(menu, MenuPanel::SlideLeft);
}

void MidiTrackMenu::initInstrumentSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withTimer()->withAction([this]()
    {
        this->initDefaultMenu();
    }));
    
    const auto &info = App::Workspace().getAudioCore().getInstruments();
    const Instrument *selectedInstrument = App::Workspace().getAudioCore().findInstrumentById(this->trackNode.getTrackInstrumentId());
    
    for (int i = 0; i < info.size(); ++i)
    {
        const bool isTicked = (info[i] == selectedInstrument);
        const String instrumentId = info[i]->getIdAndHash();
        menu.add(MenuItem::item(isTicked ? Icons::apply : Icons::instrument, info[i]->getName())->withAction([this, instrumentId]()
        {
            DBG(instrumentId);
            this->trackNode.getChangeInstrumentCallback()(instrumentId);
            this->initDefaultMenu();
            return;
        }));
    }
    
    this->updateContent(menu, MenuPanel::SlideLeft);
}
