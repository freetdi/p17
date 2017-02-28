// an example allocator
// (does it help?)

#ifndef MY_ALLOC_H
#define MY_ALLOC_H
#include <cstddef> // ptrdiff_t
#include <limits> // numeric_limits
#include <assert.h> // numeric_limits
#include <stack>
#include <gala/trace.h>

template<size_t S>
class MyPool {
	public:
		MyPool(size_t n=0)
 		    : _prealloc(NULL),
 		      _prealloc_end(NULL),
 		      _prealloc_seek(NULL)
		{
			if (n){
			  	prealloc(n);
			}
		}
		void free(void* x){
			_trash.push_front((void*)x);
		}
		void* alloc(){
			if(_prealloc_seek<_prealloc_end){ itested();
				void* s=_prealloc_seek;
				_prealloc_seek = (void*) (S + intptr_t(_prealloc_seek));
				return s;
			}else if(!_trash.empty()){ itested();
				void* t=_trash.front();
			  	_trash.pop_front();
				return t;
			}else{
				// unreachable if properly initialized.
				return malloc(S);
			}
		}
		~MyPool()
		{
		}
		void prealloc(size_t required)
		{
			size_t allocated = ((intptr_t) _prealloc_end - (intptr_t) _prealloc);
			if(allocated<S*required){
				trace1("prealloc", required);
				_trash.clear();
				::free(_prealloc);
				_prealloc = malloc(S*required);
				_prealloc_seek = _prealloc;
				_prealloc_end = (void*) ( (intptr_t)_prealloc + S*required );
			}else{
			}

		}
private:
	std::deque<void*> _trash;
	void* _prealloc;
	void* _prealloc_end;
	void* _prealloc_seek;
};

template<typename T, unsigned S=sizeof(T), MyPool<S>* P=(MyPool<sizeof(T)>*)NULL>
class POOL_ALLOC {
public:
	static MyPool<sizeof(T) > *pMyPool; // in case of NULL

	typedef size_t     size_type;
	typedef ptrdiff_t  difference_type;
	typedef T*         pointer;
	typedef const T*   const_pointer;
	typedef T&         reference;
	typedef const T&   const_reference;
	typedef T          value_type;

	template<typename X>
	struct rebind{ //
		typedef POOL_ALLOC<X, S, P> other;
	};

	POOL_ALLOC() throw()
	{
		trace2("::POOL_ALLOC", sizeof(T), intptr_t(this)%2003);
	//	pMyPool = P; // HACK
	}
	POOL_ALLOC(const POOL_ALLOC&) throw()
	{ untested();
	}
	template<typename X>
	POOL_ALLOC(const POOL_ALLOC<X>&) throw()
	{ untested();
	}
	~POOL_ALLOC() throw() { }

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }

	pointer allocate(size_type n, const void * hint = 0)
	{
		if (n!=1){
			assert(!pMyPool);
			pMyPool = new MyPool<sizeof(T)>(n);
//			perror("POOL_ALLOC::allocate: n is not 1.\n");
		}else if (P /* GCC BUG! */){ itested();
			return reinterpret_cast<T*>(P->alloc());
		}else if (unlikely(!pMyPool)){ untested();
			// inefficient/stupid.
			trace1("new pool", sizeof(T));
			pMyPool = new MyPool<sizeof(T)>();
		}else{
		}
		return reinterpret_cast<T*>(pMyPool->alloc());
	}

	void deallocate(pointer p, size_type n)
  	{
		assert(p);
		assert(pMyPool);
		if(P){
			P->free(reinterpret_cast<void *>(p));
		}else{
			pMyPool->free(reinterpret_cast<void *>(p));
		}
	}

	size_type max_size() const throw()
  	{
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	void construct(pointer p, const T& val)
  	{ untested();
		::new(p) T(val);
	}

	void destroy(pointer p)
	{ untested();
		p->~T();
	}
};

template<typename T>
inline bool operator==(const POOL_ALLOC<T>&, const POOL_ALLOC<T>&)
{
  return true;
}

template<typename T>
inline bool operator!=(const POOL_ALLOC<T>&, const POOL_ALLOC<T>&)
{
  return false;
}

#define PA_ARGS typename T, unsigned S, MyPool<S>* P
template<PA_ARGS>
MyPool<sizeof(T)>* POOL_ALLOC<T, S, P>::pMyPool = NULL;
#endif
