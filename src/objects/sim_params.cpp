#include "global.h"
#include "sim_params.h"
#include "cargs.h"
#include "input.h"
#include "particles.h"

#include <toml/toml.hpp>

#include <stdexcept>
#include <string>
#include <cassert>
#include <vector>

namespace ntt {

SimulationParams::SimulationParams(const toml::value& inputdata, Dimension dim) {
  m_inputdata = inputdata;

  m_title = readFromInput<std::string>(m_inputdata, "simulation", "title", "PIC_Sim");
  m_runtime = readFromInput<real_t>(m_inputdata, "simulation", "runtime");
  m_correction = readFromInput<real_t>(m_inputdata, "algorithm", "correction");

  auto nspec = readFromInput<int>(m_inputdata, "particles", "n_species");
  for (int i {0}; i < nspec; ++i) {
    auto label = readFromInput<std::string>(
        m_inputdata, "species_" + std::to_string(i + 1), "label", "s" + std::to_string(i + 1));
    auto mass = readFromInput<float>(m_inputdata, "species_" + std::to_string(i + 1), "mass");
    auto charge = readFromInput<float>(m_inputdata, "species_" + std::to_string(i + 1), "charge");
    auto maxnpart = static_cast<std::size_t>(
        readFromInput<double>(m_inputdata, "species_" + std::to_string(i + 1), "maxnpart"));
    auto pusher_str = readFromInput<std::string>(
        m_inputdata, "species_" + std::to_string(i + 1), "pusher", "Boris");
    ParticlePusher pusher {UNDEFINED_PUSHER};
    if ((mass == 0.0) && (charge == 0.0)) {
      pusher = PHOTON_PUSHER;
    } else if (pusher_str == "Vay") {
      pusher = VAY_PUSHER;
    } else if (pusher_str == "Boris") {
      pusher = BORIS_PUSHER;
    }
    m_species.emplace_back(ParticleSpecies(label, mass, charge, maxnpart, pusher));
  }
  m_prtl_shape = readFromInput<short>(m_inputdata, "algorithm", "particle_shape", 1);

  // hardcoded PIC regime
  m_simtype = PIC_SIM;

  auto coords = readFromInput<std::string>(m_inputdata, "domain", "coord_system", "Cartesian");
  if (coords == "Cartesian") {
    m_coord_system = CARTESIAN_COORD;
  } else if (coords == "Spherical") {
    m_coord_system = SPHERICAL_COORD;
  } else if (coords == "Cylindrical") {
    m_coord_system = CYLINDRICAL_COORD;
  } else {
    throw std::invalid_argument("Unknown coordinate system specified in the input.");
  }

  // box size/resolution
  m_resolution = readFromInput<std::vector<std::size_t>>(m_inputdata, "domain", "resolution");
  m_extent = readFromInput<std::vector<real_t>>(m_inputdata, "domain", "extent");

  if ((static_cast<short>(m_resolution.size()) < static_cast<short>(dim))
      || (static_cast<short>(m_extent.size()) < 2 * static_cast<short>(dim))) {
    throw std::invalid_argument("Not enough values in `extent` or `resolution` input.");
  }

  m_resolution.erase(m_resolution.begin() + static_cast<short>(dim), m_resolution.end());
  m_extent.erase(m_extent.begin() + 2 * static_cast<short>(dim), m_extent.end());

  auto boundaries = readFromInput<std::vector<std::string>>(m_inputdata, "domain", "boundaries");
  short b {0};
  for (auto& bc : boundaries) {
    if (bc == "PERIODIC") {
      m_boundaries.push_back(PERIODIC_BC);
    } else if (bc == "OPEN") {
      m_boundaries.push_back(OPEN_BC);
    } else {
      m_boundaries.push_back(UNDEFINED_BC);
    }
    ++b;
    if (b >= static_cast<short>(dim)) { break; }
  }
  // plasma params
  m_ppc0 = readFromInput<real_t>(m_inputdata, "units", "ppc0");
  m_larmor0 = readFromInput<real_t>(m_inputdata, "units", "larmor0");
  m_skindepth0 = readFromInput<real_t>(m_inputdata, "units", "skindepth0");
  m_sigma0 = m_larmor0 * m_larmor0 / (m_skindepth0 * m_skindepth0);
  m_charge0 = 1.0 / (m_ppc0 * m_skindepth0 * m_skindepth0);
  m_B0 = 1.0 / m_larmor0;

  // real_t maxtstep {}
  m_cfl = readFromInput<real_t>(m_inputdata, "algorithm", "CFL", 0.95);
  assert(m_cfl > 0);
}

void SimulationParams::verify() {
  if (m_simtype == UNDEFINED_SIM) { throw std::logic_error("ERROR: simulation type unspecified."); }
  if (m_coord_system == UNDEFINED_COORD) {
    throw std::logic_error("ERROR: coordinate system unspecified.");
  }
  for (auto& b : m_boundaries) {
    if (b == UNDEFINED_BC) { throw std::logic_error("ERROR: boundary conditions unspecified."); }
  }
  // TODO: maybe some other tests
}

} // namespace ntt
