/*
* Copyright (C) 2016 AtieshCore <https://at-wow.org/>
* Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Cfbg.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "Player.h"
#include "Chat.h"
#include "BattlegroundQueue.h"
#include "ObjectAccessor.h"
#include "Random.h"
#include "WorldSession.h"
#include "World.h"

#include "stdarg.h"

/*####################################################################################
###############################CROSSFACTION BATTLEGROUNDS#############################
####################################################################################*/
uint8 Unit::GetRace(bool forceoriginal) const
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        Player* pPlayer = ((Player*)this);

        if (forceoriginal)
            return pPlayer->getCFSRace();

        if (pPlayer->InArena())
            return GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE);

        if (!pPlayer->IsPlayingNative())
            return pPlayer->getFRace();
    }

    return GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE);
}

void Player::SetFakeRaceAndMorph()
{
    uint8 random;
    switch (GetClass())
    {
        case CLASS_DRUID:
        {
            if (GetCFSTeam() == ALLIANCE)
            {
                m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TAUREN : FAKE_F_TAUREN;
                m_FakeRace = RACE_TAUREN;
            }
            else
            {
                m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_NELF : FAKE_F_NELF;
                m_FakeRace = RACE_NIGHTELF;
            }

            break;
        }
        case CLASS_SHAMAN:
        {
            if (GetCFSTeam() == ALLIANCE)
            {
                random = urand(0, 2);         // RACE_TROLL, RACE_ORC, RACE_TAUREN
                switch (random)
                {
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TROLL : FAKE_F_TROLL;
                        m_FakeRace = RACE_TROLL;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TAUREN : FAKE_F_TAUREN;
                        m_FakeRace = RACE_TAUREN;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_ORC : FAKE_F_ORC;
                        m_FakeRace = RACE_ORC;
                        break;
                }
            }
            else
            {
                m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DRAENEI : FAKE_F_DRAENEI;
                m_FakeRace = RACE_DRAENEI;
            }

            break;
        }
        case CLASS_MAGE:
        {
            random = urand(0, 2);
            if (GetCFSTeam() == ALLIANCE) // UNDEAD, BLOODELF, TROLL
            {
                switch (random)
                {
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_BLOODELF : FAKE_F_BLOODELF;
                        m_FakeRace = RACE_BLOODELF;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TROLL : FAKE_F_TROLL;
                        m_FakeRace = RACE_TROLL;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_UNDEAD : FAKE_F_UNDEAD;
                        m_FakeRace = RACE_UNDEAD_PLAYER;
                        break;
                }
            }
            else // HUMAN, GNOME, DRAENEI
            {
                switch (random)
                {
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DRAENEI : FAKE_F_DRAENEI;
                        m_FakeRace = RACE_DRAENEI;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_GNOME : FAKE_F_GNOME;
                        m_FakeRace = RACE_GNOME;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_HUMAN : FAKE_F_HUMAN;
                        m_FakeRace = RACE_HUMAN;
                        break;
                }
            }

            break;
        }
        case CLASS_WARLOCK:
        {
            random = urand(0, 2);
            if (GetCFSTeam() == ALLIANCE) // UNDEAD, BLOODELF, ORC
            {
                switch (random)
                {
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_BLOODELF : FAKE_F_BLOODELF;
                        m_FakeRace = RACE_BLOODELF;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_ORC : FAKE_F_ORC;
                        m_FakeRace = RACE_ORC;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_UNDEAD : FAKE_F_UNDEAD;
                        m_FakeRace = RACE_UNDEAD_PLAYER;
                        break;
                }
            }
            else // HUMAN, GNOME
            {
                switch (random)
                {
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_HUMAN : FAKE_F_HUMAN;
                        m_FakeRace = RACE_HUMAN;
                        break;
                    case 1:
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_GNOME : FAKE_F_GNOME;
                        m_FakeRace = RACE_GNOME;
                        break;
                }
            }

            break;
        }
        case CLASS_ROGUE:
        {
            random = urand(0, 3);
            if (GetCFSTeam() == ALLIANCE) // UNDEAD, BLOODELF, ORC, TROLL
            {
                switch (random)
                {
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_UNDEAD : FAKE_F_UNDEAD;
                        m_FakeRace = RACE_UNDEAD_PLAYER;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_BLOODELF : FAKE_F_BLOODELF;
                        m_FakeRace = RACE_BLOODELF;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_ORC : FAKE_F_ORC;
                        m_FakeRace = RACE_ORC;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TROLL : FAKE_F_TROLL;
                        m_FakeRace = RACE_TROLL;
                        break;
                }
            }
            else // HUMAN, DWARF, GNOME, NELF
            {
                switch (random)
                {
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_NELF : FAKE_F_NELF;
                        m_FakeRace = RACE_NIGHTELF;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_GNOME : FAKE_F_GNOME;
                        m_FakeRace = RACE_GNOME;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_HUMAN : FAKE_F_HUMAN;
                        m_FakeRace = RACE_HUMAN;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DWARF : FAKE_F_DWARF;
                        m_FakeRace = RACE_DWARF;
                        break;
                }
            }

            break;
        }
        case CLASS_PALADIN:
        {
            random = urand(0, 2);
            if (GetCFSTeam() == ALLIANCE) // BLOODELF
            {
                m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_BLOODELF : FAKE_F_BLOODELF;
                m_FakeRace = RACE_BLOODELF;
            }
            else // HUMAN, DWARF, DRENEI
            {
                switch (random)
                {
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_HUMAN : FAKE_F_HUMAN;
                        m_FakeRace = RACE_HUMAN;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DWARF : FAKE_F_DWARF;
                        m_FakeRace = RACE_DWARF;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DRAENEI : FAKE_F_DRAENEI;
                        m_FakeRace = RACE_DRAENEI;
                        break;
                }
            }

            break;
        }
        case CLASS_PRIEST:
        {
            random = urand(0, 3);
            if (GetCFSTeam() == ALLIANCE) // UNDEAD, BLOODELF, TROLL
            {
                switch (random)
                {
                    case 3:
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_UNDEAD : FAKE_F_UNDEAD;
                        m_FakeRace = RACE_UNDEAD_PLAYER;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_BLOODELF : FAKE_F_BLOODELF;
                        m_FakeRace = RACE_BLOODELF;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TROLL : FAKE_F_TROLL;
                        m_FakeRace = RACE_TROLL;
                        break;
                }
            }
            else // HUMAN, DWARF, DRENEI, NELF
            {
                switch (random)
                {
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_NELF : FAKE_F_NELF;
                        m_FakeRace = RACE_NIGHTELF;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_GNOME : FAKE_F_GNOME;
                        m_FakeRace = RACE_GNOME;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_HUMAN : FAKE_F_HUMAN;
                        m_FakeRace = RACE_HUMAN;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DRAENEI : FAKE_F_DRAENEI;
                        m_FakeRace = RACE_DRAENEI;
                        break;
                }
            }

            break;
        }
        case CLASS_HUNTER:
        {
            random = urand(0, 3);
            if (GetCFSTeam() == ALLIANCE) // ORC, TAUREN, BLOODELF, TROLL
            {
                switch (random)
                {
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TAUREN : FAKE_F_TAUREN;
                        m_FakeRace = RACE_TAUREN;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_BLOODELF : FAKE_F_BLOODELF;
                        m_FakeRace = RACE_BLOODELF;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_ORC : FAKE_F_ORC;
                        m_FakeRace = RACE_ORC;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TROLL : FAKE_F_TROLL;
                        m_FakeRace = RACE_TROLL;
                        break;
                }
            }
            else // DWARF, DRENEI, NELF
            {
                switch (random)
                {
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_NELF : FAKE_F_NELF;
                        m_FakeRace = RACE_NIGHTELF;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DRAENEI : FAKE_F_DRAENEI;
                        m_FakeRace = RACE_DRAENEI;
                        break;
                    case 1:
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DWARF : FAKE_F_DWARF;
                        m_FakeRace = RACE_DWARF;
                        break;
                }
            }

            break;
        }
        case CLASS_WARRIOR:
        {
            random = urand(0, 4);
            if (GetCFSTeam() == ALLIANCE)
            {
                switch (random)
                {
                    case 4:
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_ORC : FAKE_F_ORC;
                        m_FakeRace = RACE_ORC;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_UNDEAD : FAKE_F_UNDEAD;
                        m_FakeRace = RACE_UNDEAD_PLAYER;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TAUREN : FAKE_F_TAUREN;
                        m_FakeRace = RACE_TAUREN;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TROLL : FAKE_F_TROLL;
                        m_FakeRace = RACE_TROLL;
                        break;
                }
            }
            else
            {
                switch (random)
                {
                    case 4:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_HUMAN : FAKE_F_HUMAN;
                        m_FakeRace = RACE_HUMAN;
                        break;
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DWARF : FAKE_F_DWARF;
                        m_FakeRace = RACE_DWARF;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_NELF : FAKE_F_NELF;
                        m_FakeRace = RACE_NIGHTELF;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_GNOME : FAKE_F_GNOME;
                        m_FakeRace = RACE_GNOME;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DRAENEI : FAKE_F_DRAENEI;
                        m_FakeRace = RACE_DRAENEI;
                        break;
                }
            }

            break;
        }
        case CLASS_DEATH_KNIGHT:
        {
            random = urand(0, 4);
            if (GetCFSTeam() == ALLIANCE)
            {
                switch (random)
                {
                    case 4:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_ORC : FAKE_F_ORC;
                        m_FakeRace = RACE_ORC;
                        break;
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_UNDEAD : FAKE_F_UNDEAD;
                        m_FakeRace = RACE_UNDEAD_PLAYER;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TAUREN : FAKE_F_TAUREN;
                        m_FakeRace = RACE_TAUREN;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_TROLL : FAKE_F_TROLL;
                        m_FakeRace = RACE_TROLL;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_BLOODELF : FAKE_F_BLOODELF;
                        m_FakeRace = RACE_BLOODELF;
                        break;
                }
            }
            else
            {
                switch (random)
                {
                    case 4:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_HUMAN : FAKE_F_HUMAN;
                        m_FakeRace = RACE_HUMAN;
                        break;
                    case 3:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DWARF : FAKE_F_DWARF;
                        m_FakeRace = RACE_DWARF;
                        break;
                    case 2:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_NELF : FAKE_F_NELF;
                        m_FakeRace = RACE_NIGHTELF;
                        break;
                    case 1:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_GNOME : FAKE_F_GNOME;
                        m_FakeRace = RACE_GNOME;
                        break;
                    case 0:
                        m_FakeMorph = GetGender() == GENDER_MALE ? FAKE_M_DRAENEI : FAKE_F_DRAENEI;
                        m_FakeRace = RACE_DRAENEI;
                        break;
                }
            }

            break;
        }
    }
}

