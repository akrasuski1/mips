#include "common.h"

#include "faiss/Index.h"
#include <map>
#include <set>


struct lsh_hash_t {
    std::vector<float> a;
    float b;
};

struct lsh_metahash_t {
    typedef unsigned long long hash_t;

    std::vector<lsh_hash_t> hashes;
    std::map<hash_t, std::set<faiss::Index::idx_t> > table;
};

struct IndexALSH: public faiss::Index {
    IndexALSH(size_t dim, size_t L, size_t K, float r = 2.5, float U = 0.83, size_t m = 3);
    void add(idx_t n, const float* data);
    void search(idx_t n, const float* data, idx_t k, float* distances, idx_t* labels) const;
    void reset();
    // void train(idx_t n, const float* data);

    std::vector<lsh_metahash_t> metahashes;

    
    // Parameters:
    size_t L;
    size_t K;
    float r;
    float U;
    size_t m;

    void hash_vectors(FloatMatrix& data);
    int dot_product_hash(const float* a, const float* x, const float b) const;
    std::vector<idx_t> answer_query(float *query, size_t k_needed = 1) const;
    lsh_metahash_t::hash_t calculate_metahash(size_t l, const float* data) const;
};
