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
 * @file thread.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Class contains creating and managing thread.
 */

#include "thread.h"


Thread::Thread() {
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}


bool Thread::create(void * t(void *)) {
  if (pthread_create(&pt, &attr, t, NULL) != 0) {
    std::cerr << "pthread_create() err=" << errno << std::endl;
    return false;
  }
  return true;
}


bool Thread::join() {
  if (pthread_join(pt, &statp) == -1) {
    std::cerr << "pthread_join() err=" << errno << std::endl;
    return false;
  }
  return true;
}


void Thread::cancel() {
  pthread_cancel(pt);
}


Thread::~Thread() {
}

