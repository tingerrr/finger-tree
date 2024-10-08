lang := 'de'
root := justfile_directory()

# typst variables
export TYPST_ROOT := root
export TYPST_FONT_PATHS := root / 'assets' / 'fonts'

# list recipes by default
[private]
default:
	@just --list --unsorted

# invoke typst with the given command and output type
typst *args:
	typst {{ args }}

# create the temporary and gitignored directories or files
[private]
prep:
	mkdir -p out

# watch the notes and supplementary material
notes: prep
	typst watch {{ 'etc' / 'notes.typ' }} {{ 'out' / 'notes.pdf' }}

# invoke typst for building with the given command and output type
build cmd type *args: prep
	typst {{ cmd }} {{ 'src' / lang / type + '.typ' }} {{ 'out' / type + '-' + lang + '.pdf' }} {{ args }}

# invoke typst compile for the given output type
compile type: (build 'compile' type)

#invoke typst watch for the given output type
watch type: (build 'watch' type)

# invoke typst-preview with the given output type
preview type *args: (build 'compile' type) && (build 'watch' type)
	# see: https://github.com/Enter-tainer/typst-preview/issues/289
	# typst-preview --root {{ root }} {{ 'src' / lang / type + '.typ' }} {{ args }}
	xdg-open {{ 'out' / type + '-' + lang + '.pdf' }} &

# query the document for todos
lint type:
	typst query {{ 'src' / lang / type + '.typ' }} '<todo>'
