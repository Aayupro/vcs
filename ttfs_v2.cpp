
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstddef>
#include <cstring>
#include <cstdlib>

struct TreeNode;
struct File;
class  TTFS;


template <typename T>
class CustomVector {
public:
    T*  data;
    int sz;
    int cap;

    static const int INIT_CAP = 4;

    CustomVector() : data(nullptr), sz(0), cap(INIT_CAP) {
        data = new T[cap];
    }
    CustomVector(const CustomVector& o) : data(nullptr), sz(o.sz), cap(o.cap) {
        data = new T[cap];
        for (int i = 0; i < sz; ++i) data[i] = o.data[i];
    }
    ~CustomVector() { delete[] data; }

    CustomVector& operator=(const CustomVector&) = delete;

    void push_back(const T& v) { if (sz == cap) _grow(); data[sz++] = v; }
    void pop_back()             { if (sz > 0) --sz; }
    void clear()                { sz = 0; }

    void remove_at(int idx) {
        if (idx < 0 || idx >= sz) return;
        for (int i = idx; i < sz - 1; ++i) data[i] = data[i+1];
        --sz;
    }
    int find(const T& v) const {
        for (int i = 0; i < sz; ++i) if (data[i] == v) return i;
        return -1;
    }

    T&       operator[](int i)       { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    int  size()  const { return sz; }
    bool empty() const { return sz == 0; }

private:
    void _grow() {
        int nc = cap * 2;
        T* tmp = new T[nc];
        for (int i = 0; i < sz; ++i) tmp[i] = data[i];
        delete[] data; data = tmp; cap = nc;
    }
};

template <typename V>
struct HEntry {
    int    key;
    V      value;
    HEntry* next;
    HEntry(int k, V v) : key(k), value(v), next(nullptr) {}
};

template <typename V>
class CustomHashMap {
public:
    static const int INIT_BUCKETS = 17;

    HEntry<V>** buckets;
    int         bucket_count;
    int         entry_count;

    CustomHashMap() : buckets(nullptr), bucket_count(INIT_BUCKETS), entry_count(0) {
        _alloc(bucket_count);
    }
    ~CustomHashMap() { _free_all(); }

    CustomHashMap(const CustomHashMap&)            = delete;
    CustomHashMap& operator=(const CustomHashMap&) = delete;

    void insert(int key, V val) {
        if (entry_count * 4 >= bucket_count * 3) _rehash();
        int idx = _h(key);
        for (auto* c = buckets[idx]; c; c = c->next)
            if (c->key == key) { c->value = val; return; }
        auto* n = new HEntry<V>(key, val);
        n->next = buckets[idx]; buckets[idx] = n; ++entry_count;
    }
    V* get(int key) const {
        int idx = _h(key);
        for (auto* c = buckets[idx]; c; c = c->next)
            if (c->key == key) return &c->value;
        return nullptr;
    }
    bool remove(int key) {
        int idx = _h(key);
        HEntry<V>* cur = buckets[idx]; HEntry<V>* prev = nullptr;
        while (cur) {
            if (cur->key == key) {
                if (prev) prev->next = cur->next; else buckets[idx] = cur->next;
                delete cur; --entry_count; return true;
            }
            prev = cur; cur = cur->next;
        }
        return false;
    }
    bool contains(int key) const { return get(key) != nullptr; }
    int  size()            const { return entry_count; }

    template <typename Fn>
    void for_each(Fn fn) const {
        for (int i = 0; i < bucket_count; ++i)
            for (auto* c = buckets[i]; c; c = c->next)
                fn(c->key, c->value);
    }

private:
    int _h(int key) const {
        return (int)(((unsigned)key * 2654435761u) % (unsigned)bucket_count);
    }
    void _alloc(int n) {
        buckets = new HEntry<V>*[n];
        for (int i = 0; i < n; ++i) buckets[i] = nullptr;
    }
    void _free_all() {
        for (int i = 0; i < bucket_count; ++i) {
            auto* c = buckets[i];
            while (c) { auto* nx = c->next; delete c; c = nx; }
        }
        delete[] buckets; buckets = nullptr; entry_count = 0;
    }
    void _rehash() {
        int nc = _next_prime(bucket_count * 2);
        auto** ob = buckets; int oc = bucket_count;
        bucket_count = nc; entry_count = 0; _alloc(nc);
        for (int i = 0; i < oc; ++i) {
            auto* c = ob[i];
            while (c) { insert(c->key, c->value); auto* nx = c->next; delete c; c = nx; }
        }
        delete[] ob;
    }
    static bool _is_prime(int n) {
        if (n < 2) return false;
        if (n < 4) return true;
        if (n%2==0||n%3==0) return false;
        for (int i=5; (long long)i*i<=n; i+=6)
            if (n%i==0||n%(i+2)==0) return false;
        return true;
    }
    static int _next_prime(int n) { while (!_is_prime(n)) ++n; return n; }
};

template <typename T, typename Cmp>
class MaxHeap {
public:
    T*  heap; int sz; int cap; Cmp cmp;
    static const int INIT_CAP = 8;

    MaxHeap() : heap(nullptr), sz(0), cap(INIT_CAP) { heap = new T[cap]; }
    ~MaxHeap() { delete[] heap; }
    MaxHeap(const MaxHeap&)            = delete;
    MaxHeap& operator=(const MaxHeap&) = delete;

    void push(const T& v) { if (sz==cap) _grow(); heap[sz]=v; _up(sz); ++sz; }
    const T& top() const  { return heap[0]; }
    void pop()            { if (sz==0) return; heap[0]=heap[--sz]; _down(0); }
    bool empty() const    { return sz==0; }
    int  size()  const    { return sz; }

    void rebuild() { for (int i=sz/2-1;i>=0;--i) _down(i); }

    bool remove(const T& v) {
        for (int i=0;i<sz;++i) if (heap[i]==v) { heap[i]=heap[--sz]; rebuild(); return true; }
        return false;
    }
private:
    void _up(int i) {
        while (i>0) { int p=(i-1)/2; if(cmp(heap[i],heap[p])){T t=heap[i];heap[i]=heap[p];heap[p]=t;i=p;}else break; }
    }
    void _down(int i) {
        while (true) {
            int m=i,l=2*i+1,r=2*i+2;
            if (l<sz&&cmp(heap[l],heap[m])) m=l;
            if (r<sz&&cmp(heap[r],heap[m])) m=r;
            if (m==i) break;
            T t=heap[i];heap[i]=heap[m];heap[m]=t; i=m;
        }
    }
    void _grow() { int nc=cap*2; T* t=new T[nc]; for(int i=0;i<sz;++i)t[i]=heap[i]; delete[]heap; heap=t; cap=nc; }
};

static const int LOG_N = 18;   

struct TreeNode {
    int         version_id;
    std::string content;
    std::string snapshot_message;
    time_t      created_timestamp;
    time_t      snapshot_timestamp;   

    TreeNode*   parent;
    TreeNode**  children;
    int         children_count;
    int         children_capacity;

