// Nicolò Grilli
// University of Bristol
// 6 Novembre 2022

#include "VelocityEllipsoidHeatSource.h"

#include "Function.h" // is this necessary?

registerMooseObject("HeatConductionApp", VelocityEllipsoidHeatSource);

InputParameters
VelocityEllipsoidHeatSource::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Double ellipsoid volumetric source heat, the motion is determined "
                             "by input velocity, starting positions and a postprocessor. ");
  params.addRequiredParam<Real>("power", "power");
  params.addParam<Real>("efficiency", 1, "process efficiency");
  params.addRequiredParam<Real>("rx", "effective transverse ellipsoid radius");
  params.addRequiredParam<Real>("ry", "effective longitudinal ellipsoid radius");
  params.addRequiredParam<Real>("rz", "effective depth ellipsoid radius");
  params.addParam<Real>(
      "factor", 1, "scaling factor that is multiplied to the heat source to adjust the intensity");
  params.addRequiredParam<RealVectorValue>("velocity", "Velocity vector");
  params.addRequiredParam<std::vector<Real>>("temp_array", "Array containing all the temperatures");
  
  // Every time the postprocessor condition is satisfied, the heat source is moved to the next set of coordinates
  params.addRequiredParam<std::vector<Real>>("init_x_coords", "Initial values of x coordinates of the heat source");
  params.addRequiredParam<std::vector<Real>>("init_y_coords", "Initial values of y coordinates of the heat source");
  params.addRequiredParam<std::vector<Real>>("init_z_coords", "Initial values of z coordinates of the heat source");
  
  params.addRequiredParam<PostprocessorName>("temperature_pp","Postprocessor with temperature value to determine heat source motion.");
      
  params.addRequiredParam<Real>("single_scan_length","Total length during one scan. "
                                                   "After this length the laser is switched off. ");
  params.addRequiredParam<Real>("threshold_temperature","When the temperature provided by the postprocessor decreases "
                                                        "below this threshold, the heat source is moved to the next "
                                                        "set of coordinates. ");       
  return params;
}

VelocityEllipsoidHeatSource::VelocityEllipsoidHeatSource(const InputParameters & parameters)
  : Material(parameters),
    _P(getParam<Real>("power")),
    _eta(getParam<Real>("efficiency")),
    _rx(getParam<Real>("rx")),
    _ry(getParam<Real>("ry")),
    _rz(getParam<Real>("rz")),
    _f(getParam<Real>("factor")),
    _velocity(getParam<RealVectorValue>("velocity")), // Scanning speed vector

    // Array containing all the temperatures
    _temp_array(getParam<std::vector<Real>>("temp_array")),
    
    // Initial values of the coordinates of the heat source
    _init_x_coords(getParam<std::vector<Real>>("init_x_coords")),
    _init_y_coords(getParam<std::vector<Real>>("init_y_coords")),
    _init_z_coords(getParam<std::vector<Real>>("init_z_coords")),
    
    // Postprocess with temperature value
    _temperature_pp(getPostprocessorValue("temperature_pp")),
    _temperature_pp_old(getPostprocessorValueOld("temperature_pp")),
    
    // Total length during one scan
    _single_scan_length(getParam<Real>("single_scan_length")),
    
    // Threshold temperature for the postprocessor condition
    _threshold_temperature(getParam<Real>("threshold_temperature")),
    
    // Volumetric heat source used by the kernel
    _volumetric_heat(declareADProperty<Real>("volumetric_heat"))
{
}

void
VelocityEllipsoidHeatSource::initQpStatefulProperties()
{
  // Initialize coordinates of the heat source
  _t_scan = _t;
  _n_track = 0;
}

void
VelocityEllipsoidHeatSource::computeQpProperties()
{
  _x_coord = _init_x_coords[_n_track];
  _y_coord = _init_y_coords[_n_track];
  _z_coord = _init_z_coords[_n_track];
  _temp_array[(int)_t] = _temperature_pp;

  const Real & x = _q_point[_qp](0);
  const Real & y = _q_point[_qp](1);
  const Real & z = _q_point[_qp](2);
  
  

  // center of the heat source
  Real x_t = _x_coord + _velocity(0) * (_t - _t_scan);
  Real y_t = _y_coord + _velocity(1) * (_t - _t_scan);
  Real z_t = _z_coord + _velocity(2) * (_t - _t_scan);
  
	
  if (abs(x_t-_x_coord) >= _single_scan_length) { // This single scan is over
	  
    _volumetric_heat[_qp] = 0.0;
	  
   // if (_temperature_pp != _temp_array[(int)_t-1]) {
	    //printf("temp: %f", _temperature_pp);
	    //printf("temp old: %f", _temp_array[(int)_t]);
	    //printf("n track: %d", _n_track);
	    //printf("t_scan: %f", _t_scan);
	    //printf("t_real: %f", _t);
   // }
	  
    checkPPcondition();  
	  
  } else {
  
    _volumetric_heat[_qp] = 6.0 * std::sqrt(3.0) * _P * _eta * _f /
                            (_rx * _ry * _rz * std::pow(libMesh::pi, 1.5)) *
                            std::exp(-(3.0 * std::pow(x - x_t, 2.0) / std::pow(_rx, 2.0) +
                                       3.0 * std::pow(y - y_t, 2.0) / std::pow(_ry, 2.0) +
                                       3.0 * std::pow(z - z_t, 2.0) / std::pow(_rz, 2.0)));
  }
}

// Check if the postprocessor temperature condition is satisfied
// and change the initial coordinates and scan time
void
VelocityEllipsoidHeatSource::checkPPcondition()
{
  if (_temperature_pp < _temp_array[(int)_t-1]) { // cooling condition
    if (_temperature_pp < _threshold_temperature) { // reached threshold temperature
		
      // update initial heat source coordinate and track time	
      //printf("n track: %d", _n_track);
      _n_track += 1;
      _velocity(0) = -_velocity(0);
      _x_coord = _init_x_coords[_n_track];
      _y_coord = _init_y_coords[_n_track];
      _z_coord = _init_z_coords[_n_track];
      _t_scan = _t;
  		
	}
  }
}
