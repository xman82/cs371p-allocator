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

    private:
        // ----
        // data
        // ----

        char a[N];
        const static size_type t_size= sizeof(T);
        const static size_type sntl_size= sizeof(size_type);  //sentinel size
        const static size_type min_blk= 2*(sntl_size)+t_size; //min space req for an allocate

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
              str_sntl=view( *(a+i) );  //*(a+i) == a[i];
              if(str_sntl < 0) {
                str_sntl-=(2*str_stnl); //look for easy negate
                str_neg=true;
              }
              end_sntl=view( *(a+(i+stnl_size+str_stnl)) );
              if(end_sntl < 0) {
                end_sntl-=(2*end_stnl); //again, look for easy (-)
                end_neg=true;
              }
              if( (str_sntl != end_sntl) || ( end_neg != str_neg ) )
                return false;
              i+=str_sntl+(2*sntl_size);	//set i past (sentinel,block,sentinel)
            }
            if(i!=N)
              return false;
            return true;}

        //----------------------
        //method view, char->int
        //----------------------

        int& view (char& c) {
          return *reinterpret_cast<int*>(&c);}

        //----------------------
        //method view, int->char
        //----------------------

        char& view (int& i) {
          return *reinterpret_cast<char*>(&i);}

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
            int stnlV=N-(2*stnl_size);
            a[0]=view(stnlV);			//set first sentinel
            zero_set(0);
            a[stnl_size + stnlV]=view(stnlV);	//set second sentinel
            //zero_set(stnli+stnlsize);//not sure if necessary...
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

            int tmp=0;
            int spc=n * t_size;		//blocks requested
            int tmp_spc=0;
            
            int i=0;
            int k=1;

            while(i < N) {
              tmp=view(*(a+i));				//next block's sentinel value

              if(tmp > 0 && (tmp - spc ) >= min_blk) {
                tmp_spc-=spc;				//new sentinel's value

                a[i]=view(tmp_spc);
                a[i + stnl_size + spc]=view(tmp_spc);

                tmp-=(stnl_size + spc + stnl_size);	//remainder free blocks
                a[i + stnl_size + spc + stnl_size]=view(tmp);
                a[i + stnl_size + spc + stnl_size + stnl_size + tmp]=view(tmp);

                assert(valid());
                return reinterpret_cast<pointer>(&a[i + stnl_size]);
              }
              i=stnl_size + abs(tmp) + stnl_size;
            }
            i=0;
            tmp_spc=0;
            while(i < N) {
              tmp=view(*(a+i));
              if(tmp > 0 && (tmp - spc) >= 0) {		//less than minblk would remain
                tmp_spc-= tmp;
                a[i]=view(tmp_spc);			//give the entire free block in this case
                a[i + stnl_size + tmp]=view(tmp_spc);

                assert(valid());
                return reinterpret_cast<pointer>(&a[i + stnl_size]);
              }
            }
            throw std::bad_alloc(); //if no return w/in while, alloc failed
            return 0;}

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
* the sentinel values of the block are reset to positive
* the previous and next blocks (if they exist) are checked
* if either block is a free block, it is coalesced with the
* current block which begins at p 
* after deallocation adjacent free blocks must be coalesced
*/
        void deallocate (pointer p, size_type) {
            int i=reinterpret_cast<char*>(p) - a;            
            assert(&a[i] == reinterpret_cast<char*>(p));
	    assert(i > stnl_size);
            int sntlV=view(a[i - stnl_size]);
            assert(sntlV < 0);

            stnlV=abs(sntlV);
            int tmp_sntlV=0;
            if((i-stnl_size) >= minblk && view(a[i-(2*sntl_size)]) >= 0) {
              tmp_sntlV=view(a[i - (2*stnl_size)]);
              assert(tmp_sntlV == view(a[i - (3*stnl_size) - tmp_sntlV]));
              
              sntlV+=(2 * sntl_size) + tmp_sntlV;			//sntlV now size of prev block & current (just deallocated) block
              i-= (2 * stnl_size - tmp_sntlV);				//i now set to beginning of new block (from prev + current)
            }

            if(N-(i+sntlV+sntl_size) >= minblk && view(a[i + sntlV + sntl_size]) >= 0) {
              tmp_sntlV=view(a[i + sntlV + sntl_size]);
              assert(tmp_sntlV == view(a[i + tmp_sntlV + (2 * sntl_size)]));

              sntlV+=(2 * sntl_size) + tmp_sntlV;			//sntlV now size of current(just deallocated) & next block
	    }

            a[i - stnl_size] = view(sntlV);
            a[i + sntlV] = view(sntlV);
            assert(valid());
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

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        const int& view (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
