#include <iostream>
#include "include/httplib.h"
#include <unordered_map>
#include <sstream>
#include <iomanip>

using namespace std;

struct CovidData {
    std::string date;
    int positive;
    int death;
};

using CovidDataTable = std::unordered_map<std::string, CovidData>;

std::string make_api_request(const std::string& path) {
    httplib::Client client("covidtracking.com");
    auto res = client.Get(path.c_str());

    if (res && res->status == 200) {
        return res->body;
    } else {
        return "";
    }
}

CovidDataTable parse_covid_data(const std::string& json_str) {
    CovidDataTable covid_data_table;

    // Buscar las posiciones de las claves de interés
    size_t date_pos = json_str.find("\"date\":");
    size_t positive_pos = json_str.find("\"positive\":");
    size_t death_pos = json_str.find("\"death\":");

    while (date_pos != std::string::npos) {
        // Extraer fechas, casos positivos y muertes
        size_t date_end = json_str.find(",", date_pos);
        size_t positive_end = json_str.find(",", positive_pos);
        size_t death_end = json_str.find("}", death_pos);

        std::string date_match = json_str.substr(date_pos + 8, date_end - date_pos - 9);  

        int positive_match = std::stoi(json_str.substr(positive_pos + 12, positive_end - positive_pos - 12));
        int death_match = std::stoi(json_str.substr(death_pos + 8, death_end - death_pos - 8));

        CovidData covid_data;
        covid_data.date = date_match;
        covid_data.positive = positive_match;
        covid_data.death = death_match;

        covid_data_table[date_match] = covid_data;

        // Buscar la siguiente instancia de las claves
        date_pos = json_str.find("\"date\":", date_end);
        positive_pos = json_str.find("\"positive\":", positive_end);
        death_pos = json_str.find("\"death\":", death_end);
    }

    return covid_data_table;
}

int main() {
    std::string path = "https://covidtracking.com/data";
    std::string response = make_api_request(path);

    if (!response.empty()) {
        CovidDataTable covid_data_table = parse_covid_data(response);

        for (const auto& entry : covid_data_table) {
            std::cout << "Fecha: " << entry.first << ", Casos confirmados: " << entry.second.positive
                      << ", Fallecimientos: " << entry.second.death << std::endl;
        }
    } else {
        std::cerr << "Error al obtener datos de la API." << std::endl;
    }

    return 0;
}

//https://covidtracking.com/data