Parse hostnames with constexpr functions into strong types

Ideally invariants in types should afford us to avoid unit testing,
by giving us compile (and runtime) guarantee about the contents.


## Basic usage

`make` will run the tests in main.ccp.
Expect the implementation to be buggy.


## Dependencies
We rely on the `NamedType` which is a powerful header only library
included in the repo.
There's also a barebone implementation of `gsl::span` included.

Apart from `std::optional` the code should happily compile in C++14
and for now this is a requirement (so no `std::string_view`s alas).
