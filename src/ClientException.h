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
 * @file clientexception.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Error Exception handler.
 */

#ifndef CLIENTEXCEPTION_H
#define	CLIENTEXCEPTION_H
#include <exception>
#include <stdio.h>
#include <stdlib.h>

class ClientException : public std::exception {
public:

  explicit ClientException(const std::string& what_arg) :
  m_what(what_arg) {
  }

  virtual ~ClientException() throw () {
  }

  virtual const char * what() const throw () {
    return m_what.c_str();
  }

private:
  std::string m_what;
};


class InputException : public std::exception {
public:
  InputException(const std::string& what_arg) :
  m_what(what_arg) {
  }

  virtual const char * what() const throw () {
    /*TODO*/
    // při chybě vstupu, zavoláme GUI, vypíšeme error
    return m_what.c_str();
  }
  
  virtual ~InputException() throw () {
  }
private:
  std::string m_what;
};

#endif	/* CLIENTEXCEPTION_H */

