/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Exceptions.hh"
#include "orc/Vector.hh"
#include "orc/Adaptor.hh"

#include <iostream>
#include <sstream>
#include <cstdlib>

namespace orc {

  ColumnVectorBatch::ColumnVectorBatch(uint64_t cap,
                                       MemoryPool& pool
                                       ): capacity(cap),
                                          numElements(0),
                                          notNull(pool, cap),
                                          hasNulls(false),
                                          memoryPool(pool) {
    // PASS
  }

  ColumnVectorBatch::~ColumnVectorBatch() {
    // PASS
  }

  void ColumnVectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      capacity = cap;
      notNull.resize(cap);
    }
  }

  LongVectorBatch::LongVectorBatch(uint64_t capacity, MemoryPool& pool
                     ): ColumnVectorBatch(capacity, pool),
                        data(pool, capacity) {
    // PASS
  }

  LongVectorBatch::~LongVectorBatch() {
    // PASS
  }

  std::string LongVectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "Long vector <" << numElements << " of " << capacity << ">";
    return buffer.str();
  }

  void LongVectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      ColumnVectorBatch::resize(cap);
      data.resize(cap);
    }
  }

  DoubleVectorBatch::DoubleVectorBatch(uint64_t capacity, MemoryPool& pool
                   ): ColumnVectorBatch(capacity, pool),
                      data(pool, capacity) {
    // PASS
  }

  DoubleVectorBatch::~DoubleVectorBatch() {
    // PASS
  }

  std::string DoubleVectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "Double vector <" << numElements << " of " << capacity << ">";
    return buffer.str();
  }

  void DoubleVectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      ColumnVectorBatch::resize(cap);
      data.resize(cap);
    }
  }

  StringVectorBatch::StringVectorBatch(uint64_t capacity, MemoryPool& pool
               ): ColumnVectorBatch(capacity, pool),
                  data(pool, capacity),
                  length(pool, capacity) {
    // PASS
  }

  StringVectorBatch::~StringVectorBatch() {
    // PASS
  }

  std::string StringVectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "Byte vector <" << numElements << " of " << capacity << ">";
    return buffer.str();
  }

  void StringVectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      ColumnVectorBatch::resize(cap);
      data.resize(cap);
      length.resize(cap);
    }
  }

  StructVectorBatch::StructVectorBatch(uint64_t cap, MemoryPool& pool
                                        ): ColumnVectorBatch(cap, pool) {
    // PASS
  }

  StructVectorBatch::~StructVectorBatch() {
    for (uint64_t i=0; i<this->fields.size(); i++) {
      delete this->fields[i];
    }
  }

  std::string StructVectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "Struct vector <" << numElements << " of " << capacity
           << "; ";
    for(std::vector<ColumnVectorBatch*>::const_iterator ptr=fields.begin();
        ptr != fields.end(); ++ptr) {
      buffer << (*ptr)->toString() << "; ";
    }
    buffer << ">";
    return buffer.str();
  }


  void StructVectorBatch::resize(uint64_t cap) {
    ColumnVectorBatch::resize(cap);
  }

  ListVectorBatch::ListVectorBatch(uint64_t cap, MemoryPool& pool
                   ): ColumnVectorBatch(cap, pool),
                      offsets(pool, cap+1) {
    // PASS
  }

  ListVectorBatch::~ListVectorBatch() {
    // PASS
  }

  std::string ListVectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "List vector <" << elements->toString() << " with "
           << numElements << " of " << capacity << ">";
    return buffer.str();
  }

  void ListVectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      ColumnVectorBatch::resize(cap);
      offsets.resize(cap + 1);
    }
  }

  MapVectorBatch::MapVectorBatch(uint64_t cap, MemoryPool& pool
                 ): ColumnVectorBatch(cap, pool),
                    offsets(pool, cap+1) {
    // PASS
  }

  MapVectorBatch::~MapVectorBatch() {
    // PASS
  }

  std::string MapVectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "Map vector <" << keys->toString() << ", "
           << elements->toString() << " with "
           << numElements << " of " << capacity << ">";
    return buffer.str();
  }

  void MapVectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      ColumnVectorBatch::resize(cap);
      offsets.resize(cap + 1);
    }
  }

  UnionVectorBatch::UnionVectorBatch(uint64_t cap, MemoryPool& pool
                                     ): ColumnVectorBatch(cap, pool),
                                        tags(pool, cap),
                                        offsets(pool, cap) {
    // PASS
  }

  UnionVectorBatch::~UnionVectorBatch() {
    for (uint64_t i=0; i < children.size(); i++) {
      delete children[i];
    }
  }

  std::string UnionVectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "Union vector <";
    for(size_t i=0; i < children.size(); ++i) {
      if (i != 0) {
        buffer << ", ";
      }
      buffer << children[i]->toString();
    }
    buffer << "; with " << numElements << " of " << capacity << ">";
    return buffer.str();
  }

  void UnionVectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      ColumnVectorBatch::resize(cap);
      tags.resize(cap);
      offsets.resize(cap);
    }
  }

  Decimal64VectorBatch::Decimal64VectorBatch(uint64_t cap, MemoryPool& pool
                 ): ColumnVectorBatch(cap, pool),
                    values(pool, cap),
                    readScales(pool, cap) {
    // PASS
  }

  Decimal64VectorBatch::~Decimal64VectorBatch() {
    // PASS
  }

  std::string Decimal64VectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "Decimal64 vector  with "
           << numElements << " of " << capacity << ">";
    return buffer.str();
  }

  void Decimal64VectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      ColumnVectorBatch::resize(cap);
      values.resize(cap);
      readScales.resize(cap);
    }
  }

  Decimal128VectorBatch::Decimal128VectorBatch(uint64_t cap, MemoryPool& pool
               ): ColumnVectorBatch(cap, pool),
                  values(pool, cap),
                  readScales(pool, cap) {
    // PASS
  }

  Decimal128VectorBatch::~Decimal128VectorBatch() {
    // PASS
  }

  std::string Decimal128VectorBatch::toString() const {
    std::ostringstream buffer;
    buffer << "Decimal128 vector  with "
           << numElements << " of " << capacity << ">";
    return buffer.str();
  }

  void Decimal128VectorBatch::resize(uint64_t cap) {
    if (capacity < cap) {
      ColumnVectorBatch::resize(cap);
      values.resize(cap);
      readScales.resize(cap);
    }
  }

  Decimal::Decimal(const Int128& _value,
                   int32_t _scale): value(_value), scale(_scale) {
    // PASS
  }

  Decimal::Decimal(const std::string& str) {
    std::size_t foundPoint = str.find(".");
    // no decimal point, it is int
    if(foundPoint == std::string::npos){
      value = Int128(str);
      scale = 0;
    }else{
      std::string copy(str);
      scale = static_cast<int32_t>(str.length() - foundPoint);
      value = Int128(copy.replace(foundPoint, 1, ""));
    }
  }

  std::string Decimal::toString() const {
    return value.toDecimalString(scale);
  }
}
