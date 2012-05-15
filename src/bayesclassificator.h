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
 * @file bayesclassificator.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Bayesian Clasificator
 * @original sources from http://nbayes.codeplex.com/ (C#)
 */
#ifndef CLASSIFICATION_H
#define	CLASSIFICATION_H
#include <map>
#include <math.h>
#include <iostream>
#include "bayesindex.h"

namespace NaiveBayes {

  typedef enum {
    firstClass = -1,
    unknownClass = 0,
    secondClass = 1
  } ClassificatorResult;

  class Classificator {
  public:
    Classificator();
    virtual ~Classificator();
    ClassificatorResult classificate(Index* first, Index * second, Index * item);
  private:

    float I;
    float invI;
    float tolerance;

    float calculateProbability(float cat1count, float cat1total, float cat2count, float cat2total);
    float GetPrediction(Index* first, Index * second, Index * item);
    void LogProbability(float prob);
    float CombineProbability();

  };
}
#endif	/* CLASSIFICATION_H */

