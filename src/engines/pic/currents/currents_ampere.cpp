/**
 * @file currents_ampere.cpp
 * @brief E^(n+1) = E' - 4 pi * dt * J
 * @implements: `AmpereCurrents` method of the `PIC` class
 * @includes: `currents_ampere.hpp
 * @depends: `pic.h`
 *
 * @notes: - minus sign in the current is included with the `coeff`, ...
 *           ... so the kernel adds `coeff * J`
 *         - charge renormalization is done to keep the charge density ...
 *           ... independent of the resolution and `ppc0`
 *
 */

#include "currents_ampere.hpp"

#include "wrapper.h"

#include "pic.h"

#include "io/output.h"

namespace ntt {

#ifdef MINKOWSKI_METRIC

  /**
   * @brief Add currents to the E-field
   */
  template <Dimension D>
  void PIC<D>::AmpereCurrents() {
    auto&      mblock = this->meshblock;
    auto       params = *(this->params());
    const auto coeff  = -mblock.timestep() * params.q0() * params.n0() /
                       (params.B0() * params.V0());
    const auto inv_n0 = ONE / params.n0();
    Kokkos::parallel_for("AmpereCurrents",
                         mblock.rangeActiveCells(),
                         CurrentsAmpere_kernel<D>(mblock, coeff, inv_n0));

    NTTLog();
  }
#else

  /**
   * @brief Add currents to the E-field
   */
  template <Dimension D>
  void PIC<D>::AmpereCurrents() {
    auto&      mblock = this->meshblock;
    auto       params = *(this->params());
    const auto coeff  = -mblock.timestep() * params.q0() * params.n0() /
                       params.B0();
    const auto inv_n0 = ONE / params.n0();

    range_t<D> range;
    // skip the axis
    if constexpr (D == Dim1) {
      range = CreateRangePolicy<Dim1>({ mblock.i1_min() }, { mblock.i1_max() });
    } else if constexpr (D == Dim2) {
      range = CreateRangePolicy<Dim2>({ mblock.i1_min(), mblock.i2_min() + 1 },
                                      { mblock.i1_max(), mblock.i2_max() });
    } else if constexpr (D == Dim3) {
      range = CreateRangePolicy<Dim3>(
        { mblock.i1_min(), mblock.i2_min() + 1, mblock.i3_min() },
        { mblock.i1_max(), mblock.i2_max(), mblock.i3_max() });
    }

    /**
     *    . . . . . . . . . . . . .
     *    .                       .
     *    .                       .
     *    .   ^= = = = = = = =^   .
     *    .   |  * * * * * * *\   .
     *    .   |  * * * * * * *\   .
     *    .   |  * * * * * * *\   .
     *    .   |  * * * * * * *\   .
     *    .   ^- - - - - - - -^   .
     *    .                       .
     *    .                       .
     *    . . . . . . . . . . . . .
     *
     */
    Kokkos::parallel_for("AmpereCurrents-1",
                         range,
                         CurrentsAmpere_kernel<D>(mblock, coeff, inv_n0));
    // do axes separately
    if constexpr (D == Dim2) {
      /**
       *    . . . . . . . . . . . . .
       *    .                       .
       *    .                       .
       *    .   ^= = = = = = = =^   .
       *    .   |*              \*  .
       *    .   |*              \*  .
       *    .   |*              \*  .
       *    .   |*              \*  .
       *    .   ^- - - - - - - -^   .
       *    .                       .
       *    .                       .
       *    . . . . . . . . . . . . .
       *
       */
      Kokkos::parallel_for(
        "AmpereCurrents-2",
        CreateRangePolicy<Dim1>({ mblock.i1_min() }, { mblock.i1_max() }),
        CurrentsAmperePoles_kernel<Dim2>(mblock, coeff, inv_n0));
    }

    NTTLog();
  }
#endif
} // namespace ntt

template void ntt::PIC<ntt::Dim1>::AmpereCurrents();
template void ntt::PIC<ntt::Dim2>::AmpereCurrents();
template void ntt::PIC<ntt::Dim3>::AmpereCurrents();
