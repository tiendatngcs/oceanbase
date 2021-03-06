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

#include "observer/virtual_table/ob_all_virtual_trans_lock_stat.h"
#include "observer/ob_server.h"
#include "storage/ob_partition_group.h"

namespace oceanbase {
using namespace common;
using namespace transaction;

namespace observer {
void ObGVTransLockStat::reset()
{
  ip_buffer_[0] = '\0';
  partition_buffer_[0] = '\0';
  trans_id_buffer_[0] = '\0';
  proxy_sessid_buffer_[0] = '\0';
  memtable_key_buffer_[0] = '\0';
  ObVirtualTableScannerIterator::reset();
}

void ObGVTransLockStat::destroy()
{
  trans_service_ = NULL;
  memset(ip_buffer_, 0, common::OB_IP_STR_BUFF);
  memset(partition_buffer_, 0, OB_MIN_BUFFER_SIZE);
  memset(trans_id_buffer_, 0, OB_MIN_BUFFER_SIZE);
  memset(proxy_sessid_buffer_, 0, OB_MIN_BUFFER_SIZE);
  memset(memtable_key_buffer_, 0, OB_MEMTABLE_KEY_BUFFER_SIZE);

  ObVirtualTableScannerIterator::reset();
}

int ObGVTransLockStat::prepare_start_to_read_()
{
  int ret = OB_SUCCESS;
  if (NULL == allocator_ || NULL == trans_service_) {
    SERVER_LOG(WARN,
        "invalid argument, allocator_ or trans_service_ is null",
        "allocator",
        OB_P(allocator_),
        "trans_service",
        OB_P(trans_service_));
    ret = OB_INVALID_ARGUMENT;
  } else if (OB_SUCCESS != (ret = trans_service_->iterate_partition(partition_iter_))) {
    TRANS_LOG(WARN, "iterate partition error", K(ret));
  } else if (!partition_iter_.is_ready()) {
    TRANS_LOG(WARN, "ObPartitionIterator is not ready");
    ret = OB_ERR_UNEXPECTED;
  } else if (OB_SUCCESS != (ret = trans_lock_stat_iter_.set_ready())) {
    TRANS_LOG(WARN, "ObTransLockStatIterator set ready error", K(ret));
  } else {
    start_to_read_ = true;
  }

  return ret;
}

int ObGVTransLockStat::get_next_trans_lock_stat_(ObTransLockStat& trans_lock_stat)
{
  int ret = OB_SUCCESS;
  ObTransLockStat cur_trans_lock_stat;
  ObPartitionKey partition;
  bool bool_ret = true;

  while (bool_ret && OB_SUCCESS == ret) {
    if (OB_ITER_END == (ret = trans_lock_stat_iter_.get_next(cur_trans_lock_stat))) {
      if (OB_SUCCESS != (ret = partition_iter_.get_next(partition))) {
        if (OB_ITER_END != ret) {
          TRANS_LOG(WARN, "ObPartitionIterator get next partition error", K(ret));
        }
      } else {
        trans_lock_stat_iter_.reset();
        if (OB_SUCCESS != (ret = trans_service_->iterate_trans_lock_stat(partition, trans_lock_stat_iter_))) {
          TRANS_LOG(WARN, "iterate transaction lock stat error", K(ret), K(partition));
        } else {
          // do nothing
        }
      }
    } else {
      bool_ret = false;
    }
  }

  if (OB_SUCC(ret)) {
    trans_lock_stat = cur_trans_lock_stat;
  }

  return ret;
}

int ObGVTransLockStat::inner_get_next_row(ObNewRow*& row)
{
  int ret = OB_SUCCESS;
  ObTransLockStat trans_lock_stat;

  if (!start_to_read_ && OB_SUCCESS != (ret = prepare_start_to_read_())) {
    SERVER_LOG(WARN, "prepare start to read error", K(ret), K(start_to_read_));
  } else if (OB_FAIL(get_next_trans_lock_stat_(trans_lock_stat))) {
    if (OB_ITER_END != ret) {
      SERVER_LOG(WARN, "ObGVTransLockStat iter error", K(ret));
    } else {
      // do nothing
    }
  } else {
    const int64_t col_count = output_column_ids_.count();
    for (int64_t i = 0; i < col_count; i++) {
      uint64_t col_id = output_column_ids_.at(i);
      switch (col_id) {
        case OB_APP_MIN_COLUMN_ID:
          // tenant_id
          cur_row_.cells_[i].set_int(trans_lock_stat.get_tenant_id());
          break;
        case OB_APP_MIN_COLUMN_ID + 1:
          // trans_id
          (void)trans_lock_stat.get_trans_id().to_string(trans_id_buffer_, OB_MIN_BUFFER_SIZE);
          cur_row_.cells_[i].set_varchar(trans_id_buffer_);
          cur_row_.cells_[i].set_collation_type(ObCharset::get_default_collation(ObCharset::get_default_charset()));
          break;
        case OB_APP_MIN_COLUMN_ID + 2:
          // svr_ip
          (void)trans_lock_stat.get_addr().ip_to_string(ip_buffer_, common::OB_IP_STR_BUFF);
          cur_row_.cells_[i].set_varchar(ip_buffer_);
          cur_row_.cells_[i].set_collation_type(ObCharset::get_default_collation(ObCharset::get_default_charset()));
          break;
        case OB_APP_MIN_COLUMN_ID + 3:
          // svr_port
          cur_row_.cells_[i].set_int(trans_lock_stat.get_addr().get_port());
          break;
        case OB_APP_MIN_COLUMN_ID + 4:
          // partition
          if (trans_lock_stat.get_partition().is_valid()) {
            (void)trans_lock_stat.get_partition().to_string(partition_buffer_, OB_MIN_BUFFER_SIZE);
            cur_row_.cells_[i].set_varchar(partition_buffer_);
            cur_row_.cells_[i].set_collation_type(ObCharset::get_default_collation(ObCharset::get_default_charset()));
          } else {
            cur_row_.cells_[i].set_varchar(ObString("NULL"));
            cur_row_.cells_[i].set_collation_type(ObCharset::get_default_collation(ObCharset::get_default_charset()));
          }
          break;
        case OB_APP_MIN_COLUMN_ID + 5:
          // table_id
          cur_row_.cells_[i].set_int(trans_lock_stat.get_memtable_key().get_table_id());
          break;
        case OB_APP_MIN_COLUMN_ID + 6:
          // rowkey
          //(void)trans_lock_stat.get_memtable_key().to_string(memtable_key_buffer_, OB_MEMTABLE_KEY_BUFFER_SIZE);
          snprintf(
              memtable_key_buffer_, OB_MEMTABLE_KEY_BUFFER_SIZE, "%s", trans_lock_stat.get_memtable_key().read_buf());
          cur_row_.cells_[i].set_varchar(memtable_key_buffer_);
          cur_row_.cells_[i].set_collation_type(ObCharset::get_default_collation(ObCharset::get_default_charset()));
          break;
        case OB_APP_MIN_COLUMN_ID + 7:
          // session_id
          cur_row_.cells_[i].set_int(trans_lock_stat.get_session_id());
          break;
        case OB_APP_MIN_COLUMN_ID + 8:
          // proxy_id
          if (trans_lock_stat.get_proxy_session_id() > 0) {
            ObAddr client_server;
            // parse client server info
            (void)get_addr_by_proxy_sessid(trans_lock_stat.get_proxy_session_id(), client_server);
            if (client_server.is_valid()) {
              client_server.to_string(proxy_sessid_buffer_, OB_MIN_BUFFER_SIZE);
              cur_row_.cells_[i].set_varchar(proxy_sessid_buffer_);
              cur_row_.cells_[i].set_collation_type(ObCharset::get_default_collation(ObCharset::get_default_charset()));
            } else {
              cur_row_.cells_[i].set_varchar(ObString("NULL"));
              cur_row_.cells_[i].set_collation_type(ObCharset::get_default_collation(ObCharset::get_default_charset()));
            }
          } else {
            cur_row_.cells_[i].set_varchar(ObString("NULL"));
            cur_row_.cells_[i].set_collation_type(ObCharset::get_default_collation(ObCharset::get_default_charset()));
          }
          break;
        case OB_APP_MIN_COLUMN_ID + 9:
          // ctx_create_time
          cur_row_.cells_[i].set_timestamp(trans_lock_stat.get_ctx_create_time());
          break;
        case OB_APP_MIN_COLUMN_ID + 10:
          // expired_time
          cur_row_.cells_[i].set_timestamp(trans_lock_stat.get_trans_expired_time());
          break;
        case OB_APP_MIN_COLUMN_ID + 11:
          // row_lock_addr
          cur_row_.cells_[i].set_uint64(uint64_t(trans_lock_stat.get_memtable_key().get_row_lock()));
          break;
      }
    }
  }

  if (OB_SUCC(ret)) {
    row = &cur_row_;
  }

  return ret;
}

}  // namespace observer
}  // namespace oceanbase
