# adding material on top of the substrate

[Problem]
  kernel_coverage_check = false
[]

[Mesh]
  [./file_input]
    type = FileMeshGenerator
    file = 'smallCoarse.e'
  []
  [./sidesets]
    input = file_input
    type = SideSetsBetweenSubdomainsGenerator
    primary_block = '1'
    paired_block = '2'
    new_boundary = 'moving_interface'
  [../] 
[]

# the new elements will be initialized at temperature 300 K
[Variables]
  [./temp]
    family = LAGRANGE
    order = FIRST
    initial_condition = 300.0
	block = '1'
  [../]
[]

# thermal conduction problem is solved only in the active domain
[Kernels]
  [./time]
    type = ADHeatConductionTimeDerivative
    variable = temp
	block = '1'
  [../]
  [./heat_conduct]
    type = ADHeatConduction
    variable = temp
    thermal_conductivity = thermal_conductivity
	block = '1'
  [../]
  [./heat_source]
    type = ADMatHeatSource
    material_property = volumetric_heat
    variable = temp
	block = '1'
  [../]
[]

[AuxVariables]
  [./level_set_var]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

# level set is initialized as 1.0
# only in the substrate
[ICs]
  [./temperature_ic_back]
    type = ConstantIC
    variable = level_set_var
    value = 1.0
	block = '1'
  [../]
  [./temperature_ic_front]
    type = ConstantIC
    variable = level_set_var
	value = 0.0
    block = '2'
  [../]
[]

[AuxKernels]
  [./add_material_in_the_ellipsoid]
    type = FunctionPathEllipsoidAux
	variable = level_set_var
	rx = 0.0018
    ry = 0.0015
    rz = 0.0015
    init_x_coords = '0.01 0.025'
    init_y_coords = '0.04 0.05'
    init_z_coords = '0.016 0.016'
    temperature_pp = temperature_in_one_element
    temp_array = '0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0'
    single_scan_length = 0.015
    threshold_temperature = 400
    velocity = '0.002 0 0'
	level_set_activation_threshold = 0.03
  [../]
[]

[BCs]
  [./top_BC]
    type = ADConvectiveHeatFluxBC
    variable = temp
    boundary = 'bottomplate frontplate backplate rightplate leftplate topnotplate moving_interface'
    T_infinity = 300.0
    heat_transfer_coefficient = 17
  []
  [./bot_down_BC]
    type = ADFunctionRadiativeBC
    variable = temp
    boundary = 'bottomplate frontplate backplate rightplate leftplate topnotplate moving_interface'
    # htc/(stefan-boltzmann*4*T_inf^3)
    emissivity_function = '0.00002532/(5.670367e-14*4*300*300*300)'
  [../]
[]

[Materials]
  [./heat]
    type = ADHeatConductionMaterial
    specific_heat = 500 # J/KgK 316LN
    thermal_conductivity = 17 # W/mmK 316LN
  [../]
  [./density]
    type = ADGenericConstantMaterial
    prop_names = 'density'
    prop_values = 7900 # Kg/mm^3 316LN
  [../]
  [./volumetric_heat]
    type = VelocityEllipsoidHeatSource
	rx = 0.0018
    ry = 0.0015
    rz = 0.0015
    power = 3000 # W -> WAAM (7000 W = CO2 laser)
    efficiency = 0.75
    factor = 1
    init_x_coords = '0.01 0.025'
    init_y_coords = '0.04 0.05'
    init_z_coords = '0.016 0.016'
    temperature_pp = temperature_in_one_element
    temp_array = '0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0'
    single_scan_length = 0.015
    threshold_temperature = 400
    velocity = '0.002 0 0'
  [../]
[]

