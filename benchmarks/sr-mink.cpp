#include "wrapper.h"

#include "field_macros.h"
#include "pic.h"
#include "sim_params.h"

#include "io/cargs.h"
#include "io/input.h"
#include "meshblock/meshblock.h"

#include "utilities/archetypes.hpp"
#include "utilities/injector.hpp"

#include <toml.hpp>

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

Inline void EMfield_2d(const ntt::coord_t<ntt::Dim2>& x_ph,
                       ntt::vec_t<ntt::Dim3>&         e_out,
                       ntt::vec_t<ntt::Dim3>&         b_out,
                       const real_t                   sx1,
                       const real_t                   sx2) {
  const real_t kx1_x1 = ntt::constant::TWO_PI * x_ph[0] / sx1;
  const real_t kx2_x2 = ntt::constant::TWO_PI * x_ph[1] / sx2;
  e_out[0]            = math::cos(kx1_x1) * math::sin(kx2_x2);
  e_out[1]            = -math::sin(kx1_x1) * math::cos(kx2_x2);
  e_out[2]            = math::cos(kx1_x1) * math::cos(kx2_x2);
  b_out[0]            = math::sin(kx1_x1) * math::cos(kx2_x2);
  b_out[1]            = -math::cos(kx1_x1) * math::sin(kx2_x2);
  b_out[2]            = math::sin(kx1_x1) * math::sin(kx2_x2);
}

Inline void EMfield_3d(const ntt::coord_t<ntt::Dim3>& x_ph,
                       ntt::vec_t<ntt::Dim3>&         e_out,
                       ntt::vec_t<ntt::Dim3>&         b_out,
                       const real_t                   sx1,
                       const real_t                   sx2,
                       const real_t) {
  const real_t kx1_x1 = ntt::constant::TWO_PI * x_ph[0] / sx1;
  const real_t kx2_x2 = ntt::constant::TWO_PI * x_ph[1] / sx2;
  e_out[0]            = math::cos(kx1_x1) * math::sin(kx2_x2);
  e_out[1]            = -math::sin(kx1_x1) * math::cos(kx2_x2);
  e_out[2]            = math::cos(kx1_x1) * math::cos(kx2_x2);
  b_out[0]            = math::sin(kx1_x1) * math::cos(kx2_x2);
  b_out[1]            = -math::cos(kx1_x1) * math::sin(kx2_x2);
  b_out[2]            = math::sin(kx1_x1) * math::sin(kx2_x2);
}

using namespace toml::literals::toml_literals;
const auto default_input {
  R"(
      [domain]
      resolution  = [8192, 8192]
      extent      = [-5.0, 5.0, -5.0, 5.0]
      boundaries  = [["PERIODIC"], ["PERIODIC"]]

      [algorithm]
      cfl = 0.0001

      [units]
      ppc0       = 2.0
      larmor0    = 2.0
      skindepth0 = 1.0

      [particles]
      n_species = 2

      [species_1]
      label    = "e-"
      mass     = 1.0
      charge   = -1.0
      maxnpart = 1e8

      [species_2]
      label    = "e+"
      mass     = 25.0
      charge   = 1.0
      maxnpart = 1e8

      [diagnostics]
      blocking_timers = true
    )"_toml
};

template <ntt::Dimension D, ntt::SimulationEngine S>
struct MaxwellianDist : public ntt::EnergyDistribution<D, S> {
  MaxwellianDist(const ntt::SimulationParams& params, const ntt::Meshblock<D, S>& mblock)
    : ntt::EnergyDistribution<D, S>(params, mblock),
      maxwellian { mblock },
      temperature { ONE } {}
  Inline void operator()(const ntt::coord_t<D>&,
                         ntt::vec_t<ntt::Dim3>& v,
                         const int&             species) const override {
    maxwellian(v, temperature);
  }

private:
  const ntt::Maxwellian<D, S> maxwellian;
  const real_t                temperature;
};

template <ntt::Dimension D>
auto Run(const toml::value input, const int n_iter) -> void {
  using namespace ntt;
  auto  sim    = PIC<D>(input);

  auto  params = *(sim.params());
  auto& mblock = sim.meshblock;

  {
    auto extent = params.extent();
    sim.ResetSimulation();
    const real_t sx1 = extent[1] - extent[0];
    const real_t sx2 = extent[3] - extent[2];
    if constexpr (D == Dim2) {
      Kokkos::parallel_for(
        "InitFields", mblock.rangeActiveCells(), Lambda(index_t i1, index_t i2) {
          set_em_fields_2d(mblock, i1, i2, EMfield_2d, sx1, sx2);
        });
    } else if constexpr (D == Dim3) {
      const real_t sx3 = extent[5] - extent[4];
      Kokkos::parallel_for(
        "InitFields", mblock.rangeActiveCells(), Lambda(index_t i1, index_t i2, index_t i3) {
          set_em_fields_3d(mblock, i1, i2, i3, EMfield_3d, sx1, sx2, sx3);
        });
    }
    sim.Exchange(ntt::GhostCells::fields);

    ntt::InjectUniform<D, ntt::PICEngine, MaxwellianDist>(
      params, sim.meshblock, { 1, 2 }, params.ppc0() * 0.5);
  }
  {
    ntt::WaitAndSynchronize();

    for (auto i { 0 }; i < n_iter; ++i) {
      sim.StepForward(ntt::DiagFlags_Timers | ntt::DiagFlags_Species);
    }
  }
}

auto main(int argc, char* argv[]) -> int {
  ntt::GlobalInitialize(argc, argv);
  try {
    ntt::CommandLineArguments cl_args;
    cl_args.readCommandLineArguments(argc, argv);
    toml::value inputdata;

    auto        n_iter_str = cl_args.getArgument("-niter", "10");
    auto        n_iter     = std::stoi(std::string(n_iter_str));

    if (cl_args.isSpecified("-input")) {
      auto inputfilename = cl_args.getArgument("-input");
      inputdata          = toml::parse(static_cast<std::string>(inputfilename));
    } else {
      inputdata = default_input;
    }
    auto resolution
      = ntt::readFromInput<std::vector<unsigned int>>(inputdata, "domain", "resolution");
    if (resolution.size() == 2) {
      Run<ntt::Dim2>(inputdata, n_iter);
    } else {
      Run<ntt::Dim3>(inputdata, n_iter);
    }
  } catch (std::exception& err) {
    std::cerr << err.what() << std::endl;
    ntt::GlobalFinalize();
    return -1;
  }
  ntt::GlobalFinalize();

  return 0;
}