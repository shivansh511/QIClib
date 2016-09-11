/*
 * QIClib (Quantum information and computation library)
 *
 * Copyright (c) 2015 - 2016  Titas Chanda (titas.chanda@gmail.com)
 *
 * This file is part of QIClib.
 *
 * QIClib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QIClib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QIClib.  If not, see <http://www.gnu.org/licenses/>.
 */

namespace qic {

//******************************************************************************

namespace protect_dis {

//******************************************************************************

template <typename T1>
double def_nlopt2(const std::vector<double>& x, std::vector<double>& grad,
                  void* my_func_data) {
  (void)grad;
  std::complex<trait::pT<T1> > I(0.0, 1.0);
  trait::pT<T1> theta = static_cast<trait::pT<T1> >(x[0]);
  trait::pT<T1> phi = static_cast<trait::pT<T1> >(x[1]);

  TO_PASS<arma::Mat<trait::eT<T1> > >* pB =
    static_cast<TO_PASS<arma::Mat<trait::eT<T1> > >*>(my_func_data);

  auto& u = SPM<trait::pT<T1> >::get_instance().basis2.at(0, 0);
  auto& d = SPM<trait::pT<T1> >::get_instance().basis2.at(1, 0);

  arma::Mat<std::complex<trait::pT<T1> > > proj1 =
    std::cos(static_cast<trait::pT<T1> >(0.5) * theta) * u +
    std::exp(I * phi) * std::sin(static_cast<trait::pT<T1> >(0.5) * theta) * d;

  arma::Mat<std::complex<trait::pT<T1> > > proj2 =
    std::sin(static_cast<trait::pT<T1> >(0.5) * theta) * u -
    std::exp(I * phi) * std::cos(static_cast<trait::pT<T1> >(0.5) * theta) * d;

  proj1 *= proj1.t();
  proj2 *= proj2.t();

  if ((*pB).nodal == 1) {
    proj1 = kron(proj1, (*pB).eye2);
    proj2 = kron(proj2, (*pB).eye2);

  } else if ((*pB).party_no == (*pB).nodal) {
    proj1 = kron((*pB).eye2, proj1);
    proj2 = kron((*pB).eye2, proj2);

  } else {
    proj1 = kron(kron((*pB).eye3, proj1), (*pB).eye4);
    proj2 = kron(kron((*pB).eye3, proj2), (*pB).eye4);
  }

  arma::Mat<std::complex<trait::pT<T1> > > rho_1 =
    (proj1 * ((*pB).rho) * proj1);
  arma::Mat<std::complex<trait::pT<T1> > > rho_2 =
    (proj2 * ((*pB).rho) * proj2);

  rho_1 += rho_2;
  trait::pT<T1> S_max = entropy(rho_1);
  return static_cast<double>(S_max);
}

//******************************************************************************

template <typename T1>
double def_nlopt3(const std::vector<double>& x, std::vector<double>& grad,
                  void* my_func_data) {
  (void)grad;
  std::complex<trait::pT<T1> > I(0.0, 1.0);

  trait::pT<T1> theta1 = static_cast<trait::pT<T1> >(0.5 * x[0]);
  trait::pT<T1> theta2 = static_cast<trait::pT<T1> >(0.5 * x[1]);
  trait::pT<T1> theta3 = static_cast<trait::pT<T1> >(0.5 * x[2]);
  trait::pT<T1> phi1 = static_cast<trait::pT<T1> >(x[3]);
  trait::pT<T1> phi2 = static_cast<trait::pT<T1> >(-x[3]);
  trait::pT<T1> del = static_cast<trait::pT<T1> >(x[4]);

  TO_PASS<arma::Mat<trait::eT<T1> > >* pB =
    static_cast<TO_PASS<arma::Mat<trait::eT<T1> > >*>(my_func_data);

  auto& U = SPM<trait::pT<T1> >::get_instance().basis3.at(0, 0);
  auto& M = SPM<trait::pT<T1> >::get_instance().basis3.at(1, 0);
  auto& D = SPM<trait::pT<T1> >::get_instance().basis3.at(2, 0);

  arma::Mat<std::complex<trait::pT<T1> > > proj1 =
    std::cos(theta1) * std::cos(theta2) * U -
    std::exp(I * phi1) * (std::exp(I * del) * std::sin(theta1) *
                            std::cos(theta2) * std::cos(theta3) +
                          std::sin(theta2) * std::sin(theta3)) *
      M +
    std::exp(I * phi2) * (-std::exp(I * del) * std::sin(theta1) *
                            std::cos(theta2) * std::sin(theta3) +
                          std::sin(theta2) * std::cos(theta3)) *
      D;

  arma::Mat<std::complex<trait::pT<T1> > > proj2 =
    std::exp(-I * del) * std::sin(theta1) * U +
    std::exp(I * phi1) * std::cos(theta1) * std::cos(theta3) * M +
    std::exp(I * phi2) * std::cos(theta1) * std::sin(theta3) * D;

  arma::Mat<std::complex<trait::pT<T1> > > proj3 =
    std::cos(theta1) * std::sin(theta2) * U +
    std::exp(I * phi1) * (-std::exp(I * del) * std::sin(theta1) *
                            std::sin(theta2) * std::cos(theta3) +
                          std::cos(theta2) * std::sin(theta3)) *
      M -
    std::exp(I * phi2) * (std::exp(I * del) * std::sin(theta1) *
                            std::sin(theta2) * std::sin(theta3) +
                          std::cos(theta2) * std::cos(theta3)) *
      D;

  proj1 *= proj1.t();
  proj2 *= proj2.t();
  proj3 *= proj3.t();

  if ((*pB).nodal == 1) {
    proj1 = kron(proj1, (*pB).eye2);
    proj2 = kron(proj2, (*pB).eye2);
    proj3 = kron(proj3, (*pB).eye2);

  } else if ((*pB).party_no == (*pB).nodal) {
    proj1 = kron((*pB).eye2, proj1);
    proj2 = kron((*pB).eye2, proj2);
    proj3 = kron((*pB).eye2, proj3);

  } else {
    proj1 = kron(kron((*pB).eye3, proj1), (*pB).eye4);
    proj2 = kron(kron((*pB).eye3, proj2), (*pB).eye4);
    proj3 = kron(kron((*pB).eye3, proj3), (*pB).eye4);
  }

  arma::Mat<std::complex<trait::pT<T1> > > rho_1 =
    (proj1 * ((*pB).rho) * proj1);
  arma::Mat<std::complex<trait::pT<T1> > > rho_2 =
    (proj2 * ((*pB).rho) * proj2);
  arma::Mat<std::complex<trait::pT<T1> > > rho_3 =
    (proj3 * ((*pB).rho) * proj3);

  rho_1 += rho_2 + rho_3;
  trait::pT<T1> S_max = entropy(rho_1);
  return (static_cast<double>(S_max));
}

//******************************************************************************

}  // namespace protect

//****************************************************************************

template <typename T1>
inline deficit_space<T1>::deficit_space(const T1& rho1, arma::uword nodal,
                                        arma::uvec dim)
    : _rho(_internal::as_Mat(rho1)), _nodal(nodal), _is_computed(false),
      _is_reg_computed(false), _is_sab_computed(false) {
#ifndef QICLIB_NO_DEBUG
  if (_rho.n_elem == 0)
    throw Exception("qic::deficit_space", Exception::type::ZERO_SIZE);

  if (_rho.n_rows != _rho.n_cols)
    throw Exception("qic::deficit_space", Exception::type::MATRIX_NOT_SQUARE);
#endif

  init(std::move(dim));
}

//****************************************************************************

template <typename T1>
inline deficit_space<T1>::deficit_space(const T1& rho1, arma::uword nodal,
                                        arma::uword dim)
    : _rho(_internal::as_Mat(rho1)), _nodal(nodal), _is_computed(false),
      _is_reg_computed(false), _is_sab_computed(false) {
#ifndef QICLIB_NO_DEBUG
  if (_rho.n_elem == 0)
    throw Exception("qic::deficit_space", Exception::type::ZERO_SIZE);

  if (_rho.n_rows != _rho.n_cols)
    throw Exception("qic::deficit_space", Exception::type::MATRIX_NOT_SQUARE);

  if (dim == 0)
    throw Exception("qic::deficit_space", Exception::type::INVALID_DIMS);
#endif

  arma::uword n = static_cast<arma::uword>(
    std::llround(std::log(_rho.n_rows) / std::log(dim)));

  arma::uvec dim2(n);
  dim2.fill(dim);

  init(std::move(dim2));
}

//****************************************************************************

template <typename T1> inline void deficit_space<T1>::init(arma::uvec dim) {
  _dim = std::move(dim);
  _party_no = _dim.n_elem;

#ifndef QICLIB_NO_DEBUG
  if (arma::any(_dim == 0))
    throw Exception("qic::deficit_space", Exception::type::INVALID_DIMS);

  if (arma::prod(_dim) != _rho.n_rows)
    throw Exception("qic::deficit_space",
                    Exception::type::DIMS_MISMATCH_MATRIX);

  if (_nodal <= 0 || _nodal > _party_no)
    throw Exception("qic::deficit_space", "Invalid measured party index!");
#endif

  _deficit2 = (_dim(_nodal - 1) == 2);
  _deficit3 = (_dim(_nodal - 1) == 3);

#ifndef QICLIB_NO_DEBUG
  if (!_deficit2 && !_deficit3)
    throw Exception("qic::deficit_space",
                    "Measured party is not qubit or qutrit!");
#endif

  if (_deficit2) {
    _deficit_global_opt = nlopt::GN_DIRECT_L;
    _deficit_global_xtol = 4.0e-2;
    _deficit_global_ftol = 0;
    _deficit_global = true;
    _deficit_local_opt = nlopt::LN_COBYLA;
    _deficit_local_xtol = 10 * _precision::eps<double>::value;
    _deficit_local_ftol = 0.0;

    _deficit_angle_range = {1.0, 2.0};
    _deficit_angle_ini = {0.1, 0.1};
  }

  if (_deficit3) {
    _deficit_global_opt = nlopt::GN_DIRECT_L;
    _deficit_global_xtol = 0.25;
    _deficit_global_ftol = 0;
    _deficit_global = true;
    _deficit_local_opt = nlopt::LN_COBYLA;
    _deficit_local_xtol = 10 * _precision::eps<double>::value;
    _deficit_local_ftol = 0.0;

    _deficit_angle_range = 2.0 * arma::ones<arma::vec>(5);
    _deficit_angle_ini = 2.0 * arma::ones<arma::vec>(5);
  }
}

//****************************************************************************

template <typename T1>
inline deficit_space<T1>&
deficit_space<T1>::global_algorithm(nlopt::algorithm a) noexcept {
  _deficit_global_opt = a;
  _is_computed = false;
  return *this;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::global_xtol(double a) noexcept {
  _deficit_global_xtol = a;
  _is_computed = false;
  return *this;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::global_ftol(double a) noexcept {
  _deficit_global_ftol = a;
  _is_computed = false;
  return *this;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::global_opt(bool a) noexcept {
  _deficit_global = a;
  _is_computed = false;
  return *this;
}

template <typename T1>
inline deficit_space<T1>&
deficit_space<T1>::local_algorithm(nlopt::algorithm a) noexcept {
  _deficit_local_opt = a;
  _is_computed = false;
  return *this;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::local_xtol(double a) noexcept {
  _deficit_local_xtol = a;
  _is_computed = false;
  return *this;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::local_ftol(double a) noexcept {
  _deficit_local_ftol = a;
  _is_computed = false;
  return *this;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::angle_range(const arma::vec& a) {
#ifndef QICLIB_NO_DEBUG
  if (_deficit2 && a.n_elem != 2)
    throw Exception(
      "qic::deficit_space::set_angle_range",
      "Number of elements has to be 2, when measured party is a qubit!");

  if (_deficit3 && a.n_elem != 5)
    throw Exception(
      "qic::deficit_space::set_angle_range",
      "Number of elements has to be 5, when measured party is a qutrit!");
#endif

  _deficit_angle_range = a;
  _is_computed = false;
  return *this;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::initial_angle(const arma::vec& a) {
#ifndef QICLIB_NO_DEBUG
  if (_deficit2 && a.n_elem != 2)
    throw Exception(
      "qic::deficit_space::set_angle_initial",
      "Number of elements has to be 2, when measured party is a qubit!");

  if (_deficit3 && a.n_elem != 5)
    throw Exception(
      "qic::deficit_space::set_angle_initial",
      "Number of elements has to be 5, when measured party is a qutrit!");
#endif

  _deficit_angle_ini = a;
  _is_computed = false;
  return *this;
}

//****************************************************************************

template <typename T1> inline void deficit_space<T1>::s_a_b() {
  if (!_is_sab_computed) {
    _S_A_B = entropy(_rho);
    _is_sab_computed = true;
  }
}

//****************************************************************************

template <typename T1> inline deficit_space<T1>& deficit_space<T1>::compute() {
  s_a_b();
  if (_deficit2) {
    arma::uword dim1 = arma::prod(_dim);
    dim1 /= 2;
    arma::uword dim2(1);
    for (arma::uword i = 0; i < _nodal - 1; ++i) dim2 *= _dim.at(i);

    arma::uword dim3(1);
    for (arma::uword i = _nodal; i < _party_no; ++i) dim3 *= _dim.at(i);

    arma::Mat<trait::pT<T1> > eye2 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim1, dim1);
    arma::Mat<trait::pT<T1> > eye3 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim2, dim2);
    arma::Mat<trait::pT<T1> > eye4 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim3, dim3);

    protect_dis::TO_PASS<arma::Mat<trait::eT<T1> > > pass(
      _rho, eye2, eye3, eye4, _nodal, _party_no);

    std::vector<double> lb(2);
    std::vector<double> ub(2);

    lb[0] = 0.0;
    lb[1] = 0.0;
    ub[0] = _deficit_angle_range.at(0) * arma::datum::pi;
    ub[1] = _deficit_angle_range.at(1) * arma::datum::pi;

    std::vector<double> x(2);
    x[0] = _deficit_angle_ini.at(0) * arma::datum::pi;
    x[1] = _deficit_angle_ini.at(0) * arma::datum::pi;

    double minf;

    if (_deficit_global == true) {
      double minf1;
      nlopt::opt opt1(_deficit_global_opt, 2);
      opt1.set_lower_bounds(lb);
      opt1.set_upper_bounds(ub);
      opt1.set_min_objective(protect_dis::def_nlopt2<T1>,
                             static_cast<void*>(&pass));
      opt1.set_xtol_rel(_deficit_global_xtol);
      opt1.set_ftol_rel(_deficit_global_ftol);
      opt1.optimize(x, minf1);
    }

    nlopt::opt opt(_deficit_local_opt, 2);
    opt.set_lower_bounds(lb);
    opt.set_upper_bounds(ub);
    opt.set_min_objective(protect_dis::def_nlopt2<T1>,
                          static_cast<void*>(&pass));
    opt.set_xtol_rel(_deficit_local_xtol);
    opt.set_ftol_rel(_deficit_local_ftol);
    opt.optimize(x, minf);

    _result = -_S_A_B + static_cast<trait::pT<T1> >(minf);
    _tp = {static_cast<trait::pT<T1> >(x[0]),
           static_cast<trait::pT<T1> >(x[1])};
    _is_computed = true;
  }

  if (_deficit3) {
    arma::uword dim1 = arma::prod(_dim);
    dim1 /= 3;
    arma::uword dim2(1);
    for (arma::uword i = 0; i < _nodal - 1; ++i) dim2 *= _dim.at(i);

    arma::uword dim3(1);
    for (arma::uword i = _nodal; i < _party_no; ++i) dim3 *= _dim.at(i);

    arma::Mat<trait::pT<T1> > eye2 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim1, dim1);
    arma::Mat<trait::pT<T1> > eye3 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim2, dim2);
    arma::Mat<trait::pT<T1> > eye4 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim3, dim3);

    protect_dis::TO_PASS<arma::Mat<trait::eT<T1> > > pass(
      _rho, eye2, eye3, eye4, _nodal, _party_no);

    std::vector<double> lb(5);
    std::vector<double> ub(5);

    for (arma::uword i = 0; i < 5; i++) {
      lb[i] = 0.0;
      ub[i] = _deficit_angle_range.at(i) * arma::datum::pi;
    }

    std::vector<double> x(5);
    for (arma::uword i = 0; i < 5; i++) {
      x[i] = _deficit_angle_ini.at(i) * arma::datum::pi;
    }

    double minf;

    if (_deficit_global == true) {
      double minf1;
      nlopt::opt opt1(_deficit_global_opt, 5);
      opt1.set_lower_bounds(lb);
      opt1.set_upper_bounds(ub);
      opt1.set_min_objective(protect_dis::def_nlopt3<T1>,
                             static_cast<void*>(&pass));
      opt1.set_xtol_rel(_deficit_global_xtol);
      opt1.set_ftol_rel(_deficit_global_ftol);
      opt1.optimize(x, minf1);
    }

    nlopt::opt opt(_deficit_local_opt, 5);
    opt.set_lower_bounds(lb);
    opt.set_upper_bounds(ub);
    opt.set_min_objective(protect_dis::def_nlopt3<T1>,
                          static_cast<void*>(&pass));
    opt.set_xtol_rel(_deficit_local_xtol);
    opt.set_ftol_rel(_deficit_local_ftol);
    opt.optimize(x, minf);

    _result = -_S_A_B + static_cast<trait::pT<T1> >(minf);
    _tp = arma::conv_to<arma::Col<trait::pT<T1> > >::from(x);
    _is_computed = true;
  }
  return *this;
}
//****************************************************************************

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::compute_reg() {
  s_a_b();
  if (_deficit2) {
    arma::uword dim1 = arma::prod(_dim);
    dim1 /= 2;
    arma::uword dim2(1);
    for (arma::uword i = 0; i < _nodal - 1; ++i) dim2 *= _dim.at(i);
    arma::uword dim3(1);
    for (arma::uword i = _nodal; i < _party_no; ++i) dim3 *= _dim.at(i);

    arma::Mat<trait::pT<T1> > eye2 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim1, dim1);
    arma::Mat<trait::pT<T1> > eye3 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim2, dim2);
    arma::Mat<trait::pT<T1> > eye4 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim3, dim3);