void Player::SendBattleGroundChat(uint32 msgtype, std::string message)
{
    // Select distance to broadcast to.
    float distance = msgtype == CHAT_MSG_SAY ? sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY) : sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL);
    uint32 language = LANG_UNIVERSAL;
    if (Battleground* pBattleGround = GetBattleground())
    {
        if (pBattleGround->isArena()) // Only fake chat in BG's. CFBG should not interfere with arenas.
            return;

        for (Battleground::BattlegroundPlayerMap::const_iterator itr = pBattleGround->GetPlayers().begin(); itr != pBattleGround->GetPlayers().end(); ++itr)
        {
            if (Player* pPlayer = ObjectAccessor::FindPlayer(itr->first))
            {
                if (GetDistance2d(pPlayer->GetPositionX(), pPlayer->GetPositionY()) <= distance)
                {
                    WorldPacket data(SMSG_MESSAGECHAT, 200);

                    if (GetTeam() == pPlayer->GetTeam())
                    {
                        if (pPlayer->IsPlayingNative())
                            language = pPlayer->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH;
                        else
                            language = pPlayer->GetTeam() == ALLIANCE ? LANG_ORCISH : LANG_COMMON;
                    }
                    else if (msgtype != CHAT_MSG_EMOTE)
                    {
                        if (pPlayer->IsPlayingNative())
                            language = pPlayer->GetTeam() == ALLIANCE ? LANG_ORCISH : LANG_COMMON;
                        else
                            language = pPlayer->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH;
                    }

                    BuildPlayerChat(&data, msgtype, message, language);
                    pPlayer->GetSession()->SendPacket(&data);
                }
            }
        }
    }
}

