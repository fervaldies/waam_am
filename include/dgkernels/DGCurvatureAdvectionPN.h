// Nicolo Grilli
// University of Bristol
// 4 Agosto 2021

// DG Dislocation curvature advection.
// Upwind condition is calculated both on edge/screw dislocations
// in this element and on the neighbouring element.
// The forward and backward motion of
// positive and negative GND is taken into account.
// This kernel represents the first term on the right-hand side 
// of Eq. (12c) in:
// Katrin Schulz, Lydia Wagner, Christian Wieners
// A mesoscale continuum approach of dislocation dynamics and the
// approximation by a Runge-Kutta discontinuous Galerkin method
// International Journal of Plasticity 120 (2019) 248–261

#pragma once

#include "DGKernel.h"

class DGCurvatureAdvectionPN : public DGKernel
{
public:
  static InputParameters validParams();

  DGCurvatureAdvectionPN(const InputParameters & parameters);

protected:
  virtual void getDislocationVelocity();
  virtual Real computeQpResidual(Moose::DGResidualType type) override;
  virtual Real computeQpJacobian(Moose::DGJacobianType type) override;
  virtual Real computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned int jvar) override;

  /// advection velocity
  RealVectorValue _velocity;
  
  // GND dislocation density: rho_x or rho_y for edge or screw
  const VariableValue & _rho_gnd;
  
  const bool _rho_gnd_coupled;
  unsigned int _rho_gnd_var; 

  // Total dislocation density: rho_t 
  const VariableValue & _rho_tot;
  
  const bool _rho_tot_coupled;
  unsigned int _rho_tot_var;
  
  // Coupled dislocation densities in the neighbouring element
  const VariableValue & _rho_gnd_neighbor;
  const VariableValue & _rho_tot_neighbor;
  
  // Coupled dislocation density of the other type
  // If _rho_gnd is edge dislocations
  // this coupled variable is screw dislocation density and vice versa
  // This is used to calculate the fraction of GND density _rho_gnd
  // compared with the total edge + screw GND density
  // the following relationship applies for the variables in Hochrainer model
  // in the case of a purely signed dislocation (mixed edge/screw, no SSD):
  // rho_t = sqrt(rho_e^2 + rho_s^2)
  const VariableValue & _rho_gnd_ot;
  
  const bool _rho_gnd_ot_coupled;
  unsigned int _rho_gnd_ot_var;
  
  // Coupled dislocation density of the other type in the neighbouring element
  const VariableValue & _rho_gnd_ot_neighbor;

  // Tolerance on small values of rho_tot
  const Real _rho_tot_tol;
  
  // Edge slip directions of all slip systems
  const MaterialProperty<std::vector<Real>> & _edge_slip_direction;

  // Screw slip directions of all slip systems
  const MaterialProperty<std::vector<Real>> & _screw_slip_direction;
  
  // Dislocation velocity value (signed) on all slip systems
  const MaterialProperty<std::vector<Real>> & _dislo_velocity;

  // Slip system index to determine slip direction
  const unsigned int _slip_sys_index;
  
  // Character of dislocations (edge or screw)
  const enum class DisloCharacter { edge, screw } _dislo_character;
  
  // No need to check that variable _u on which the kernel is applied is
  // GND density or total density like in DGAdvectionCoupled
  // because this kernel is applied only on the curvature
  
  // Check that |rho_gnd| / rho_tot <= 1
  // In theory, total dislocation density cannot become higher than GND density
  // but it can happen because of numerical error
  bool _check_gnd_rho_ratio; 
  
};
