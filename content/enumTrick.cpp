
// X Macro for generating enums and strings
#define ENUM(name, a) name##_##a,
#define STRINGS(name, a) #a,
#define MakeEnum(name, tableMaker) \
enum name \
{ \
    tableMaker(name, ENUM) \ 
name##_COUNT\
}; \
char* name##Strings[name##_COUNT + 1] = \
{ \
tableMaker(name, STRINGS) \
"" \
}; \
char* name##ToString(name x) { return name##Strings[x]; };


// Example 1
#define MyEnumTable(n, X) \
    X(n, None) \
    X(n, PlaceComponent) \
    X(n, PlaceWire) \
    X(n, DragWire)
MakeEnum(MyEnum, MyEnumTable);

// Example 2
#define AnimalEnumTable(n, X) \
    X(n, Dragon) \
    X(n, Gryphon) \
    X(n, Dog) \
    X(n, Cat)
MakeEnum(AnimalEnum, AnimalEnumTable);

