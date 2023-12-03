#include <iostream>
#include "httplib.h"
#include "json.hpp"
#include <unordered_map>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;
using namespace std;

// Definición de la estructura para almacenar datos relacionados con COVID-19
struct CovidData {
    std::string date;  // Fecha del dato
    int positive;      // Número de casos positivos
    int death;         // Número de fallecimientos
};

// Alias para un mapa no ordenado que almacenará datos de COVID-19 por fecha
using CovidDataTable = std::unordered_map<std::string, CovidData>;

// Función para realizar una solicitud a la API y obtener la respuesta
std::string make_api_request(const std::string& path) {
    // Crear un cliente HTTP para conectarse a la API
    httplib::Client client("covidtracking.com");

    // Realizar una solicitud GET al path especificado
    auto res = client.Get(path.c_str());

    // Verificar si la solicitud fue exitosa (código de estado 200)
    if (res && res->status == 200) {
        return res->body;  // Devolver el cuerpo de la respuesta
    } else {
        return "";  // Devolver una cadena vacía si la solicitud no fue exitosa
    }
}

// Función para analizar los datos de COVID-19 desde una cadena JSON utilizando la biblioteca json.hpp
CovidDataTable parse_covid_data(const std::string& json_str) {
    CovidDataTable covid_data_table;

    try {
        // Parsear la cadena JSON
        json json_data = json::parse(json_str);

        // Iterar sobre los elementos del array JSON
        for (const auto& data_entry : json_data) {
            // Obtener valores de las claves y convertirlos a los tipos adecuados
            std::string date_match = data_entry["date"];
            int positive_match = data_entry["positive"];
            int death_match = data_entry["death"];

            // Crear una instancia de la estructura CovidData y asignar valores
            CovidData covid_data;
            covid_data.date = date_match;
            covid_data.positive = positive_match;
            covid_data.death = death_match;

            // Almacenar la instancia en la tabla usando la fecha como clave
            covid_data_table[date_match] = covid_data;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "Error al analizar datos JSON: " << e.what() << std::endl;
    }

    return covid_data_table;
}

// Función principal
int main() {
    std::string path = "/v2/us/daily.json";
    std::string response = make_api_request(path);

    // Verificar si la respuesta no está vacía
    if (!response.empty()) {
        // Intentar analizar los datos de COVID-19 y almacenarlos en una tabla
        CovidDataTable covid_data_table = parse_covid_data(response);

        // Iterar sobre la tabla e imprimir los detalles de cada dato
        for (const auto& entry : covid_data_table) {
            std::cout << "Fecha: " << entry.first << ", Casos Confirmados Totales: " << entry.second.positive
                      << ", Fallecimientos: " << entry.second.death << std::endl;
        }
    } else {
        std::cerr << "Error al obtener datos de la API." << std::endl;  // Imprimir mensaje de error si la respuesta está vacía
    }

    return 0;  // Devolver 0 para indicar que el programa se ejecutó correctamente
}
