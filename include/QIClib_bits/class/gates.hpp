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

template <typename T1, typename Enable = typename std::enable_if<
                         std::is_floating_point<T1>::value, void>::type>
class GATES final : public _internal::Singleton<const GATES<T1> > {
  friend class _internal::Singleton<const GATES<T1> >;

 public:
  typename arma::Mat<T1>::template fixed<2, 2> X{0};
  typename arma::Mat<std::complex<T1> >::template fixed<2, 2> Y{0};
  typename arma::Mat<T1>::template fixed<2, 2> Z{0};
  typename arma::Mat<T1>::template fixed<2, 2> Had{0};
  
  typename arma::Mat<T1>::template fixed<4, 4> CNOT{0};
  typename arma::Mat<T1>::template fixed<4, 4> CZ{0};
  typename arma::Mat<T1>::template fixed<4, 4> swap{0};

  typename arma::Mat<T1>::template fixed<8, 8> Tof{0};
  typename arma::Mat<T1>::template fixed<8, 8> Fred{0};

 private:
  GATES()
      : X(arma::real(SPM<T1>::get_instance().S.at(1))),
        Y(SPM<T1>::get_instance().S.at(2)),
        Z(arma::real(SPM<T1>::get_instance().S.at(3))) {

    Had << std::sqrt(0.5) << std::sqrt(0.5) << arma::endr << std::sqrt(0.5)
        << -std::sqrt(0.5) << arma::endr;

    //**************************************************************************

    CNOT.fill(0.0);
    CNOT.at(0, 0) = CNOT.at(1, 1) = CNOT.at(2, 3) = CNOT.at(3, 2) = 1.0;

    CZ.fill(0.0);
    CZ.at(0, 0) = CZ.at(1, 1) = CZ.at(2, 2) = 1.0;
    CZ.at(3, 3) = -1.0;

    swap.fill(0.0);
    swap.at(0, 0) = swap.at(1, 2) = swap.at(2, 1) = swap.at(3, 3) = 1.0;

    //**************************************************************************

    Tof.fill(0.0);
    Tof.at(0, 0) = Tof.at(1, 1) = Tof.at(2, 2) = Tof.at(3, 3) = Tof.at(4, 4) =
      Tof.at(5, 5) = Tof.at(6, 7) = Tof.at(7, 6) = 1.0;

    Fred.fill(0.0);
    Fred.at(0, 0) = Fred.at(1, 1) = Fred.at(2, 2) = Fred.at(3, 3) =
      Fred.at(4, 4) = Fred.at(5, 6) = Fred.at(6, 5) = Fred.at(7, 7) = 1.0;

    //**************************************************************************
  }
  ~GATES() = default;

 public:
  //**************************************************************************

  arma::Mat<std::complex<T1> > U2(T1 theta, const arma::Col<T1>& unit) const {
#ifndef QICLIB_NO_DEBUG
    if (unit.size() != 3)
      throw Exception("qic::GATES::U2", "Vector is not 3-dimensional!");

    if (std::abs(arma::norm(unit) - 1.0) > _precision::eps<T1>::value)
      throw Exception("qic::GATES::U2", "Vector is not unit vector!");
#endif

    const auto& I = _internal::cond_I<std::complex<T1> >::value;
    return std::cos(0.5 * theta) * arma::eye<arma::Mat<std::complex<T1> > > +
           I * std::sin(0.5 * theta) *
             (unit.at(0) * X + unit.at(1) * Y + unit.at(2) * Z);
  }

  //**************************************************************************

  arma::Mat<std::complex<T1> > qft(arma::uword dim) const {
#ifndef QICLIB_NO_DEBUG
    if (dim == 0)
      throw Exception("qic::GATES::qft", Exception::type::INVALID_DIMS);
#endif

    arma::Mat<std::complex<T1> > ret(dim, dim);
    const auto& I = _internal::cond_I<std::complex<T1> >::value;

    QICLIB_OPENMP_FOR_COLLAPSE_2
    for (arma::uword j = 0; j < dim; ++j) {
      for (arma::uword i = 0; i < dim; ++i) {
        ret.at(i, j) = 1.0 / std::sqrt(static_cast<T1>(dim)) *
                       std::exp(2.0 * I * arma::Datum<T1>::pi *
                                static_cast<T1>(i * j) / static_cast<T1>(dim));
      }
    }

    return ret;
  }

  //**************************************************************************

