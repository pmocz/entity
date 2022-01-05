#ifndef OBJECTS_GEOMETRY_GRID_H
#define OBJECTS_GEOMETRY_GRID_H

#include "global.h"

#include <tuple>
#include <string>

namespace ntt {

  template <Dimension D>
  struct CoordinateGrid {
    const std::string label;
    const long int Nx1, Nx2, Nx3;
    const real_t x1_min, x1_max;
    const real_t x2_min, x2_max;
    const real_t x3_min, x3_max;

    CoordinateGrid(const std::string& label_,
                   std::vector<std::size_t> resolution,
                   std::vector<real_t> extent)
        : label {label_},
          Nx1 {resolution.size() > 0 ? resolution[0] : 1},
          Nx2 {resolution.size() > 1 ? resolution[1] : 1},
          Nx3 {resolution.size() > 2 ? resolution[2] : 1},
          x1_min {resolution.size() > 0 ? extent[0] : ZERO},
          x1_max {resolution.size() > 0 ? extent[1] : ZERO},
          x2_min {resolution.size() > 1 ? extent[2] : ZERO},
          x2_max {resolution.size() > 1 ? extent[3] : ZERO},
          x3_min {resolution.size() > 2 ? extent[4] : ZERO},
          x3_max {resolution.size() > 2 ? extent[5] : ZERO} {}
    virtual ~CoordinateGrid() = default;

    // coordinate transformations
    Inline auto CU_to_Idi(const real_t& xi) const -> std::pair<long int, float> {
      // TODO: this is a hack
      auto i {static_cast<long int>(xi + N_GHOSTS)};
      float di {static_cast<float>(xi) - static_cast<float>(i)};
      return {i, di};
    }

