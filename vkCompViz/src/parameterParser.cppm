export module parameterParser;
import std;

export class ParameterParser
{
    private:
        std::string lastName{};
        std::map<std::string, float> parametersMap;
    public:
        void read();
        [[nodiscard]] float get(std::string name, float defaultValue = 0.0f) const;
        [[nodiscard]] const std::map<std::string, float> &get() const
        {
            return parametersMap;
        }
};