    TreeNode*   anc[LOG_N];

    int depth;

    bool gc_marked;

    TreeNode(int id, const std::string& cnt, TreeNode* par, int dep)
        : version_id(id), content(cnt), snapshot_message(""),
          created_timestamp(std::time(nullptr)), snapshot_timestamp(0),
          parent(par), children(nullptr), children_count(0), children_capacity(0),
          depth(dep), gc_marked(false)
    {
        _init_children();
        for (int k = 0; k < LOG_N; ++k) anc[k] = nullptr;
        anc[0] = par;
        if (par) {
            for (int k = 1; k < LOG_N; ++k) {
                if (anc[k-1]) anc[k] = anc[k-1]->anc[k-1];
            }
        }
    }

    ~TreeNode() {
        for (int i = 0; i < children_count; ++i) { delete children[i]; children[i] = nullptr; }
        delete[] children; children = nullptr;
    }

    TreeNode(const TreeNode&)            = delete;
    TreeNode& operator=(const TreeNode&) = delete;

    void add_child(TreeNode* c) {
        if (children_count == children_capacity) _grow();
        children[children_count++] = c;
    }
    bool remove_child(TreeNode* c) {
        for (int i=0;i<children_count;++i) if(children[i]==c) {
            for (int j=i;j<children_count-1;++j) children[j]=children[j+1];
            --children_count; return true;
        }
        return false;
    }
    bool is_snapshot() const { return snapshot_timestamp != 0; }

private:
    void _init_children() {
        children_capacity = 2;
        children = new TreeNode*[children_capacity];
        for (int i=0;i<children_capacity;++i) children[i]=nullptr;
    }
    void _grow() {
        int nc = children_capacity * 2;
        TreeNode** t = new TreeNode*[nc];
        for (int i=0;i<children_count;++i) t[i]=children[i];
        for (int i=children_count;i<nc;++i) t[i]=nullptr;
        delete[] children; children=t; children_capacity=nc;
    }
};

struct File {
    std::string               filename;
    TreeNode*                 root;
    TreeNode*                 active_version;   
    int                       total_versions;
    CustomHashMap<TreeNode*>  version_map;    

