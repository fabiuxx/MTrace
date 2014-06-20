#include "mtrace.h"

#if TRACE_MEMORY_ALLOCATIONS

typedef struct mtinfo_t {
    size_t          size;                       // Cantidad de bytes asignados.
    char*           file;                       // Archivo donde se realiza la asignacion.
    short           line;                       // Linea dentro del archivo.
    void            *ptr;                       // Puntero a los bytes asignados.
    struct mtinfo_t *next;                      // Elemento siguiente en la lista.
    struct mtinfo_t *prev;                      // Elemento previo en la lista.
} mtinfo_t;

/*
 * Inicializa un objeto mtinfot_t.
 */
static inline void mtinfo_init(mtinfo_t *self, void *ptr, size_t size, char *file, uint16_t line) {
    self->ptr  = ptr;
    self->size = size;
    self->file = file;
    self->line = line;
}

static uint32_t     m_count = 0;                // Cantidad de asignaciones realizadas.
static uint32_t     f_count = 0;                // Cantidad de liberaciones realizadas.
static const size_t PAD = sizeof(void*);        // Tamanho extra que se reserva en cada asignacion.
static int          trace_list_size  = 0;       // Cantidad de elementos en la lista.
static mtinfo_t     *trace_list_tail = null;    // Ultimo elemento de la lista.

/*
 * Obtiene la estructura mtinfo_t asociada a un puntero previamente
 * obtenido con __malloc.
 */
static inline mtinfo_t* get_trace_info_from_ptr(void *ptr) {
    void **tmp = (ptr - PAD);
    return cast(*tmp, mtinfo_t*);
}

/*
 * Elimina la memoria asignada a un objeto mtinfo_t.
 */
static inline void free_trace_info(mtinfo_t *info) {
    free(info->ptr);
    info->next = null;
    info->prev = null;
    free(info);
}

/*
 * Agrega un objeto mtinfo_t a la lista global de objetos.
 */
static void add_trace_info(mtinfo_t *info) {
    if(trace_list_tail != null) {
        info->prev = trace_list_tail;
        trace_list_tail->next = info;
    }
    trace_list_tail = info;
    trace_list_size ++;
}

/*
 * Elimina un objeto mtinfo_t de la lista global de objetos.
 */
static void remove_trace_info(mtinfo_t *info) {
    if(trace_list_tail != null) {
        // CASO 1: Se elimina la ultima asignacion de memoria.
        if(info == trace_list_tail) {
            // Se actualiza la cola de la lista.
            trace_list_tail = trace_list_tail->prev;
        }
        // CASO 2: Se elimina una asignacion antigua que esta en algun lugar
        // de la lista.
        else {
            mtinfo_t *_prev = info->prev;
            mtinfo_t *_next = info->next;
            if(_prev != null) {
                _prev->next = _next;
            }
            if(_next != null) {
                _next->prev = _prev;
            }
        }
    }
    // Eliminacion del objeto y actualizacion del estado de la lista.
    free_trace_info(info);
    trace_list_size --;
}

/*
 * Retorna un puntero a los size bytes asignados y se asocia la asignacion
 * a un objeto mtinfo_t correspondiente.
 */
void* __malloc(size_t size, char *file, uint16_t line) {
    // Se reservan size bytes mas una cantidad extra.
    void *ptr = malloc(size + PAD);
    if(ptr != null) {
        // Creamos la estructura que contiene los detalles de la asignacion.
        mtinfo_t *info = malloc(sizeof(mtinfo_t));
        mtinfo_init(info, ptr, size, file, line);
        info->next = null;
        info->prev = null;
        // Se inserta la direccion del objeto mtinfo_t al inicio del puntero
        // a retornar.
        void **tmp = ptr;
        *tmp = V(info);
        // Se agrega lel objeto mtinfo_t a la lista global.
        add_trace_info(info);
        // Se actualiza la cantidad de asignaciones realizadas.
        m_count ++;
        // Se retorna el puntero de tal forma que el codigo cliente no sepa
        // que existe un puntero a un objeto mtinfo_t al frente.
        return (ptr + PAD);
    }
    // No hay mas memoria T_T.
    return null;
}

