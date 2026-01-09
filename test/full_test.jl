using Printf
x::Int64 = 0
y::String = "Hello"
z::Float64 = 3.5

if true
    @printf("X is true\n")
else
    @printf("X is false\n")
end

println("Greeting: $(y), number: $(z)")
