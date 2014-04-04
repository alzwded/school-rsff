#include "pathfinder.hxx"
namespace Pathfinder {
    class priv_ {
        friend Path ComputePath();

        struct comp {
            bool operator()(std::pair<Sensor*, Sensor*> const &left, std::pair<Sensor*, Sensor*> const& right) const {
                if(left.first == right.first && left.second == right.second
                || left.first == right.second && left.second == right.first)
                {
                    return false;
                }
                if(left.first == right.first) return left.second < right.second;
                else return left.first < right.first;
            }
        };

        typedef std::map<std::pair<Sensor*, Sensor*>, float, comp> map_t;
        map_t distanceMap_;

        priv_();
        void ComputePathRec(Sensor const&, Path&);
    };
} // namespace

static Sensor const* StartingSensor = NULL;

Path Pathfinder::ComputePath()
{
    using namespace Pathfinder;
    priv_ o;
    Path p;
    o.ComputePathRec(*StartingSensor, p);
    return p;
}

Pathfinder::priv_::priv_()
{
    // compute distanceMap
}

void Pathfinder::priv_::ComputePathRec(Sensor const&, Path&)
{
}
