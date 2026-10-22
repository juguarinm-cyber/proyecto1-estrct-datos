#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <algorithm> // Necesario para std::max

using namespace std;

// ==========================================
// CLASE PEDIDO (Estructura de datos base)
// ==========================================
// Unidad básica de información que se moverá por todo el sistema.
class Pedido {
public:
    int id_pedido;
    string nombre_plato;

    // Constructores (vacío y con parámetros)
    Pedido() { id_pedido = 0; nombre_plato = ""; }
    Pedido(int id, string nombre) { id_pedido = id; nombre_plato = nombre; }
};

// ==========================================
// CLASE MENUARREGLO (Arreglo Dinámico)
// Complejidades: Acceso O(1), Inserción O(1) amortizado, Eliminación O(n)
// ==========================================
class MenuArreglo {
protected:
    Pedido * datos; // Puntero al arreglo dinámico en memoria
    int N;          // Cantidad actual de platos reales en el menú
    int tamano;     // Capacidad máxima de "sillas" reservadas en memoria
    double aureo = 1.618; // Factor de crecimiento (Razón Áurea)

public:
    MenuArreglo() {
        N = 0;
        tamano = 10; // Inicia con capacidad para 10 platos
        datos = new Pedido[tamano];
    }

    ~MenuArreglo() {
        delete[] datos; // Libera la memoria al cerrar el programa
    }

    // Agrega un plato. Si se llena, multiplica la capacidad por 1.618
    void agregar(Pedido p) {
        if(N == tamano) {
            // max() previene que tamaños pequeños (ej. 1) no crezcan por el truncamiento de decimales a enteros
            int ntamano = max(tamano + 1, (int)(tamano * aureo));
            Pedido * ndatos = new Pedido[ntamano]; // Crea arreglo más grande
            for(int i = 0; i < N; i++) {
                ndatos[i] = datos[i]; // Copia los datos viejos
            }
            delete[] datos; // Destruye el arreglo viejo
            datos = ndatos; // Apunta al nuevo arreglo
            tamano = ntamano;
        }
        datos[N] = p;
        N++;
    }

    // Muestra todos los elementos (Costo O(n))
    void mostrar_menu() {
        cout << "\n--- Menu del Restaurante ---\n";
        for(int i = 0; i < N; i++) {
            cout << i + 1 << ": " << datos[i].nombre_plato << "\n";
        }
        cout << "----------------------------\n";
    }

    // Retorna el pedido de una posición exacta al instante (Costo O(1))
    Pedido obtener(int indice) {
        if (indice >= 0 && indice < N) {
            return datos[indice];
        }
        return Pedido(-1, "Error");
    }

    int longitud() {
        return N;
    }

    // Modifica un plato existente sobrescribiendo su string (Costo O(1))
    void modificar(int indice, string nuevo_nombre) {
        if (indice >= 0 && indice < N) {
            datos[indice].nombre_plato = nuevo_nombre;
            cout << "Plato modificado correctamente.\n";
        } else {
            cout << "Indice no valido.\n";
        }
    }

    // Elimina un plato y desplaza los demás a la izquierda (Costo O(n))
    void eliminar(int indice) {
        if (indice >= 0 && indice < N) {
            // Desplazamiento para cubrir el "hueco" dejado por el plato eliminado
            for(int i = indice; i < N - 1; i++) {
                datos[i] = datos[i+1];
            }
            N--;
            cout << "Plato eliminado correctamente.\n";

            // Encoger arreglo (Shrink): Si sobra mucha memoria, reduce el tamaño
            if (tamano > 10 && N * aureo * aureo < tamano) {
                int ntamano = max(10, (int)(tamano / aureo));
                Pedido * ndatos = new Pedido[ntamano];
                for(int i = 0; i < N; i++) {
                    ndatos[i] = datos[i];
                }
                delete[] datos;
                datos = ndatos;
                tamano = ntamano;
            }
        } else {
            cout << "Indice no valido.\n";
        }
    }
};

// ==========================================
// CLASE NODOPEDIDO (Eslabón para Colas y Pilas)
// ==========================================
// Actúa como una caja que guarda el Pedido y una cuerda (puntero) hacia la siguiente caja
class NodoPedido {
public:
    Pedido dato;
    NodoPedido * sig;

    NodoPedido(Pedido _dato) {
        dato = _dato;
        sig = NULL; // Al crearse, no apunta a nadie todavía
    }
};

// ==========================================
// CLASE COLACOCINA (FIFO - First In, First Out)
// Complejidades: Encolar O(1), Desencolar O(1)
// ==========================================
class ColaCocina {
protected:
    NodoPedido * cabeza; // Frente de la fila (el más viejo, próximo a salir)
    NodoPedido * ultimo; // Final de la fila (el más nuevo, recién llegado)

public:
    ColaCocina() {
        cabeza = NULL;
        ultimo = NULL;
    }

