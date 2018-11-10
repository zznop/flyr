Import('env')

sources = [
    '$SRCDIR/core/main.c',
    '$SRCDIR/core/utils.c',
    '$SRCDIR/core/load.c',
    '$SRCDIR/core/unload.c',
    '$SRCDIR/core/conversion.c',
    '$SRCDIR/core/build.c',
    '$SRCDIR/core/mutate.c',
    '$SRCDIR/core/output.c',
    '$SRCDIR/core/parson/parson.c',
]

flyr = env.Program(
    'flyr',
    source = sources,
)

Return('flyr')