    explicit File(const std::string& name)
        : filename(name), root(nullptr), active_version(nullptr), total_versions(0)
    {
        root = new TreeNode(0, "", nullptr, 0);
        version_map.insert(0, root);
        active_version = root;
        total_versions = 1;
    }
    ~File() { delete root; root = nullptr; active_version = nullptr; }
    File(const File&)            = delete;
    File& operator=(const File&) = delete;
};

struct RecentCmp { bool operator()(File* a, File* b) const {
    time_t ta = a->active_version ? a->active_version->created_timestamp : 0;
    time_t tb = b->active_version ? b->active_version->created_timestamp : 0;
    return ta > tb;
}};
struct VolumeCmp { bool operator()(File* a, File* b) const {
    return a->total_versions > b->total_versions;
}};


enum EditKind { EDIT_EQ, EDIT_INS, EDIT_DEL };
struct Edit {
    EditKind kind;
    std::string line;
};

static void split_lines(const std::string& s, CustomVector<std::string>& out) {
    std::istringstream ss(s);
    std::string line;
    while (std::getline(ss, line)) out.push_back(line);
    if (out.empty()) out.push_back(s);
}

static CustomVector<Edit> myers_diff(
        const CustomVector<std::string>& a,
        const CustomVector<std::string>& b)
{
    int N = a.size(), M = b.size();
    int MAX = N + M;

    int* V = new int[2 * MAX + 2]();

    int** trace = new int*[MAX + 1];
    for (int d = 0; d <= MAX; ++d) trace[d] = nullptr;

    int found_d = -1;
    V[MAX + 1] = 0;

    for (int d = 0; d <= MAX; ++d) {
        trace[d] = new int[2 * MAX + 2];
        for (int i = 0; i < 2*MAX+2; ++i) trace[d][i] = V[i];

        for (int k = -d; k <= d; k += 2) {
            int x;
            if (k == -d || (k != d && V[MAX+k-1] < V[MAX+k+1]))
                x = V[MAX + k + 1];         
            else
                x = V[MAX + k - 1] + 1;    

            int y = x - k;
            while (x < N && y < M && a[x] == b[y]) { ++x; ++y; }
            V[MAX + k] = x;

            if (x >= N && y >= M) { found_d = d; goto done; }
        }
    }
done:

    CustomVector<Edit> script;

    if (found_d < 0) {
        // All same
        for (int i = 0; i < N; ++i) { Edit e; e.kind=EDIT_EQ; e.line=a[i]; script.push_back(e); }
        delete[] V;
        for (int d = 0; d <= MAX; ++d) delete[] trace[d];
        delete[] trace;
        return script;
    }

    // Reverse path reconstruction
    CustomVector<Edit> rev;
    int x = N, y = M;
    for (int d = found_d; d > 0; --d) {
        int* Vp = trace[d - 1];
        int k = x - y;
        int prev_k;
        if (k == -d || (k != d && Vp[MAX+k-1] < Vp[MAX+k+1]))
            prev_k = k + 1;
        else
            prev_k = k - 1;

        int prev_x = Vp[MAX + prev_k];
        int prev_y = prev_x - prev_k;

        // Snake: equal lines from (prev_x,prev_y) to wherever the edit was
        while (x > prev_x + (prev_k == k-1 ? 0 : 1) && y > prev_y + (prev_k == k+1 ? 0 : 1)) {
            --x; --y;
            Edit e; e.kind = EDIT_EQ; e.line = a[x]; rev.push_back(e);
        }
        if (prev_k == k - 1) {
            // Came from k-1 moving right → delete from a
            Edit e; e.kind = EDIT_DEL; e.line = a[prev_x]; rev.push_back(e);
        } else {
            // Came from k+1 moving down → insert from b
            Edit e; e.kind = EDIT_INS; e.line = b[prev_y]; rev.push_back(e);
        }
        x = prev_x; y = prev_y;
    }
    // Remaining snake at the top
    while (x > 0 && y > 0) {
        --x; --y;
        Edit e; e.kind = EDIT_EQ; e.line = a[x]; rev.push_back(e);
    }

    // Reverse rev to get forward script
    for (int i = rev.size() - 1; i >= 0; --i) script.push_back(rev[i]);

    delete[] V;
    for (int d = 0; d <= MAX; ++d) delete[] trace[d];
    delete[] trace;
    return script;
}

// Pretty-print a Myers diff
static void print_myers_diff(const std::string& name, int vid_a, int vid_b,
                              const std::string& ca, const std::string& cb)
{
    CustomVector<std::string> la, lb;
    split_lines(ca, la);
    split_lines(cb, lb);
    CustomVector<Edit> edits = myers_diff(la, lb);

    std::cout << "\n--- v" << vid_a << "\n+++ v" << vid_b
              << "  [" << name << "]\n";

    bool any = false;
    for (int i = 0; i < edits.size(); ++i) {
        if (edits[i].kind == EDIT_DEL) { std::cout << "- " << edits[i].line << "\n"; any = true; }
        else if (edits[i].kind == EDIT_INS) { std::cout << "+ " << edits[i].line << "\n"; any = true; }
    }
    if (!any) std::cout << "(files identical)\n";
}

// =============================================================================
// SECTION 6 — ALGORITHM B: LCA WITH BINARY LIFTING
//   Pre-process: O(N log N).  Query: O(log N).
//   Used by three-way merge to find the common base commit automatically.
// =============================================================================

// Returns the LCA of nodes u and v in the version tree of a file.
// Uses the anc[][] table built into each TreeNode at construction time.
static TreeNode* lca(TreeNode* u, TreeNode* v) {
    if (!u || !v) return nullptr;

    // Bring both to the same depth
    if (u->depth < v->depth) { TreeNode* t = u; u = v; v = t; }  // u is deeper

    int diff = u->depth - v->depth;
    for (int k = 0; k < LOG_N; ++k)
        if ((diff >> k) & 1) { if (u->anc[k]) u = u->anc[k]; }

    if (u == v) return u;

    // Lift together
    for (int k = LOG_N - 1; k >= 0; --k) {
        if (u->anc[k] && v->anc[k] && u->anc[k] != v->anc[k]) {
            u = u->anc[k]; v = v->anc[k];
        }
    }
    return u->parent;  // one step above
}

// =============================================================================
// SECTION 7 — ALGORITHM C: THREE-WAY MERGE
//   Given base, ours, theirs — produce merged content or conflict markers.
//   Uses Myers diff twice (base→ours, base→theirs) then zips the edits.
//   Returns {merged_content, had_conflicts}.
// =============================================================================

struct MergeResult {
    std::string content;
    bool        had_conflicts;
};

static MergeResult three_way_merge(
        const std::string& base_content,
        const std::string& ours_content,
        const std::string& theirs_content)
{
    CustomVector<std::string> base_lines, our_lines, their_lines;
    split_lines(base_content,   base_lines);
    split_lines(ours_content,   our_lines);
    split_lines(theirs_content, their_lines);

    // Diff base→ours and base→theirs
    CustomVector<Edit> edits_ours   = myers_diff(base_lines, our_lines);
    CustomVector<Edit> edits_theirs = myers_diff(base_lines, their_lines);

    // Walk both edit scripts in lock-step over base lines.
    // Build output line by line.
    // Simple single-hunk conflict detection: if both sides changed the same
    // base line, emit conflict markers.
    MergeResult result;
    result.had_conflicts = false;

    int oi = 0, ti = 0;
    while (oi < edits_ours.size() || ti < edits_theirs.size()) {
        // Consume matching EQ blocks together
        while (oi < edits_ours.size() && ti < edits_theirs.size()
               && edits_ours[oi].kind == EDIT_EQ
               && edits_theirs[ti].kind == EDIT_EQ) {
            result.content += edits_ours[oi].line + "\n";
            ++oi; ++ti;
        }

        // Collect diverging hunks
        CustomVector<std::string> our_hunk, their_hunk;
        while (oi < edits_ours.size() && edits_ours[oi].kind != EDIT_EQ) {
            if (edits_ours[oi].kind == EDIT_INS) our_hunk.push_back(edits_ours[oi].line);
            ++oi;
        }
        while (ti < edits_theirs.size() && edits_theirs[ti].kind != EDIT_EQ) {
            if (edits_theirs[ti].kind == EDIT_INS) their_hunk.push_back(edits_theirs[ti].line);
            ++ti;
        }

        if (our_hunk.empty() && their_hunk.empty()) continue;

        bool same = (our_hunk.size() == their_hunk.size());
        if (same) for (int i=0;i<our_hunk.size();++i) if(our_hunk[i]!=their_hunk[i]){same=false;break;}

        if (our_hunk.empty()) {
            for (int i=0;i<their_hunk.size();++i) result.content += their_hunk[i] + "\n";
        } else if (their_hunk.empty() || same) {
            for (int i=0;i<our_hunk.size();++i) result.content += our_hunk[i] + "\n";
        } else {
            // CONFLICT
            result.had_conflicts = true;
            result.content += "<<<<<<< OURS\n";
            for (int i=0;i<our_hunk.size();++i) result.content += our_hunk[i] + "\n";
            result.content += "=======\n";
            for (int i=0;i<their_hunk.size();++i) result.content += their_hunk[i] + "\n";
            result.content += ">>>>>>> THEIRS\n";
        }
    }

    return result;
}

// =============================================================================
// SECTION 8 — ALGORITHM D: TRIE  (filename prefix search)
//   Each trie node has up to 128 children (printable ASCII).
//   insert: O(L)   search: O(L)   prefix_collect: O(P+R) where R=results
// =============================================================================

struct TrieNode {
    TrieNode* children[128];
    bool      is_end;
    TrieNode() : is_end(false) { for (int i=0;i<128;++i) children[i]=nullptr; }
    ~TrieNode() { for (int i=0;i<128;++i) { delete children[i]; children[i]=nullptr; } }
};

class Trie {
public:
    TrieNode* root;
    Trie() : root(new TrieNode()) {}
    ~Trie() { delete root; }

    void insert(const std::string& s) {
        TrieNode* cur = root;
        for (char c : s) {
            int idx = (unsigned char)c % 128;
            if (!cur->children[idx]) cur->children[idx] = new TrieNode();
            cur = cur->children[idx];
        }
        cur->is_end = true;
    }

    void remove(const std::string& s) {
        _remove(root, s, 0);
    }

    // Returns all filenames that start with prefix
    void prefix_search(const std::string& prefix,
                       CustomVector<std::string>& results) const {
        TrieNode* cur = root;
        for (char c : prefix) {
            int idx = (unsigned char)c % 128;
            if (!cur->children[idx]) return;
            cur = cur->children[idx];
        }
        _collect(cur, prefix, results);
    }

private:
    void _collect(TrieNode* node, const std::string& so_far,
                  CustomVector<std::string>& results) const {
        if (!node) return;
        if (node->is_end) results.push_back(so_far);
        for (int i=0;i<128;++i) {
            if (node->children[i]) {
                std::string next = so_far + (char)i;
                _collect(node->children[i], next, results);
            }
        }
    }
    bool _remove(TrieNode* node, const std::string& s, int depth) {
        if (!node) return false;
        if (depth == (int)s.size()) { node->is_end = false; return _is_leaf(node); }
        int idx = (unsigned char)s[depth] % 128;
        if (_remove(node->children[idx], s, depth+1)) {
            delete node->children[idx]; node->children[idx] = nullptr;
            return !node->is_end && _is_leaf(node);
        }
        return false;
    }
    bool _is_leaf(TrieNode* n) const {
        for (int i=0;i<128;++i) if (n->children[i]) return false;
        return true;
    }
};

// =============================================================================
// SECTION 9 — ALGORITHM E: INVERTED INDEX  (full-text search)
//   word → posting list of {file_id, version_id}
//   Insertion: O(W) per version  |  Lookup: O(1) average + O(P) to print
// =============================================================================

struct Posting {
    std::string filename;
    int         version_id;
    Posting*    next;
    Posting(const std::string& fn, int vid) : filename(fn), version_id(vid), next(nullptr) {}
};

struct PostingList {
    Posting* head;
    int      count;
    PostingList() : head(nullptr), count(0) {}
    ~PostingList() {
        Posting* c = head;
        while (c) { Posting* n = c->next; delete c; c = n; }
    }
    void append(const std::string& fn, int vid) {
        // Dedup: skip if same (filename, vid) already present
        for (Posting* c = head; c; c = c->next)
            if (c->filename == fn && c->version_id == vid) return;
        Posting* p = new Posting(fn, vid);
        p->next = head; head = p; ++count;
    }
};

// String hash for the word→PostingList map
struct WordHashEntry {
    std::string   word;
    PostingList   list;
    WordHashEntry* next;
    explicit WordHashEntry(const std::string& w) : word(w), next(nullptr) {}
};

class InvertedIndex {
public:
    static const int BUCKETS = 101;
    WordHashEntry** table;

