#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <memory>
#include <map>
#include <fstream>
using namespace std;

// convierte una cadena a minúsculas
string toLowerCase(const string& str) {
    string lowerStr = str; // copia de la cadena original
    for (char& c : lowerStr) {
        c = tolower(c); // convierte cada carácter a minúscula
    }
    return lowerStr; // devuelve la cadena en minúsculas
}

// prototipo para generar contraseñas diarias
string generarContrasenaDiaria();

// -----GESTION DE ACCESOS-----
// clase NodoAcceso representa un nodo en la lista de accesos
class NodoAcceso {
public:
    string nombreUsuario; // nombre del usuario
    time_t horaAcceso; // hora de acceso
    int perfil; // perfil del usuario (1: usuario, 2: supervisor, 3: analista)
    string contrasena; // contraseña del usuario
    string telefono; // teléfono del usuario
    NodoAcceso* siguiente; // puntero al siguiente nodo

    // constructor para inicializar los atributos del nodo
    NodoAcceso(const string& nombre, time_t hora, int p, const string& pass = "", const string& phone = "")
        : nombreUsuario(nombre), horaAcceso(hora), perfil(p), contrasena(pass), telefono(phone), siguiente(nullptr) {}
};

// clase ListaEnlazadaAccesos gestiona una lista enlazada de accesos
class ListaEnlazadaAccesos {
private:
    NodoAcceso* cabeza; // puntero al primer nodo de la lista

    // inserta un nodo en orden cronológico usando recursión
    void insertarRecursivo(NodoAcceso*& actual, NodoAcceso* nuevo) {
        if (!actual || difftime(nuevo->horaAcceso, actual->horaAcceso) < 0) { // si la lista está vacía o el nuevo nodo es más antiguo
            nuevo->siguiente = actual; // enlaza el nuevo nodo al inicio
            actual = nuevo; // actualiza la cabeza de la lista
            return;
        }
        insertarRecursivo(actual->siguiente, nuevo); // continúa con el siguiente nodo
    }

    // busca un nodo por nombre o por hora usando recursión
    NodoAcceso* buscarRecursivo(NodoAcceso* actual, const string& usuario, time_t hora, bool buscarPorUsuario) {
        if (!actual) { // si no hay más nodos
            cout << "Error: No se encontro el registro correspondiente." << endl; // mensaje de error
            return nullptr; // devuelve un puntero nulo
        }
        if (buscarPorUsuario && toLowerCase(actual->nombreUsuario) == toLowerCase(usuario)) { // compara nombres en minúsculas
            return actual; // devuelve el nodo encontrado
        } else if (!buscarPorUsuario && difftime(actual->horaAcceso, hora) == 0) { // compara por hora
            return actual; // devuelve el nodo encontrado
        }
        return buscarRecursivo(actual->siguiente, usuario, hora, buscarPorUsuario); // recursión para buscar en el siguiente nodo
    }

    // muestra los nodos de la lista usando recursión
    void mostrarRecursivo(NodoAcceso* actual) {
        if (!actual) return; // si no hay más nodos, detiene la recursión
        cout << "Usuario: " << actual->nombreUsuario
             << ", Hora: " << ctime(&(actual->horaAcceso))
             << ", Perfil: " << actual->perfil << endl; // muestra los detalles del nodo
        mostrarRecursivo(actual->siguiente); // muestra el siguiente nodo
    }

public:
    ListaEnlazadaAccesos() : cabeza(nullptr) {} // inicializa una lista vacía

    ~ListaEnlazadaAccesos() {
        while (cabeza) { // mientras haya nodos en la lista
            NodoAcceso* temp = cabeza; // almacena el nodo actual
            cabeza = cabeza->siguiente; // pasa al siguiente nodo
            delete temp; // elimina el nodo actual
        }
    }
    // devuelve el puntero a la cabeza de la lista
    NodoAcceso* getCabeza() const {
        return cabeza;
    }

    // inserta un nodo en la lista
    void insertar(const string& nombre, time_t hora, int perfil, const string& pass = "", const string& phone = "") {
        NodoAcceso* nuevo = new NodoAcceso(nombre, hora, perfil, pass, phone); // crea un nuevo nodo
        insertarRecursivo(cabeza, nuevo); // llama a la función recursiva para insertar el nodo
    }

