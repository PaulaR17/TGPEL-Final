#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <memory>
using namespace std;

// Declaración del prototipo de la función
string generarContrasenaDiaria();

// Clase NodoAcceso
class NodoAcceso {
public:
    string nombreUsuario;
    time_t horaAcceso;
    int perfil; // 1: Usuario, 2: Supervisor, 3: Analista
    string contrasena;
    string telefono;
    NodoAcceso* siguiente;

    NodoAcceso(const string& nombre, time_t hora, int p, const string& pass = "", const string& phone = "")
        : nombreUsuario(nombre), horaAcceso(hora), perfil(p), contrasena(pass), telefono(phone), siguiente(nullptr) {}
};

// Clase ListaEnlazadaAccesos
class ListaEnlazadaAccesos {
private:
    NodoAcceso* cabeza;

    // Función recursiva para insertar en orden cronológico
    void insertarRecursivo(NodoAcceso*& actual, NodoAcceso* nuevo) {
        if (!actual || difftime(nuevo->horaAcceso, actual->horaAcceso) < 0) {
            nuevo->siguiente = actual;
            actual = nuevo;
            return;
        }
        insertarRecursivo(actual->siguiente, nuevo);
    }

    // Función recursiva para buscar un nodo por usuario o hora
    NodoAcceso* buscarRecursivo(NodoAcceso* actual, const string& usuario, time_t hora, bool buscarPorUsuario) {
        if (!actual) {
            cout << "Error: No se encontró el registro correspondiente." << endl;
            return nullptr;
        }
        if (buscarPorUsuario && actual->nombreUsuario == usuario) {
            return actual;
        } else if (!buscarPorUsuario && difftime(actual->horaAcceso, hora) == 0) {
            return actual;
        }
        return buscarRecursivo(actual->siguiente, usuario, hora, buscarPorUsuario);
    }

    // Función recursiva para mostrar la lista
    void mostrarRecursivo(NodoAcceso* actual) {
        if (!actual) return;
        cout << "Usuario: " << actual->nombreUsuario
             << ", Hora: " << ctime(&(actual->horaAcceso))
             << ", Perfil: " << actual->perfil << endl;
        mostrarRecursivo(actual->siguiente);
    }

public:
    ListaEnlazadaAccesos() : cabeza(nullptr) {}

    ~ListaEnlazadaAccesos() {
        while (cabeza) {
            NodoAcceso* temp = cabeza;
            cabeza = cabeza->siguiente;
            delete temp;
        }
    }

    // Insertar un nuevo nodo
    void insertar(const string& nombre, time_t hora, int perfil, const string& pass = "", const string& phone = "") {
        NodoAcceso* nuevo = new NodoAcceso(nombre, hora, perfil, pass, phone);
        insertarRecursivo(cabeza, nuevo);
    }

    // Buscar un nodo por nombre
    NodoAcceso* buscarPorNombre(const string& usuario) {
        return buscarRecursivo(cabeza, usuario, 0, true);
    }

    // Buscar un nodo por hora
    NodoAcceso* buscarPorHora(time_t hora) {
        return buscarRecursivo(cabeza, "", hora, false);
    }

    // Validar credenciales de un usuario
    bool validarCredenciales(const string& usuario, const string& contrasena, const string& telefono = "", const string& contrasenaAleatoria = "") {
        NodoAcceso* nodo = buscarPorNombre(usuario);
        if (!nodo) {
            cout << "Error: Usuario no registrado." << endl;
            return false;
        }
        if (nodo->contrasena != contrasena) {
            cout << "Error: Contraseña incorrecta." << endl;
            return false;
        }
        if (nodo->perfil == 3 && nodo->telefono != telefono) {
            cout << "Error: Teléfono incorrecto." << endl;
            return false;
        }
        if (nodo->perfil == 3 && !contrasenaAleatoria.empty() && contrasenaAleatoria != generarContrasenaDiaria()) {
            cout << "Error: Contraseña aleatoria incorrecta." << endl;
            return false;
        }
        cout << "Credenciales válidas para el usuario " << usuario << "." << endl;
        return true;
    }

    // Mostrar todos los nodos
    void mostrar() {
        mostrarRecursivo(cabeza);
    }
};

// Clase para gestionar la pila de seguridad
class PilaSeguridad {
private:
    string niveles[3] = {"Bajo", "Medio", "Alto"};

public:
    void ajustarNivel(int perfil) {
        if (perfil == 1) {
            niveles[0] = "Bajo";
            niveles[1] = "Medio";
            niveles[2] = "Alto";
        } else if (perfil == 2) {
            niveles[0] = "Medio";
            niveles[1] = "Bajo";
            niveles[2] = "Alto";
        } else if (perfil == 3) {
            niveles[0] = "Alto";
            niveles[1] = "Medio";
            niveles[2] = "Bajo";
        }
    }

    void mostrarPila() {
        cout << "Niveles de seguridad en la pila:" << endl;
        for (const string& nivel : niveles) {
            cout << nivel << endl;
        }
    }
};

// Función para generar una contraseña diaria basada en la fecha
string generarContrasenaDiaria() {
    time_t ahora = time(0);
    tm* tiempoLocal = localtime(&ahora);

    // Usar día, mes y año como semilla
    int semilla = tiempoLocal->tm_year * 10000 + (tiempoLocal->tm_mon + 1) * 100 + tiempoLocal->tm_mday;
    srand(semilla);

    const string caracteres = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    string contrasena;
    for (int i = 0; i < 8; ++i) {
        contrasena += caracteres[rand() % caracteres.size()];
    }
    return contrasena;
}

// Pruebas del sistema de login y control de seguridad
void pruebas() {
    unique_ptr<ListaEnlazadaAccesos> accesos = make_unique<ListaEnlazadaAccesos>();
    unique_ptr<PilaSeguridad> pila = make_unique<PilaSeguridad>();

    // Crear registros
    time_t ahora = time(0);
    time_t haceUnaHora = ahora - 3600;
    time_t haceDosHoras = ahora - 7200;

    accesos->insertar("juan", ahora, 1, "password1");
    accesos->insertar("ana", haceUnaHora, 2, "password2");
    accesos->insertar("carlos", haceDosHoras, 3, "password3", "987654321");

    // Mostrar registros
    cout << "Lista de accesos registrados:" << endl;
    accesos->mostrar();

    // Buscar por nombre
    auto nodoNombre = accesos->buscarPorNombre("ana");
    if (nodoNombre) {
        cout << "\nRegistro encontrado por nombre: " << nodoNombre->nombreUsuario << ", " << ctime(&(nodoNombre->horaAcceso)) << endl;
    }

    // Buscar por hora
    auto nodoHora = accesos->buscarPorHora(haceDosHoras);
    if (nodoHora) {
        cout << "\nRegistro encontrado por hora: " << nodoHora->nombreUsuario << ", " << ctime(&(nodoHora->horaAcceso)) << endl;
    }

    // Ajustar pila de seguridad
    cout << "\nAjustando nivel de seguridad para perfil 2:" << endl;
    pila->ajustarNivel(2);
    pila->mostrarPila();

    // Validación de credenciales
    cout << "\nValidación de credenciales:" << endl;
    string contrasenaDiaria = generarContrasenaDiaria();
    cout << "Contraseña diaria generada: " << contrasenaDiaria << endl;

    accesos->validarCredenciales("juan", "password1");
    accesos->validarCredenciales("ana", "password2");
    accesos->validarCredenciales("carlos", "password3", "987654321", contrasenaDiaria);
}

int main() {
    pruebas();
    return 0;
}