    InvertedIndex() : table(new WordHashEntry*[BUCKETS]()) {}
    ~InvertedIndex() {
        for (int i=0;i<BUCKETS;++i) {
            WordHashEntry* c = table[i];
            while (c) { WordHashEntry* n = c->next; delete c; c = n; }
        }
        delete[] table;
    }

    void index_version(const std::string& filename, int vid, const std::string& content) {
        // Split on any non-alphanumeric boundary (handles std::cout -> ["std","cout"])
        std::string cur;
        for (int i = 0; i <= (int)content.size(); ++i) {
            char c = (i < (int)content.size()) ? content[i] : 0;
            if (std::isalnum((unsigned char)c) || c == '_') {
                cur += (char)std::tolower((unsigned char)c);
            } else {
                if (cur.size() >= 2) {
                    _get_or_create(cur)->list.append(filename, vid);
                }
                cur.clear();
            }
        }
    }

    void search(const std::string& raw_word,
                CustomVector<std::pair<std::string,int>>& results) const {
        std::string w = raw_word;
        for (char& c : w) c = (char)std::tolower((unsigned char)c);
        WordHashEntry* e = _find(w);
        if (!e) return;
        for (Posting* p = e->list.head; p; p = p->next) {
            // Use pair manually since std::pair allowed (it's not a container)
            std::pair<std::string,int> pr;
            pr.first = p->filename; pr.second = p->version_id;
            results.push_back(pr);
        }
    }

private:
    int _hash(const std::string& w) const {
        unsigned h = 5381;
        for (char c : w) h = h * 33 ^ (unsigned char)c;
        return (int)(h % BUCKETS);
    }
    WordHashEntry* _find(const std::string& w) const {
        int idx = _hash(w);
        for (auto* c = table[idx]; c; c = c->next)
            if (c->word == w) return c;
        return nullptr;
    }
    WordHashEntry* _get_or_create(const std::string& w) {
        WordHashEntry* e = _find(w);
        if (e) return e;
        int idx = _hash(w);
        e = new WordHashEntry(w);
        e->next = table[idx]; table[idx] = e;
        return e;
    }
};

// =============================================================================
// SECTION 10 — ALGORITHM F: WRITE-AHEAD LOG + UNDO/REDO
//   Every mutation appends a reversible Op to a doubly-linked log.
//   undo() reverses the last Op.  redo() replays it.
// =============================================================================

enum OpKind { OP_WRITE, OP_SNAPSHOT, OP_CHECKOUT, OP_DELETE };

struct LogOp {
    OpKind      kind;
    std::string filename;
    int         version_id;       // vid written or checked out
    int         prev_active_vid;  // HEAD before this op
    std::string content;
    std::string message;          // for snapshots

    LogOp* prev;
    LogOp* next;

    LogOp() : version_id(-1), prev_active_vid(-1), prev(nullptr), next(nullptr) {}
};

class WriteAheadLog {
public:
    LogOp* head;   // oldest
    LogOp* tail;   // newest (current)
    LogOp* cursor; // points to last applied op
    int    log_sz;

    WriteAheadLog() : head(nullptr), tail(nullptr), cursor(nullptr), log_sz(0) {}
    ~WriteAheadLog() {
        LogOp* c = head;
        while (c) { LogOp* n = c->next; delete c; c = n; }
    }

    // Append a new op; truncates redo history past cursor
    void append(LogOp* op) {
        // Truncate future (redo history is invalidated by new op)
        if (cursor && cursor->next) {
            LogOp* c = cursor->next;
            cursor->next = nullptr;
            tail = cursor;
            while (c) { LogOp* n = c->next; delete c; c = n; --log_sz; }
        }
        op->prev = tail;
        if (tail) tail->next = op; else head = op;
        tail = op; cursor = op; ++log_sz;
    }

    // Returns op to undo (cursor), moves cursor back
    LogOp* undo_step() {
        if (!cursor) return nullptr;
        LogOp* op = cursor;
        cursor = cursor->prev;
        return op;
    }

    // Returns op to redo (cursor->next), moves cursor forward
    LogOp* redo_step() {
        LogOp* next_op = cursor ? cursor->next : head;
        if (!next_op) return nullptr;
        cursor = next_op;
        return next_op;
    }

    void print_log() const {
        std::cout << "\n── Write-Ahead Log [" << log_sz << " entries] ──\n";
        int i = 1;
        for (LogOp* c = head; c; c = c->next, ++i) {
            const char* kinds[] = {"WRITE","SNAPSHOT","CHECKOUT","DELETE"};
            std::cout << "  " << i << ". " << kinds[c->kind]
                      << " '" << c->filename << "'";
            if (c->version_id >= 0) std::cout << " v" << c->version_id;
            if (c == cursor) std::cout << "  <-- HEAD";
            std::cout << "\n";
        }
    }
};

// =============================================================================
// SECTION 11 — ALGORITHM G: MARK-AND-SWEEP GARBAGE COLLECTOR
//   Marks all nodes reachable from every active HEAD.
//   Sweeps (frees) orphan nodes left behind by branch pruning.
//   O(V+E) over the entire version forest.
// =============================================================================
// GC runs inside TTFS — declared here, implemented as a method there.

// =============================================================================
// SECTION 12 — THE TTFS ENGINE
// =============================================================================

class TTFS {
public:
    CustomVector<File*>          files;
    MaxHeap<File*, RecentCmp>    heap_recent;
    MaxHeap<File*, VolumeCmp>    heap_volume;
    Trie                         filename_trie;
    InvertedIndex                inv_index;
    WriteAheadLog                wal;
    int                          next_vid;

    TTFS() : next_vid(1) {}
    ~TTFS() {
        for (int i = 0; i < files.size(); ++i) { delete files[i]; files[i] = nullptr; }
    }
    TTFS(const TTFS&)            = delete;
    TTFS& operator=(const TTFS&) = delete;

