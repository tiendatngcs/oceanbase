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

#ifndef OCEANBASE_SQL_ENGINE_EXPR_OB_EXPR_SYS_PRIVILEGE_CHECK_
#define OCEANBASE_SQL_ENGINE_EXPR_OB_EXPR_SYS_PRIVILEGE_CHECK_
#include "sql/engine/expr/ob_expr_operator.h"
namespace oceanbase {
namespace sql {
class ObExprSysPrivilegeCheck : public ObFuncExprOperator {
public:
  explicit ObExprSysPrivilegeCheck(common::ObIAllocator& alloc);
  virtual ~ObExprSysPrivilegeCheck();
  virtual int calc_result_typeN(
      ObExprResType& type, ObExprResType* types, int64_t param_num, common::ObExprTypeCtx& type_ctx) const override;

  /// obj_array(level, tenant, db, table)
  virtual int calc_resultN(
      common::ObObj& result, const common::ObObj* obj_array, int64_t param_num, common::ObExprCtx& expr_ctx) const override;
  virtual int cg_expr(ObExprCGCtx& op_cg_ctx, const ObRawExpr& raw_expr, ObExpr& rt_expr) const override;
  static int eval_sys_privilege_check(const ObExpr& expr, ObEvalCtx& ctx, ObDatum& expr_datum);

private:
  static int check_show_priv(bool& allow_show, ObExecContext& exec_ctx, const common::ObString& level,
      const uint64_t tenant_id, const common::ObString& db_name, const common::ObString& table_name);
  DISALLOW_COPY_AND_ASSIGN(ObExprSysPrivilegeCheck);
};
}  // namespace sql
}  // namespace oceanbase
#endif  // OCEANBASE_SQL_ENGINE_EXPR_OB_EXPR_SYS_PRIVILEGE_CHECK_
