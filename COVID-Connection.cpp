#include <iostream>
#include "include/httplib.h"
#include <unordered_map>
#include <sstream>
#include <iomanip>

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

// Función para analizar los datos de COVID-19 desde una cadena JSON
CovidDataTable parse_covid_data(const std::string& json_str) {
    CovidDataTable covid_data_table;  // Crear una tabla para almacenar datos de COVID-19

    // Buscar las posiciones de las claves de interés en la cadena JSON
    size_t date_pos = json_str.find("\"date\":");
    size_t positive_pos = json_str.find("\"positive\":");
    size_t death_pos = json_str.find("\"death\":");

    // Iterar mientras se encuentren instancias de la clave "date" en la cadena JSON
    while (date_pos != std::string::npos) {
        // Extraer fechas, casos positivos y muertes
        size_t date_end = json_str.find(",", date_pos);
        size_t positive_end = json_str.find(",", positive_pos);
        size_t death_end = json_str.find("}", death_pos);

        // Obtener valores de las subcadenas y convertirlos a los tipos adecuados
        std::string date_match = json_str.substr(date_pos + 8, date_end - date_pos - 9);
        int positive_match = std::stoi(json_str.substr(positive_pos + 12, positive_end - positive_pos - 12));
        int death_match = std::stoi(json_str.substr(death_pos + 8, death_end - death_pos - 8));

        // Crear una instancia de la estructura CovidData y asignar valores
        CovidData covid_data;
        covid_data.date = date_match;
        covid_data.positive = positive_match;
        covid_data.death = death_match;

        // Almacenar la instancia en la tabla usando la fecha como clave
        covid_data_table[date_match] = covid_data;

        // Buscar la siguiente instancia de las claves
        date_pos = json_str.find("\"date\":", date_end);
        positive_pos = json_str.find("\"positive\":", positive_end);
        death_pos = json_str.find("\"death\":", death_end);
    }

    return covid_data_table;  // Devolver la tabla de datos de COVID-19
}

// Función principal
int main() {
    std::string path = "https://covidtracking.com/data";
    std::string response = make_api_request(path);

    // Verificar si la respuesta no está vacía
    if (!response.empty()) {
        // Intentar analizar los datos de COVID-19 y almacenarlos en una tabla
        CovidDataTable covid_data_table = parse_covid_data(response);

        // Iterar sobre la tabla e imprimir los detalles de cada dato
        for (const auto& entry : covid_data_table) {
            std::cout << "Fecha: " << entry.first << ", Casos confirmados: " << entry.second.positive
                      << ", Fallecimientos: " << entry.second.death << std::endl;
        }
    } else {
        std::cerr << "Error al obtener datos de la API." << std::endl;  // Imprimir mensaje de error si la respuesta está vacía
    }

    return 0;  // Devolver 0 para indicar que el programa se ejecutó correctamente
}
