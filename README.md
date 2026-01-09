# Math-Julia – Documentación

### Versión: 1.0.0
### Fecha: 09/01/2026

Math-Julia es un superset de Julia diseñado para facilitar la escritura de código matemático con tipado explícito opcional y sintaxis simplificada, inspirada en Julia pero con características propias como := y modo strict.

1. Instalación y uso básico

Ejecutable principal:
main – compila y transpila archivos .mjl a Julia (.jl) y opcionalmente ejecuta el main.

Sintaxis:
```bash
mjl init              # Crear config.json interactivo
mjl config.json       # Transpilar los archivos listados en config.json
mjl config.json julia # Transpilar y ejecutar el main con Julia
mjl -v                # Mostrar versión
```
Configuración inicial:
Al ejecutar ./main init:
1. Se pregunta si se quiere activar el modo strict (ver sección 2.1).
2. Se define el archivo principal (Main) y la lista de archivos .mjl a transpilar.
3. Se genera un config.json con esta información.

Ejemplo config.json:
```json
{

    "strict": true,

    "Main": "t1.mjl",

    "Files": ["t1.mjl", "t2.mjl"]

}
```
2. Tipado y modo strict

2.1 Tipado
- strict = true obliga a declarar tipos al usar = (asignación simple).  
- Si no se especifica tipo, se permite #no-strict para ignorar el modo strict en esa línea.  
- La sintaxis := siempre transpila a = y no requiere tipado.

Tipos básicos:

Z          -> Int64

N          -> UInt64

R          -> Float64

C          -> Complex

Q          -> Rational{Int64}

Str        -> String

Ejemplo:
```julia
x::Z = 10

y::R = 3.14

r::C = 1 + 2im

q::Q = 2//3

s::Str = "hola"
```
2.2 Declaraciones sin tipo
```julia
x := 2           # Siempre permitido, transpila a x = 2

y = 2 #no-strict  # Ignora strict
```
Si strict = true y no se usa #no-strict o :=, se produce error.

1. Funciones y sintaxis simplificada

3.1 Definición de funciones inline
```julia
f(x) := 2x * im
g(y) = 2y * im #no-strict
```

3.2 Strings y f-strings
```julia
name := "Ian"
msg := f"Hola {name}, resultado = {x}"
```
- Las f-strings se transpilan a la interpolación de Julia: "$name"  
- Se soportan llaves dobles {{}} para literal {.

1. Entrada y salida

4.1 Imprimir
```julia
printf("x = %d, y = %.2f\n", x, y) #no-strict necesario por signo =
```
- Transpila a @printf de Julia (requiere using Printf automático si se detecta @printf).

4.2 Leer
```julia
name := input("Ingrese su nombre: ")
```
- Se transpila a:
```julia
(prompt -> (print(prompt); readline()))("Ingrese su nombre: ")
```
5. Booleanos y evaluación en tiempo de transpile

5.1 Función to_bool()
```
x := to_bool(true)   # true
y := to_bool(0)        # false
z := to_bool("texto")  # true
```
- Evalúa literales en tiempo de transpile:
  - true -> true
  - false -> false
  - 0 -> false, cualquier otro número -> true
  - Strings no vacíos -> true

6. Arrays y matrices

6.1 Declaración de arrays
```
a::Array2d{R} = [1.0 2.0; 3.0 4.0]
b::Array3d{Z} = zeros(Int64, 2,2,2)
```
- Array***N***d{tipo} transpila a Array{tipo, n} en Julia.

7. Transpilación y ejecución

7.1 Flujo de trabajo
1. Escribir archivos .mjl  
2. Configurar config.json (mjl init)  
3. Transpilar: mjl config.json -> genera .jl  
4. Ejecutar: mjl config.json julia -> transpila y ejecuta main.

7.2 Funciones internas del transpiler

- regex_replace_fn: reemplazo avanzado con lambda (usado para f-strings y to_bool)  
- verify::types(code, file): chequeo de tipos en modo strict  
- translate::map(code, dict): reemplazos de sintaxis de Math-Julia a Julia  
- translate::formatStrings(code): f-strings -> interpolación Julia  
- translate::evalToBoolean(literal): evalúa literales a booleano  
- translate::toBooleanReplace(m): función auxiliar para regex_replace_fn

Hay tests de ejemplo en el repositorio
