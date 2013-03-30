#pragma once



#include <thrust/detail/config.h>

// get the execution policies definitions first
#include <thrust/system/hpx/detail/execution_policy.h>

// get the definition of par
#include <thrust/system/hpx/detail/par.h>

// now get all the algorithm definitions

#include <thrust/system/hpx/adjacent_difference.h>


// define these entities here for the purpose of Doxygenating them
// they are actually defined elsewhere
#if 0
namespace thrust
{
namespace system
{
namespace hpx
{


/*! \addtogroup execution_policies
 *  \{
 */


/*! \p thrust::hpx::execution_policy is the base class for all Thrust parallel execution
 *  policies which are derived from Thrust's HPX backend system.
 */
template<typename DerivedPolicy>
struct execution_policy : thrust::execution_policy<DerivedPolicy>
{};


/*! \p hpx::tag is a type representing Thrust's standard C++ backend system in C++'s type system.
 *  Iterators "tagged" with a type which is convertible to \p hpx::tag assert that they may be
 *  "dispatched" to algorithm implementations in the \p hpx system.
 */
struct tag : thrust::system::hpx::execution_policy<tag> { unspecified };


/*! \p thrust::hpx::par is the parallel execution policy associated with Thrust's HPX
 *  backend system.
 *
 *  Instead of relying on implicit algorithm dispatch through iterator system tags, users may
 *  directly target Thrust's HPX backend system by providing \p thrust::hpx::par as an algorithm
 *  parameter.
 *
 *  Explicit dispatch can be useful in avoiding the introduction of data copies into containers such
 *  as \p thrust::hpx::vector.
 *
 *  The type of \p thrust::hpx::par is implementation-defined.
 *
 *  The following code snippet demonstrates how to use \p thrust::hpx::par to explicitly dispatch an
 *  invocation of \p thrust::for_each to the HPX backend system:
 *
 *  \code
 *  #include <thrust/for_each.h>
 *  #include <thrust/system/hpx/execution_policy.h>
 *  #include <cstdio>
 *
 *  struct printf_functor
 *  {
 *    __host__ __device__
 *    void operator()(int x)
 *    {
 *      printf("%d\n");
 *    }
 *  };
 *  ...
 *  int vec[3];
 *  vec[0] = 0; vec[1] = 1; vec[2] = 2;
 *
 *  thrust::for_each(thrust::hpx::par, vec.begin(), vec.end(), printf_functor());
 *
 *  // 0 1 2 is printed to standard output in some unspecified order
 *  \endcode
 */
static const unspecified par;

} // end cpp
} // end system
} // end thrust
#endif


