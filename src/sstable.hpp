/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 14:45:42
**/

#ifndef NOLSM

#ifndef _SSTABLE_HPP_
#define _SSTABLE_HPP_

#include <vector>
#include <string>
#include <cassert>

#include "utility.hpp"
#include "block.hpp"

namespace Mushroom {

class SSTable
{
	friend class SSTableManager;

	public:
		SSTable(table_t table_no, uint32_t level, uint32_t key_len);

		SSTable();

		static uint32_t FileSize(uint32_t level);

		bool ReachThreshold();

		bool Append(const Key &key, BlockManager *block_manager);

		void FormKeySlice(KeySlice *slice) const;

		uint32_t KeyLength() const { return key_len_; }

		const std::vector<Block *>& Blocks() const { return blocks_; }

		void Generate(const BLinkTree *b_link_tree, BlockManager *block_manager);

		void Reset();

		void Reset(table_t table_no, uint32_t level, uint32_t key_len);

		class Iterator {
			public:
				Iterator(const SSTable *sstable)
				:key_len_(sstable->KeyLength()), curr_(0), blocks_(sstable->Blocks()) {
					assert(blocks_.size());
					iter_ = new Block::Iterator(blocks_[curr_++], key_len_);
				}

				bool Next() {
					if (iter_->Next()) return true;
					else if (curr_ < blocks_.size()) {
						delete iter_;
						iter_ = new Block::Iterator(blocks_[curr_++], key_len_);
						return true;
					} else return false;
				}

				~Iterator() { delete iter_; }

				const Key& key() { return iter_->key_; }

			private:
				uint32_t                    key_len_;
				uint32_t                    curr_;
				Block::Iterator            *iter_;
				const std::vector<Block *> &blocks_;
		};

	private:
		struct BlockInfo {
			BlockInfo():block_num_(0) { }

			void AppendKeyRange(const Block *block, uint32_t key_len);

			uint32_t                  block_num_;
			std::vector<std::string>  smallest_;
			std::vector<std::string>  largest_;
		};

		table_t              table_no_;
		uint32_t             level_;
		uint32_t             key_len_;
		bool                 pin_;
		BlockInfo            info_;
		std::vector<Block *> blocks_;
		SSTable             *next_;
};

} // namespace Mushroom

#endif /* _SSTABLE_HPP_ */

#endif /* NOLSM */