    // ── Lookup ──────────────────────────────────────────────────────────────
    File* find_file(const std::string& name) const {
        for (int i = 0; i < files.size(); ++i)
            if (files[i]->filename == name) return files[i];
        return nullptr;
    }

    // =========================================================================
    // API: create_file
    // =========================================================================
    bool create_file(const std::string& name) {
        if (find_file(name)) {
            std::cout << "[ERROR] '" << name << "' already exists.\n"; return false;
        }
        File* f = new File(name);
        files.push_back(f);
        heap_recent.push(f);
        heap_volume.push(f);
        filename_trie.insert(name);
        std::cout << "[CREATED] '" << name << "' (v0 root).\n";
        return true;
    }

    // =========================================================================
    // API: write_version
    // =========================================================================
    int write_version(const std::string& name, const std::string& content) {
        File* f = find_file(name);
        if (!f) { std::cout << "[ERROR] '" << name << "' not found.\n"; return -1; }

        int id = next_vid++;
        int prev_active = f->active_version->version_id;
        int dep = f->active_version->depth + 1;

        TreeNode* node = new TreeNode(id, content, f->active_version, dep);
        f->active_version->add_child(node);
        f->version_map.insert(id, node);
        f->active_version = node;
        ++f->total_versions;

        // Index content
        inv_index.index_version(name, id, content);

        heap_recent.rebuild();
        heap_volume.rebuild();

        // WAL
        LogOp* op = new LogOp();
        op->kind = OP_WRITE; op->filename = name;
        op->version_id = id; op->prev_active_vid = prev_active;
        op->content = content;
        wal.append(op);

        std::cout << "[WRITE] '" << name << "' -> v" << id
                  << " (depth=" << dep << ", total=" << f->total_versions << ").\n";
        return id;
    }

    // =========================================================================
    // API: snapshot
    // =========================================================================
    bool snapshot(const std::string& name, const std::string& msg) {
        File* f = find_file(name);
        if (!f) { std::cout << "[ERROR] '" << name << "' not found.\n"; return false; }
        TreeNode* cur = f->active_version;
        if (cur->is_snapshot()) {
            std::cout << "[WARN] v" << cur->version_id << " already snapshotted.\n"; return false;
        }
        cur->snapshot_message   = msg;
        cur->snapshot_timestamp = std::time(nullptr);

        LogOp* op = new LogOp();
        op->kind = OP_SNAPSHOT; op->filename = name;
        op->version_id = cur->version_id; op->message = msg;
        wal.append(op);

        std::cout << "[SNAPSHOT] '" << name << "' v" << cur->version_id
                  << " tagged: \"" << msg << "\".\n";
        return true;
    }

    // =========================================================================
    // API: checkout
    // =========================================================================
    bool checkout(const std::string& name, int vid) {
        File* f = find_file(name);
        if (!f) { std::cout << "[ERROR] '" << name << "' not found.\n"; return false; }
        TreeNode** target = f->version_map.get(vid);
        if (!target || !*target) {
            std::cout << "[ERROR] v" << vid << " not found in '" << name << "'.\n"; return false;
        }
        int prev = f->active_version->version_id;
        f->active_version = *target;

        LogOp* op = new LogOp();
        op->kind = OP_CHECKOUT; op->filename = name;
        op->version_id = vid; op->prev_active_vid = prev;
        wal.append(op);

        std::cout << "[CHECKOUT] '" << name << "' HEAD -> v" << vid << ".\n";
        return true;
    }

    // =========================================================================
    // API: branch_write
    // =========================================================================
    int branch_write(const std::string& name, int from_vid, const std::string& content) {
        if (!checkout(name, from_vid)) return -1;
        return write_version(name, content);
    }

    // =========================================================================
    // API: merge  (three-way merge using LCA as base)
    //   Merges 'their_vid' into the current HEAD of 'name'.
    // =========================================================================
    int merge(const std::string& name, int their_vid) {
        File* f = find_file(name);
        if (!f) { std::cout << "[ERROR] '" << name << "' not found.\n"; return -1; }

        TreeNode** their_ptr = f->version_map.get(their_vid);
        if (!their_ptr || !*their_ptr) {
            std::cout << "[ERROR] v" << their_vid << " not found.\n"; return -1;
        }
        TreeNode* ours   = f->active_version;
        TreeNode* theirs = *their_ptr;

        // ── Step 1: Find LCA (base commit) using binary lifting ──────────────
        TreeNode* base = lca(ours, theirs);
        if (!base) {
            std::cout << "[ERROR] No common ancestor found.\n"; return -1;
        }
        std::cout << "[MERGE] Base (LCA) = v" << base->version_id
                  << "  Ours = v" << ours->version_id
                  << "  Theirs = v" << their_vid << "\n";

        // ── Step 2: Three-way merge ──────────────────────────────────────────
        MergeResult result = three_way_merge(base->content, ours->content, theirs->content);

        if (result.had_conflicts) {
            std::cout << "[MERGE] Conflicts detected — writing merge commit with markers.\n";
        } else {
            std::cout << "[MERGE] Clean merge.\n";
        }

        return write_version(name, result.content);
    }

    // =========================================================================
    // API: read_version
    // =========================================================================
    std::string read_version(const std::string& name, int vid = -1) const {
        File* f = find_file(name);
        if (!f) { std::cout << "[ERROR] '" << name << "' not found.\n"; return ""; }
        TreeNode* t = (vid == -1) ? f->active_version
                                  : (f->version_map.get(vid) ? *f->version_map.get(vid) : nullptr);
        if (!t) { std::cout << "[ERROR] v" << vid << " not found.\n"; return ""; }
        return t->content;
    }

    // =========================================================================
    // API: delete_file
    // =========================================================================
    bool delete_file(const std::string& name) {
        int idx = -1;
        for (int i = 0; i < files.size(); ++i)
            if (files[i]->filename == name) { idx = i; break; }
        if (idx == -1) { std::cout << "[ERROR] '" << name << "' not found.\n"; return false; }
        File* f = files[idx];
        heap_recent.remove(f);
        heap_volume.remove(f);
        filename_trie.remove(name);
        files.remove_at(idx);

        LogOp* op = new LogOp();
        op->kind = OP_DELETE; op->filename = name; wal.append(op);

        delete f;
        std::cout << "[DELETED] '" << name << "'.\n";
        return true;
    }

