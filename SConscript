Import('env')

sources = [
    '$SRCDIR/main.c',
    '$SRCDIR/utils.c',
    '$SRCDIR/load.c',
    '$SRCDIR/unload.c',
    '$SRCDIR/conversion.c',
    '$SRCDIR/build.c',
    '$SRCDIR/mutate.c',
    '$SRCDIR/output.c',
    '$SRCDIR/parson/parson.c',
]

dudley = env.Program(
    'dudley',
    source = sources,
)

Return('dudley')
