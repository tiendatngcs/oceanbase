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

#ifndef SRC_STORAGE_OB_PARTITION_BASE_DATA_RESTORE_READER_H_
#define SRC_STORAGE_OB_PARTITION_BASE_DATA_RESTORE_READER_H_

#include "share/ob_define.h"
#include "share/scheduler/ob_dag_scheduler.h"
#include "storage/ob_i_partition_base_data_reader.h"
#include "storage/ob_partition_base_data_oss_reader.h"

namespace oceanbase {

namespace storage {
class ObRestoreInfo;

class ObRestoreMacroDagWrapper;

/* class ObMigrateInfoRestoreFetcher: public ObIObMigrateInfoFetcher */
/* { */
/* public: */
/*   ObMigrateInfoRestoreFetcher(): saved_info_() {} */
/*   virtual ~ObMigrateInfoRestoreFetcher() {} */
/*   int init(const ObSavedStorageInfo &old_saved_info); */
/*   virtual int fetch_migrate_info_result(obrpc::ObMigrateInfoFetchResult &info); */

/* private: */
/*   ObSavedStorageInfo saved_info_; */
/*   DISALLOW_COPY_AND_ASSIGN(ObMigrateInfoRestoreFetcher); */
/* }; */

class ObPartitionBaseDataMetaRestoreReader {
public:
  ObPartitionBaseDataMetaRestoreReader();
  virtual ~ObPartitionBaseDataMetaRestoreReader();
  int init(common::ObInOutBandwidthThrottle& bandwidth_throttle, const common::ObPartitionKey& pkey,
      const ObDataStorageInfo& data_info, ObRestoreInfo& restore_info);
  int fetch_partition_meta(ObPGPartitionStoreMeta& partition_store_meta);
  int fetch_sstable_meta(const uint64_t index_id, blocksstable::ObSSTableBaseMeta& sstable_meta);
  int64_t get_data_size() const
  {
    return reader_.get_data_size();
  }
  int fetch_sstable_pair_list(const uint64_t index_id, common::ObIArray<blocksstable::ObSSTablePair>& pair_list);
  int fetch_all_table_ids(common::ObIArray<uint64_t>& table_id_array);
  int fetch_table_keys(const uint64_t index_id, obrpc::ObFetchTableInfoResult& table_res);
  TO_STRING_KV(K_(pkey), K_(restore_info), K_(last_read_size), K_(partition_store_meta), K_(snapshot_version),
      K_(schema_version), K_(data_version));

private:
  int prepare(const common::ObPartitionKey& pkey, const ObDataStorageInfo& data_info);
  int get_smallest_base_version(ObPartitionStore* partition_store, int64_t& base_version);
  int get_freeze_info(const int64_t snapshot_version, const ObPartitionKey& pkey, const ObDataStorageInfo& data_info);

private:
  bool is_inited_;
  common::ObPartitionKey pkey_;
  ObRestoreInfo* restore_info_;
  ObPartitionMetaStorageReader reader_;
  common::ObArenaAllocator allocator_;
  common::ObInOutBandwidthThrottle* bandwidth_throttle_;
  int64_t last_read_size_;
  ObPGPartitionStoreMeta partition_store_meta_;
  int64_t snapshot_version_;
  int64_t schema_version_;
  int64_t data_version_;
  DISALLOW_COPY_AND_ASSIGN(ObPartitionBaseDataMetaRestoreReader);
};

class ObPartitionKeyChangeUtil {
public:
  static int change_dst_pkey_to_src_pkey(
      const ObPartitionKey& dst_pkey, const ObRestoreInfo& restore_info, ObPartitionKey& src_pkey);
};

}  // namespace storage
}  // namespace oceanbase

#endif /* SRC_STORAGE_OB_PARTITION_BASE_DATA_RESTORE_READER_H_ */
