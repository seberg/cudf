/*
 * Copyright (c) 2019-2024, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cudf/column/column.hpp>
#include <cudf/scalar/scalar.hpp>

#include <rmm/mr/device/per_device_resource.hpp>
#include <rmm/resource_ref.hpp>

#include <memory>

namespace cudf {

/**
 * @addtogroup transformation_binaryops
 * @{
 * @file
 * @brief Column APIs for binary ops
 */

/**
 * @brief Types of binary operations that can be performed on data.
 */
enum class binary_operator : int32_t {
  ADD,          ///< operator +
  SUB,          ///< operator -
  MUL,          ///< operator *
  DIV,          ///< operator / using common type of lhs and rhs
  TRUE_DIV,     ///< operator / after promoting type to floating point
  FLOOR_DIV,    ///< operator //
                ///< integer division rounding towards negative
                ///< infinity if both arguments are integral;
                ///< floor division for floating types (using C++ type
                ///< promotion for mixed integral/floating arguments)
                ///< If different promotion semantics are required, it
                ///< is the responsibility of the caller to promote
                ///< manually before calling in to this function.
  MOD,          ///< operator %
  PMOD,         ///< positive modulo operator
                ///< If remainder is negative, this returns (remainder + divisor) % divisor
                ///< else, it returns (dividend % divisor)
  PYMOD,        ///< operator % but following Python's sign rules for negatives
  POW,          ///< lhs ^ rhs
  INT_POW,      ///< int ^ int, used to avoid floating point precision loss. Returns 0 for negative
                ///< exponents.
  LOG_BASE,     ///< logarithm to the base
  ATAN2,        ///< 2-argument arctangent
  SHIFT_LEFT,   ///< operator <<
  SHIFT_RIGHT,  ///< operator >>
  SHIFT_RIGHT_UNSIGNED,  ///< operator >>> (from Java)
                         ///< Logical right shift. Casts to an unsigned value before shifting.
  BITWISE_AND,           ///< operator &
  BITWISE_OR,            ///< operator |
  BITWISE_XOR,           ///< operator ^
  LOGICAL_AND,           ///< operator &&
  LOGICAL_OR,            ///< operator ||
  EQUAL,                 ///< operator ==
  NOT_EQUAL,             ///< operator !=
  LESS,                  ///< operator <
  GREATER,               ///< operator >
  LESS_EQUAL,            ///< operator <=
  GREATER_EQUAL,         ///< operator >=
  NULL_EQUALS,           ///< Returns true when both operands are null; false when one is null; the
                         ///< result of equality when both are non-null
  NULL_MAX,              ///< Returns max of operands when both are non-null; returns the non-null
                         ///< operand when one is null; or invalid when both are null
  NULL_MIN,              ///< Returns min of operands when both are non-null; returns the non-null
                         ///< operand when one is null; or invalid when both are null
  GENERIC_BINARY,        ///< generic binary operator to be generated with input
                         ///< ptx code
  NULL_LOGICAL_AND,  ///< operator && with Spark rules: (null, null) is null, (null, true) is null,
                     ///< (null, false) is false, and (valid, valid) == LOGICAL_AND(valid, valid)
  NULL_LOGICAL_OR,   ///< operator || with Spark rules: (null, null) is null, (null, true) is true,
                     ///< (null, false) is null, and (valid, valid) == LOGICAL_OR(valid, valid)
  INVALID_BINARY     ///< invalid operation
};
/**
 * @brief Performs a binary operation between a scalar and a column.
 *
 * The output contains the result of `op(lhs, rhs[i])` for all `0 <= i < rhs.size()`
 * The scalar is the left operand and the column elements are the right operand.
 * This distinction is significant in case of non-commutative binary operations
 *
 * Regardless of the operator, the validity of the output value is the logical
 * AND of the validity of the two operands except NullMin and NullMax (logical OR).
 *
 * @param lhs         The left operand scalar
 * @param rhs         The right operand column
 * @param op          The binary operator
 * @param output_type The desired data type of the output column
 * @param stream CUDA stream used for device memory operations and kernel launches
 * @param mr          Device memory resource used to allocate the returned column's device memory
 * @return            Output column of `output_type` type containing the result of
 *                    the binary operation
 * @throw cudf::logic_error if @p output_type dtype isn't fixed-width
 * @throw cudf::logic_error if @p output_type dtype isn't boolean for comparison and logical
 * operations.
 * @throw cudf::data_type_error if the operation is not supported for the types of @p lhs and @p rhs
 */
std::unique_ptr<column> binary_operation(
  scalar const& lhs,
  column_view const& rhs,
  binary_operator op,
  data_type output_type,
  rmm::cuda_stream_view stream      = cudf::get_default_stream(),
  rmm::device_async_resource_ref mr = rmm::mr::get_current_device_resource());

/**
 * @brief Performs a binary operation between a column and a scalar.
 *
 * The output contains the result of `op(lhs[i], rhs)` for all `0 <= i < lhs.size()`
 * The column elements are the left operand and the scalar is the right operand.
 * This distinction is significant in case of non-commutative binary operations
 *
 * Regardless of the operator, the validity of the output value is the logical
 * AND of the validity of the two operands except NullMin and NullMax (logical OR).
 *
 * @param lhs         The left operand column
 * @param rhs         The right operand scalar
 * @param op          The binary operator
 * @param output_type The desired data type of the output column
 * @param stream CUDA stream used for device memory operations and kernel launches
 * @param mr          Device memory resource used to allocate the returned column's device memory
 * @return            Output column of `output_type` type containing the result of
 *                    the binary operation
 * @throw cudf::logic_error if @p output_type dtype isn't fixed-width
 * @throw cudf::logic_error if @p output_type dtype isn't boolean for comparison and logical
 * operations.
 * @throw cudf::data_type_error if the operation is not supported for the types of @p lhs and @p rhs
 */
