/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef SRC_SQL_ENGINE_EXPR_OB_EXPR_UNIX_TIMESTAMP_H_
#define SRC_SQL_ENGINE_EXPR_OB_EXPR_UNIX_TIMESTAMP_H_

#include "sql/engine/expr/ob_expr_operator.h"

namespace oceanbase {
namespace sql {
class ObExprUnixTimestamp : public ObFuncExprOperator {
public:
  explicit ObExprUnixTimestamp(common::ObIAllocator& alloc);
  virtual ~ObExprUnixTimestamp();
  virtual int calc_result_typeN(
      ObExprResType& type, ObExprResType* type_array, int64_t param, common::ObExprTypeCtx& type_ctx) const override;
  virtual int calc_resultN(
      common::ObObj& result, const common::ObObj* date_param, int64_t param, common::ObExprCtx& expr_ctx) const override;
  static int calc(
      common::ObObj& result, const common::ObObj& dt_date, const ObExprResType& res_type, common::ObCastCtx& cast_ctx);
  static int eval_unix_timestamp(const ObExpr& expr, ObEvalCtx& ctx, ObDatum& res);
  virtual int cg_expr(ObExprCGCtx& expr_cg_ctx, const ObRawExpr& raw_expr, ObExpr& rt_expr) const override;

private:
  int calc_result_type_literal(ObExprResType& type, ObExprResType& type1) const;
  int calc_result_type_column(ObExprResType& type, ObExprResType& type1) const;
  // disallow copy
  DISALLOW_COPY_AND_ASSIGN(ObExprUnixTimestamp);
};
}  // namespace sql
}  // namespace oceanbase

#endif /* SRC_SQL_ENGINE_EXPR_OB_EXPR_UNIX_TIMESTAMP_H_ */
