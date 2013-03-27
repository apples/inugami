#ifndef INUGAMI_GEOMETRY_H
#define INUGAMI_GEOMETRY_H

#include <glm/glm.hpp>

#include <array>
#include <string>
#include <vector>

namespace Inugami {

class Geometry
{
public:
    using Vec3 = ::glm::vec3;
    using Vec2 = ::glm::vec2;

    class Vertex
    {
    public:
        Vertex();
        bool operator==(const Vertex& in) const;
        Vec3 pos;
        Vec3 norm;
        Vec2 tex;
    };

    using Point    = std::array<int,1>;
    using Line     = std::array<int,2>;
    using Triangle = std::array<int,3>;

    static Geometry fromRect(float w, float h);
    static Geometry fromCube(float w, float h, float d);
    static Geometry fromOBJ(const std::string& filename);

    Geometry();
    Geometry(const Geometry& in);
    Geometry(Geometry&& in);
    virtual ~Geometry();

    Geometry& operator=(const Geometry& in);
    Geometry& operator=(Geometry&& in);

    std::vector<Vertex>   vertices;

    std::vector<Point>    points;
    std::vector<Line>     lines;
    std::vector<Triangle> triangles;
};

} // namespace Inugami

#endif // INUGAMI_GEOMETRY_H
