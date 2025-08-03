module objLoader;
import std;

ObjLoader::ObjLoader(std::string path)
{
    std::ifstream file(path);
    if(!file.is_open())
        throw std::invalid_argument("Unable to open file: " + path);

    std::string line;
    while(std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        float x, y, z;
        if(prefix == "v")
        {
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
        else if(prefix == "vn")
        {
            iss >> x >> y >> z;
            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
        }
        else if(prefix == "f")
        {
            std::string point;
            while(iss >> point)
            {
                point = std::regex_replace(point, std::regex("//"), "/0/");
                std::replace(point.begin(), point.end(), '/', ' ');
                std::istringstream pointStream(point);
                pointStream >> x >> y >> z;
                indices.push_back(x);
                indices.push_back(z);
            }
        }
    }
    file.close();
}

std::vector<float> ObjLoader::data() const
{
    std::vector<float> result;
    result.insert(result.end(), vertices.begin(), vertices.end());
    result.insert(result.end(), normals.begin(), normals.end());
    result.insert(result.end(), indices.begin(), indices.end());
    return result;
}
