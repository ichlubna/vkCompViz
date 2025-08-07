export module objLoader;
import std;

export class ObjLoader
{
    public:
        ObjLoader(std::string path);
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> indices;
        std::vector<float> data() const;
        void normalizeVertices();
        void dump() const;
        class BoundingBox
        {
            public:
                BoundingBox(std::size_t dimension = 3);
                void update(std::vector<float> coords);
                std::size_t dimension;
                std::vector<float> min;
                std::vector<float> max;
        } boundingBox;
};