    // conversion from code units (CU) to cartesian (Cart)
    virtual Inline auto coord_CU_to_Cart(const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto coord_CU_to_Cart(const real_t&, const real_t&) const -> std::tuple<real_t, real_t> {
      return {-1.0, -1.0}; }
    virtual Inline auto coord_CU_to_Cart(const real_t&, const real_t&, const real_t&) const -> std::tuple<real_t, real_t, real_t> { return {-1.0, -1.0, -1.0}; }

    // conversion from cartesian (Cart) to code units (CU)
    virtual Inline auto coord_CART_to_CU(const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto coord_CART_to_CU(const real_t&, const real_t&) const -> std::tuple<real_t, real_t> { return {-1.0, -1.0}; }
    virtual Inline auto coord_CART_to_CU(const real_t&, const real_t&, const real_t&) const -> std::tuple<real_t, real_t, real_t> { return {-1.0, -1.0, -1.0}; }

    // conversion from code units (CU) to spherical (Sph)
    virtual Inline auto coord_CU_to_Sph(const real_t&, const real_t&) const -> std::tuple<real_t, real_t> { return {-1.0, -1.0}; }
    virtual Inline auto coord_CU_to_Sph(const real_t&, const real_t&, const real_t&) const -> std::tuple<real_t, real_t, real_t> { return {-1.0, -1.0, -1.0}; }

    // // velocity conversion
    // virtual Inline auto transform_ux1TOux(const real_t&) const -> real_t { return -1.0; }
    // virtual Inline auto transform_ux1ux2TOuxuy(const real_t&, const real_t&) const -> std::tuple<real_t, real_t> { return {-1.0, -1.0}; }
    // virtual Inline auto transform_ux1ux2ux3TOuxuyuz(const real_t&, const real_t&, const real_t&) const -> std::tuple<real_t, real_t, real_t> { return {-1.0, -1.0, -1.0}; }

    // virtual Inline auto transform_uxTOux1(const real_t&) const -> real_t { return -1.0; }
    // virtual Inline auto transform_uxuyTOux1ux2(const real_t&, const real_t&) const -> std::tuple<real_t, real_t> { return {-1.0, -1.0}; };
    // virtual Inline auto transform_uxuyuzTOux1ux2ux3(const real_t&, const real_t&, const real_t&) const -> std::tuple<real_t, real_t, real_t> { return {-1.0, -1.0, -1.0}; }

    virtual Inline auto h11(const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto h11(const real_t&, const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto h11(const real_t&, const real_t&, const real_t&) const -> real_t { return -1.0; }

    virtual Inline auto h22(const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto h22(const real_t&, const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto h22(const real_t&, const real_t&, const real_t&) const -> real_t { return -1.0; }

    virtual Inline auto h33(const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto h33(const real_t&, const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto h33(const real_t&, const real_t&, const real_t&) const -> real_t { return -1.0; }

    virtual Inline auto sqrt_det_h(const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto sqrt_det_h(const real_t&, const real_t&) const -> real_t { return -1.0; }
    virtual Inline auto sqrt_det_h(const real_t&, const real_t&, const real_t&) const -> real_t { return -1.0; }

    virtual Inline auto polar_area(const real_t&, const real_t&) const -> real_t { return -1.0; }

    // CNT -> contravariant (upper index)
    // CVR -> covariant (lower index)
    // HAT -> local orthonormal (hatted index)
    //
    // CNT -> HAT
    Inline auto vec_CNT_to_HAT_x1(const real_t& ax1, const real_t& x1) -> real_t {
      return std::sqrt(h11(x1)) * ax1;
    }
    Inline auto vec_CNT_to_HAT_x1(const real_t& ax1, const real_t& x1, const real_t& x2) -> real_t {
      return std::sqrt(h11(x1, x2)) * ax1;
    }
    Inline auto vec_CNT_to_HAT_x1(const real_t& ax1, const real_t& x1, const real_t& x2, const real_t& x3) -> real_t {
      return std::sqrt(h11(x1, x2, x3)) * ax1;
    }

    Inline auto vec_CNT_to_HAT_x2(const real_t& ax2, const real_t& x1) -> real_t {
      return std::sqrt(h22(x1)) * ax2;
    }
    Inline auto vec_CNT_to_HAT_x2(const real_t& ax2, const real_t& x1, const real_t& x2) -> real_t {
      return std::sqrt(h22(x1, x2)) * ax2;
    }
    Inline auto vec_CNT_to_HAT_x2(const real_t& ax2, const real_t& x1, const real_t& x2, const real_t& x3) -> real_t {
      return std::sqrt(h22(x1, x2, x3)) * ax2;
    }

    Inline auto vec_CNT_to_HAT_x3(const real_t& ax3, const real_t& x1) -> real_t {
      return std::sqrt(h33(x1)) * ax3;
    }
    Inline auto vec_CNT_to_HAT_x3(const real_t& ax3, const real_t& x1, const real_t& x2) -> real_t {
      return std::sqrt(h33(x1, x2)) * ax3;
    }
    Inline auto vec_CNT_to_HAT_x3(const real_t& ax3, const real_t& x1, const real_t& x2, const real_t& x3) -> real_t {
      return std::sqrt(h33(x1, x2, x3)) * ax3;
    }

    // LOC -> CNT
    Inline auto vec_HAT_to_CNT_x1(const real_t& ax1, const real_t& x1) -> real_t {
      return ax1 / std::sqrt(h11(x1));
    }
    Inline auto vec_HAT_to_CNT_x1(const real_t& ax1, const real_t& x1, const real_t& x2) -> real_t {
      return ax1 / std::sqrt(h11(x1, x2));
    }
    Inline auto vec_HAT_to_CNT_x1(const real_t& ax1, const real_t& x1, const real_t& x2, const real_t& x3) -> real_t {
      return ax1 / std::sqrt(h11(x1, x2, x3));
    }

    Inline auto vec_HAT_to_CNT_x2(const real_t& ax2, const real_t& x1) -> real_t {
      return ax2 / std::sqrt(h22(x1));
    }
    Inline auto vec_HAT_to_CNT_x2(const real_t& ax2, const real_t& x1, const real_t& x2) -> real_t {
      return ax2 / std::sqrt(h22(x1, x2));
    }
    Inline auto vec_HAT_to_CNT_x2(const real_t& ax2, const real_t& x1, const real_t& x2, const real_t& x3) -> real_t {
      return ax2 / std::sqrt(h22(x1, x2, x3));
    }

    Inline auto vec_HAT_to_CNT_x3(const real_t& ax3, const real_t& x1) -> real_t {
      return ax3 / std::sqrt(h33(x1));
    }
    Inline auto vec_HAT_to_CNT_x3(const real_t& ax3, const real_t& x1, const real_t& x2) -> real_t {
      return ax3 / std::sqrt(h33(x1, x2));
    }
    Inline auto vec_HAT_to_CNT_x3(const real_t& ax3, const real_t& x1, const real_t& x2, const real_t& x3) -> real_t {
      return ax3 / std::sqrt(h33(x1, x2, x3));
    }
  };

} // namespace ntt

#endif