    // busca un nodo por nombre
    NodoAcceso* buscarPorNombre(const string& usuario) {
        return buscarRecursivo(cabeza, usuario, 0, true); // llama a la función recursiva para buscar por nombre
    }

    // busca un nodo por hora
    NodoAcceso* buscarPorHora(time_t hora) {
        return buscarRecursivo(cabeza, "", hora, false); // llama a la función recursiva para buscar por hora
    }


    // valida las credenciales de un usuario
    bool validarCredenciales(const string& usuario, const string& contrasena = "", const string& telefono = "", const string& contrasenaAleatoria = "") {
        NodoAcceso* nodo = buscarPorNombre(usuario); // busca al usuario por nombre
        if (!nodo) { // si no encuentra el nodo
            cout << "Error: Usuario no registrado." << endl; // mensaje de error
            return false;
        }
        if (nodo->contrasena != contrasena) { // verifica la contraseña
            cout << "Error: Contrasenia incorrecta." << endl; // mensaje de error
            return false;
        }
        if (nodo->perfil == 3 && nodo->telefono != telefono) { // verifica el teléfono para el perfil 3
            cout << "Error: Telefono incorrecto." << endl; // mensaje de error
            return false;
        }
        if (nodo->perfil == 3 && !contrasenaAleatoria.empty() && contrasenaAleatoria != generarContrasenaDiaria()) { // verifica la contraseña aleatoria para el perfil 3
            cout << "Error: Contrasenia aleatoria incorrecta." << endl; // mensaje de error
            return false;
        }
        cout << "Credenciales validas para el usuario " << usuario << "." << endl; // mensaje de éxito
        return true; // devuelve verdadero si las credenciales son válidas
    }

    // muestra todos los nodos de la lista
    void mostrar() {
        mostrarRecursivo(cabeza); // llama a la función recursiva para mostrar los nodos
    }
};

// -----GESTION DE SEGURIDAD-----
// clase para gestionar la pila de seguridad
class PilaSeguridad {
private:
    string niveles[3] = {"Bajo", "Medio", "Alto"}; // niveles de seguridad

public:
    // ajusta el nivel de seguridad según el perfil
    void ajustarNivel(int perfil) {
        if (perfil == 1) { // perfil de usuario general
            niveles[0] = "Bajo";
            niveles[1] = "Medio";
            niveles[2] = "Alto";
        } else if (perfil == 2) { // perfil de supervisor
            niveles[0] = "Medio";
            niveles[1] = "Bajo";
            niveles[2] = "Alto";
        } else if (perfil == 3) { // perfil de analista
            niveles[0] = "Alto";
            niveles[1] = "Medio";
            niveles[2] = "Bajo";
        }
    }

    // muestra los niveles de seguridad en la pila
    void mostrarPila() {
        cout << "Niveles de seguridad en la pila:" << endl; // encabezado
        for (const string& nivel : niveles) { // recorre los niveles
            cout << nivel << endl; // muestra cada nivel
        }
    }
};

// genera una contraseña diaria basada en la fecha
string generarContrasenaDiaria() {
    time_t ahora = time(0); // obtiene la hora actual
    tm* tiempoLocal = localtime(&ahora); // convierte la hora a tiempo local

    // usa el dia, mes y año como semilla para generar una contraseña
    int semilla = tiempoLocal->tm_year * 10000 + (tiempoLocal->tm_mon + 1) * 100 + tiempoLocal->tm_mday;
    srand(semilla); // establece la semilla

    const string caracteres = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"; // caracteres posibles para la contraseña
    string contrasena;
    for (int i = 0; i < 8; ++i) {
        contrasena += caracteres[rand() % caracteres.size()]; // selecciona caracteres aleatorios
    }
    return contrasena; // devuelve la contraseña generada
}

// -------ACTIVIDADES-------
// nodo para actividades
class NodoCola {
public:
    string usuario; // usuario al que pertenece la actividad
    string actividad; // descripcion de la actividad
    time_t hora; // hora en que se asigna la actividad
    NodoCola* siguiente; // puntero al siguiente nodo