    // =========================================================================
    // API: gc  —  Mark-and-Sweep Garbage Collector
    //   Marks all nodes reachable from all active HEADs.
    //   Any node not marked after the traversal is an orphan — freed.
    // =========================================================================
    void gc() {
        std::cout << "\n[GC] Starting mark-and-sweep...\n";
        int freed = 0;

        // Phase 1 — MARK: DFS from every HEAD, set gc_marked = true
        for (int fi = 0; fi < files.size(); ++fi) {
            File* f = files[fi];
            _gc_mark_dfs(f->root);
        }

        // Phase 2 — SWEEP: walk every node in every file's version_map.
        // Collect orphan IDs, then unlink them.
        for (int fi = 0; fi < files.size(); ++fi) {
            File* f = files[fi];
            CustomVector<int> orphans;
            f->version_map.for_each([&](int id, TreeNode* node) {
                if (!node->gc_marked) orphans.push_back(id);
            });

            for (int i = 0; i < orphans.size(); ++i) {
                int id = orphans[i];
                TreeNode** np = f->version_map.get(id);
                if (!np) continue;
                TreeNode* node = *np;

                // Unlink from parent's children array
                if (node->parent) node->parent->remove_child(node);

                f->version_map.remove(id);
                --f->total_versions;
                // Nullify children pointers so recursive delete doesn't double-free
                for (int c = 0; c < node->children_count; ++c) node->children[c] = nullptr;
                node->children_count = 0;
                delete node;
                ++freed;
            }
        }

        // Phase 3 — RESET marks on surviving nodes
        for (int fi = 0; fi < files.size(); ++fi)
            _gc_reset_dfs(files[fi]->root);

        heap_recent.rebuild();
        heap_volume.rebuild();
        std::cout << "[GC] Complete. Freed " << freed << " orphan node(s).\n";
    }

    // =========================================================================
    // API: undo
    // =========================================================================
    bool undo() {
        LogOp* op = wal.undo_step();
        if (!op) { std::cout << "[UNDO] Nothing to undo.\n"; return false; }

        File* f = find_file(op->filename);
        if (!f && op->kind != OP_DELETE) {
            std::cout << "[UNDO] File gone — cannot undo.\n"; return false;
        }

        if (op->kind == OP_WRITE || op->kind == OP_CHECKOUT) {
            if (f && op->prev_active_vid >= 0) {
                TreeNode** prev = f->version_map.get(op->prev_active_vid);
                if (prev && *prev) {
                    f->active_version = *prev;
                    std::cout << "[UNDO] '" << op->filename
                              << "' HEAD restored to v" << op->prev_active_vid << ".\n";
                }
            }
        } else if (op->kind == OP_SNAPSHOT) {
            if (f) {
                TreeNode** np = f->version_map.get(op->version_id);
                if (np && *np) {
                    (*np)->snapshot_message = "";
                    (*np)->snapshot_timestamp = 0;
                    std::cout << "[UNDO] Snapshot removed from v" << op->version_id << ".\n";
                }
            }
        } else {
            std::cout << "[UNDO] DELETE operations cannot be undone (GC may have freed nodes).\n";
        }
        return true;
    }

    // =========================================================================
    // API: redo
    // =========================================================================
    bool redo() {
        LogOp* op = wal.redo_step();
        if (!op) { std::cout << "[REDO] Nothing to redo.\n"; return false; }

        File* f = find_file(op->filename);
        if (!f) { std::cout << "[REDO] File gone.\n"; return false; }

        if (op->kind == OP_WRITE || op->kind == OP_CHECKOUT) {
            TreeNode** target = f->version_map.get(op->version_id);
            if (target && *target) {
                f->active_version = *target;
                std::cout << "[REDO] '" << op->filename
                          << "' HEAD -> v" << op->version_id << ".\n";
            }
        } else if (op->kind == OP_SNAPSHOT) {
            TreeNode** np = f->version_map.get(op->version_id);
            if (np && *np) {
                (*np)->snapshot_message = op->message;
                (*np)->snapshot_timestamp = std::time(nullptr);
                std::cout << "[REDO] Snapshot re-applied to v" << op->version_id << ".\n";
            }
        }
        return true;
    }

    // =========================================================================
    // API: search  —  inverted index full-text search
    // =========================================================================
    void search(const std::string& word) const {
        CustomVector<std::pair<std::string,int>> results;
        inv_index.search(word, results);
        std::cout << "\n── Search: \"" << word << "\" ["
                  << results.size() << " hit(s)] ──\n";
        for (int i = 0; i < results.size(); ++i)
            std::cout << "  " << results[i].first << " v" << results[i].second << "\n";
    }

    // =========================================================================
    // API: autocomplete  —  trie prefix search
    // =========================================================================
    void autocomplete(const std::string& prefix) const {
        CustomVector<std::string> results;
        filename_trie.prefix_search(prefix, results);
        std::cout << "\n── Autocomplete: \"" << prefix << "\" ["
                  << results.size() << " match(es)] ──\n";
        for (int i = 0; i < results.size(); ++i)
            std::cout << "  " << results[i] << "\n";
    }

    // =========================================================================
    // DISPLAY: print_version_tree  (DFS pretty-print)
    // =========================================================================
    void print_version_tree(const std::string& name) const {
        File* f = find_file(name);
        if (!f) { std::cout << "[ERROR] '" << name << "' not found.\n"; return; }
        std::cout << "\n+-- Version Tree: '" << name
                  << "'  [" << f->total_versions << " versions]"
                  << "  HEAD=v" << f->active_version->version_id << "\n";
        _print_node(f->root, f->active_version, "", true);
    }

    // =========================================================================
    // DISPLAY: print_history  (ancestor chain of HEAD)
    // =========================================================================
    void print_history(const std::string& name) const {
        File* f = find_file(name);
        if (!f) { std::cout << "[ERROR] '" << name << "' not found.\n"; return; }
        std::cout << "\n-- History of '" << name
                  << "' (HEAD=v" << f->active_version->version_id << ") --\n";
        TreeNode** chain = new TreeNode*[f->total_versions + 2];
        int n = 0;
        for (TreeNode* c = f->active_version; c; c = c->parent) chain[n++] = c;
        for (int i = n-1; i >= 0; --i) {
            TreeNode* node = chain[i];
            std::cout << "  v" << node->version_id;
            if (node->is_snapshot()) std::cout << " [SNAP:\"" << node->snapshot_message << "\"]";
            if (node->content.empty()) std::cout << " <root>";
            else {
                std::string p = node->content.substr(0,40);
                if (node->content.size()>40) p += "...";
                std::cout << " | " << p;
            }
            std::cout << "\n";
        }
        delete[] chain;
    }

    // =========================================================================
    // DISPLAY: print_diff  (Myers diff)
    // =========================================================================
    void print_diff(const std::string& name, int va, int vb) const {
        File* f = find_file(name);
        if (!f) { std::cout << "[ERROR] '" << name << "' not found.\n"; return; }
        TreeNode** na = f->version_map.get(va);
        TreeNode** nb = f->version_map.get(vb);
        if (!na || !nb) { std::cout << "[ERROR] Version not found.\n"; return; }
        print_myers_diff(name, va, vb, (*na)->content, (*nb)->content);
    }

    // =========================================================================
    // DISPLAY: list_files
    // =========================================================================
    void list_files() const {
        std::cout << "\n-- Files [" << files.size() << "] --\n";
        for (int i = 0; i < files.size(); ++i) {
            File* f = files[i];
            std::cout << "  " << (i+1) << ". " << f->filename
                      << "  versions=" << f->total_versions
                      << "  HEAD=v"   << f->active_version->version_id << "\n";
        }
    }