[Functions]
  [./path_x]
    type = PiecewiseLinear
    x = '0.00 25.7 25.8 51.5 51.6 77.3 77.4 103.1 103.2 128.9 129.0 154.7 154.8 180.5 180.6 206.3'
    y = '0.010 0.190 0.162 0.010 0.010 0.134 0.106 0.010 0.010 0.078 0.050 0.010 0.010 0.122 0.150 0.010'
  [../]
  [./path_y]
    type = PiecewiseConstant
    x = '0.00 25.7 25.8 51.5 51.6 77.3 77.4 103.1 103.2 128.9 129.0 154.7 154.8 180.5 180.6 206.3'
    y = '0.04175 0.04175 0.04725 0.04725 0.05275 0.05275 0.05825 0.05825 0.05825 0.05825 0.05275 0.05275 0.04725 0.04725 0.04175 0.04175'
  [../]
  [./path_z]
    type = PiecewiseConstant
    x = '0.00 25.7 25.8 51.5 51.6 77.3 77.4 103.1 103.2 128.9 129.0 154.7 154.8 180.5 180.6 206.3'
    y = '0.016 0.016 0.016 0.016 0.016 0.016 0.016 0.016 0.019 0.019 0.019 0.019 0.019 0.019 0.019 0.019'
  [../]
[]

# User Object to activate elements
[UserObjects]
  [./activated_elem_uo]
    type = ActDeactElementsCoupled
    execute_on = timestep_begin
    coupled_var = level_set_var
    activate_value = 0.5
	activate_type = 'above'
    active_subdomain_id = 1
	deactive_subdomain_id = 2
    expand_boundary_name = 'moving_interface'
  [../]
[]

[Postprocessors]
  [./temperature_in_one_element]
    type = PointValue
    variable = temp
    point = '0.01 0.04 0.016'
  [../]
  [./thermc_1]
    type = PointValue
    variable = temp
    point = '0.020 0.050 0'
  []
  [./thermc_2]
    type = PointValue
    variable = temp
    point = '0.060 0.050 0'
  []
  [./thermc_3]
    type = PointValue
    variable = temp
    point = '0.100 0.050 0'
  []
  [./thermc_4]
    type = PointValue
    variable = temp
    point = '0.140 0.050 0'
  []
  [./thermc_5]
    type = PointValue
    variable = temp
    point = '0.180 0.050 0'
  []
  [./thermc_6]
    type = PointValue
    variable = temp
    point = '0.020 0.070 0.0155'
  []
  [./thermc_7]
    type = PointValue
    variable = temp
    point = '0.060 0.070 0.0155'
  []
  [./thermc_8]
    type = PointValue
    variable = temp
    point = '0.100 0.070 0.0155'
  []
  [./thermc_9]
    type = PointValue
    variable = temp
    point = '0.140 0.070 0.0155'
  []
  [./thermc_10]
    type = PointValue
    variable = temp
    point = '0.180 0.070 0.0155'
  []
  [./thermc_11]
    type = PointValue
    variable = temp
    point = '0.100 0.031 0.0155'
  []
  [./thermc_12]
    type = PointValue
    variable = temp
    point = '0.100 0.027 0.0155'
  []
  [./thermc_13]
    type = PointValue
    variable = temp
    point = '0.100 0.022 0.0155'
  []
[]

[Preconditioning]
  [./full]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart -pc_hypre_boomeramg_strong_threshold -pc_hypre_boomeramg_agg_nl -pc_hypre_boomeramg_agg_num_paths -pc_hypre_boomeramg_max_levels -pc_hypre_boomeramg_coarsen_type -pc_hypre_boomeramg_interp_type -pc_hypre_boomeramg_P_max -pc_hypre_boomeramg_truncfactor -pc_hypre_boomeramg_print_statistics'
  petsc_options_value = 'hypre boomeramg 51 0.7 4 5 25 PMIS ext+i 2 0.3 0'

  l_max_its = 50
  nl_max_its = 50

  end_time = 206.3
  dt = 0.1
  dtmin = 1e-20
[]

[Outputs]
  print_linear_residuals = true
  perf_graph = true
  checkpoint = true
  [./exodus]
    type = Exodus
    interval = 1
  [../]
  [./outfile]
    type = CSV
    file_base = 'Thermocouples'
    delimiter = ','
    time_data = true
  [../]
[]