/*
 * Redimensiona (agranda/achica) un bloque de memoria previamente asignado
 * con __malloc y retorna el puntero a ese nuevo (o el mismo) bloque.
 */
void* __realloc(void *ptr, size_t new_size, char *file, uint16_t line) {
    // Se obtiene la informacion original de la asignacion previamente realizada.
    mtinfo_t *info = get_trace_info_from_ptr(ptr);
    // Si no hay redimension, se deja tal cual.
    if(new_size == info->size) {
        return ptr;
    }
    // El puntero al bloque asignado previamente con __malloc.
    void *old_ptr = info->ptr;
    // Se redimensiona el bloque asignado previamente.
    void *new_ptr = realloc(old_ptr, new_size + PAD);
    void *pointer = (old_ptr == new_ptr) ? old_ptr : new_ptr;
    // Se inserta la informacion del objeto mtinfo_t al inicio del puntero.
    void **tmp = pointer;
    *tmp = V(info);
    // Se actualiza el objeto mtinfo_t.
    mtinfo_init(info, pointer, new_size, file, line);
    return (pointer + PAD);
}

/*
 * Libera la memoria reservada mediante una llamada previa a __malloc.
 */
void inline __free(void *ptr) {
    // Se elimina la memoria efectiva y la informacion de rastero asociada.
    mtinfo_t *info = get_trace_info_from_ptr(ptr);
    remove_trace_info(info);
    // Se actualiza la cantidad de liberaciones de memoria realizadas.
    f_count ++;
}

/*
 * Imprime en pantalla estadisticas sobre la asignacion/liberacion de memoria.
 */
void memory_state() {
    println("Memory alloc calls: %u.", m_count);
    println("Memory free calls:  %u.", f_count);
    println("Memory allocs not freed: %u.", trace_list_size);
    if(trace_list_tail != null && trace_list_size > 0) {
        mtinfo_t *info = trace_list_tail;
        // Se itera la lista desde el final hacia el inicio.
        while(info != null) {
            println("  at [%s] %d bytes, line %hu.", info->file, info->size, info->line);
            info = info->prev;
        }
    }
}

/*
 * Libera la memoria de todas las asignaciones que no fueron explicitamente
 * liberadas.
 */
void memory_clean() {
    if(trace_list_tail != null && trace_list_size > 0) {
        mtinfo_t *info = trace_list_tail;
        mtinfo_t *tmp  = null;
        // Se itera la lista desde el final hacia el inicio.
        while(info != null) {
            tmp = info->prev;
            free_trace_info(info);
            info = tmp;
        }
        // Se resetea la lista de objetos mtinfo_t.
        trace_list_tail = null;
        trace_list_size = 0;
        // Se resetean los contadores de alloc/free.
        m_count = 0;
        f_count = 0;
    }
}

#else

static uint32_t m_count = 0;                // Cantidad de asignaciones realizadas.
static uint32_t f_count = 0;                // Cantidad de liberaciones realizadas.

/*
 * Wrapper para la funcion malloc.
 */
inline void* __malloc(size_t size) {
    void *ptr = malloc(size);
    m_count ++;
    return ptr;
}

/*
 * Wrapper para la funcion realloc.
 */
inline void* __realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

/*
 * Wrapper para la funcion free.
 */
inline void  __free(void *ptr) {
    free(ptr);
    f_count ++;
}

/*
 * Imprime en pantalla estadisticas sobre la asignacion/liberacion de memoria.
 */
inline void memory_state() {
    println("Memory alloc calls: %u.", m_count);
    println("Memory free calls:  %u.", f_count);
    println("Memory allocs not freed: %u.", m_count - f_count);
}

#endif