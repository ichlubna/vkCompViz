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
};