std::unique_ptr<column> binary_operation(
  column_view const& lhs,
  scalar const& rhs,
  binary_operator op,
  data_type output_type,
  rmm::cuda_stream_view stream      = cudf::get_default_stream(),
  rmm::device_async_resource_ref mr = rmm::mr::get_current_device_resource());

/**
 * @brief Performs a binary operation between two columns.
 *
 * The output contains the result of `op(lhs[i], rhs[i])` for all `0 <= i < lhs.size()`
 *
 * Regardless of the operator, the validity of the output value is the logical
 * AND of the validity of the two operands except NullMin and NullMax (logical OR).
 *
 * @param lhs         The left operand column
 * @param rhs         The right operand column
 * @param op          The binary operator
 * @param output_type The desired data type of the output column
 * @param stream CUDA stream used for device memory operations and kernel launches
 * @param mr          Device memory resource used to allocate the returned column's device memory
 * @return            Output column of `output_type` type containing the result of
 *                    the binary operation
 * @throw cudf::logic_error if @p lhs and @p rhs are different sizes
 * @throw cudf::logic_error if @p output_type dtype isn't boolean for comparison and logical
 * operations.
 * @throw cudf::logic_error if @p output_type dtype isn't fixed-width
 * @throw cudf::data_type_error if the operation is not supported for the types of @p lhs and @p rhs
 */
std::unique_ptr<column> binary_operation(
  column_view const& lhs,
  column_view const& rhs,
  binary_operator op,
  data_type output_type,
  rmm::cuda_stream_view stream      = cudf::get_default_stream(),
  rmm::device_async_resource_ref mr = rmm::mr::get_current_device_resource());

/**
 * @brief Performs a binary operation between two columns using a
 * user-defined PTX function.
 *
 * The output contains the result of `op(lhs[i], rhs[i])` for all `0 <= i < lhs.size()`
 *
 * Regardless of the operator, the validity of the output value is the logical
 * AND of the validity of the two operands
 *
 * @param lhs         The left operand column
 * @param rhs         The right operand column
 * @param ptx         String containing the PTX of a binary function
 * @param output_type The desired data type of the output column. It is assumed
 *                    that output_type is compatible with the output data type
 *                    of the function in the PTX code
 * @param stream CUDA stream used for device memory operations and kernel launches
 * @param mr          Device memory resource used to allocate the returned column's device memory
 * @return            Output column of `output_type` type containing the result of
 *                    the binary operation
 * @throw cudf::logic_error if @p lhs and @p rhs are different sizes
 * @throw cudf::logic_error if @p lhs and @p rhs dtypes aren't numeric
 * @throw cudf::logic_error if @p output_type dtype isn't numeric
 */
std::unique_ptr<column> binary_operation(
  column_view const& lhs,
  column_view const& rhs,
  std::string const& ptx,
  data_type output_type,
  rmm::cuda_stream_view stream      = cudf::get_default_stream(),
  rmm::device_async_resource_ref mr = rmm::mr::get_current_device_resource());

/**
 * @brief Computes the `scale` for a `fixed_point` number based on given binary operator `op`
 *
 * @param op           The binary_operator used for two `fixed_point` numbers
 * @param left_scale   Scale of left `fixed_point` number
 * @param right_scale  Scale of right `fixed_point` number
 * @return             The resulting `scale` of the computed `fixed_point` number
 */
int32_t binary_operation_fixed_point_scale(binary_operator op,
                                           int32_t left_scale,
                                           int32_t right_scale);

/**
 * @brief Computes the `data_type` for a `fixed_point` number based on given binary operator `op`
 *
 * @param op   The binary_operator used for two `fixed_point` numbers
 * @param lhs  `cudf::data_type` of left `fixed_point` number
 * @param rhs  `cudf::data_type` of right `fixed_point` number
 * @return     The resulting `cudf::data_type` of the computed `fixed_point` number
 */
cudf::data_type binary_operation_fixed_point_output_type(binary_operator op,
                                                         cudf::data_type const& lhs,
                                                         cudf::data_type const& rhs);

namespace binops {

/**
 * @brief Computes output valid mask for op between a column and a scalar
 *
 * @param col     Column to compute the valid mask from
 * @param s       Scalar to compute the valid mask from
 * @param stream  CUDA stream used for device memory operations and kernel launches
 * @param mr      Device memory resource used to allocate the returned valid mask
 * @return        Computed validity mask
 */
std::pair<rmm::device_buffer, size_type> scalar_col_valid_mask_and(
  column_view const& col,
  scalar const& s,
  rmm::cuda_stream_view stream      = cudf::get_default_stream(),
  rmm::device_async_resource_ref mr = rmm::mr::get_current_device_resource());

namespace compiled {
namespace detail {

/**
 * @brief struct binary operation using `NaN` aware sorting physical element comparators
 *
 * @param out mutable view of output column
 * @param lhs view of left operand column
 * @param rhs view of right operand column
 * @param is_lhs_scalar true if @p lhs is a single element column representing a scalar
 * @param is_rhs_scalar true if @p rhs is a single element column representing a scalar
 * @param op binary operator identifier
 * @param stream CUDA stream used for device memory operations
 */
void apply_sorting_struct_binary_op(mutable_column_view& out,
                                    column_view const& lhs,
                                    column_view const& rhs,
                                    bool is_lhs_scalar,
                                    bool is_rhs_scalar,
                                    binary_operator op,
                                    rmm::cuda_stream_view stream);
}  // namespace detail
}  // namespace compiled
}  // namespace binops

/** @} */  // end of group
}  // namespace cudf