    ~ColaCocina() {
        NodoPedido * aux = cabeza;
        while(cabeza != NULL) {
            cabeza = cabeza->sig;
            delete aux;
            aux = cabeza;
        }
    }

    bool esvacia() {
        return cabeza == NULL;
    }

    // Inserta un nuevo pedido al final de la fila (O(1))
    void insertar_al_ultimo(Pedido p) {
        NodoPedido * nuevo = new NodoPedido(p);
        if(esvacia()) {
            cabeza = nuevo;
            ultimo = cabeza;
        } else {
            ultimo->sig = nuevo; // El que antes era el último, ahora apunta al nuevo
            ultimo = nuevo;      // El puntero 'ultimo' se actualiza al recién llegado
        }
        cout << "Pedido agregado a la cocina: " << p.nombre_plato << "\n";
    }

    // Saca el pedido en el frente de la cola (O(1))
    Pedido eliminar_primero() {
        if(esvacia()) {
            cout << "La cocina no tiene pedidos pendientes.\n";
            return Pedido(-1, "Nada");
        }
        Pedido d = cabeza->dato;
        NodoPedido * aux = cabeza;
        cabeza = cabeza->sig; // La cabeza avanza al segundo de la fila
        if(esvacia()) {
            ultimo = NULL; // Si la fila quedó vacía, el último también es NULL
        }
        delete aux;
        return d;
    }

    // Recorre todos los nodos para mostrarlos sin eliminarlos
    void mostrar_todos() {
        if (esvacia()) {
            cout << "Cocina: No hay pedidos en espera.\n";
            return;
        }
        cout << "\n--- PEDIDOS EN COCINA (FIFO) ---\n";
        NodoPedido * actual = cabeza;
        int turno = 1;
        while (actual != NULL) {
            cout << turno++ << ". " << actual->dato.nombre_plato << "\n";
            actual = actual->sig;
        }
        cout << "--------------------------------\n";
    }
};

// ==========================================
// CLASE PILACAJA (LIFO - Last In, First Out)
// Complejidades: Apilar O(1), Desapilar O(1)
// ==========================================
class PilaCaja {
protected:
    NodoPedido * cabeza; // Cima de la pila (la factura más reciente)

public:
    PilaCaja() {
        cabeza = NULL;
    }

    ~PilaCaja() {
        NodoPedido * aux = cabeza;
        while(cabeza != NULL) {
            cabeza = cabeza->sig;
            delete aux;
            aux = cabeza;
        }
    }

    bool esvacia() {
        return cabeza == NULL;
    }

    // Inserta una nueva factura en el tope de la torre (O(1))
    void insertar_al_inicio(Pedido p) {
        NodoPedido * nuevo = new NodoPedido(p);
        if(esvacia()) {
            cabeza = nuevo;
        } else {
            nuevo->sig = cabeza; // El nuevo nodo se "para" sobre la cabeza anterior
            cabeza = nuevo;      // La cabeza se actualiza al nuevo tope
        }
        cout << "Factura registrada para: " << p.nombre_plato << "\n";
    }

    // Saca y retorna la última factura del tope (O(1))
    Pedido eliminar_primero() {
        if(esvacia()) {
            cout << "La caja no tiene facturas registradas.\n";
            return Pedido(-1, "Nada");
        }
        Pedido d = cabeza->dato;
        NodoPedido * aux = cabeza;
        cabeza = cabeza->sig; // La cabeza baja un nivel en la torre
        delete aux;
        return d;
    }

    // Muestra todo el historial apilado
    void mostrar_todos() {
        if (esvacia()) {
            cout << "Caja: No hay facturas en el historial.\n";
            return;
        }
        cout << "\n--- HISTORIAL DE CAJA (LIFO) ---\n";
        NodoPedido * actual = cabeza;
        int turno = 1;
        while (actual != NULL) {
            cout << turno++ << ". " << actual->dato.nombre_plato << "\n";
            actual = actual->sig;
        }
        cout << "--------------------------------\n";
    }
};

