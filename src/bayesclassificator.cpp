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
 * GNU General Public License for more detailm_text.
 *
 * You should have received a copy of the GNU General Public License
 * along with rrv.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file bayesclassificator.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */
#include "bayesclassificator.h"

#define DEBUG 0

namespace NaiveBayes {


  Classificator::Classificator() : tolerance(0.05f),
  I(0),
  invI(0) {
  }


  Classificator::~Classificator() {
  }


  ClassificatorResult Classificator::classificate(Index* first, Index * second, Index * item) {

    float prediction = GetPrediction(first, second, item);

#if DEBUG
    std::cout << prediction << std::endl;
#endif
    if (prediction <= 0.5f - tolerance)
      return secondClass;

    if (prediction >= 0.5f + tolerance)
      return firstClass;

    return unknownClass;
  }


  float Classificator::calculateProbability(float cat1count, float cat1total, float cat2count, float cat2total) {
    float bw = cat1count / cat1total;
    float gw = cat2count / cat2total;
    float pw = ((bw) / ((bw) + (gw)));
    float s = 1.0;
    float x = 0.5;
    float n = cat1count + cat2count;
    float fw = ((s * x) + (n * pw)) / (s + n);

    LogProbability(fw);

    return fw;
  }


  float Classificator::GetPrediction(Index* first, Index * second, Index * item) {

    std::map<std::string, int>::iterator it;
    std::map<std::string, int>::iterator pos;
    int firstCount = first->getTokenCount();
    int secondCount = second->getTokenCount();
    int firstEntryCount;
    int secondEntryCount;
    I = 0;
    invI = 0;

    // add all words to local map
    for (it = item->get()->begin(); it != item->get()->end(); it++) {

      pos = first->get()->find(it->first);
      firstEntryCount = (pos != first->get()->end()) ? first->get()->find(it->first)->second : 0;


      pos = second->get()->find(it->first);
      secondEntryCount = (pos != second->get()->end()) ? second->get()->find(it->first)->second : 0;

      float probability = calculateProbability(firstEntryCount, firstCount, secondEntryCount, secondCount);

#if DEBUG
      std::cout << it->first << "\t\t" << probability << "\t (" << firstEntryCount << "/"
              << firstCount << ")" << "\t (" << secondEntryCount << "/" << secondCount << ")" << std::endl;
#endif
    }

    float prediction = CombineProbability();
    return prediction;
  }


  void Classificator::LogProbability(float prob) {
    if (!isnan(prob)) {
      I = I == 0 ? prob : I * prob;
      invI = invI == 0 ? (1 - prob) : invI * (1 - prob);
    }
  }


  float Classificator::CombineProbability() {
    return I / (I + invI);
  }
}