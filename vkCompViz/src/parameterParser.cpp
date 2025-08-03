module parameterParser;
import std;

[[nodiscard]] std::vector<std::string> split(std::string input, char delimiter)
{
    std::vector<std::string> result;
    std::size_t position = input.find(delimiter);
    if(position == std::string::npos)
        result.push_back(input);
    while(position != std::string::npos)
    {
        result.push_back(input.substr(0, position));
        input.erase(0, position + 1);
        position = input.find(delimiter);
        if(position == std::string::npos)
            result.push_back(input);
    }
    return result;
}

float ParameterParser::get(std::string name, float defaultValue) const
{
    if(parametersMap.find(name) == parametersMap.end())
        return defaultValue;
    return parametersMap.at(name);
}

void ParameterParser::read()
{
    std::cout << "Enter parameters as a=1.0 b=2.0..." << std::endl;
    if(!lastName.empty())
        std::cout << "Last parameter was " << lastName << " you can enter only value to update it" << std::endl;
    parametersMap.clear();
    std::string input;
    std::getline(std::cin, input);

    if(!lastName.empty())
    {
        bool singleValue = true;
        try
        {
            std::stof(input);
        }
        catch(...)
        {
            singleValue = false;
        }
        if(singleValue)
        {
            parametersMap[lastName] = std::stof(input);
            return;
        }
    }

    auto parameters = split(input, ' ');
    for(auto parameter : parameters)
    {
        auto nameValue = split(parameter, '=');
        if(nameValue.size() != 2)
        {
            std::cout << "Invalid parameter format, use a=1.0 b=2.0..." << std::endl;
            parametersMap.clear();
            return;
        }
        parametersMap[nameValue[0]] = std::stof(nameValue[1]);
        if(parameters.size() == 1)
            lastName = nameValue[0];
    }
    if(parameters.size() != 1)
        lastName.clear();
}
