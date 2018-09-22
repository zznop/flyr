Import('env')

sources = [
    '$SRCDIR/main.c',
    '$SRCDIR/utils.c',
    '$SRCDIR/parse.c',
    '$SRCDIR/build.c',
    '$SRCDIR/mutate.c',
    '$SRCDIR/parson/parson.c',
]

dudley = env.Program(
    'dudley',
    source = sources,
)

Return('dudley')
