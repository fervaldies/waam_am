// Nicolò Grilli
// 28 Ottobre 2020
// National University of Singapore

#pragma once

#include "AuxKernel.h"
#include "LaserTempReadFile.h"

// Read temperature due to laser scan during SLM from CFD simulation

// Forward declarations

class LaserTempReadFileAux : public AuxKernel
{
public:
  static InputParameters validParams();

  LaserTempReadFileAux(const InputParameters & parameters);
  virtual ~LaserTempReadFileAux() {}

protected:

  /// Base name of the material system used to calculate the temperature
  const std::string _base_name;
  
  /// The LaserTempReadFile GeneralUserObject to read element specific temperature values from file
  const LaserTempReadFile * const _temperature_read_user_object;
  
  /// Time interval between two temperature data field
  const Real _temperature_time_step;

  const Real _melting_temperature_high;
  const Real _melting_temperature_low;
  const Real _gas_temperature_high;
  const Real _gas_temperature_low;
  
  const Real _reference_temperature;
  
  bool _degrade_eigenstrain;
  
  /// Number of temperature data field in time
  /// if the variable temperature_step becomes bigger than the
  /// total number of temperature time steps, the temperature
  /// is kept constant, element by element, at the last read
  /// temperature time step
  const unsigned int _temperature_num_step;

  virtual Real computeValue();

};
