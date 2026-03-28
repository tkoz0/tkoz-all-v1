# code/repo style guidelines

High maintainability, readability, and performance are all priorities.

## naming conventions

Use `camelCaseNames` for all variable and function names.

Use `CapsEachWord` for class and struct names.

Prefixes
- `m` for class/struct members
- `c` for constants (static or global)
  - concepts count as constants so use `c` for those too
- `s` for mutable static variables
- `g` for mutable global variables
- `a` for function arguments (optional/maybe?)
- `l` for local variables (optional/maybe?)
- `I` for interface classes (pure virtual only, no member state)

Other
- prefix `c` and suffix `T` for non type template parameters
- suffix `T` for typename template parameters
- do the same for constants or types that depend on template parameters

## functions

Use trailing return types: `auto function(args...) -> ReturnType`

Language keyword/feature checklist
- `explicit` use by default for single argument constructors unless implicit
  conversion is desired, if implicit, put `/*implicit*/` on the function
- `inline` use for everything small that is defined in a header, although this
  may be redundant
- `constexpr` for header defined functions that make sense to allow evaluation
  at compile time if possible
- `const` for member functions that do not modify the class state
  - in rare cases, maybe also `&`, `const&`, `&&`, `const&&`, or even consider
    `volatile`
- `noexcept` try to put on all (especially small functions) declarations for
  functions which easily can be shown to not throw
- `= default` use for default/copy/move constructors or destructors where
  appropriate
  - try to specify these as default/delete or define them, rather than let a
    compiler choose from seeing no definition or declaration
- `= delete` use for default/copy/move constructors where appropriate
  - try to delete these functions by default unless there is a good reason to
    have them
- `virtual` for any function that can be overridden
- `override` for any function that overrides a base member
- `final` on functions occasionally if appropriate
- `= 0` pure virtual (cannot instantiate a class with any of these)
- `[[nodiscard]]` on any function returning a value without side effects,
  often accessor member functions
- `[[noreturn]]` on any functions that infinite loop or unconditionally throw

Other
- pass by value and move if the class takes and stores a copy of the argument,
  especially useful for setters that take things like vectors
- take references if possible, otherwise pointers/shared_ptr may be ok if it is
  possible for an object to be null

## classes

- `final` when appropriate, some things are not meant for subclasses
- `/*abstract*/` label classes that contain any pure virtual functions
- try to avoid multiple inheritance, multiple interfaces is ok
- `class` for most object types that encapsulate functionality
- `struct` for grouping data for convenience with no invariants
- put visibility specifier explicitly, do not depend on class/struct default
- prefer keeping things in minimum visibility, move up later if needed

## includes

Sort from "most specific to most general" (current project, then my libraries,
then 3rd party libraries, then boost, then STL). This reduces the chance of
missing transitive includes.