    arma::Col<trait::pT<T1> > ret;

    for (arma::uword i = 0; i < 3; ++i) {
      arma::Mat<std::complex<trait::pT<T1> > > proj1 =
        SPM<trait::pT<T1> >::get_instance().proj2.at(0, i + 1);

      arma::Mat<std::complex<trait::pT<T1> > > proj2 =
        SPM<trait::pT<T1> >::get_instance().proj2.at(1, i + 1);

      if (_nodal == 1) {
        proj1 = kron(proj1, eye2);
        proj2 = kron(proj2, eye2);
      } else if (_party_no == _nodal) {
        proj1 = kron(eye2, proj1);
        proj2 = kron(eye2, proj2);
      } else {
        proj1 = kron(kron(eye3, proj1), eye4);
        proj2 = kron(kron(eye3, proj2), eye4);
      }

      arma::Mat<std::complex<trait::pT<T1> > > rho_1 = (proj1 * _rho * proj1);
      arma::Mat<std::complex<trait::pT<T1> > > rho_2 = (proj2 * _rho * proj2);

      rho_1 += rho_2;
      trait::pT<T1> S_max = entropy(rho_1);
      ret.at(i) = -_S_A_B + S_max;
    }
    _result_reg = arma::min(ret);
    _result_reg_all = std::move(ret);
    _is_reg_computed = true;
  }

  if (_deficit3) {
    arma::uword dim1 = arma::prod(_dim);
    dim1 /= 3;
    arma::uword dim2(1);
    for (arma::uword i = 0; i < _nodal - 1; ++i) dim2 *= _dim.at(i);

    arma::uword dim3(1);
    for (arma::uword i = _nodal; i < _party_no; ++i) dim3 *= _dim.at(i);

    arma::Mat<trait::pT<T1> > eye2 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim1, dim1);
    arma::Mat<trait::pT<T1> > eye3 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim2, dim2);
    arma::Mat<trait::pT<T1> > eye4 =
      arma::eye<arma::Mat<trait::pT<T1> > >(dim3, dim3);

    arma::Col<trait::pT<T1> > ret;

    for (arma::uword i = 0; i < 3; ++i) {
      arma::Mat<std::complex<trait::pT<T1> > > proj1 =
        SPM<trait::pT<T1> >::get_instance().proj3.at(0, i + 1);

      arma::Mat<std::complex<trait::pT<T1> > > proj2 =
        SPM<trait::pT<T1> >::get_instance().proj3.at(1, i + 1);

      arma::Mat<std::complex<trait::pT<T1> > > proj3 =
        SPM<trait::pT<T1> >::get_instance().proj3.at(2, i + 1);

      if (_nodal == 1) {
        proj1 = kron(proj1, eye2);
        proj2 = kron(proj2, eye2);
        proj3 = kron(proj3, eye2);

      } else if (_party_no == _nodal) {
        proj1 = kron(eye2, proj1);
        proj2 = kron(eye2, proj2);
        proj3 = kron(eye2, proj3);

      } else {
        proj1 = kron(kron(eye3, proj1), eye4);
        proj2 = kron(kron(eye3, proj2), eye4);
        proj3 = kron(kron(eye3, proj3), eye4);
      }

      arma::Mat<std::complex<trait::pT<T1> > > rho_1 = (proj1 * _rho * proj1);
      arma::Mat<std::complex<trait::pT<T1> > > rho_2 = (proj2 * _rho * proj2);
      arma::Mat<std::complex<trait::pT<T1> > > rho_3 = (proj3 * _rho * proj3);

      rho_1 += rho_2 + rho_3;

      trait::pT<T1> S_max = entropy(rho_1);
      ret.at(i) = -_S_A_B + S_max;
    }
    _result_reg = arma::min(ret);
    _result_reg_all = std::move(ret);
    _is_reg_computed = true;
  }
  return *this;
}
//****************************************************************************

