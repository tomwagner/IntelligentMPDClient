/**
 * Copyright (C) 2012 Tomas Wagner <xwagne01@stud.fit.vutbr.cz>
 *
 * This file is part of impc (Intelligent Music Player Client).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rrv.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file sfeedback.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */
#include "sfeedback.h"


namespace DataStorage {


  SFeedback::SFeedback() :
  m_rightCount(0),
  m_wrongCount(0),
  m_votedRight(false),
  m_votedWrong(false),
  //  m_voted(false),
  m_synced(false) {

  }


  SFeedback::~SFeedback() {

  }


  //  void SFeedback::rememberVoting() {
  //    m_voted = true;
  //  }


  void SFeedback::voteRight() {
    if (!canIVote()) return;
    m_rightCount++;
    m_votedRight = true;
  }


  void SFeedback::voteWrong() {
    if (!canIVote()) return;
    m_wrongCount++;
    m_votedWrong = true;
  }


  bool SFeedback::canIVote() {
    if (m_votedRight || m_votedWrong)
      return false;
    return true;
    //    return (!m_voted);
  }


  bool SFeedback::isSynced() {
    return m_synced;
  }


  void SFeedback::setSynced(bool s) {
    //    if (m_voted) {
    if ((m_votedRight) || (m_votedWrong)) {
      m_synced = s;
    }
  }
}