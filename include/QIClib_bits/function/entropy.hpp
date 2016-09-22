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

// ***************************************************************************

template <typename T1,
          typename TR = typename std::enable_if<
            is_floating_point_var<trait::pT<T1> >::value, trait::pT<T1> >::type>
inline TR entropy(const T1& rho1) {
  const auto& rho = as_Mat(rho1);

  bool checkV = true;
  if (rho.n_cols == 1)
    checkV = false;

#ifndef QICLIB_NO_DEBUG
  if (rho.n_elem == 0)
    throw Exception("qic::entropy", Exception::type::ZERO_SIZE);

  if (checkV)
    if (rho.n_rows != rho.n_cols)
      throw Exception("qic::entropy",
                      Exception::type::MATRIX_NOT_SQUARE_OR_CVECTOR);
#endif

  if (!checkV) {
    return 0;

  } else {
    auto eig = arma::eig_sym(rho);
    trait::pT<T1> S = 0.0;
    for (const auto& i : eig)
      S -= i > _precision::eps<trait::pT<T1> >::value ? i * std::log2(i) : 0;
    return S;
  }
}

//****************************************************************************

template <typename T1,
          typename TR = typename std::enable_if<
            is_floating_point_var<trait::eT<T1> >::value, trait::eT<T1> >::type>
inline TR shannon(const T1& prob1) {
  const auto& prob = as_Mat(prob1);

#ifndef QICLIB_NO_DEBUG
  if (prob.n_elem == 0)
    throw Exception("qic::shannon", Exception::type::ZERO_SIZE);

  if (prob.n_cols != 1)
    throw Exception("qic::shannon", Exception::type::MATRIX_NOT_CVECTOR);

  if (arma::any(as_Col(prob) < -_precision::eps<trait::pT<T1> >::value))
    throw Exception("qic::shannon", "Invalid probaility distribution");
#endif

  trait::eT<T1> S = 0.0;

  for (const auto& i : prob)
    S -= i > _precision::eps<trait::pT<T1> >::value ? i * std::log2(i) : 0;
  return S;
}

//****************************************************************************

template <typename T1, typename TR = typename std::enable_if<
                         is_floating_point_var<T1>::value, T1>::type>
inline TR shannon(const std::vector<T1>& prob1) {
  return shannon(static_cast<arma::Col<T1> >(prob1));
}

//****************************************************************************

template <typename T1, typename TR = typename std::enable_if<
                         std::is_arithmetic<T1>::value, T1>::type>
inline TR shannon(const std::initializer_list<T1>& prob1) {
  return shannon(static_cast<arma::Col<double> >(prob1));
}

//****************************************************************************

template <typename T1,
          typename TR = typename std::enable_if<
            is_floating_point_var<trait::pT<T1> >::value, trait::pT<T1> >::type>
inline TR renyi(const T1& rho1, const trait::pT<T1>& alpha) {
  const auto& rho = as_Mat(rho1);

  bool checkV = true;
  if (rho.n_cols == 1)
    checkV = false;

#ifndef QICLIB_NO_DEBUG
  if (rho.n_elem == 0)
    throw Exception("qic::renyi", Exception::type::ZERO_SIZE);

  if (checkV)
    if (rho.n_rows != rho.n_cols)
      throw Exception("qic::renyi",
                      Exception::type::MATRIX_NOT_SQUARE_OR_CVECTOR);

  if (alpha < -_precision::eps<trait::pT<T1> >::value)
    throw Exception("qic::renyi", Exception::type::OUT_OF_RANGE);
#endif

  if (alpha < _precision::eps<trait::pT<T1> >::value) {
    return std::log2(static_cast<trait::pT<T1> >(rho.n_rows));

  } else if (!checkV) {
    return 0;

  } else {
    if (std::abs(alpha - 1) < _precision::eps<trait::pT<T1> >::value) {
      return entropy(rho);

    } else if (alpha == arma::Datum<trait::pT<T1> >::inf) {
      return -std::log2(arma::max(arma::eig_sym(rho)));

    } else {
      auto eig = arma::eig_sym(rho);
      trait::pT<T1> ret(0.0);
      for (const auto& x : eig)
        ret +=
          x > _precision::eps<trait::pT<T1> >::value ? std::pow(x, alpha) : 0;
      return std::log2(ret) / (1.0 - alpha);
    }
  }
}

//****************************************************************************

template <typename T1,
          typename TR = typename std::enable_if<
            is_floating_point_var<trait::eT<T1> >::value, trait::eT<T1> >::type>
inline TR renyi_prob(const T1& prob1, const trait::eT<T1>& alpha) {
  const auto& prob2 = as_Mat(prob1);

#ifndef QICLIB_NO_DEBUG
  if (prob2.n_elem == 0)
    throw Exception("qic::renyi_prob", Exception::type::ZERO_SIZE);

  if (prob2.n_cols != 1)
    throw Exception("qic::renyi_prob", Exception::type::MATRIX_NOT_CVECTOR);

  if (alpha < -_precision::eps<trait::pT<T1> >::value)
    throw Exception("qic::renyi_prob", Exception::type::OUT_OF_RANGE);

  if (arma::any(as_Col(prob2) < -_precision::eps<trait::pT<T1> >::value))
    throw Exception("qic::renyi", "Invalid probaility distribution");
#endif

  const auto& prob = as_Col(prob2);

  if (alpha < _precision::eps<trait::pT<T1> >::value) {
    return std::log2(static_cast<trait::eT<T1> >(prob.n_elem));

  } else if (std::abs(alpha - 1) < _precision::eps<trait::pT<T1> >::value) {
    return shannon(prob);

  } else if (alpha == arma::Datum<trait::eT<T1> >::inf) {
    return -std::log2(arma::max(prob));

  } else {
    trait::eT<T1> ret(0.0);
    for (const auto& x : prob)
      ret +=
        x > _precision::eps<trait::pT<T1> >::value ? std::pow(x, alpha) : 0;
    return std::log2(ret) / (1.0 - alpha);
  }
}