    NodoCola(const string& user, const string& act, time_t t)
        : usuario(user), actividad(act), hora(t), siguiente(nullptr) {}
};

// cola para gestionar actividades
class ColaActividades {
private:
    NodoCola* frente; // primer nodo de la cola
    NodoCola* final; // ultimo nodo de la cola

    // muestra las actividades de forma recursiva
    void mostrarRecursivo(NodoCola* actual) {
        if (!actual) return; // si no hay mas nodos, termina
        cout << "Actividad: " << actual->actividad
             << ", Asignada a: " << actual->usuario
             << ", Hora: " << ctime(&(actual->hora)); // muestra los detalles de la actividad
        mostrarRecursivo(actual->siguiente); // pasa al siguiente nodo
    }

public:
    ColaActividades() : frente(nullptr), final(nullptr) {} // inicializa una cola vacia

    ~ColaActividades() {
        while (frente) { // mientras haya nodos en la cola
            NodoCola* temp = frente; // almacena el nodo actual
            frente = frente->siguiente; // pasa al siguiente nodo
            delete temp; // elimina el nodo actual
        }
    }

    // obtiene el primer nodo de la cola
    NodoCola* getFrente() const {
        return frente;
    }


    // agrega una actividad a la cola
    void enqueue(const string& usuario, const string& actividad) {
        time_t ahora = time(0);
        NodoCola* nuevo = new NodoCola(toLowerCase(usuario), actividad, ahora); // Normalizamos el nombre
        if (!final) {
            frente = final = nuevo;
        } else {
            final->siguiente = nuevo;
            final = nuevo;
        }
    }

    // elimina la actividad mas antigua de la cola
    void dequeue() {
        if (!frente) { // si la cola esta vacia
            cout << "No hay actividades para eliminar." << endl; // mensaje de error
            return;
        }
        NodoCola* temp = frente; // almacena el nodo actual
        frente = frente->siguiente; // pasa al siguiente nodo
        if (!frente) final = nullptr; // si la cola queda vacia, actualiza el puntero final
        delete temp; // elimina el nodo actual
    }

    // muestra las actividades asignadas a un usuario
    void mostrar(const string& usuario) {
        if (!frente) {
            cout << "No hay actividades en la cola general." << endl;
            return;
        }

        NodoCola* actual = frente;
        bool hayActividades = false;

        cout << "Recorriendo la cola para verificar actividades asignadas..." << endl;
        while (actual) {
            cout << "Verificando actividad: Usuario: " << actual->usuario
                 << ", Actividad: " << actual->actividad << endl;

            if (toLowerCase(actual->usuario) == toLowerCase(usuario)) {
                cout << "- Actividad: " << actual->actividad
                     << ", Hora: " << ctime(&(actual->hora)) << endl;
                hayActividades = true;
            }
            actual = actual->siguiente;
        }

        if (!hayActividades) {
            cout << "No hay actividades asignadas para " << usuario << "." << endl;
        }
    }

    // verifica si un usuario tiene actividades asignadas
    bool tieneActividades(const string& usuario) {
        NodoCola* actual = frente;
        while (actual) {
            if (actual->usuario == usuario) {
                return true; // encontro una actividad para el usuario
            }
            actual = actual->siguiente;
        }
        return false; // no encontro actividades para el usuario
    }
};

// Declaración global de colaGeneral
ColaActividades colaGeneral;

