#include "pathfinder.hxx"
#include <cmath>
#include <utility>
#include <set>

static Sensor const* StartingSensor = NULL;
void Pathfinder::SetStartingSensor(Sensor const& s)
{
    StartingSensor = &s;
}

namespace Pathfinder {
    typedef std::set<Sensor const*> sensors_t;

    class priv_ {
        friend Path ComputePath(Sensor::vector const& sensors);

        // store each unique pair only once
        struct comp {
            bool operator()(std::pair<Sensor const*, Sensor const*> const &left, std::pair<Sensor const*, Sensor const*> const& right) const {
                if(left.first == right.first && left.second == right.second
                || left.first == right.second && left.second == right.first)
                {
                    return false;
                }
                if(left.first == right.first) return left.second < right.second;
                else return left.first < right.first;
            }
        };

        typedef std::map<std::pair<Sensor const*, Sensor const*>, float, comp> map_t;
        map_t distanceMap_;
        std::set<Sensor const*> spent_;

        priv_(Sensor::vector const& sensors);
        void ComputePathRec(sensors_t&, size_t, Path&);
    };
} // namespace

Path Pathfinder::ComputePath(Sensor::vector const& sensors)
{
    using namespace Pathfinder;
    priv_ o(sensors);
    Path p;
    ++p;
    Pathfinder::sensors_t ss;
    ss.insert(StartingSensor);
    o.ComputePathRec(ss, 0, p);
    return p;
}

Pathfinder::priv_::priv_(Sensor::vector const& sensors)
{
    // compute distanceMap
    for(Sensor::vector::const_iterator i = sensors.begin();
            i != sensors.end(); ++i)
    {
        for(Sensor::vector::const_iterator j = i + 1;
                j != sensors.end(); ++j)
        {
            if(i == j) continue;

            Point3D const& p1 = (*i).location;
            Point3D const& p2 = (*j).location;
            float t1 = p1.x - p2.x;
            float t2 = p1.y - p2.y;
            float t3 = p1.z - p2.z;
            float d = sqrtf(t1*t1 + t2*t2 + t3*t3);

            distanceMap_[ std::make_pair(&*i, &*j) ] = d;
        }
    }
}

#ifdef TRACE
# include <cstdio>
#endif
void Pathfinder::priv_::ComputePathRec(Pathfinder::sensors_t& ss, size_t level, Path& path)
{
#ifdef TRACE
    printf("level: %d\n", level);
#endif
    // don't go through these nodes again
    for(Pathfinder::sensors_t::iterator k = ss.begin();
            k != ss.end(); ++k)
    {
        spent_.insert(*k);
    }

    // list of nodes to be processed in the next level
    Pathfinder::sensors_t deferred;
    // process each node on the current level and build a list of
    //     the nodes relayed the message
    for(Pathfinder::sensors_t::iterator k = ss.begin();
            k != ss.end(); ++k) {
        Sensor const& s = **k;
        // build a map of distances from the current node
        //     to the still available nodes
        std::map<Sensor const*, float> localDistanceMap;
        for(map_t::iterator i = distanceMap_.begin();
                s.type != Sensor::CENTRAL && i != distanceMap_.end(); ++i)
        {
            if(i->first.first == &s) {
                if(spent_.find(i->first.second) == spent_.end()
                        && i->first.second->type != Sensor::SENSOR) {
                    localDistanceMap.insert(std::make_pair(i->first.second, i->second));
                }
            } else if(i->first.second == &s) {
                if(spent_.find(i->first.first) == spent_.end()
                        && i->first.first->type != Sensor::SENSOR) {
                    localDistanceMap.insert(std::make_pair(i->first.first, i->second));
                }
            }
        }

        // relay the message to each node that is within range
        for(std::map<Sensor const*, float>::iterator i = localDistanceMap.begin();
                i != localDistanceMap.end(); ++i)
        {
#ifdef TRACE
            printf("dist: %f ; range: %f\n", i->second, s.range);
#endif
            if(i->second <= s.range) {
                path[level].push_back(Edge(s, *i->first));
                deferred.insert(i->first);
            }
        }
    }

    // if there are nodes to be processed in the next level, do it
    if(deferred.size() > 0) {
        if(path.size() <= level + 1) {
            ++path;
        }
        ComputePathRec(deferred, level + 1, path);
    }
}