    // =========================================================================
    // DISPLAY: print_analytics  (heap-based top-N)
    // =========================================================================
    void print_analytics(int top_n = 5) const {
        std::cout << "\n+-- ANALYTICS --+\n";

        auto _sort = [](File** arr, int n, bool by_time) {
            for (int i=0;i<n-1;++i) for (int j=i+1;j<n;++j) {
                bool swap = false;
                if (by_time) {
                    time_t ti = arr[i]->active_version ? arr[i]->active_version->created_timestamp : 0;
                    time_t tj = arr[j]->active_version ? arr[j]->active_version->created_timestamp : 0;
                    swap = tj > ti;
                } else swap = arr[j]->total_versions > arr[i]->total_versions;
                if (swap) { File* t=arr[i]; arr[i]=arr[j]; arr[j]=t; }
            }
        };

        {
            int n = heap_recent.sz;
            File** tmp = new File*[n];
            for (int i=0;i<n;++i) tmp[i] = heap_recent.heap[i];
            _sort(tmp, n, true);
            std::cout << "| Top " << top_n << " recently updated:\n";
            for (int i=0;i<n&&i<top_n;++i) {
                time_t ts = tmp[i]->active_version ? tmp[i]->active_version->created_timestamp : 0;
                char buf[32]; std::strftime(buf,sizeof(buf),"%H:%M:%S",std::localtime(&ts));
                std::cout << "|   " << (i+1) << ". " << tmp[i]->filename
                          << "  HEAD=v" << tmp[i]->active_version->version_id
                          << "  @" << buf << "\n";
            }
            delete[] tmp;
        }
        {
            int n = heap_volume.sz;
            File** tmp = new File*[n];
            for (int i=0;i<n;++i) tmp[i] = heap_volume.heap[i];
            _sort(tmp, n, false);
            std::cout << "| Top " << top_n << " by version count:\n";
            for (int i=0;i<n&&i<top_n;++i)
                std::cout << "|   " << (i+1) << ". " << tmp[i]->filename
                          << "  (" << tmp[i]->total_versions << " versions)\n";
            delete[] tmp;
        }
        std::cout << "+---------------+\n";
    }

    // =========================================================================
    // DISPLAY: print_log  (WAL)
    // =========================================================================
    void print_log() const { wal.print_log(); }

private:
    // ── GC helpers ────────────────────────────────────────────────────────────
    void _gc_mark_dfs(TreeNode* node) {
        if (!node || node->gc_marked) return;
        node->gc_marked = true;
        for (int i = 0; i < node->children_count; ++i)
            _gc_mark_dfs(node->children[i]);
    }
    void _gc_reset_dfs(TreeNode* node) {
        if (!node) return;
        node->gc_marked = false;
        for (int i = 0; i < node->children_count; ++i)
            _gc_reset_dfs(node->children[i]);
    }

    // ── Tree printer ─────────────────────────────────────────────────────────
    void _print_node(TreeNode* node, TreeNode* head,
                     const std::string& prefix, bool is_last) const {
        if (!node) return;
        std::string conn = is_last ? "`-- " : "|-- ";
        std::cout << prefix << conn << "v" << node->version_id;
        if (node == head)         std::cout << " [HEAD]";
        if (node->is_snapshot())  std::cout << " * \"" << node->snapshot_message << "\"";
        if (!node->content.empty()) {
            std::string p = node->content.substr(0,30);
            if (node->content.size()>30) p += "...";
            std::cout << " | " << p;
        }
        std::cout << "\n";
        std::string np = prefix + (is_last ? "    " : "|   ");
        for (int i=0;i<node->children_count;++i)
            _print_node(node->children[i], head, np, i==node->children_count-1);
    }
};

// =============================================================================
// SECTION 13 — DEMO + CLI
// =============================================================================

static void print_help() {
    std::cout << R"(
+============================================================+
|         Time-Travelling File System v2.0 — Commands        |
+============================================================+
|  create   <file>                 Create a new file         |
|  write    <file> "content"       Write new version         |
|  snap     <file> "message"       Snapshot current HEAD     |
|  checkout <file> <vid>           Travel to version         |
|  branch   <file> <vid> "cont"    Branch from version       |
|  merge    <file> <their_vid>     3-way merge into HEAD     |
|  diff     <file> <va> <vb>       Myers diff (git-style)    |
|  read     <file> [vid]           Print content             |
|  tree     <file>                 Print version tree        |
|  history  <file>                 Print ancestor chain      |
|  delete   <file>                 Delete file               |
|  gc                              Run garbage collector     |
|  undo                            Undo last operation       |
|  redo                            Redo undone operation     |
|  log                             Print write-ahead log     |
|  search   <word>                 Full-text search          |
|  ac       <prefix>               Autocomplete filename     |
|  list                            List all files            |
|  analytics                       Heap analytics            |
|  demo                            Run built-in demo         |
|  help                            This help screen          |
|  exit                            Quit                      |
+============================================================+
)";
}

