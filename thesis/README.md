# Project structure
The project is structured as follows:
- `src`: the language agnostic document source files like figures and bibliographies
- `src/de`: the german thesis document files
- `src/de/thesis.typ`: the entnry point of the german thesis document
- `assets`: other relevant assets for compilation and styling

# Tooling
The following tooling is required for compilation
- [typst][typst]

# Compiling
To compile the document into a PDF run the following commands in the same directory as this README:

```
mkdir -p out
typst compile --root . src/de/thesis.typ out/thesis.pdf
```

First time compilation requires an internet connection to cache the packages used for this document.

[typst]: typst.app
[ctf]: https://github.com/tingerrr/chiral-thesis-fhe