void Player::MorphFit(bool value)
{
    if (!IsPlayingNative() && value)
    {
        SetDisplayId(GetFakeMorph());
        SetNativeDisplayId(GetFakeMorph());
    }
    else
        InitDisplayIds();
}

void Player::FitPlayerInTeam(bool action, Battleground* pBattleGround)
{
    if (!pBattleGround)
        pBattleGround = GetBattleground();

    if ((!pBattleGround || pBattleGround->isArena()) && action)
        return;

    if (!IsPlayingNative() && action)
        SetFactionForRace(GetRace());
    else
        SetFactionForRace(getCFSRace());

    MorphFit(action);
    UpdateFactionForSelfAndControllList();
    UpdateFakeQueryName(pBattleGround);

    if (pBattleGround && action)
        SendChatMessage("%sYou are playing for the %s%s in this %s", MSG_COLOR_WHITE, GetTeam() == ALLIANCE ? MSG_COLOR_DARKBLUE"alliance" : MSG_COLOR_RED"horde", MSG_COLOR_WHITE, pBattleGround->GetName().c_str());
}

bool BattlegroundQueue::CheckCrossFactionMatch(BattlegroundBracketId bracket_id, Battleground* bg)
{
    if (!sWorld->getBoolConfig(CROSSFACTION_SYSTEM_BATTLEGROUNDS) || bg->isArena())
        return false; // Only do this if crossbg's are enabled.

    // Here we will add all players to selectionpool, later we check if there are enough and launch a bg.
    FillXPlayersToBG(bracket_id, bg, true);

    if (sBattlegroundMgr->isTesting() && (m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() || m_SelectionPools[TEAM_HORDE].GetPlayerCount()))
        return true;

    uint8 MPT = bg->GetMinPlayersPerTeam();
    if (m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() < MPT || m_SelectionPools[TEAM_HORDE].GetPlayerCount() < MPT)
        return false;

    return true;
}

