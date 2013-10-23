// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert> // assert
#include <cstddef> // ptrdiff_t, size_t
#include <new> // new
#include <stdexcept> //invalid arg
// ---------
// Allocator
// ---------

template <typename T, int N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T value_type;

        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        typedef value_type* pointer;
        typedef const value_type* const_pointer;

        typedef value_type& reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;} // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}
        
        //----------------------
        //method view, char->int
        //----------------------

        int& view (char& c) {
          return *reinterpret_cast<int*>(&c);}

        //----------------------
        //method view, int->char
        //----------------------

        char& view1 (int& i) {
          return *reinterpret_cast<char*>(&i);}

    private:
        // ----
        // data
        // ----

        char a[N];
        const static size_type t_size= sizeof(T);
        const static size_type sntl_size= sizeof(size_type);  //sentinel size
        const static size_type min_blk= 2*(sntl_size)+t_size; //min space req for an allocate

        // --------
        // zero_set
        // --------

        /**
        * sets stnl_size-1 elements to 0 at a[index]
        */
        void zero_set(size_type index) {
          int k=1;
          while(k < stnl_size) {
            a[index+k]=0;
            ++k;
          }
          return;
        }

        // -----
        // valid
        // -----

        /**
* O(1) in space
* O(n) in time
* <your documentation>
*/
        bool valid () const {
            int i=0;
            int str_sntl=0;
            int end_sntl=0;
            bool str_neg=false;

            while(i < N) {
              str_sntl=view( (a+i) );
              if(str_sntl < 0) {
                str_sntl-=(2*str_stnl); //look for easy negate
                str_neg=true;
              }
              end_sntl=view( a+(i+stnl_size+str_stnl) );
              if(end_sntl < 0) {
                end_sntl-=(2*end_stnl); //again, look for easy (-)
                end_neg=true;
              }
              if( (str_sntl != end_sntl) || ( end_neg != str_neg ) ) //or..?
                return false;
              i+=str_sntl+(2*sntl_size);
            }
            if(i!=N)
              return false;
            return true;}

    public:
        // ------------
        // constructors
        // ------------

        /**
* O(1) in space
* O(1) in time
* <your documentation>
*/
        Allocator () {
            int stnli=N-(2*stnl_size);
            char* stnlc = &view1(stnli);
            a[0]=*stnlc;
            zero_set(0);
            a[stnli+stnl_size]=*stnlc;
            zero_set(stnli+stnlsize);
            assert(valid());}

        // Default copy, destructor, and copy assignment
        // Allocator (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
* O(1) in space
* O(n) in time
* <your documentation>
* after allocation there must be enough space left for a valid block
* the smallest allowable block is sizeof(T) + (2 * sizeof(int))
* choose the first block that fits
*/
        pointer allocate (size_type n) {
            assert(n > 0);
            int i=0;
            int k=1;

            int tmp=view(a);		//view(a[0]);?
            int spc=n * t_size;		//blocks requested
            int tmp_spc=0;
            
            while(i < N) {
              tmp=view(a[i]);		//view(a+i)?

              if(tmp > 0 && (tmp-spc) >= min_blk) {
                tmp_spc-= spc;
                a[i]=*reinterpret_cast<char*>(&tmp_spc);
                a[i + stnl_size + spc]=*reinterpret_cast<char*>(&tmp_spc);

                tmp-=(spc + stnl_size + stnl_size);
                a[i + (2 * stnl_size)]=*reinterpret_cast<char*>(&tmp);
                a[i + (3 * stnl_size) + tmp]=*reinterpret_cast<char*>(&tmp);
                assert(valid());
                return reinterpret_cast<pointer)(&a[i + stnl_size]);
              }
              i=abs(tmp) + (2 * stnl_size);
            }
            throw std::bad_alloc(); //if no return w/in while, alloc failed
            return 0;} // replace!

        // ---------
        // construct
        // ---------

        /**
* O(1) in space
* O(1) in time
* <your documentation>
*/
        void construct (pointer p, const_reference v) {
            new (p) T(v); // this is correct and exempt
            assert(valid());} // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
* O(1) in space
* O(1) in time
* <your documentation>
* after deallocation adjacent free blocks must be coalesced
*/
        void deallocate (pointer p, size_type) {
            int i=(reintrpret_cast<char*>(p) - &a[0]);            
            assert(&a[i] == reinterpret_cast<char*>(p));
            int sntlV=view(a[i - stnl_size]);
            assert(sntlV < 0);

            stnlV=abs(sntlV);
            a[i - stnl_size] = *reinterpret_cast<char*>(&sntlV);
            a[i + sntlV] = *reinterpret_cast<char*>(&sntlV);
            assert(valid());

            i=0;
            int tmp;
            while(i<N) {
              tmp=view(a[i]);
              if(tmp>0) {

              }
           }
        }

        // -------
        // destroy
        // -------

        /**
* O(1) in space
* O(1) in time
* <your documentation>
*/
        void destroy (pointer p) {
            p->~T(); // this is correct
            assert(valid());}};

#endif // Allocator_h