// funcion para asignar actividad automaticamente
void asignarActividadAutomaticamente(ListaEnlazadaAccesos& accesos, ColaActividades& colaGeneral, const string& usuario) {
    NodoAcceso* nodo = accesos.buscarPorNombre(usuario); // busca el nodo del usuario por nombre
    if (!nodo) { // verifica si el nodo no existe
        cout << "Error: Usuario no encontrado para asignar actividad." << endl; // muestra error si no existe
        return; // termina la funcion
    }

    // lista de actividades predeterminadas
    string actividades[] = {
        "Actualizar datos personales.", // actividad 1
        "Configurar autenticacion en dos pasos.", // actividad 2
        "Revisar historial de accesos.", // actividad 3
        "Aceptar terminos y condiciones." // actividad 4
    };

    if (colaGeneral.tieneActividades(usuario)) { // verifica si el usuario ya tiene actividades asignadas
        cout << "El usuario " << usuario << " ya tiene actividades pendientes." << endl; // informa al usuario
        return; // termina la funcion
    }

    int actividadIndex = rand() % 4; // selecciona una actividad aleatoria de la lista
    string actividadSeleccionada = actividades[actividadIndex]; // asigna la actividad seleccionada

    colaGeneral.enqueue(usuario, actividadSeleccionada); // agrega la actividad a la cola del usuario
    cout << "Actividad asignada automaticamente a " << usuario << ": " << actividadSeleccionada << endl; // confirma la asignacion
}

// -----MENUS-----
// menu para el usuario general
void menuUsuario() {
    cout << "\n=== Menu del Usuario ===\n"; // muestra el encabezado
    cout << "Aqui se mostraran las opciones para usuarios generales.\n"; // descripcion
}

// menu para asignar actividades
void asignarActividad(unique_ptr<ColaActividades>& cola, const string& usuario, const string& actividad, int perfilUsuario) {
    if (perfilUsuario != 1 && perfilUsuario != 2) { // valida que el perfil sea permitido
        cout << "Error: No tienes permiso para asignar actividades a este perfil." << endl; // mensaje de error
        return; // termina la funcion
    }

    cola->enqueue(usuario, actividad); // agrega la actividad a la cola del usuario
    cout << "Actividad asignada a " << usuario << ": " << actividad << endl; // confirma la asignacion
}

// revisa actividades asignadas a un usuario
void revisarActividades(unique_ptr<ColaActividades>& cola, const string& usuario, int perfilUsuario) {
    if (perfilUsuario != 1) { // valida que el perfil tenga permisos
        cout << "Error: No tienes permiso para revisar actividades de este perfil." << endl; // mensaje de error
        return; // termina la funcion
    }

    cout << "Actividades de " << usuario << ":" << endl; // muestra encabezado
    cola->mostrar(usuario); // llama a la funcion para mostrar actividades
}

// gestion de actividades propias
void gestionarActividadesPropias(ColaActividades* cola, const string& usuario) {
    cout << "Tus actividades asignadas:" << endl; // encabezado
    cola->mostrar(usuario); // muestra las actividades del usuario
}

// -----MENU SUPERVISOR-----
// menu para supervisores
void menuSupervisor(ListaEnlazadaAccesos& accesos, ColaActividades* colaGeneral, ColaActividades* colaSupervisor, const string& supervisor) {
    int opcion; // opcion seleccionada

    do {
        cout << "\n=== Menu del Supervisor ===" << endl; // encabezado
        cout << "1. Asignar actividad a un usuario general" << endl; // opcion 1
        cout << "2. Revisar actividades de un usuario general" << endl; // opcion 2
        cout << "3. Revisar tus propias actividades" << endl; // opcion 3
        cout << "4. Salir" << endl; // opcion 4
        cout << "Selecciona una opcion: "; // prompt de seleccion
        cin >> opcion; // lee la opcion seleccionada

        switch (opcion) {
            case 1: { // asignar actividad
                string usuario, actividad;
                cout << "Introduce el nombre del usuario: "; // solicita el nombre del usuario
                cin.ignore();
                getline(cin, usuario); // lee el nombre del usuario
                cout << "Introduce la actividad a asignar: "; // solicita la actividad
                getline(cin, actividad); // lee la actividad

                NodoAcceso* nodoUsuario = accesos.buscarPorNombre(usuario); // busca al usuario por nombre
                if (!nodoUsuario) { // verifica si el usuario no existe
                    cout << "Error: Usuario no encontrado." << endl; // mensaje de error
                } else if (nodoUsuario->perfil != 1) { // verifica si el perfil no es de usuario general
                    cout << "Error: Solo puedes asignar actividades a usuarios generales." << endl; // mensaje de error
                } else {
                    if (colaGeneral->tieneActividades(usuario)) { // verifica si el usuario ya tiene actividades
                        cout << "El usuario " << usuario << " ya tiene actividades pendientes. No se asignaran nuevas." << endl; // mensaje de error
                    } else {
                        colaGeneral->enqueue(usuario, actividad); // asigna la actividad
                        cout << "Actividad asignada a " << usuario << ": " << actividad << endl; // confirma la asignacion
                    }
                }
                break;
            }
            case 2: { // revisar actividades
                string usuario;
                cout << "Introduce el nombre del usuario a revisar: "; // solicita el nombre del usuario
                cin.ignore();
                getline(cin, usuario); // lee el nombre del usuario

                colaGeneral->mostrar(usuario); // muestra las actividades del usuario
                break;
            }
            case 3: { // revisar actividades propias
                colaSupervisor->mostrar(supervisor); // muestra las actividades propias
                break;
            }
            case 4:
                cout << "Saliendo del sistema..." << endl; // mensaje de salida
                break;
            default:
                cout << "Opcion no valida. Intentalo de nuevo." << endl; // mensaje de opcion invalida
        }
    } while (opcion != 4); // repite hasta que se seleccione salir
}