// This function will invite players in the least populated faction, which makes battleground queues much faster.
// This function will return true if cross faction battlegrounds are enabled, otherwise return false,
// which is useful in FillPlayersToBG. Because then we can interrupt the regular invitation if cross faction bg's are enabled.
bool BattlegroundQueue::FillXPlayersToBG(BattlegroundBracketId bracket_id, Battleground* bg, bool start)
{
    uint8 queuedPeople = 0;
    for (GroupsQueueType::const_iterator itr = m_QueuedGroups[bracket_id][BG_QUEUE_CROSSFACTION].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_CROSSFACTION].end(); ++itr)
        if (!(*itr)->IsInvitedToBGInstanceGUID)
            queuedPeople += (*itr)->Players.size();

    if (sWorld->getBoolConfig(CROSSFACTION_SYSTEM_BATTLEGROUNDS) && (sBattlegroundMgr->isTesting() || queuedPeople >= bg->GetMinPlayersPerTeam() * 2 || !start))
    {
        int32 aliFree = start ? bg->GetMaxPlayersPerTeam() : bg->GetFreeSlotsForTeam(ALLIANCE);
        int32 hordeFree = start ? bg->GetMaxPlayersPerTeam() : bg->GetFreeSlotsForTeam(HORDE);
        // Empty selection pools. They will be refilled from queued groups.
        m_SelectionPools[TEAM_ALLIANCE].Init();
        m_SelectionPools[TEAM_HORDE].Init();
        int32 valiFree = aliFree;
        int32 vhordeFree = hordeFree;
        int32 diff = 0;

        // Add teams to their own factions as far as possible.
        if (start)
        {
            QueuedGroupMap m_PreGroupMap_a, m_PreGroupMap_h;
            int32 m_SmallestOfTeams = 0;
            int32 queuedAlliance = 0;
            int32 queuedHorde = 0;

            for (GroupsQueueType::const_iterator itr = m_QueuedGroups[bracket_id][BG_QUEUE_CROSSFACTION].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_CROSSFACTION].end(); ++itr)
            {
                if ((*itr)->IsInvitedToBGInstanceGUID)
                    continue;

                bool alliance = (*itr)->CFSTeam == ALLIANCE;

                if (alliance)
                {
                    m_PreGroupMap_a.insert(std::make_pair((*itr)->Players.size(), *itr));
                    queuedAlliance += (*itr)->Players.size();
                }
                else
                {
                    m_PreGroupMap_h.insert(std::make_pair((*itr)->Players.size(), *itr));
                    queuedHorde += (*itr)->Players.size();
                }
            }

            m_SmallestOfTeams = std::min(std::min(aliFree, queuedAlliance), std::min(hordeFree, queuedHorde));

            valiFree -= PreAddPlayers(m_PreGroupMap_a, m_SmallestOfTeams, aliFree);
            vhordeFree -= PreAddPlayers(m_PreGroupMap_h, m_SmallestOfTeams, hordeFree);
        }
        QueuedGroupMap m_QueuedGroupMap;

        for (GroupsQueueType::const_iterator itr = m_QueuedGroups[bracket_id][BG_QUEUE_CROSSFACTION].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_CROSSFACTION].end(); ++itr)
            m_QueuedGroupMap.insert(std::make_pair((*itr)->Players.size(), *itr));

        for (QueuedGroupMap::reverse_iterator itr = m_QueuedGroupMap.rbegin(); itr != m_QueuedGroupMap.rend(); ++itr)
        {
            GroupsQueueType allypool = m_SelectionPools[TEAM_ALLIANCE].SelectedGroups;
            GroupsQueueType hordepool = m_SelectionPools[TEAM_HORDE].SelectedGroups;

            GroupQueueInfo* ginfo = itr->second;

            // If player already was invited via pre adding (add to own team first) or he was already invited to a bg, skip.
            if (ginfo->IsInvitedToBGInstanceGUID ||
                    std::find(allypool.begin(), allypool.end(), ginfo) != allypool.end() ||
                    std::find(hordepool.begin(), hordepool.end(), ginfo) != hordepool.end() ||
                    (m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() >= bg->GetMinPlayersPerTeam() &&
                    m_SelectionPools[TEAM_HORDE].GetPlayerCount() >= bg->GetMinPlayersPerTeam()))
                continue;

            diff = abs(valiFree - vhordeFree);
            bool moreAli = valiFree < vhordeFree;

            if (diff > 0)
                ginfo->Team = moreAli ? HORDE : ALLIANCE;

            bool alliance = ginfo->Team == ALLIANCE;

            if (m_SelectionPools[alliance ? TEAM_ALLIANCE : TEAM_HORDE].AddGroup(ginfo, alliance ? aliFree : hordeFree))
                alliance ? valiFree -= ginfo->Players.size() : vhordeFree -= ginfo->Players.size();
        }

        return true;
    }
    return false;
}

