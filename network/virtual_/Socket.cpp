/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "Socket.hpp"

namespace oatpp { namespace network { namespace virtual_ {
  
void Socket::setMaxAvailableToReadWrtie(os::io::Library::v_size maxToRead, os::io::Library::v_size maxToWrite) {
  m_pipeIn->getReader()->setMaxAvailableToRead(maxToRead);
  m_pipeOut->getWriter()->setMaxAvailableToWrite(maxToWrite);
}
  
os::io::Library::v_size Socket::read(void *data, os::io::Library::v_size count) {
  return m_pipeIn->getReader()->read(data, count);
}

os::io::Library::v_size Socket::write(const void *data, os::io::Library::v_size count) {
  return m_pipeOut->getWriter()->write(data, count);
}

void Socket::setNonBlocking(bool nonBlocking) {
  m_pipeIn->getReader()->setNonBlocking(nonBlocking);
  m_pipeOut->getWriter()->setNonBlocking(nonBlocking);
}

void Socket::close() {
  m_pipeIn->close();
  m_pipeOut->close();
  m_pipeIn.reset();
  m_pipeOut.reset();
}
  
}}}
