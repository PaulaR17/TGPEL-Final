#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <memory>
using namespace std;

string toLowerCase(const string& str) {
    string lowerStr = str;
    for (char& c : lowerStr) {
        c = tolower(c);
    }
    return lowerStr;
}
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
        if (buscarPorUsuario && toLowerCase(actual->nombreUsuario) == toLowerCase(usuario)) {
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



class NodoCola {
public:
    string usuario;  // Usuario al que pertenece la actividad
    string actividad; // Descripción de la actividad
    time_t hora;      // Hora en la que se asigna la actividad
    NodoCola* siguiente; // Puntero al siguiente nodo

    NodoCola(const string& user, const string& act, time_t t)
        : usuario(user), actividad(act), hora(t), siguiente(nullptr) {}
};

class ColaActividades {
private:
    NodoCola* frente;
    NodoCola* final;

    // Función recursiva para mostrar actividades
    void mostrarRecursivo(NodoCola* actual) {
        if (!actual) return;
        cout << "Actividad: " << actual->actividad
             << ", Asignada a: " << actual->usuario
             << ", Hora: " << ctime(&(actual->hora));
        mostrarRecursivo(actual->siguiente);
    }

public:
    ColaActividades() : frente(nullptr), final(nullptr) {}

    ~ColaActividades() {
        while (frente) {
            NodoCola* temp = frente;
            frente = frente->siguiente;
            delete temp;
        }
    }

    // Agregar actividad
    void enqueue(const string& usuario, const string& actividad) {
        time_t ahora = time(0);
        NodoCola* nuevo = new NodoCola(usuario, actividad, ahora);

        if (!final) {
            frente = final = nuevo;
        } else {
            final->siguiente = nuevo;
            final = nuevo;
        }
    }

    // Eliminar la actividad más antigua
    void dequeue() {
        if (!frente) {
            cout << "No hay actividades para eliminar." << endl;
            return;
        }
        NodoCola* temp = frente;
        frente = frente->siguiente;
        if (!frente) final = nullptr;
        delete temp;
    }

    // Mostrar todas las actividades
    void mostrar() {
        if (!frente) {
            cout << "No hay actividades en la cola." << endl;
            return;
        }
        mostrarRecursivo(frente);
    }
};

void asignarActividad(ColaActividades& cola, const string& usuario, const string& actividad, int perfilUsuario) {
    if (perfilUsuario != 1 && perfilUsuario != 2) { // Permitir nivel 1 y nivel 2
        cout << "Error: No tienes permiso para asignar actividades a este perfil." << endl;
        return;
    }

    cola.enqueue(usuario, actividad);
    cout << "Actividad asignada a " << usuario << ": " << actividad << endl;
}


void revisarActividades(ColaActividades& cola, const string& usuario, int perfilUsuario) {
    if (perfilUsuario != 1) { // Validar permisos
        cout << "Error: No tienes permiso para revisar actividades de este perfil." << endl;
        return;
    }

    cout << "Actividades de " << usuario << ":" << endl;
    cola.mostrar();
}

void gestionarActividadesPropias(ColaActividades& cola) {
    cout << "Tus actividades asignadas:" << endl;
    cola.mostrar();
}

void menuSupervisor(ListaEnlazadaAccesos& accesos, ColaActividades& colaGeneral, ColaActividades& colaSupervisor, const string& supervisor) {
    int opcion;

    do {
        cout << "\n=== Menu del Supervisor ===" << endl;
        cout << "1. Asignar actividad a un usuario general" << endl;
        cout << "2. Revisar actividades de un usuario general" << endl;
        cout << "3. Revisar tus propias actividades" << endl;
        cout << "4. Salir" << endl;
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1: {
            string usuario, actividad;
            cout << "Introduce el nombre del usuario: ";
            cin.ignore(); // Limpiar el buffer
            getline(cin, usuario);
            cout << "Introduce la actividad a asignar: ";
            getline(cin, actividad);

            // Buscar el usuario en la lista de accesos
            NodoAcceso* nodoUsuario = accesos.buscarPorNombre(usuario);
            if (!nodoUsuario) {
                cout << "Error: Usuario no encontrado." << endl;
            } else if (nodoUsuario->perfil != 1) {
                cout << "Error: Solo puedes asignar actividades a usuarios generales." << endl;
            } else {
                asignarActividad(colaGeneral, usuario, actividad, nodoUsuario->perfil);
            }
            break;
        }
        case 2: {
            string usuario;
            cout << "Introduce el nombre del usuario a revisar: ";
            cin.ignore(); // Limpiar el buffer
            getline(cin, usuario);

            // Buscar el usuario en la lista de accesos
            NodoAcceso* nodoUsuario = accesos.buscarPorNombre(usuario);
            if (!nodoUsuario) {
                cout << "Error: Usuario no encontrado." << endl;
            } else if (nodoUsuario->perfil != 1) {
                cout << "Error: Solo puedes revisar actividades de usuarios generales." << endl;
            } else {
                revisarActividades(colaGeneral, usuario, nodoUsuario->perfil);
            }
            break;
        }
        case 3: {
            cout << "Tus actividades asignadas:" << endl;
            gestionarActividadesPropias(colaSupervisor);
            break;
        }
        case 4:
            cout << "Saliendo del sistema..." << endl;
            break;
        default:
            cout << "Opción no válida. Inténtalo de nuevo." << endl;
        }
    } while (opcion != 4);
}


void pruebasSupervisor() {
    ListaEnlazadaAccesos accesos;
    ColaActividades colaGeneral;
    ColaActividades colaSupervisor;

    // Insertar usuarios en la lista de accesos
    accesos.insertar("juan", time(0), 1, "password1");  // Usuario general
    accesos.insertar("ana", time(0), 2, "password2");   // Supervisor
    accesos.insertar("carlos", time(0), 3, "password3", "987654321"); // Analista

    // Obtener el nodo del Supervisor
    NodoAcceso* supervisor = accesos.buscarPorNombre("ana");
    if (!supervisor) {
        cout << "Error: Supervisor no encontrado. No se puede iniciar el sistema." << endl;
    }

    // Mostrar menú interactivo del Supervisor
    menuSupervisor(accesos, colaGeneral, colaSupervisor, supervisor->nombreUsuario);
}


int main() {
    pruebas();
    pruebasSupervisor();
    return 0;
}