int32 BattlegroundQueue::PreAddPlayers(QueuedGroupMap m_PreGroupMap, int32 MaxAdd, uint32 MaxInTeam)
{
    int32 LeftToAdd = MaxAdd;
    uint32 Added = 0;

    for (QueuedGroupMap::reverse_iterator itr = m_PreGroupMap.rbegin(); itr != m_PreGroupMap.rend(); ++itr)
    {
        int32 PlayerSize = itr->first;
        bool alliance = itr->second->CFSTeam == ALLIANCE;

        if (PlayerSize <= LeftToAdd && m_SelectionPools[alliance ? TEAM_ALLIANCE : TEAM_HORDE].AddGroup(itr->second, MaxInTeam))
            LeftToAdd -= PlayerSize, Added -= PlayerSize;
    }

    return LeftToAdd;
}

void Player::SendChatMessage(const char *format, ...)
{
    if (!IsInWorld())
        return;

    if (format)
    {
        va_list ap;
        char str[2048];
        va_start(ap, format);
        vsnprintf(str, 2048, format, ap);
        va_end(ap);

        ChatHandler(GetSession()).SendSysMessage(str);
    }
}

void Player::BuildPlayerChat(WorldPacket* data, uint8 msgtype, const std::string& text, uint32 language) const
{
    *data << uint8(msgtype);
    *data << uint32(language);
    *data << uint64(GetGUID());
    *data << uint32(0);                                      // constant unknown time
    *data << uint64(GetGUID());
    *data << uint32(text.length() + 1);
    *data << text;
    *data << uint8(GetChatTag());
}

void Player::UpdateFakeQueryName(Battleground* pBattleGround)
{
    for (Battleground::BattlegroundPlayerMap::const_iterator itr = pBattleGround->GetPlayers().begin(); itr != pBattleGround->GetPlayers().end(); ++itr)
    {
        // Here we invalidate players in the bg to the added player
        WorldPacket data1(SMSG_INVALIDATE_PLAYER, 8);
        data1 << itr->first;
        GetSession()->SendPacket(&data1);

        if (Player* pPlayer = ObjectAccessor::FindPlayer(itr->first))
        {
            GetSession()->SendNameQueryOpcode(pPlayer->GetGUID()); // Send namequery answer instantly if player is available
            // Here we invalidate the player added to players in the bg
            WorldPacket data2(SMSG_INVALIDATE_PLAYER, 8);
            data2 << GetGUID();
            pPlayer->GetSession()->SendPacket(&data2);
            pPlayer->GetSession()->SendNameQueryOpcode(GetGUID());
        }
    }
}