// -----ANALISTA-----
// funcion recursiva para contar accesos
void contarAccesosRecursivo(NodoAcceso* actual, map<string, int>& conteos) {
    if (!actual) return; // si no hay mas nodos, termina
    conteos[actual->nombreUsuario]++; // incrementa el conteo para el usuario actual
    contarAccesosRecursivo(actual->siguiente, conteos); // pasa al siguiente nodo
}

// genera estadisticas de accesos
void generarEstadisticasAccesos(ListaEnlazadaAccesos& accesos) {
    map<string, int> conteos; // mapa para almacenar conteos
    contarAccesosRecursivo(accesos.getCabeza(), conteos); // cuenta accesos recursivamente

    cout << "Estadisticas de accesos:\n"; // encabezado
    for (const auto& par : conteos) { // recorre los conteos
        cout << par.first << ": " << par.second << " accesos\n"; // muestra el conteo para cada usuario
    }

    // guarda el informe en un archivo
    ofstream archivo("informe_accesos.txt");
    archivo << "Informe de Accesos:\n";
    archivo << "Total de accesos: " << conteos.size() << "\n";
    for (const auto& par : conteos) {
        archivo << par.first << ": " << par.second << " accesos\n";
    }
    archivo.close(); // cierra el archivo
}

// -----ACTIVIDADES SOSPECHOSAS-----
// función recursiva para detectar actividades sospechosas
void detectarSospechosasRecursivo(NodoCola* actual, map<string, map<string, int>>& patrones, time_t intervalo = 600) {
    if (!actual) return; // caso base: si la cola está vacía o ya no hay más nodos, termina la recursión

    patrones[actual->usuario][actual->actividad]++; // incrementa el contador de actividad para el usuario en el mapa
    detectarSospechosasRecursivo(actual->siguiente, patrones); // llama recursivamente con el siguiente nodo
}

// genera un informe de actividades sospechosas
void generarInformeSospechosas(ColaActividades& cola) {
    map<string, map<string, int>> patrones; // estructura para almacenar patrones de actividades por usuario
    detectarSospechosasRecursivo(cola.getFrente(), patrones); // llena los patrones usando la función recursiva

    cout << "Informe de actividades sospechosas:\n"; // mensaje inicial en consola
    ofstream archivo("informe_sospechosas.txt"); // abre un archivo para guardar el informe
    archivo << "Informe de Actividades Sospechosas:\n"; // escribe el encabezado en el archivo

    // recorre los patrones para identificar actividades sospechosas
    for (const auto& usuario : patrones) {
        for (const auto& actividad : usuario.second) {
            if (actividad.second > 2) { // si una actividad se repite más de dos veces, es sospechosa
                cout << "Usuario: " << usuario.first << ", Actividad: " << actividad.first
                     << ", Repeticiones: " << actividad.second << "\n"; // imprime los detalles en consola
                archivo << "Usuario: " << usuario.first << ", Actividad: " << actividad.first
                        << ", Repeticiones: " << actividad.second << "\n"; // guarda los detalles en el archivo
            }
        }
    }
    archivo.close(); // cierra el archivo después de guardar los datos
}

