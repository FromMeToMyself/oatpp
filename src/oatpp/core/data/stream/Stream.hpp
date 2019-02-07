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

#ifndef oatpp_data_Stream
#define oatpp_data_Stream

#include "oatpp/core/async/Coroutine.hpp"
#include "oatpp/core/data/buffer/IOBuffer.hpp"

#include "oatpp/core/data/IODefinitions.hpp"

namespace oatpp { namespace data{ namespace stream {
  
class Errors {
public:
  static const char* const ERROR_ASYNC_FAILED_TO_WRITE_DATA;
  static const char* const ERROR_ASYNC_FAILED_TO_READ_DATA;
};
  
class OutputStream {
public:
  
  /**
   * Write data to stream up to count bytes, and return number of bytes actually written
   * It is a legal case if return result < count. Caller should handle this!
   */
  virtual data::v_io_size write(const void *data, data::v_io_size count) = 0;
  
  data::v_io_size write(const char* data){
    return write((p_char8)data, std::strlen(data));
  }
  
  data::v_io_size write(const oatpp::String& str){
    return write(str->getData(), str->getSize());
  }
  
  data::v_io_size writeChar(v_char8 c){
    return write(&c, 1);
  }
  
  data::v_io_size writeAsString(v_int32 value);
  data::v_io_size writeAsString(v_int64 value);
  data::v_io_size writeAsString(v_float32 value);
  data::v_io_size writeAsString(v_float64 value);
  data::v_io_size writeAsString(bool value);
  
};
  
class InputStream {
public:
  /**
   * Read data from stream up to count bytes, and return number of bytes actually read
   * It is a legal case if return result < count. Caller should handle this!
   */
  virtual data::v_io_size read(void *data, data::v_io_size count) = 0;
};
  
class IOStream : public InputStream, public OutputStream {
public:
  typedef data::v_io_size v_size;
};

class CompoundIOStream : public oatpp::base::Controllable, public IOStream {
public:
  OBJECT_POOL(CompoundIOStream_Pool, CompoundIOStream, 32);
  SHARED_OBJECT_POOL(Shared_CompoundIOStream_Pool, CompoundIOStream, 32);
private:
  std::shared_ptr<OutputStream> m_outputStream;
  std::shared_ptr<InputStream> m_inputStream;
public:
  CompoundIOStream(const std::shared_ptr<OutputStream>& outputStream,
                   const std::shared_ptr<InputStream>& inputStream)
    : m_outputStream(outputStream)
    , m_inputStream(inputStream)
  {}
public:
  
  static std::shared_ptr<CompoundIOStream> createShared(const std::shared_ptr<OutputStream>& outputStream,
                                                  const std::shared_ptr<InputStream>& inputStream){
    return Shared_CompoundIOStream_Pool::allocateShared(outputStream, inputStream);
  }
  
  data::v_io_size write(const void *data, data::v_io_size count) override {
    return m_outputStream->write(data, count);
  }
  
  data::v_io_size read(void *data, data::v_io_size count) override {
    return m_inputStream->read(data, count);
  }
    
};
  
const std::shared_ptr<OutputStream>& operator <<
(const std::shared_ptr<OutputStream>& s, const oatpp::String& str);

const std::shared_ptr<OutputStream>& operator <<
(const std::shared_ptr<OutputStream>& s, const char* str);
  
const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_int32 value);

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_int64 value);

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_float32 value);

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_float64 value);

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, bool value);

/**
 * Read bytes from @fromStream" and write to @toStream" using @buffer of size @bufferSize
 * transfer up to transferSize or until error if transferSize == 0
 * throws in case readCount != writeCount
 */
oatpp::data::v_io_size transfer(const std::shared_ptr<InputStream>& fromStream,
                                        const std::shared_ptr<OutputStream>& toStream,
                                        oatpp::data::v_io_size transferSize,
                                        void* buffer,
                                        oatpp::data::v_io_size bufferSize);
  
  
/**
 * Same as transfer but asynchronous
 */
oatpp::async::Action transferAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                   const oatpp::async::Action& actionOnReturn,
                                   const std::shared_ptr<InputStream>& fromStream,
                                   const std::shared_ptr<OutputStream>& toStream,
                                   oatpp::data::v_io_size transferSize,
                                   const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer);
  
/**
 *  Async write data withot starting new Coroutine.
 *  Should be called from a separate Coroutine method
 */
oatpp::async::Action writeSomeDataAsyncInline(oatpp::data::stream::OutputStream* stream,
                                              const void*& data,
                                              data::v_io_size& size,
                                              const oatpp::async::Action& nextAction);
  
oatpp::async::Action writeExactSizeDataAsyncInline(oatpp::data::stream::OutputStream* stream,
                                                   const void*& data,
                                                   data::v_io_size& size,
                                                   const oatpp::async::Action& nextAction);

oatpp::async::Action readSomeDataAsyncInline(oatpp::data::stream::InputStream* stream,
                                             void*& data,
                                             data::v_io_size& bytesLeftToRead,
                                             const oatpp::async::Action& nextAction);

oatpp::async::Action readExactSizeDataAsyncInline(oatpp::data::stream::InputStream* stream,
                                                  void*& data,
                                                  data::v_io_size& bytesLeftToRead,
                                                  const oatpp::async::Action& nextAction);

/**
 * Read exact amount of bytes to stream
 * returns exact amount of bytes was read.
 * return result can be < size only in case of some disaster like connection reset by peer
 */
oatpp::data::v_io_size readExactSizeData(oatpp::data::stream::InputStream* stream, void* data, data::v_io_size size);
  
/**
 * Write exact amount of bytes to stream.
 * returns exact amount of bytes was written.
 * return result can be < size only in case of some disaster like broken pipe
 */
oatpp::data::v_io_size writeExactSizeData(oatpp::data::stream::OutputStream* stream, const void* data, data::v_io_size size);
  
}}}

#endif /* defined(_data_Stream) */
