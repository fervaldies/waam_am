// Fernando Valient Dies & Nicolò Grilli
// University of Sydney & University of Bristol
// 29-11-2022

#include "FunctionPathEllipsoidAux.h"

#include "Function.h"

registerMooseObject("MooseApp", FunctionPathEllipsoidAux);

InputParameters
FunctionPathEllipsoidAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("This AuxKernel increases an AuxVariable from 0 to 1 if the qp is inside "
                                             "an ellipsoid that is moving according to a path provided through a function. "
											 "It can be applied to a level set variable "
                                             "to simulate the material deposition during wire arc additive manufacturing "
											 "together with ActDeactElementsCoupled.");
  params.addParam<Real>("low_level_set_var", 0.0, "The lowest value of the level set variable.");
  params.addParam<Real>("high_level_set_var", 1.0, "The highest value of the level set variable.");
  params.addRequiredParam<Real>("rx", "effective transverse ellipsoid radius");
  params.addRequiredParam<Real>("ry", "effective longitudinal ellipsoid radius");
  params.addRequiredParam<Real>("rz", "effective depth ellipsoid radius");
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
  params.addParam<Real>("level_set_activation_threshold", 0.5, "Threshold value of the ellipsoid function "
									"that activates the level set.");	
  return params;
}

FunctionPathEllipsoidAux::FunctionPathEllipsoidAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _low_level_set_var(getParam<Real>("low_level_set_var")),
	_high_level_set_var(getParam<Real>("high_level_set_var")),
    _rx(getParam<Real>("rx")),
    _ry(getParam<Real>("ry")),
    _rz(getParam<Real>("rz")),
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
	_level_set_activation_threshold(getParam<Real>("level_set_activation_threshold"))
{
}

void
FunctionPathEllipsoidAux::initQpStatefulProperties()
{
  // Initialize coordinates of the heat source
  _n_track = 0;
  _t_scan = _t;
}

Real
FunctionPathEllipsoidAux::computeValue()
{
  // value of the level set variable at the previous time step
  Real old_level_set = _u[_qp];
  
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
  
  Real val;	
  if (abs(x_t-_x_coord) >= _single_scan_length) { // This single scan is over
	  
    val = 0.0;
	  
    checkPPcondition();  
	  
  } else {
  
  // ellipsoid function value
  
  
  val = 6.0 * std::sqrt(3.0) /
          (_rx * _ry * _rz * std::pow(libMesh::pi, 1.5)) *
          std::exp(-(3.0 * std::pow(x - x_t, 2.0) / std::pow(_rx, 2.0) +
                          3.0 * std::pow(y - y_t, 2.0) / std::pow(_ry, 2.0) +
                           3.0 * std::pow(z - z_t, 2.0) / std::pow(_rz, 2.0)));
  }
  
  if (val > _level_set_activation_threshold) { // heat source activating this _qp
	  
	  return _high_level_set_var;
	  
  } else {
	  
    if (old_level_set > _low_level_set_var) { // this was already activated
		 
      return _high_level_set_var;
		 
    } else { // this remains inactive

      return _low_level_set_var;

    }	  
	
  }
}

// Check if the postprocessor temperature condition is satisfied
// and change the initial coordinates and scan time
void
FunctionPathEllipsoidAux::checkPPcondition()
{
  if (_temperature_pp < _temp_array[(int)_t-1]) { // cooling condition
    if (_temperature_pp < _threshold_temperature) { // reached threshold temperature
		
      // update initial heat source coordinate and track time	
      //printf("n track FunctionPathE: %d", _n_track);
      _n_track += 1;
      _velocity(0) = -_velocity(0);
      _x_coord = _init_x_coords[_n_track];
      _y_coord = _init_y_coords[_n_track];
      _z_coord = _init_z_coords[_n_track];
      _t_scan = _t;
  		
	}
  }
}
