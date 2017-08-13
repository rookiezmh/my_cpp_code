#ifndef GALAXY_HASH_MANAGER_H_
#define GALAXY_HASH_MANAGER_H_

#include "hasher.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>

namespace galaxy {

using std::pair;

const uint32_t kDefaultBlockSize = 10 * 1024 * 1024 /* 10M */;

template<typename Key, typename Val>
class HashManager {
public:
    
    typedef Key key_type;
    
    typedef Val mapped_type;

    struct HashItem {
        Key key;
        Val val;
        HashItem() { }
        HashItem(const pair<Key, Val> &p) : key(p.first), val(p.second) { }
        HashItem(const Key &k, const Val &v) : key(k), val(v) { }
    };

public:
    
    HashManager();
    
    ~HashManager() {}
    
    int WriteHashToFile(const char *file_name);

    int LoadHashFromFile(const char *file_name);

    int Insert(const pair<Key, Val> &p);

    int Insert(const Key &key, const Val &val);

    int FindItem(const Key &key) const;
    
    int FindItem(const Key &key, Val &val) const;

    int FinishHash();

private:
    
    void InitBlock();

    HashItem *FindItemPtr(const Key &key) const;

    // No copying allowed.
    HashManager(const HashManager &);
    HashManager &operator =(const HashManager &);
    
    void *memory_;

    uint32_t *hash_list_;

    uint32_t bucket_size_;

    struct HashBlock {
        HashItem * list;
        uint32_t num;
    };

    HashBlock current_block_;

    HashItem *item_list_;

    HashItem **block_list_;

    uint32_t block_num_;

    uint32_t block_limit_;

    uint32_t item_num_per_block_;
};

template<typename Key, typename Val>
void HashManager<Key, Val>::InitBlock() {
    item_num_per_block_ = kDefaultBlockSize / sizeof(HashItem);
    if (item_num_per_block_ < 1) {
        item_num_per_block_ = 1;
    } 
    for (uint32_t i = 0; i < block_num_; ++i) {
        free(block_list_[i]);
    }
    free(block_list_);
    block_num_  = 0;
    block_list_ = NULL;
    block_limit_ = 0;
    current_block_.list = NULL;
    current_block_.num = item_num_per_block_;
} 

template<typename Key, typename Val>
HashManager<Key, Val>::HashManager() : 
    memory_(NULL),
    hash_list_(NULL),
    bucket_size_(0),
    item_list_(NULL), 
    block_list_(NULL),
    block_num_(0) { 
        InitBlock();
    }

template<typename Key, typename Val>
int HashManager<Key, Val>::Insert(const Key &key, const Val &val) {
    if (current_block_.num >= item_num_per_block_) {
        if (block_num_ >= block_limit_) {
            block_limit_ = block_limit_ << 1;
            if (block_limit_ < 1000) {
                block_limit_ = 1000;
            }
            HashItem **t = (HashItem **) realloc(block_list_, sizeof(HashItem *) * block_limit_);
            if (t == NULL) {
                return -1;
            }
            block_list_ = t;
        }
        HashItem *t = (HashItem *) malloc(sizeof(HashItem) *item_num_per_block_);
        if (t == NULL) {
            return -1;
        }
        current_block_.list = t;
        current_block_.num = 0;
        block_list_[block_num_++] = current_block_.list;
    }
    HashItem *ip = &current_block_.list[current_block_.num++];
    ip->key = key;
    ip->val = val;
    return 0;
} 

template<typename Key, typename Val>
int HashManager<Key, Val>::Insert(const pair<Key, Val> &p) {
    return Insert(p.first, p.second);
}

template<typename Key, typename Val>
int HashManager<Key, Val>::FinishHash() {
    if (block_num_ == 0 || block_list_ == NULL) {
        return -1;
    } 
    uint32_t item_num = (block_num_ - 1) * item_num_per_block_ + current_block_.num;
    uint32_t bucket_size = item_num * 2 + 1;
    uint32_t *hash_num = (uint32_t *) calloc(bucket_size, sizeof(uint32_t));
    if (memory_ != NULL) {
        free(memory_);
    }
    memory_ = calloc(sizeof(HashItem) * item_num + sizeof(uint32_t) * (bucket_size + 1), 1);   
    hash_list_ = (uint32_t *) memory_;
    item_list_ = (HashItem *) (hash_list_ + bucket_size + 1);
    for (uint32_t i = 0; i < block_num_ - 1; ++i) {
        for (uint32_t j = 0; j < item_num_per_block_; ++j) {
            uint32_t h = Hash(block_list_[i][j].key);
            hash_list_[h % bucket_size]++;
        }
    }
    for (uint32_t i = 0; i < current_block_.num; ++i) {
        uint32_t h = Hash(current_block_.list[i].key);
        hash_list_[h % bucket_size]++;
    }
    uint32_t idx, oidx;
    idx = oidx = 0;
    for (uint32_t i = 0; i < bucket_size + 1; ++i) {
        // set hash_list_[bucket_size_]为item总数
        oidx = idx;
        idx += hash_list_[i];
        hash_list_[i] = oidx;
    }
    for (uint32_t i = 0; i < block_num_ - 1; ++i) {
        for (uint32_t j = 0; j < item_num_per_block_; ++j) {
            uint32_t h = Hash(block_list_[i][j].key) % bucket_size;
            item_list_[hash_list_[h] + hash_num[h]++] = block_list_[i][j];
        }         
    }
    for (uint32_t j = 0; j < current_block_.num; ++j) {
        uint32_t h = Hash(current_block_.list[j].key) % bucket_size;
        item_list_[hash_list_[h] + hash_num[h]++] = current_block_.list[j];
    }
    free(hash_num);
    InitBlock();
    bucket_size_ = bucket_size;
    return 0;
}

template<typename Key, typename Val>
int HashManager<Key, Val>::WriteHashToFile(const char *file_name) {
    if (bucket_size_ == 0) {
        return -1;
    }
    FILE *fp = fopen(file_name, "w");
    if (fp == NULL) {
        return -1;
    }
    if (fwrite(&bucket_size_, sizeof(bucket_size_), 1, fp) != 1){
        fclose(fp);
        return -1;
    }
    uint32_t sz = hash_list_[bucket_size_] * sizeof(HashItem) + (bucket_size_ + 1) * sizeof(uint32_t);
    if (fwrite(memory_, 1, sz, fp) != sz) {
        fclose(fp);
        return -1;
    } 
    return 0;
}

template<typename Key, typename Val>
typename HashManager<Key, Val>::HashItem *HashManager<Key, Val>::FindItemPtr(const Key &k) const {
    if (bucket_size_ == 0 || hash_list_ == NULL) {
        return NULL;    
    }
    uint32_t *entry;
    HashItem *pstart;
    HashItem *pend;
    entry = hash_list_ + Hash(k) % bucket_size_;
    pstart = item_list_ + *entry;
    pend = item_list_ + *(entry + 1);
    while (pstart != pend) {
        if (pstart->key == k) {
            return pstart;
        }
    }
    return NULL;
}

template<typename Key, typename Val>
int HashManager<Key, Val>::FindItem(const Key &k) const {
    HashItem *p = FindItemPtr(k);
    if (p == NULL) {
        return -1;
    } 
    return 0;
}

template<typename Key, typename Val>
int HashManager<Key, Val>::FindItem(const Key &k, Val &val) const {
    HashItem *p = FindItemPtr(k);
    if (p == NULL) {
        return -1;
    } 
    val = p->val;
    return 0;
}

}
#endif // GALAXY_HASH_MANAGER_H_
