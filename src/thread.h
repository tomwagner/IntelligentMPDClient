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
 * @file thread.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Class contains creating and managing thread.
 */

#ifndef THREAD_H
#define	THREAD_H

#include "global.h"
#include <errno.h>
#include <pthread.h>

class Thread {
public:
  Thread();
  virtual ~Thread();
  bool create(void * (void *));
  bool join();
  void cancel();
private:
  pthread_t pt; /* identifikace vlákna */
  pthread_attr_t attr;
  void *statp;

};

#endif	/* THREAD_H */

