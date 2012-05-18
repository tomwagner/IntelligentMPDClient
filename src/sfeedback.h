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
 * @file sfeedback.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#ifndef SFEEDBACK_H
#define	SFEEDBACK_H

#include <string>
#include "stext.h"

typedef enum {
  rightClass = -1,
  unknownClass = 0,
  wrongClass = 1
} FeedbackResult;

namespace DataStorage {

  class SFeedback {
  public:
    SFeedback();
    virtual ~SFeedback();


    void voteWrong();
    void voteRight();
    //    void inline voteRight() {
    //      m_rightCount++;
    //      rememberVoting();
    //    }
    //
    //    void inline voteWrong() {
    //      m_wrongCount++;
    //      rememberVoting();
    //    }

    FeedbackResult isRight() const {
      if (m_rightCount == m_wrongCount) return unknownClass;
      else if (m_rightCount > m_wrongCount) return rightClass;
      else if (m_rightCount < m_wrongCount) return wrongClass;
    }

    unsigned inline getNumOfRight() const {
      return m_rightCount;
    }

    unsigned inline getNumOfWrong() const {
      return m_wrongCount;
    }

    void inline setNumOfRight(unsigned num) {
      m_rightCount = num;
    }

    void inline setNumOfWrong(unsigned num) {
      m_wrongCount = num;
    }

    //    void inline setVoted(bool v) {
    //      m_voted = v;
    //    }

    void inline setVotedRight(bool v) {
      m_votedRight = v;
    }

    void inline setVotedWrong(bool v) {
      m_votedWrong = v;
    }

    bool inline isVotedRight() {
      return m_votedRight;
    }

    bool inline isVotedWrong() {
      return m_votedWrong;
    }


    bool canIVote();
    bool isSynced();
    void setSynced(bool s);
  protected:
    unsigned int m_rightCount;
    unsigned int m_wrongCount;

    //    bool m_voted;
    bool m_synced;

    bool m_votedRight;
    bool m_votedWrong;

  private:
//    void rememberVoting();

  };
}
#endif

