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

#ifndef __OB_SQL_I_END_TRANS_CALLBACK_H__
#define __OB_SQL_I_END_TRANS_CALLBACK_H__

#include "share/ob_define.h"
#include "lib/atomic/ob_atomic.h"
#include "lib/utility/utility.h"

namespace oceanbase {

namespace transaction {
class ObTransID;
}

namespace sql {

enum ObEndTransCallbackType {
  SYNC_CALLBACK_TYPE = 0, /* Synchronous waiting, such as DDL statement */
  /* *
   * Asynchronously execute operations after transaction submission,
   * such as DML statements, COMMINT/ROLLBACK statements to send execution results to the client
   * */
  ASYNC_CALLBACK_TYPE,
  NULL_CALLBACK_TYPE,
  WRITE_FROZEN_STAT_CALLBACK_TYPE,
  MOCK_CALLBACK_TYPE,
  MAX_CALLBACK_TYPE
};

/* The life cycle of ObIEndTransCallback:
 * Idea 1. Create at StartTrans and terminate after EndTrans call ends
 * In disconnect mode, synchronous waiting is adopted.
 * This action is unpredictable during StartTrans
 * Idea 2. Create before calling EndTrans and terminate after calling EndTrans
 * Need to know the current operation type (ac=0/1, commit/rollback, dml, disconnect)
 *
 * Explicit_end_trans (commit/rollback),
 * on_plan_end (ac=1 dml) two cases need to pass in ObIEndTransCallback from outside,
 * In other cases, sync mode is used,
 * and ObIEndTransCallback is internally constructed and released
 *
 * After calling end_trans and before the callback occurs,
 * is it possible to do the operation [may cause an error]?
 * The so-called [errors may occur] operation refers to calling a function and obtaining an error code,
 * which affects the SQL error output.
 * Assuming that an error code is generated by doing such an operation,
 * then the error code must be saved, otherwise it will be swallowed.
 * Can only be saved in Callback.
 * But there is a multithreading problem:
 * What should I do if Callback is already called when the error code is saved?
 * A simpler way: do nothing after end_trans,
 * and all other operations will be executed in the callback.
 *
 */
class ObIEndTransCallback {
public:
  ObIEndTransCallback()
  {
    reset();
  }
  virtual ~ObIEndTransCallback(){};
  /*
   * In some scenarios (such as cmd triggering implicit submission),
   * you need to wait for the callback to be successfully called synchronously
   * In order to continue the subsequent operation, so the wait () operation is introduced
   */
  virtual int wait()
  {
    return common::OB_NOT_IMPLEMENT;
  }
  /*
   * Called after the transaction is completed, the specific action is defined by the object itself
   * Such as: wake up wait(); return a packet to the client; return a packet to the RPC caller, etc.
   */
  virtual void callback(int cb_param) = 0;
  virtual void callback(int cb_param, const transaction::ObTransID& trans_id) = 0;
  virtual const char* get_type() const = 0;
  virtual ObEndTransCallbackType get_callback_type() const = 0;
  // Indicates whether it is possible for the same callback object to be shared
  // by multiple end trans functions at the same time,
  // and it is possible to call the callback function of the same callback object concurrently
  virtual bool is_shared() const = 0;

  void set_last_error(int last_err)
  {
    last_err_ = last_err;
  }

  inline void handout()
  {
    ATOMIC_INC(&call_counter_);
  }

  inline void handin()
  {
    ATOMIC_INC(&callback_counter_);
  }
  inline void reset()
  {
    last_err_ = common::OB_SUCCESS;
    call_counter_ = 0;
    callback_counter_ = 0;
  }

protected:
  inline void CHECK_BALANCE(const char* type) const
  {
    if (OB_UNLIKELY(callback_counter_ != call_counter_)) {
      SQL_LOG(ERROR,
          "Callback times mismatch. bug!!!",
          K(type),
          K(this),
          K(common::lbt()),
          K_(callback_counter),
          K_(call_counter));
    }
  }

protected:
  int last_err_;
  volatile uint64_t call_counter_;
  volatile uint64_t callback_counter_;
};

}  // namespace sql
}  // namespace oceanbase

#endif /* __OB_SQL_I_END_TRANS_CALLBACK_H__ */
//// end of header file
