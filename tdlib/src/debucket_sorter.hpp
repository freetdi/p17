//
//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//
//
// Revision History:
//   13 June 2001: Changed some names for clarity. (Jeremy Siek)
//   01 April 2001: Modified to use new <boost/limits.hpp> header. (JMaddock)
//
//   2016: modified by Lukas Larisch
//         then hacked by Felix Salfelder
//         this is now a deque sorter...
//         (anyway, it doesn't help yet)
//
#ifndef BOOST_GRAPH_DETAIL_BUCKET_SORTER_HPP
#define BOOST_GRAPH_DETAIL_BUCKET_SORTER_HPP

#include <vector>
#include <cassert>
#include <boost/limits.hpp>
#include "trace.hpp"

namespace boost {

  template <class BucketType, class ValueType, class Bucket, 
            class ValueIndexMap>
  class bucket_sorter {
  public:
    typedef BucketType bucket_type;
    typedef ValueType value_type;
    typedef typename std::vector<value_type>::size_type size_type;
    
    bucket_sorter(size_type _length, bucket_type _max_bucket, 
                  const Bucket& _bucket = Bucket(), 
                  const ValueIndexMap& _id = ValueIndexMap()) 
      : head(_max_bucket, invalid_value()),
        tail(_max_bucket, invalid_value()),
        next(_length, invalid_value()), 
        prev(_length, invalid_value()),
        id_to_value(_length),
        bucket(_bucket), id(_id) { }
    
    void remove(const value_type& x) {
      const size_type i = get(id, x);
      const size_type& next_node = next[i];
      const size_type& prev_node = prev[i];
    
      //check if i is the end of the bucket list 
      if ( next_node != invalid_value() )
        prev[next_node] = prev_node; 
      //check if i is the begin of the bucket list
      if ( prev_node != invalid_value() )
        next[prev_node] = next_node;
      else //need update head of current bucket list
        head[ bucket[x] ] = next_node;
    }

    void push(const value_type& x) {
      id_to_value[get(id, x)] = x;
      (*this)[bucket[x]].push(x);
    }

    void push_back(const value_type& x) { untested();
      id_to_value[get(id, x)] = x;
      (*this)[bucket[x]].push_back(x);
    }
    
    void update(const value_type& x) {
      remove(x);
      (*this)[bucket[x]].push(x);
    }
    //  private: 
    //    with KCC, the nested stack class is having access problems
    //    despite the friend decl.
    static size_type invalid_value() {
      return (std::numeric_limits<size_type>::max)();
    }
    
    typedef typename std::vector<size_type>::iterator Iter;
    typedef typename std::vector<value_type>::iterator IndexValueMap;
    
  public:
    friend class stack;

    class stack {
    public:
      stack(bucket_type _bucket_id, Iter h, Iter t, Iter n, Iter p, IndexValueMap v,
            const ValueIndexMap& _id)
      : bucket_id(_bucket_id), head(h), tail(t), next(n), prev(p), value(v), id(_id) {}

      // Avoid using default arg for ValueIndexMap so that the default
      // constructor of the ValueIndexMap is not required if not used.
      stack(bucket_type _bucket_id, Iter h, Iter t, Iter n, Iter p, IndexValueMap v)
        : bucket_id(_bucket_id), head(h), tail(t), next(n), prev(p), value(v) {}
      
      void push(const value_type& x) {
         return push_front(x);
      }
      void push_front(const value_type& x) {
        const size_type new_head = get(id, x);
        const size_type current = head[bucket_id];
        if ( current != invalid_value() ){
          prev[current] = new_head;
        }else{
          tail[bucket_id] = new_head;
        }
        prev[new_head] = invalid_value();
        next[new_head] = current;
        head[bucket_id] = new_head;
      }
      void push_back(const value_type& x) {
        const size_type new_tail = get(id, x);
        const size_type current = tail[bucket_id];

        if ( current != invalid_value() ){ untested();
          next[current] = new_tail;
        }else{ untested();
          head[bucket_id] = new_tail;
        }
        next[new_tail] = invalid_value();
        prev[new_tail] = current;
        tail[bucket_id] = new_tail;
      }
      void pop() {
			return pop_front();
		}
      void pop_front() {
        size_type current = head[bucket_id];
        size_type next_node = next[current];
        head[bucket_id] = next_node;
        if ( next_node != invalid_value() ){ untested();
          prev[next_node] = invalid_value();
        }else{ untested();
          tail[bucket_id] = invalid_value();
          head[bucket_id] = invalid_value();
        }
      }
      void pop_back() {
        size_type current = tail[bucket_id];
        size_type prev_node = prev[current];

        tail[bucket_id] = prev_node;
        if ( prev_node != invalid_value() ){ untested();
          next[prev_node] = invalid_value();
        }else{ untested();
          head[bucket_id] = invalid_value();
          tail[bucket_id] = invalid_value();
        }
      }
      value_type& top() { return value[ head[bucket_id] ]; }
      const value_type& top() const { return value[ head[bucket_id] ]; }
      value_type& bottom() { return value[ tail[bucket_id] ]; }
      const value_type& bottom() const { return value[ tail[bucket_id] ]; }
      bool empty() const { return head[bucket_id] == invalid_value(); }
      unsigned size() const{ return head.size(); }
    private:
      bucket_type bucket_id;
      Iter head;
      Iter tail;
      Iter next;
      Iter prev;
      IndexValueMap value;
      ValueIndexMap id;
    };
    
    stack operator[](const bucket_type& i) {
      assert(i < head.size());
      return stack(i, head.begin(), tail.begin(), next.begin(), prev.begin(),
                   id_to_value.begin(), id);
    }
  protected:
    std::vector<size_type>   head;
    std::vector<size_type>   tail;
    std::vector<size_type>   next;
    std::vector<size_type>   prev;
    std::vector<value_type>  id_to_value;
    Bucket bucket;
    ValueIndexMap id;
  };
  
}

#endif