// -----MENU ANALISTA-----
// menú interactivo para el analista
void menuAnalista(ListaEnlazadaAccesos& accesos, ColaActividades& cola) {
    int opcion; // variable para almacenar la opción del usuario
    do {
        cout << "\n=== Menu del Analista ===\n"; // encabezado del menú
        cout << "1. Generar estadisticas de accesos\n"; // opción para estadísticas
        cout << "2. Detectar actividades sospechosas\n"; // opción para detectar actividades
        cout << "3. Salir\n"; // opción para salir
        cout << "Selecciona una opcion: ";
        cin >> opcion; // lee la opción del usuario

        switch (opcion) {
            case 1:
                generarEstadisticasAccesos(accesos); // llama a la función para generar estadísticas
                break;
            case 2:
                generarInformeSospechosas(cola); // llama a la función para generar el informe de sospechosas
                break;
            case 3:
                cout << "Saliendo del menu del analista...\n"; // mensaje de salida
                break;
            default:
                cout << "Opcion no valida. Intentalo de nuevo.\n"; // mensaje de error si la opción es inválida
        }
    } while (opcion != 3); // repite mientras el usuario no seleccione salir
}

// -----INICIAR SESION-----
// función para manejar el inicio de sesión y asignar actividades
void iniciarSesion(ListaEnlazadaAccesos& accesos, ColaActividades& colaGeneral) {
    string usuario, contrasena, telefono, contrasenaAleatoria; // variables para almacenar las credenciales

    cout << "\n*** Bienvenido al sistema de login ***\n"; // mensaje inicial
    cout << "Contrasenia diaria: " << generarContrasenaDiaria() << endl; // muestra la contraseña diaria

    // solicita el nombre de usuario
    cout << "Ingrese su nombre de usuario: ";
    cin >> usuario;

    // busca el nodo del usuario en la lista
    NodoAcceso* nodo = accesos.buscarPorNombre(usuario); // busca por nombre de usuario
    if (!nodo) { // si no encuentra el nodo
        cout << "Error: Usuario no encontrado." << endl; // mensaje de error
        return; // termina la función
    }

    // lógica para perfil general (perfil == 1)
    if (nodo->perfil == 1) {
        cout << "Login exitoso. Bienvenido, " << nodo->nombreUsuario << "!\n"; // mensaje de bienvenida

        // asigna automáticamente una actividad al usuario
        asignarActividadAutomaticamente(accesos, colaGeneral, usuario);

        // muestra las actividades asignadas al usuario
        cout << "\nActividades asignadas a " << usuario << ":\n";
        colaGeneral.mostrar(usuario);
    }
    // lógica para perfil supervisor (perfil == 2)
    else if (nodo->perfil == 2) {
        cout << "Ingrese su contrasenia: ";
        cin >> contrasena;

        if (nodo->contrasena == contrasena) { // verifica la contraseña
            cout << "Login exitoso. Bienvenido, " << nodo->nombreUsuario << "!\n";

            // crea una cola local para el supervisor
            ColaActividades colaSupervisor;

            // llama al menú del supervisor
            menuSupervisor(accesos, &colaGeneral, &colaSupervisor, nodo->nombreUsuario);
        } else {
            cout << "Error: Contrasenia incorrecta." << endl; // mensaje de error si la contraseña no coincide
        }
    }
    // lógica para perfil analista (perfil == 3)
    else if (nodo->perfil == 3) {
        cout << "Ingrese su contrasenia: ";
        cin >> contrasena;
        cout << "Ingrese su telefono: ";
        cin >> telefono;
        cout << "Ingrese la contrasenia diaria: ";
        cin >> contrasenaAleatoria;

        if (nodo->contrasena == contrasena && nodo->telefono == telefono &&
            contrasenaAleatoria == generarContrasenaDiaria()) { // verifica todas las credenciales del analista
            cout << "Login exitoso. Bienvenido, " << nodo->nombreUsuario << "!\n";

            // llama al menú del analista
            menuAnalista(accesos, colaGeneral);
        } else {
            cout << "Error: Credenciales incorrectas." << endl; // mensaje de error si las credenciales son inválidas
        }
    }
    // caso para perfiles no reconocidos
    else {
        cout << "Error: Perfil no reconocido." << endl; // mensaje de error si el perfil no es válido
    }
}


