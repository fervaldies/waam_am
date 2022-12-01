// Nicolò Grilli
// University of Bristol
// 23 Ottobre 2021

#pragma once

#include "FiniteStrainUObasedCP.h"

/**
 * FiniteStrainUObasedCPDamageVol is the coupling between
 * crystal plasticity and damage
 * Free energy is decomposed into volumetric and non-volumetric parts as in:
 * Nicolo Grilli and Marisol Koslowski
 * The effect of crystal anisotropy and plastic response 
 * on the dynamic fracture of energetic materials
 * Journal of Applied Physics 126, 155101 (2019)
 */
class FiniteStrainUObasedCPDamageVol : public FiniteStrainUObasedCP
{
public:
  static InputParameters validParams();

  FiniteStrainUObasedCPDamageVol(const InputParameters & parameters);

protected:

  /**
   * initializes the stateful properties such as
   * stress, plastic deformation gradient, slip system resistances, etc.
   * adding initialization of _elastic_deformation_grad and _fpdot
   */
  virtual void initQpStatefulProperties();

  /**
   * update stress and internal variable after solve.
   * Adding the calculation of the time derivative of the plastic
   * deformation gradient that is necessary to calculate the plastic work
   */
  virtual void postSolveQp();
  
  /**
   * calculate stress residual.
   * Damage is added to the stress calculation
   */
  virtual void calcResidual();
  
  /**
   * calculate jacobian.
   */
  virtual void calcJacobian();
  
  /**
   * Method to split elastic energy based on strain volumetric/non-volumetric decomposition
   * @param F_pos tensile part of total elastic energy
   * @param F_neg compressive part of total elastic energy
   * @param ee elastic Green-Lagrange strain
   * @param ce right Cauchy–Green deformation tensor (elastic)
   * @param pk2_new second Piola-Kirchhoff stress
   */
  virtual void computeStrainVolumetric(Real & F_pos, Real & F_neg, 
                                       RankTwoTensor & ee, RankTwoTensor & ce, 
									   RankTwoTensor & pk2_new);

  // compute history variable and assign to _E
  // which is used by the fracture model for damage growth
  // Damage grows only because of the positive part of the elastic energy F_pos									 
  virtual void computeHistoryVariable(Real & F_pos, Real & F_neg);
  
  /**
   * calculate the elastic tangent moduli for preconditioner,
   * with damage included
   */
  virtual void elasticTangentModuli();

  /**
   * calculate the exact tangent moduli for preconditioner,
   * with damage included
   */
  virtual void elastoPlasticTangentModuli();

  /// Variable defining the phase field damage parameter
  const VariableValue & _c;
  
  /// Use current value of history variable
  bool _use_current_hist;
  
  /// History variable that prevents crack healing, declared in this material
  MaterialProperty<Real> & _H;

  /// Old value of history variable
  const MaterialProperty<Real> & _H_old;
  
  /// Material property for elastic energy
  MaterialProperty<Real> & _E;

  /// Derivative of elastic energy w.r.t damage variable
  MaterialProperty<Real> & _dEdc;

  /// Second-order derivative of elastic energy w.r.t damage variable
  MaterialProperty<Real> & _d2Ed2c;
  
  /// Derivative of stress w.r.t damage variable
  MaterialProperty<RankTwoTensor> & _dstress_dc;
  
  /// Second-order derivative of elastic energy w.r.t damage variable and strain
  MaterialProperty<RankTwoTensor> & _d2Fdcdstrain;
  
  /// Material property for energetic degradation function
  /// for instance, (1-c)^2 can be used
  const MaterialProperty<Real> & _D;

  /// Derivative of degradation function w.r.t damage variable
  const MaterialProperty<Real> & _dDdc;

  /// Second-order derivative of degradation w.r.t damage variable
  const MaterialProperty<Real> & _d2Dd2c;
  
  /// time derivative of the plastic deformation gradient
  /// it is necessary to update the plastic work
  MaterialProperty<RankTwoTensor> & _fpdot;
  
  /// Elastic deformation gradient needed by the user object
  /// PlasticWorkRate to calculate the plastic work
  MaterialProperty<RankTwoTensor> & _elastic_deformation_grad;
  
  const Real _bulk_modulus_ref; // reference bulk modulus for vol/non-vol decomposition

};
