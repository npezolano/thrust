#pragma once

#include <thrust/detail/config.h>
#include <thrust/system/hpx/detail/execution_policy.h>
#include <thrust/memory.h>
#include <thrust/detail/type_traits.h>
#include <thrust/detail/allocator/malloc_allocator.h>
#include <ostream>

namespace thrust
{
namespace system
{
namespace hpx
{

template<typename> class pointer;

} // end hpx
} // end system
} // end thrust


/*! \cond
 */

// specialize std::iterator_traits to avoid problems with the name of
// pointer's constructor shadowing its nested pointer type
// do this before pointer is defined so the specialization is correctly
// used inside the definition
namespace std
{

template<typename Element>
  struct iterator_traits<thrust::system::hpx::pointer<Element> >
{
  private:
    typedef thrust::system::hpx::pointer<Element> ptr;

  public:
    typedef typename ptr::iterator_category       iterator_category;
    typedef typename ptr::value_type              value_type;
    typedef typename ptr::difference_type         difference_type;
    typedef ptr                                   pointer;
    typedef typename ptr::reference               reference;
}; // end iterator_traits

} // end std

/*! \endcond
 */


namespace thrust
{
namespace system
{

/*! \addtogroup system_backends Systems
 *  \ingroup system
 *  \{
 */

/*! \namespace thrust::system::hpx
 *  \brief \p thrust::system::hpx is the namespace containing functionality for allocating, manipulating,
 *         and deallocating memory available to Thrust's HPX backend system.
 *         The identifiers are provided in a separate namespace underneath <tt>thrust::system</tt>
 *         for import convenience but are also aliased in the top-level <tt>thrust::hpx</tt>
 *         namespace for easy access.
 *
 */
namespace hpx
{

// forward declaration of reference for pointer
template<typename Element> class reference;

/*! \cond
 */



template<typename T>
  class pointer
    : public thrust::pointer<
               T,
               thrust::system::hpx::tag,
               thrust::system::hpx::reference<T>,
               thrust::system::hpx::pointer<T>
             >
{
  /*! \cond
   */

  private:
    typedef thrust::pointer<
      T,
      thrust::system::hpx::tag,
      thrust::system::hpx::reference<T>,
      thrust::system::hpx::pointer<T>
    > super_t;

  /*! \endcond
   */

  public:
    // note that omp::pointer's member functions need __host__ __device__
    // to interoperate with nvcc + iterators' dereference member function

    /*! \p pointer's no-argument constructor initializes its encapsulated pointer to \c 0.
     */
    __host__ __device__
    pointer() : super_t() {}

    /*! This constructor allows construction of a <tt>pointer<const T></tt> from a <tt>T*</tt>.
     *
     *  \param ptr A raw pointer to copy from, presumed to point to a location in memory
     *         accessible by the \p omp system.
     *  \tparam OtherT \p OtherT shall be convertible to \p T.
     */
    template<typename OtherT>
    __host__ __device__
    explicit pointer(OtherT *ptr) : super_t(ptr) {}

    /*! This constructor allows construction from another pointer-like object with related type.
     *
     *  \param other The \p OtherPointer to copy.
     *  \tparam OtherPointer The system tag associated with \p OtherPointer shall be convertible
     *          to \p thrust::system::omp::tag and its element type shall be convertible to \p T.
     */
    template<typename OtherPointer>
    __host__ __device__
    pointer(const OtherPointer &other,
            typename thrust::detail::enable_if_pointer_is_convertible<
              OtherPointer,
              pointer
            >::type * = 0) : super_t(other) {}

    /*! Assignment operator allows assigning from another pointer-like object with related type.
     *
     *  \param other The other pointer-like object to assign from.
     *  \tparam OtherPointer The system tag associated with \p OtherPointer shall be convertible
     *          to \p thrust::system::omp::tag and its element type shall be convertible to \p T.
     */
    template<typename OtherPointer>
    __host__ __device__
    typename thrust::detail::enable_if_pointer_is_convertible<
      OtherPointer,
      pointer,
      pointer &
    >::type
    operator=(const OtherPointer &other)
    {
      return super_t::operator=(other);
    }
}; // end pointer


/*! \p reference is a wrapped reference to an object stored in memory available to the \p omp system.
 *  \p reference is the type of the result of dereferencing a \p omp::pointer.
 *
 *  \tparam T Specifies the type of the referenced object.
 */
template<typename T>
  class reference
    : public thrust::reference<
               T,
               thrust::system::hpx::pointer<T>,
               thrust::system::hpx::reference<T>
             >
{
  /*! \cond
   */

  private:
    typedef thrust::reference<
      T,
      thrust::system::hpx::pointer<T>,
      thrust::system::hpx::reference<T>
    > super_t;

  /*! \endcond
   */

  public:
    /*! \cond
     */

    typedef typename super_t::value_type value_type;
    typedef typename super_t::pointer    pointer;

    /*! \endcond
     */

    /*! This constructor initializes this \p reference to refer to an object
     *  pointed to by the given \p pointer. After this \p reference is constructed,
     *  it shall refer to the object pointed to by \p ptr.
     *
     *  \param ptr A \p pointer to copy from.
     */
    __host__ __device__
    explicit reference(const pointer &ptr)
      : super_t(ptr)
    {}

    /*! This constructor accepts a const reference to another \p reference of related type.
     *  After this \p reference is constructed, it shall refer to the same object as \p other.
     *
     *  \param other A \p reference to copy from.
     *  \tparam OtherT The element type of the other \p reference.
     *
     *  \note This constructor is templated primarily to allow initialization of <tt>reference<const T></tt>
     *        from <tt>reference<T></tt>.
     */
    template<typename OtherT>
    __host__ __device__
    reference(const reference<OtherT> &other,
              typename thrust::detail::enable_if_convertible<
                typename reference<OtherT>::pointer,
                pointer
              >::type * = 0)
      : super_t(other)
    {}

    /*! Copy assignment operator copy assigns from another \p reference of related type.
     *
     *  \param other The other \p reference to assign from.
     *  \return <tt>*this</tt>
     *  \tparam OtherT The element type of the other \p reference.
     */
    template<typename OtherT>
    reference &operator=(const reference<OtherT> &other);

    /*! Assignment operator assigns from a \p value_type.
     *
     *  \param x The \p value_type to assign from.
     *  \return <tt>*this</tt>
     */
    reference &operator=(const value_type &x);
}; // end reference

/*! Exchanges the values of two objects referred to by \p reference.
 *  \p x The first \p reference of interest.
 *  \p y The second \p reference ot interest.
 */
template<typename T>
__host__ __device__
void swap(reference<T> x, reference<T> y);

/*! Allocates an area of memory available to Thrust's <tt>omp</tt> system.
 *  \param n Number of bytes to allocate.
 *  \return A <tt>omp::pointer<void></tt> pointing to the beginning of the newly
 *          allocated memory. A null <tt>omp::pointer<void></tt> is returned if
 *          an error occurs.
 *  \note The <tt>omp::pointer<void></tt> returned by this function must be
 *        deallocated with \p omp::free.
 *  \see omp::free
 *  \see std::malloc
 */
inline pointer<void> malloc(std::size_t n);

/*! Allocates a typed area of memory available to Thrust's <tt>omp</tt> system.
 *  \param n Number of elements to allocate.
 *  \return A <tt>omp::pointer<T></tt> pointing to the beginning of the newly
 *          allocated memory. A null <tt>omp::pointer<T></tt> is returned if
 *          an error occurs.
 *  \note The <tt>omp::pointer<T></tt> returned by this function must be
 *        deallocated with \p omp::free.
 *  \see omp::free
 *  \see std::malloc
 */
template<typename T>
inline pointer<T> malloc(std::size_t n);

/*! Deallocates an area of memory previously allocated by <tt>omp::malloc</tt>.
 *  \param ptr A <tt>omp::pointer<void></tt> pointing to the beginning of an area
 *         of memory previously allocated with <tt>omp::malloc</tt>.
 *  \see omp::malloc
 *  \see std::free
 */
inline void free(pointer<void> ptr);

// XXX upon c++11
// template<typename T> using allocator = thrust::detail::malloc_allocator<T,tag,pointer<T> >;

/*! \p omp::allocator is the default allocator used by the \p omp system's containers such as
 *  <tt>omp::vector</tt> if no user-specified allocator is provided. \p omp::allocator allocates
 *  (deallocates) storage with \p omp::malloc (\p omp::free).
 */
template<typename T>
  struct allocator
    : thrust::detail::malloc_allocator<
        T,
        tag,
        pointer<T>
      >
{
  /*! The \p rebind metafunction provides the type of an \p allocator
   *  instantiated with another type.
   *
   *  \tparam U The other type to use for instantiation.
   */
  template<typename U>
    struct rebind
  {
    /*! The typedef \p other gives the type of the rebound \p allocator.
     */
    typedef allocator<U> other;
  };

  /*! No-argument constructor has no effect.
   */
  __host__ __device__
  inline allocator() {}

  /*! Copy constructor has no effect.
   */
  __host__ __device__
  inline allocator(const allocator &) {}

  /*! Constructor from other \p allocator has no effect.
   */
  template<typename U>
  __host__ __device__
  inline allocator(const allocator<U> &) {}

  /*! Destructor has no effect.
   */
  __host__ __device__
  inline ~allocator() {}
}; // end allocator

} // end hpx

/*! \}
 */

} // end system

/*! \namespace thrust::omp
 *  \brief \p thrust::omp is a top-level alias for thrust::system::omp.
 */
namespace hpx
{

using thrust::system::hpx::pointer;
using thrust::system::hpx::reference;
using thrust::system::hpx::malloc;
using thrust::system::hpx::free;
using thrust::system::hpx::allocator;

} // end hpx

} // end thrust

#include <thrust/system/hpx/detail/memory.inl>

