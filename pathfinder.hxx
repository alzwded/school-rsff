#ifndef PATHFINDER_HXX
#define PATHFINDER_HXX

#include "core.hxx"
#include <vector>
#include <utility>
#include <map>

class Path {
public:
    typedef std::vector<Edge> Edges_t;
private:
    std::vector<Edges_t> path_;
public:
    Path& operator++() {
        path_.push_back(Edges_t());
        return *this;
    }
    Edges_t& operator[](size_t frame) {
        return path_[frame];
    }
    size_t size() const {
        return path_.size();
    }
    void clear() {
        path_.clear();
    }
};

namespace Pathfinder {
    void SetStartingSensor(Sensor const& s);
    Path ComputePath(Sensor::vector const& sensors);
} // namespace


#endif