  template <typename T2, typename TR = typename std::enable_if<
                           is_floating_point_var<trait::pT<T2> >::value,
                           arma::Mat<trait::eT<T2> > >::type>
  inline TR make_ctrl(const T1& A, arma::uvec ctrl, arma::uvec sys,
                      arma::uvec dim) {
    const auto& A1 = _internal::as_Mat(A);

    arma::uword d = ctrl.n_elem > 0 ? dim.at(ctrl.at(0) - 1) : 1;
    arma::uword N = arma::prod(dim);

#ifndef QICLIB_NO_DEBUG
    if (A1.n_elem == 0)
      throw Exception("qic::GATES::make_ctrl", Exception::type::ZERO_SIZE);

    if (A1.n_rows != A1.n_cols)
      throw Exception("qic::GATES::make_ctrl",
                      Exception::type::MATRIX_NOT_SQUARE);

    for (arma::uword i = 1; i < ctrl.n_elem; ++i)
      if (dim.at(ctrl.at(i) - 1) != d)
        throw Exception("qic::GATES::make_ctrl",
                        Exception::type::DIMS_NOT_EQUAL);

    if (dim.n_elem == 0 || arma::any(dim == 0))
      throw Exception("qic::GATES::make_ctrl", Exception::type::INVALID_DIMS);

    if (arma::prod(dim(sys - 1)) != A1.n_rows)
      throw Exception("qic::GATES::make_ctrl",
                      Exception::type::DIMS_MISMATCH_MATRIX);

    const arma::uvec ctrlsys = arma::join_cols(sys, ctrl);

    if (ctrlsys.n_elem > dim.n_elem ||
        arma::unique(ctrlsys).eval().n_elem != ctrlsys.n_elem ||
        arma::any(ctrlsys > dim.n_elem) || arma::any(ctrlsys == 0))
      throw Exception("qic::GATES::make_ctrl", Exception::type::INVALID_SUBSYS);
#endif

    _internal::dim_collapse_sys_ctrl(dim, sys, ctrl);

    const arma::uword n = dim.n_elem;
    const arma::uword m = sys.n_elem;
    const arma::uword o = ctrl.n_elem;

    arma::uvec keep(n - m);
    arma::uword keep_count(0);
    for (arma::uword run = 0; run < n; ++run) {
      if (!arma::any(sys == run + 1)) {
        keep.at(keep_count) = run + 1;
        ++keep_count;
      }
    }

    arma::uword product[_internal::MAXQDIT];
    product[n - 1] = 1;
    for (arma::sword i = n - 2; i >= 0; --i)
      product[i] = product[i + 1] * dim.at(i + 1);

    arma::uword productr[_internal::MAXQDIT];
    productr[m - 1] = 1;
    for (arma::sword i = m - 2; i >= 0; --i)
      productr[i] = productr[i + 1] * dim.at(sys(i) - 1);

    arma::uword p_num = std::max(static_cast<arma::uword>(1), d - 1);

    arma::field<arma::Mat<trait::eT<T2> > > Ap(p_num + 1);
    for (arma::uword i = 0; i <= p_num; ++i) Ap.at(i) = powm_gen(A1, i);

    arma::Mat<trait::eT<T2> > U(N, N, arma::fill::zeros);

    const arma::uword loop_no = 2 * n;
    constexpr auto loop_no_buffer = 2 * _internal::MAXQDIT + 1;
    arma::uword loop_counter[loop_no_buffer] = {0};
    arma::uword MAX[loop_no_buffer];

    for (arma::uword i = 0; i < n; ++i) {
      MAX[i] = dim.at(i);
      if (arma::any(keep == i + 1))
        MAX[i + n] = 1;
      else
        MAX[i + n] = dim.at(i);
    }
    MAX[loop_no] = 2;

    arma::uword p1 = 0;

    while (loop_counter[loop_no] == 0) {
      arma::uword count1(0), count2(0);
      for (arma::uword i = 0; i < n; ++i) {
        count1 += (arma::any(ctrl == i + 1) && loop_counter[i] != 0) ? 1 : 0;
        count2 += loop_counter[i + n] == 0 ? 1 : 0;
      }

      if ((count1 != o) && (count2 == n)) {
        arma::uword I(0);
        for (arma::uword i = 0; i < n; ++i) I += product[i] * loop_counter[i];
        U.at(I, I) = static_cast<trait::eT<T2> >(1.0);

      } else if (count1 == o) {
        arma::uword I(0), J(0), K(0), L(0);
        int power = o == 0 ? 1 : 0;

        for (arma::uword i = 0; i < n; ++i) {
          if (arma::any(keep == i + 1)) {
            I += product[i] * loop_counter[i];
            J += product[i] * loop_counter[i];

          } else {
            I += product[i] * loop_counter[i];
            J += product[i] * loop_counter[i + n];
          }

          arma::uword counter(0);
          while (any(sys == i + 1)) {
            if (sys.at(counter) != i + 1) {
              ++counter;
            } else {
              K += productr[counter] * loop_counter[i];
              L += productr[counter] * loop_counter[i + n];
              break;
            }
          }
        }

        if (o != 0) {
          arma::uword counter_1(1);
          for (arma::uword j = 1; j < o; ++j)
            counter_1 +=
              loop_counter[ctrl.at(0) - 1] == loop_counter[ctrl.at(j) - 1] ? 1
                                                                           : 0;
          power = counter_1 == o ? loop_counter[ctrl.at(0) - 1] : 0;
        }
        U.at(I, J) = Ap.at(power).at(K, L);
      }

      ++loop_counter[0];
      while (loop_counter[p1] == MAX[p1]) {
        loop_counter[p1] = 0;
        loop_counter[++p1]++;
        if (loop_counter[p1] != MAX[p1])
          p1 = 0;
      }
    }
    return U;
  }

  //**************************************************************************

  template <typename T2, typename TR = typename std::enable_if<
                           is_floating_point_var<trait::pT<T1> >::value,
                           arma::Mat<trait::eT<T2> > >::type>
  inline TR make_ctrl(const T1& A, arma::uvec ctrl, arma::uvec sys,
                      arma::uword n, arma::uword dim = 2) {
    const auto& A1 = _internal::as_Mat(A);

#ifndef QICLIB_NO_DEBUG
    if (n == 0)
      throw Exception("qic::GATES::make_ctrl", Exception::type::OUT_OF_RANGE);
    if (dim == 0)
      throw Exception("qic::GATES::make_ctrl", Exception::type::INVALID_DIMS);
#endif

    arma::uvec dim2(n);
    dim2.fill(dim);
    return make_ctrl(A1, std::move(ctrl), std::move(sys), std::move(dim2));
  }

  //**************************************************************************
};

//******************************************************************************

static const GATES<double>& gates _QICLIB_UNUSED_ =
  GATES<double>::get_instance();
static const GATES<float>& gatesf _QICLIB_UNUSED_ =
  GATES<float>::get_instance();

//******************************************************************************

}  // namespace qic