// -----PRUEBAS DEL SISTEMA-----
// pruebas del sistema de login y control de seguridad
void pruebas() {
    unique_ptr<ListaEnlazadaAccesos> accesos = make_unique<ListaEnlazadaAccesos>(); // crea una lista enlazada de accesos
    unique_ptr<PilaSeguridad> pila = make_unique<PilaSeguridad>(); // crea una pila de seguridad

    // crea registros de usuarios con diferentes perfiles y horarios
    time_t ahora = time(0); // obtiene la hora actual
    time_t haceUnaHora = ahora - 3600; // calcula hace una hora
    time_t haceDosHoras = ahora - 7200; // calcula hace dos horas

    accesos->insertar("juan", ahora, 1); // inserta un usuario general
    accesos->insertar("ana", haceUnaHora, 2, "password2"); // inserta un supervisor
    accesos->insertar("carlos", haceDosHoras, 3, "password3", "987654321"); // inserta un analista

    // muestra los registros de accesos
    cout << "Lista de accesos registrados:" << endl;
    accesos->mostrar();

    // busca un nodo por nombre de usuario
    auto nodoNombre = accesos->buscarPorNombre("ana"); // busca al usuario "ana"
    if (nodoNombre) { // verifica si se encontro el nodo
        cout << "\nRegistro encontrado por nombre: " << nodoNombre->nombreUsuario << ", " << ctime(&(nodoNombre->horaAcceso)) << endl; // muestra los detalles del nodo
    }

    // busca un nodo por hora de acceso
    auto nodoHora = accesos->buscarPorHora(haceDosHoras); // busca por la hora hace dos horas
    if (nodoHora) { // verifica si se encontro el nodo
        cout << "\nRegistro encontrado por hora: " << nodoHora->nombreUsuario << ", " << ctime(&(nodoHora->horaAcceso)) << endl; // muestra los detalles del nodo
    }

    // ajusta la pila de seguridad para un perfil
    cout << "\nAjustando nivel de seguridad para perfil 2:" << endl; // indica que ajustara la seguridad para el supervisor
    pila->ajustarNivel(2); // ajusta el nivel de seguridad para el perfil de supervisor
    pila->mostrarPila(); // muestra los niveles de seguridad en la pila

    // validacion de credenciales
    cout << "\nValidacion de credenciales:" << endl; // mensaje de inicio de la validacion
    string contrasenaDiaria = generarContrasenaDiaria(); // genera la contrasena diaria
    cout << "Contrasenia diaria generada: " << contrasenaDiaria << endl; // muestra la contrasena diaria

    accesos->validarCredenciales("juan"); // valida las credenciales del usuario "juan"
    accesos->validarCredenciales("ana", "password2"); // valida las credenciales del supervisor "ana"
    accesos->validarCredenciales("carlos", "password3", "987654321", contrasenaDiaria); // valida las credenciales del analista "carlos"


    int opcion;
    do {
        cout << "\n=== Sistema de Login ===" << endl;
        cout << "1. Iniciar sesion" << endl;
        cout << "2. Salir" << endl;
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                iniciarSesion(*accesos, colaGeneral); // llama a la función para iniciar sesión
            break;
            case 2:
                cout << "Saliendo del sistema..." << endl;
            break;
            default:
                cout << "Opcion no valida. Intentalo de nuevo." << endl;
            break;
        }
    } while (opcion != 2); // el menú sigue mostrando opciones hasta que se elige salir
}

// -----FUNCION PRINCIPAL-----
// funcion principal del programa
int main() {
    srand(time(0)); // inicializa el generador de numeros aleatorios con la hora actual
    pruebas(); // ejecuta las pruebas del sistema
    return 0; // finaliza la ejecucion del programa
}