//****************************************************************************

template <typename T1, typename T2,
          typename TR = typename std::enable_if<
            is_floating_point_var<T1>::value && std::is_arithmetic<T2>::value,
            T1>::type>
inline TR renyi_prob(const std::vector<T1>& prob1, const T2& alpha) {
  return renyi_prob(static_cast<arma::Col<T1> >(prob1), static_cast<T1>(alpha));
}

//****************************************************************************

template <
  typename T1, typename T2,
  typename TR = typename std::enable_if<
    std::is_arithmetic<T1>::value && std::is_arithmetic<T2>::value, T1>::type>
inline TR renyi_prob(const std::initializer_list<T1>& prob1, const T2& alpha) {
  return renyi_prob(static_cast<arma::Col<double> >(prob1),
                    static_cast<double>(alpha));
}

// ***************************************************************************

template <typename T1, typename TR = typename std::enable_if<
                         is_floating_point_var<trait::pT<T1> >::value,
                         trait::pT<T1> >::value>
inline TR tsallis(const T1& rho1, const trait::pT<T1>& alpha) {
  const auto& rho = as_Mat(rho1);

  bool checkV = true;
  if (rho.n_cols == 1)
    checkV = false;

#ifndef QICLIB_NO_DEBUG
  if (rho.n_elem == 0)
    throw Exception("qic::tsallis", Exception::type::ZERO_SIZE);

  if (checkV)
    if (rho.n_rows != rho.n_cols)
      throw Exception("qic::tsallis",
                      Exception::type::MATRIX_NOT_SQUARE_OR_CVECTOR);

  if (alpha < -_precision::eps<trait::pT<T1> >::value)
    throw Exception("qic::tsallis", Exception::type::OUT_OF_RANGE);
#endif

  if (!checkV) {
    return 0;

  } else if (std::abs(alpha - 1) < _precision::eps<trait::pT<T1> >::value) {
    return std::log(2.0) * entropy(std::forward<T1>(rho));

  } else {
    auto eig = arma::eig_sym(rho);
    trait::pT<T1> ret(0.0);
    for (const auto& x : eig)
      ret +=
        x > _precision::eps<trait::pT<T1> >::value ? std::pow(x, alpha) : 0;
    return (ret - 1.0) / (1.0 - alpha);
  }
}

//****************************************************************************

template <typename T1,
          typename TR = typename std::enable_if<
            is_floating_point_var<trait::eT<T1> >::value, trait::eT<T1> >::type>
inline TR tsallis_prob(const T1& prob1, const trait::eT<T1>& alpha) {
  const auto& prob2 = as_Mat(prob1);

#ifndef QICLIB_NO_DEBUG
  if (prob2.n_elem == 0)
    throw Exception("qic::tsallis_prob", Exception::type::ZERO_SIZE);

  if (prob2.n_cols != 1)
    throw Exception("qic::tsallis_prob", Exception::type::MATRIX_NOT_CVECTOR);
  if (alpha < 0)
    throw Exception("qic::tsallis_prob", Exception::type::OUT_OF_RANGE);

  if (arma::any(as_Col(prob2) < -_precision::eps<trait::pT<T1> >::value))
    throw Exception("qic::tsallis_prob", "Invalid probaility distribution");
#endif

  const auto& prob = as_Col(prob2);

  if (std::abs(alpha - 1) < _precision::eps<trait::pT<T1> >::value) {
    return std::log(2.0) * shannon(prob);

  } else {
    trait::eT<T1> ret(0.0);
    for (const auto& x : prob)
      ret +=
        x > _precision::eps<trait::pT<T1> >::value ? std::pow(x, alpha) : 0;
    return (ret - 1.0) / (1.0 - alpha);
  }
}

//****************************************************************************

template <typename T1, typename T2,
          typename TR = typename std::enable_if<
            is_floating_point_var<T1>::value && std::is_arithmetic<T2>::value,
            T1>::type>
inline TR tsallis_prob(const std::vector<T1>& prob1, const T2& alpha) {
  return tsallis_prob(static_cast<arma::Col<T1> >(prob1),
                      static_cast<T1>(alpha));
}

//****************************************************************************

template <
  typename T1, typename T2,
  typename TR = typename std::enable_if<
    std::is_arithmetic<T1>::value && std::is_arithmetic<T2>::value, T1>::type>
inline TR tsallis_prob(const std::initializer_list<T1>& prob1,
                       const T2& alpha) {
  return renyi_prob(static_cast<arma::Col<double> >(prob1),
                    static_cast<double>(alpha));
}

//******************************************************************************

}  // namespace qic