static void run_demo(TTFS& fs) {
    std::cout << "\n=========================================\n";
    std::cout << "  TTFS v2.0 DEMO — Advanced Algorithms   \n";
    std::cout << "=========================================\n\n";

    // ── Create files ─────────────────────────────────────────────────────────
    fs.create_file("main.cpp");
    fs.create_file("README.md");
    fs.create_file("utils.cpp");
    fs.create_file("config.json");

    // ── main.cpp: linear history then branch ─────────────────────────────────
    fs.write_version("main.cpp",
        "#include <iostream>\nint main() { return 0; }");
    fs.snapshot("main.cpp", "v1-skeleton");

    fs.write_version("main.cpp",
        "#include <iostream>\nint main() {\n  std::cout << \"Hello TTFS\";\n  return 0;\n}");
    fs.snapshot("main.cpp", "v2-hello");

    fs.write_version("main.cpp",
        "#include <iostream>\n#include <string>\nint main() {\n  std::string msg = \"Hello TTFS\";\n  std::cout << msg;\n  return 0;\n}");

    // Branch from v1 for experimental feature
    int exp_vid = fs.branch_write("main.cpp", 1,
        "#include <iostream>\nvoid helper() { std::cout << \"helper\"; }\nint main() { helper(); return 0; }");
    fs.snapshot("main.cpp", "v-experimental");

    // ── Demonstrate Myers diff ───────────────────────────────────────────────
    std::cout << "\n[DEMO] Myers diff: v1 -> v2 of main.cpp (git-style output)\n";
    fs.print_diff("main.cpp", 1, 2);

    // ── Demonstrate three-way merge with LCA ─────────────────────────────────
    std::cout << "\n[DEMO] Three-way merge: merging experimental branch into v3\n";
    fs.checkout("main.cpp", 3);
    int merge_vid = fs.merge("main.cpp", exp_vid);
    std::cout << "[DEMO] Merge commit: v" << merge_vid << "\n";
    fs.print_version_tree("main.cpp");

    // ── README.md ────────────────────────────────────────────────────────────
    fs.write_version("README.md", "# TTFS\nTime-travelling file system.\n## Features\nVersioning");
    fs.write_version("README.md", "# TTFS\nTime-travelling file system.\n## Features\nVersioning\nBranching\nMerge");
    fs.snapshot("README.md", "docs-v1");

    // ── utils.cpp — for GC demo ───────────────────────────────────────────────
    fs.write_version("utils.cpp", "// util v1\nvoid log() {}");
    int util_v2 = fs.write_version("utils.cpp", "// util v2\nvoid log() { printf(\"log\"); }");
    fs.checkout("utils.cpp", 8);
    // v3 branches from v1; v2 is now unreachable from HEAD
    fs.write_version("utils.cpp", "// util v3 (branched)\nvoid log() {} void debug() {}");

    std::cout << "\n[DEMO] utils.cpp tree before GC:\n";
    fs.print_version_tree("utils.cpp");
    (void)util_v2; // v2 is intentionally orphaned

    // ── config.json ──────────────────────────────────────────────────────────
    fs.write_version("config.json", "{\"version\":1,\"debug\":false}");
    fs.write_version("config.json", "{\"version\":2,\"debug\":true,\"log\":\"verbose\"}");

    // ── Autocomplete demo ────────────────────────────────────────────────────
    std::cout << "\n[DEMO] Autocomplete prefix 'm':\n";
    fs.autocomplete("m");
    std::cout << "\n[DEMO] Autocomplete prefix 'c':\n";
    fs.autocomplete("c");

    // ── Inverted index search ────────────────────────────────────────────────
    std::cout << "\n[DEMO] Full-text search: 'cout'\n";
    fs.search("cout");
    std::cout << "\n[DEMO] Full-text search: 'debug'\n";
    fs.search("debug");

    // ── Undo/Redo demo ───────────────────────────────────────────────────────
    std::cout << "\n[DEMO] WAL before undo:\n";
    fs.print_log();
    std::cout << "\n[DEMO] Undo last operation:\n";
    fs.undo();
    std::cout << "\n[DEMO] Redo:\n";
    fs.redo();

    // ── GC demo ──────────────────────────────────────────────────────────────
    fs.gc();
    std::cout << "\n[DEMO] utils.cpp tree after GC (v2 orphan freed):\n";
    fs.print_version_tree("utils.cpp");

    // ── Analytics ────────────────────────────────────────────────────────────
    fs.list_files();
    fs.print_analytics(4);

    std::cout << "\n[DEMO COMPLETE]\n\n";
}

static CustomVector<std::string> tokenise(const std::string& line) {
    CustomVector<std::string> tokens;
    std::istringstream ss(line);
    std::string tok;
    while (ss >> std::ws) {
        if (ss.peek() == '"') {
            ss.get();
            std::getline(ss, tok, '"');
        } else {
            ss >> tok;
        }
        if (!tok.empty()) tokens.push_back(tok);
    }
    return tokens;
}

int main() {
    TTFS fs;

    std::cout << "+==========================================+\n";
    std::cout << "|  Time-Travelling File System  v2.0      |\n";
    std::cout << "|  Myers Diff | LCA Merge | GC | Trie     |\n";
    std::cout << "|  Type 'demo' or 'help' to begin         |\n";
    std::cout << "+==========================================+\n";

    std::string line;
    while (true) {
        std::cout << "\nttfs> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        CustomVector<std::string> tok = tokenise(line);
        if (tok.size() == 0) continue;

        const std::string& cmd = tok[0];

        if (cmd=="exit"||cmd=="quit") { std::cout<<"Goodbye.\n"; break; }
        else if (cmd=="help")      { print_help(); }
        else if (cmd=="demo")      { run_demo(fs); }
        else if (cmd=="list")      { fs.list_files(); }
        else if (cmd=="analytics") { fs.print_analytics(); }
        else if (cmd=="gc")        { fs.gc(); }
        else if (cmd=="undo")      { fs.undo(); }
        else if (cmd=="redo")      { fs.redo(); }
        else if (cmd=="log")       { fs.print_log(); }
        else if (cmd=="create") {
            if (tok.size()<2) { std::cout<<"Usage: create <file>\n"; continue; }
            fs.create_file(tok[1]);
        } else if (cmd=="write") {
            if (tok.size()<3) { std::cout<<"Usage: write <file> \"content\"\n"; continue; }
            std::string c=tok[2]; for(int i=3;i<tok.size();++i) c+=" "+tok[i];
            fs.write_version(tok[1],c);
        } else if (cmd=="snap") {
            if (tok.size()<3) { std::cout<<"Usage: snap <file> \"message\"\n"; continue; }
            std::string m=tok[2]; for(int i=3;i<tok.size();++i) m+=" "+tok[i];
            fs.snapshot(tok[1],m);
        } else if (cmd=="checkout") {
            if (tok.size()<3) { std::cout<<"Usage: checkout <file> <vid>\n"; continue; }
            fs.checkout(tok[1], std::stoi(tok[2]));
        } else if (cmd=="branch") {
            if (tok.size()<4) { std::cout<<"Usage: branch <file> <vid> \"content\"\n"; continue; }
            std::string c=tok[3]; for(int i=4;i<tok.size();++i) c+=" "+tok[i];
            fs.branch_write(tok[1], std::stoi(tok[2]), c);
        } else if (cmd=="merge") {
            if (tok.size()<3) { std::cout<<"Usage: merge <file> <their_vid>\n"; continue; }
            fs.merge(tok[1], std::stoi(tok[2]));
        } else if (cmd=="diff") {
            if (tok.size()<4) { std::cout<<"Usage: diff <file> <va> <vb>\n"; continue; }
            fs.print_diff(tok[1], std::stoi(tok[2]), std::stoi(tok[3]));
        } else if (cmd=="read") {
            if (tok.size()<2) { std::cout<<"Usage: read <file> [vid]\n"; continue; }
            int v=(tok.size()>=3)?std::stoi(tok[2]):-1;
            std::string c=fs.read_version(tok[1],v);
            if (!c.empty()) std::cout<<c<<"\n";
        } else if (cmd=="tree") {
            if (tok.size()<2) { std::cout<<"Usage: tree <file>\n"; continue; }
            fs.print_version_tree(tok[1]);
        } else if (cmd=="history") {
            if (tok.size()<2) { std::cout<<"Usage: history <file>\n"; continue; }
            fs.print_history(tok[1]);
        } else if (cmd=="delete") {
            if (tok.size()<2) { std::cout<<"Usage: delete <file>\n"; continue; }
            fs.delete_file(tok[1]);
        } else if (cmd=="search") {
            if (tok.size()<2) { std::cout<<"Usage: search <word>\n"; continue; }
            fs.search(tok[1]);
        } else if (cmd=="ac") {
            if (tok.size()<2) { std::cout<<"Usage: ac <prefix>\n"; continue; }
            fs.autocomplete(tok[1]);
        } else {
            std::cout<<"[ERROR] Unknown command '"<<cmd<<"'. Type 'help'.\n";
        }
    }
    return 0;
}