template <typename T1>
inline const arma::Col<trait::pT<T1> >&
deficit_space<T1>::opt_angles() noexcept {
  if (!_is_computed)
    compute();
  return _tp;
}

template <typename T1>
inline const trait::pT<T1>& deficit_space<T1>::result() noexcept {
  if (!_is_computed)
    compute();
  return _result;
}

template <typename T1>
inline const trait::pT<T1>& deficit_space<T1>::result_reg() noexcept {
  if (!_is_reg_computed)
    compute_reg();
  return _result_reg;
}

template <typename T1>
inline const arma::Col<trait::pT<T1> >&
deficit_space<T1>::result_reg_all() noexcept {
  if (!_is_reg_computed)
    compute_reg();
  return _result_reg_all;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::refresh() noexcept {
  _is_computed = false;
  _is_reg_computed = false;
  _is_sab_computed = false;

  if (_deficit2) {
    _deficit_global_opt = nlopt::GN_DIRECT_L;
    _deficit_global_xtol = 4.0e-2;
    _deficit_global_ftol = 0;
    _deficit_global = true;
    _deficit_local_opt = nlopt::LN_COBYLA;
    _deficit_local_xtol = 10 * _precision::eps<double>::value;
    _deficit_local_ftol = 0.0;

    _deficit_angle_range = {1.0, 2.0};
    _deficit_angle_ini = {0.1, 0.1};
  }

  if (_deficit3) {
    _deficit_global_opt = nlopt::GN_DIRECT_L;
    _deficit_global_xtol = 0.25;
    _deficit_global_ftol = 0;
    _deficit_global = true;
    _deficit_local_opt = nlopt::LN_COBYLA;
    _deficit_local_xtol = 10 * _precision::eps<double>::value;
    _deficit_local_ftol = 0.0;

    _deficit_angle_range = 2.0 * arma::ones<arma::vec>(5);
    _deficit_angle_ini = 2.0 * arma::ones<arma::vec>(5);
  }
  return *this;
}

template <typename T1>
inline deficit_space<T1>& deficit_space<T1>::reset_party(arma::uword nodal) {
  _is_computed = false;
  _is_reg_computed = false;
  _nodal = nodal;

#ifndef QICLIB_NO_DEBUG
  if (_nodal <= 0 || _nodal > _party_no)
    throw Exception("qic::deficit_space::change_party",
                    "Invalid measured party index!");
#endif

  _deficit2 = (_dim(_nodal - 1) == 2);
  _deficit3 = (_dim(_nodal - 1) == 3);

#ifndef QICLIB_NO_DEBUG
  if (!_deficit2 && !_deficit3)
    throw Exception("qic::deficit_space::change_party",
                    "Measured party is not qubit or qutrit!");
#endif

  if (_deficit2) {
    _deficit_global_opt = nlopt::GN_DIRECT_L;
    _deficit_global_xtol = 4.0e-2;
    _deficit_global_ftol = 0;
    _deficit_global = true;
    _deficit_local_opt = nlopt::LN_COBYLA;
    _deficit_local_xtol = 10 * _precision::eps<double>::value;
    _deficit_local_ftol = 0.0;

    _deficit_angle_range = {1.0, 2.0};
    _deficit_angle_ini = {0.1, 0.1};
  }

  if (_deficit3) {
    _deficit_global_opt = nlopt::GN_DIRECT_L;
    _deficit_global_xtol = 0.25;
    _deficit_global_ftol = 0;
    _deficit_global = true;
    _deficit_local_opt = nlopt::LN_COBYLA;
    _deficit_local_xtol = 10 * _precision::eps<double>::value;
    _deficit_local_ftol = 0.0;

    _deficit_angle_range = 2.0 * arma::ones<arma::vec>(5);
    _deficit_angle_ini = 2.0 * arma::ones<arma::vec>(5);
  }
  return *this;
}

//******************************************************************************

}  // namespace qic
