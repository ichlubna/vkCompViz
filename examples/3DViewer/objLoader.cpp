module objLoader;
import std;

ObjLoader::BoundingBox::BoundingBox(std::size_t dimension) : dimension(dimension)
{
    for(std::size_t i = 0; i < dimension; ++i)
    {
        min.push_back(std::numeric_limits<float>::max());
        max.push_back(std::numeric_limits<float>::min());
    }
}

void ObjLoader::BoundingBox::update(std::vector<float> coords)
{
    for(std::size_t i = 0; i < dimension; ++i)
    {
        min[i] = std::min(min[i], coords[i]);
        max[i] = std::max(max[i], coords[i]);
    }
}

void ObjLoader::normalizeVertices()
{
    for(std::size_t i = 0; i < vertices.size(); i += 3)
    {
        vertices[i] = (vertices[i] - boundingBox.min[0]) / (boundingBox.max[0] - boundingBox.min[0]);
        vertices[i + 1] = (vertices[i + 1] - boundingBox.min[1]) / (boundingBox.max[1] - boundingBox.min[1]);
        vertices[i + 2] = (vertices[i + 2] - boundingBox.min[2]) / (boundingBox.max[2] - boundingBox.min[2]);
    }
}

void ObjLoader::dump() const
{
    std::cout << "Indices:" << std::endl;
    for(auto const& index : indices)
        std::cout << index << " ";
    std::cout << std::endl;
    std::cout << "Vertices starting at: " << indices.size() << std::endl;
    std::cout << "Vertices:" << std::endl;
    for(auto const& vertex : vertices)
        std::cout << vertex << " ";
    std::cout << std::endl;
    std::cout << "Normals starting at: " << indices.size() + vertices.size() << std::endl;
    std::cout << "Normals:" << std::endl;
    for(auto const& normal : normals)
        std::cout << normal << " ";
    std::cout << std::endl;
}

ObjLoader::ObjLoader(std::string path) : boundingBox()
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
            boundingBox.update({x, y, z});
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
                indices.push_back(x - 1);
                indices.push_back(z - 1);
            }
        }
    }
    file.close();
}

std::vector<float> ObjLoader::data() const
{
    std::vector<float> result;
    result.insert(result.end(), indices.begin(), indices.end());
    result.insert(result.end(), vertices.begin(), vertices.end());
    result.insert(result.end(), normals.begin(), normals.end());
    return result;
}