// ==========================================
// FUNCION PRINCIPAL E INTERFAZ DE USUARIO
// ==========================================
int main() {
    MenuArreglo menu;
    ColaCocina cocina;
    PilaCaja caja;

    // Carga de datos iniciales
    menu.agregar(Pedido(101, "Hamburguesa Sencilla"));
    menu.agregar(Pedido(102, "Papas Fritas"));
    menu.agregar(Pedido(103, "Gaseosa"));

    int opcion = 0;
    string entrada_opcion;

    while (opcion != 5) {
        cout << "\n--- SISTEMA DE GESTION DE PEDIDOS ---\n";
        cout << "1. Gestion de Menu (Arreglos Dinamicos)\n";
        cout << "2. Tomar Pedido (Colas)\n";
        cout << "3. Cocina / Despacho (Colas)\n";
        cout << "4. Caja / Facturacion (Pilas)\n";
        cout << "5. Salir\n";
        cout << "Ingrese una opcion: ";

        cin >> entrada_opcion;
        // Blindaje contra errores de entrada
        try { opcion = stoi(entrada_opcion); } catch(...) { opcion = -1; }

        if (opcion == 1) {
            int subopcion = -1;
            string entrada_sub;
            while (subopcion != 0) {
                cout << "\n--- GESTION DE MENU ---\n";
                cout << "1. Ver Menu\n";
                cout << "2. Agregar nuevo plato a la carta\n";
                cout << "3. Modificar plato existente\n";
                cout << "4. Eliminar plato de la carta\n";
                cout << "0. Volver al menu principal\n";
                cout << "Ingrese una opcion: ";

                cin >> entrada_sub;
                try { subopcion = stoi(entrada_sub); } catch(...) { subopcion = -1; }

                if (subopcion == 1) {
                    menu.mostrar_menu();
                } else if (subopcion == 2) {
                    cout << "Ingrese el nombre del plato: ";
                    string nombre_plato;
                    cin >> ws;
                    getline(cin, nombre_plato);
                    int nuevo_id = 100 + menu.longitud() + 1;
                    menu.agregar(Pedido(nuevo_id, nombre_plato));
                    cout << "Plato agregado exitosamente.\n";
                } else if (subopcion == 3) {
                    menu.mostrar_menu();
                    cout << "Ingrese el numero del plato a modificar: ";
                    string input_mod;
                    cin >> input_mod;
                    int plato_mod = -1;
                    try { plato_mod = stoi(input_mod); } catch(...) {}

                    if (plato_mod > 0 && plato_mod <= menu.longitud()) {
                        cout << "Ingrese el nuevo nombre para este plato: ";
                        string nuevo_nombre;
                        cin >> ws;
                        getline(cin, nuevo_nombre);
                        menu.modificar(plato_mod - 1, nuevo_nombre);
                    } else {
                        cout << "Opcion no valida.\n";
                    }
                } else if (subopcion == 4) {
                    menu.mostrar_menu();
                    cout << "Ingrese el numero del plato a eliminar: ";
                    string input_elim;
                    cin >> input_elim;
                    int plato_eliminar = -1;
                    try { plato_eliminar = stoi(input_elim); } catch(...) {}
                    menu.eliminar(plato_eliminar - 1);
                } else if (subopcion != 0) {
                    cout << "Opcion no valida. Intente de nuevo.\n";
                }
            }
        }
        else if (opcion == 2) {
            menu.mostrar_menu();
            cout << "Ingrese los numeros de los platos (use comas, espacios o & para separarlos): ";
            string input;
            cin >> ws;
            getline(cin, input);

            // Filtro que extrae unicamente los numeros ingresados
            for (int i = 0; i < input.length(); i++) {
                if (!isdigit(input[i])) {
                    input[i] = ' ';
                }
            }

            stringstream ss(input);
            string token;
            while (ss >> token) {
                try {
                    int plato_opcion = stoi(token);
                    Pedido plato_elegido = menu.obtener(plato_opcion - 1);
                    if (plato_elegido.id_pedido != -1) {
                        cocina.insertar_al_ultimo(plato_elegido);
                    } else {
                        cout << "Plato no valido.\n";
                    }
                } catch (...) {}
            }
        }
        else if (opcion == 3) {
            cocina.mostrar_todos();
            if (!cocina.esvacia()) {
                cout << "Desea despachar el pedido en frente y enviarlo a caja? (1=Si, 0=No): ";
                string confirmar;
                cin >> confirmar;
                if (confirmar == "1") {
                    Pedido despachado = cocina.eliminar_primero();
                    caja.insertar_al_inicio(despachado);
                }
            }
        }
        else if (opcion == 4) {
            caja.mostrar_todos();
            if (!caja.esvacia()) {
                cout << "Desea anular la transaccion mas reciente en la cima? (1=Si, 0=No): ";
                string anular;
                cin >> anular;
                if (anular == "1") {
                    Pedido anulado = caja.eliminar_primero();
                    cout << "Se anulo el cobro de: " << anulado.nombre_plato << "\n";
                }
            }
        }
        else if (opcion != 5) {
            cout << "Opcion no valida. Por favor seleccione un numero del 1 al 5.\n";
        }
    }
    cout << "Saliendo del sistema...\n";
    return 0;
}
